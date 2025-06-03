// Copyright 2018-2023, Athian Games. All Rights Reserved. 


#include "PostgreSQLBPLibrary.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/BufferArchive.h"
#include "Templates/SharedPointer.h"
#include "RHI.h"
#include "RenderingThread.h"
#include "RenderDeferredCleanup.h"
#include "TextureResource.h"

PGconn* DBConnection;

void *v_PostgreSQLdllHandle;
IImageWrapperModule* ImageWrapperModule = nullptr;

class BlobBuilder : public std::streambuf
{
public:
	BlobBuilder(const std::string& buffer)
	{
		char* d;

		d = const_cast<char*>(buffer.data());
		setg(d, d, d + buffer.size());
	}
};


FString GetDLLPath(FString DLLName)
{
	//Checks for the Plugin path from the current plugin directory. 
	FString Pluginpath = IPluginManager::Get().FindPlugin(TEXT("PostgreSQL"))->GetBaseDir();
	FString DllPath = FPaths::Combine(*Pluginpath, TEXT("Source"), TEXT("SQLLib"), DLLName);

	return DllPath;
	
}

wchar_t* UPostgreSQLBPLibrary::GetWCharfromChar(const char* Input)
{
	const size_t length = 1 + strlen(Input);
	wchar_t* wcsText = new wchar_t[length];
	
	size_t convertedSize = 0;

	_locale_t local = _create_locale(LC_ALL, "Chinese");
	errno_t ret = _mbstowcs_s_l(&convertedSize, wcsText, length, Input, _TRUNCATE, local);
	
	return wcsText;
}


char* UPostgreSQLBPLibrary::GetCharfromFString(FString Query)
{
	const TCHAR* queryTChar = *Query;

	size_t len;
	_locale_t local = _create_locale(LC_ALL, "Chinese");

	_wcstombs_s_l(&len, nullptr, 0, queryTChar, 0, local);

	size_t convertedSize = 0;
	
	char *charBuffer = static_cast<char*>(malloc(len));
	_wcstombs_s_l(&convertedSize, charBuffer, len,
		queryTChar, _TRUNCATE, local);

	return charBuffer;
}

void  UPostgreSQLBPLibrary::CloseConnection()
{
	if (DBConnection)
	{
		try
		{
			ConnStatusType DBConnectionStatus = PQstatus(DBConnection);
			if(DBConnectionStatus == CONNECTION_MADE)
			{
				PQfinish(DBConnection);
			}
			
			PQreset(DBConnection);
			
		}
		catch (const std::exception& ex)
		{
			string message = ex.what();
		}

		
	}
}

bool UPostgreSQLBPLibrary::SetConnectionProperties(FString Server, FString DBName, FString UserID, FString Password, TMap<FString, FString> ExtraParams,
	FString& ErrorMessage)
{
	CloseConnection();

	FString FormattedString = "host=" + Server + " dbname=" + DBName + " user=" + UserID + " password=" + Password;

	for (auto eParam : ExtraParams)
	{
		FormattedString = FormattedString + " " + eParam.Key + "=" + eParam.Value;
	}

	std::string connectionstring(TCHAR_TO_UTF8(*FormattedString));
	const char* qstring = connectionstring.c_str();

	try
	{
		DBConnection = PQconnectdb(qstring);
		ConnStatusType ConnectionStatus = PQstatus(DBConnection);
		if (ConnectionStatus == CONNECTION_OK)
		{
			return true;
		}

		if (ConnectionStatus == CONNECTION_BAD)
		{
			ErrorMessage = "Connection to database failed: " + FString(PQerrorMessage(DBConnection));
			CloseConnection();
			return false;
		}
	}
	catch (const std::exception& ex)
	{
		ErrorMessage = ex.what();
		CloseConnection();
		return false;
	}

	return false;
}


bool UPostgreSQLBPLibrary::BeginConnection()
{
	if (DBConnection)
	{
		PGresult* result = PQexec(DBConnection, "BEGIN");
		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			PQclear(result);
			result = PQexec(DBConnection, "END");
			PQclear(result);
			return false ;
		}
		else
		{
			PQclear(result);
			return true;
		}
	}

	return false;
}

void UPostgreSQLBPLibrary::EndConnection()
{
	if (DBConnection)
	{
		PGresult* result = PQexec(DBConnection, "END");
		PQclear(result);
	}
}

void UPostgreSQLBPLibrary::GetErrorMessage(FString& ErrorMessage)
{
	if (DBConnection)
	{
		ErrorMessage = PQerrorMessage(DBConnection);
		if (ErrorMessage.IsEmpty())
		{
			ErrorMessage = "Unknown Error";
		}
	}
}

void UPostgreSQLBPLibrary::UpdateDataFromQuery(FString Query, bool& IsSuccessful, FString &ErrorMessage)
{
	if (DBConnection)
	{
		string query(TCHAR_TO_UTF8(*Query));
		const char* q = query.c_str();

		try
		{
			if (BeginConnection())
			{
				PGresult* result = PQexec(DBConnection, q);

				if (PQresultStatus(result) != PGRES_COMMAND_OK)
				{
					GetErrorMessage(ErrorMessage);
					IsSuccessful = false;
				}
				else
				{
					IsSuccessful = true;
				}

				PQclear(result);
				EndConnection();
			}
			else
			{
				GetErrorMessage(ErrorMessage);
				IsSuccessful = false;

			}

		}
		catch (const exception& ex)
		{
			const char* emessage = ex.what();
			ErrorMessage = FString(UTF8_TO_TCHAR(emessage));
			IsSuccessful = false;
			
		}

	}


}



void UPostgreSQLBPLibrary::SelectDataFromQuery(FString Query, bool& IsSuccessful, FString &ErrorMessage, 
	TArray<FPostgreSQLDataTable>& ResultByColumn, TArray<FPostgreSQLDataRow>& ResultByRow)
{
	string query(TCHAR_TO_UTF8(*Query));
	const char* q = query.c_str();

	if (DBConnection)
	{
		try
		{
			if (BeginConnection())
			{

				PGresult* result = PQexec(DBConnection, q);

				if (PQresultStatus(result) != PGRES_TUPLES_OK)
				{
					GetErrorMessage(ErrorMessage);
					IsSuccessful = false;
					
					PQclear(result);
					EndConnection();

					return;
				}
				else
				{
					IsSuccessful = true;
				}

				int nFields = PQnfields(result);

				for (int i = 0; i < nFields; i++)
				{
					char* cName = PQfname(result, i);
					FPostgreSQLDataTable NewDataTable;
					wchar_t* wcname = UTF8_TO_TCHAR(cName);
					NewDataTable.ColumnName = FString(wcname);
					ResultByColumn.Add(NewDataTable);

				}

				int nTuples = PQntuples(result);
				for (int i = 0; i < nTuples; i++)
				{
					TArray<FString> RowData;
					for (int c = 0; c < nFields; c++)
					{
						char* columnvalue = PQgetvalue(result, i, c);
						wchar_t* wcvalue = UTF8_TO_TCHAR(columnvalue);
						FString value = FString(wcvalue);

						ResultByColumn[c].ColumnData.Add(value);
						RowData.Add(value);
					}

					FPostgreSQLDataRow Row;
					Row.RowData = RowData;
					ResultByRow.Add(Row);
				}


				IsSuccessful = true;
				PQclear(result);

				EndConnection();
			}
			else
			{
				GetErrorMessage(ErrorMessage);
				IsSuccessful = false;

			}


		}
		catch (const exception & ex)
		{
			const char* emessage = ex.what();
			ErrorMessage = FString(UTF8_TO_TCHAR(emessage));
			IsSuccessful = false;

		}

	}

}

void UPostgreSQLBPLibrary::FlushImageRenderingCommands()
{
	if (!GIsRHIInitialized)
	{
		return;
	}

	// Need to flush GT because render commands from threads other than GT are sent to
	// the main queue of GT when RT is disabled
	if (!GIsThreadedRendering
		&& !FTaskGraphInterface::Get().IsThreadProcessingTasks(ENamedThreads::GameThread)
		&& !FTaskGraphInterface::Get().IsThreadProcessingTasks(ENamedThreads::GameThread_Local))
	{
		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread_Local);
	}

	ENQUEUE_RENDER_COMMAND(FlushPendingDeleteRHIResourcesCmd)(
		[](FRHICommandListImmediate& RHICmdList)
	{
		RHICmdList.ImmediateFlush(EImmediateFlushType::FlushRHIThreadFlushResources);
	});


	// Find the objects which may be cleaned up once the rendering thread command queue has been flushed.
	FPendingCleanupObjects* PendingCleanupObjects = GetPendingCleanupObjects();

	// Issue a fence command to the rendering thread and wait for it to complete.
	FRenderCommandFence Fence;
	Fence.BeginFence();
	Fence.Wait();

	// Delete the objects which were enqueued for deferred cleanup before the command queue flush.
	delete PendingCleanupObjects;
}

void UPostgreSQLBPLibrary::GetTexturePixels(UTexture2D* Texture, TArray<FColor>& OutPixels)
{

	OutPixels.Reset();

	ENQUEUE_RENDER_COMMAND(FSavePostgreSQLImageResource)(
		[OutPixels, Texture](FRHICommandListImmediate& RHICmdList)
	{
		TArray<FColor> OutData = OutPixels;
		FTextureResource* TextureResource = Texture->GetResource();
		FTexture2DRHIRef Texture2DRHIRef = TextureResource->TextureRHI->GetTexture2D();

		FIntRect Rect = FIntRect(0, 0, Texture->GetImportedSize().X, Texture->GetImportedSize().Y);
		FReadSurfaceDataFlags Flags = FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX);
		
		RHICmdList.ReadSurfaceData(
			Texture2DRHIRef,
			Rect,
			OutData,
			Flags
		);
	});

	FlushImageRenderingCommands();
}

int UPostgreSQLBPLibrary::GetRawImageSize(FString ImagePath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (PlatformFile.FileExists(*ImagePath))
	{
		if (IFileHandle* FileHandle = PlatformFile.OpenRead(*ImagePath))
		{
			int64 FileSize = FileHandle->Size();
			delete FileHandle;
			return FileSize;
		}
	}

	return 0;
}

// UTexture2D* UPostgreSQLBPLibrary::LoadTexturefromCharData(const char* ImageChar)
// {
// 	// Assuming ImageChar is raw PNG or JPG image data
// 	TArray<uint8> ImageData;
//     
// 	// Convert char* to TArray<uint8>
// 	int ImageCharLength = strlen(ImageChar);
// 	for (int i = 0; i < ImageCharLength; i++)
// 	{
// 		ImageData.Add(static_cast<uint8>(ImageChar[i]));
// 	}
//     
// 	UTexture2D* LoadedTexture = nullptr;
//
// 	CreateImageWrapperModule();
//
// 	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::PNG);
//     
// 	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
// 	{
// 		TArray<uint8> UncompressedBGRA;
// 		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
// 		{
// 			LoadedTexture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
//
// 			// Ensure a valid texture could be created
// 			if (!LoadedTexture) return nullptr;
//
// 			void* TextureData = LoadedTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
// 			FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
// 			LoadedTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
//
// 			// Update the rendering resource from data
// 			LoadedTexture->UpdateResource();
// 		}
// 	}
//
// 	return LoadedTexture;
// }
//
// char* UPostgreSQLBPLibrary::GetCharFromTextureData(UTexture2D* Texture, FString Path)
// {
// 	FVector2D ImageSize = FVector2D(Texture->GetSurfaceWidth(), Texture->GetSurfaceHeight());
// 	char* ImageBytes = new char[ImageSize.X * ImageSize.Y * 4];
//
// 	FTextureResource* TextureResource = Texture->GetResource();
// 	if (TextureResource && TextureResource->TextureRHI)
// 	{
// 		FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
//
// 		TArray<FColor> OutData;
// 		OutData.Reserve(ImageSize.X * ImageSize.Y);
// 		OutData.AddZeroed(ImageSize.X * ImageSize.Y);
//
// 		FReadSurfaceDataFlags ReadDataFlags(RCM_UNorm);
// 		RHICmdList.ReadSurfaceData(TextureResource->TextureRHI, FIntRect(0, 0, ImageSize.X, ImageSize.Y), OutData, ReadDataFlags);
//
// 		for (int32 i = 0; i < OutData.Num(); i++)
// 		{
// 			ImageBytes[i * 4] = OutData[i].R;
// 			ImageBytes[i * 4 + 1] = OutData[i].G;
// 			ImageBytes[i * 4 + 2] = OutData[i].B;
// 			ImageBytes[i * 4 + 3] = OutData[i].A;
// 		}
// 	}
//
// 	return ImageBytes;
// }

bool UPostgreSQLBPLibrary::SaveTextureToPath(UTexture2D* Texture, const FString Path)
{

	// if (Texture && FPaths::ValidatePath(Path))
	// {
	//
	// 	FVector2D ImageSize = FVector2D(Texture->GetSurfaceWidth(), Texture->GetSurfaceHeight());
	// 	if (char* imagebytes = GetCharFromTextureData(Texture, Path))
	// 	{
	// 		 return false;
	// 	}
	//
	// }
	return false;
}

void UPostgreSQLBPLibrary::UpdateImageFromPath(FString Query, FString UpdateParameter, int ParameterID, FString ImagePath, bool& IsSuccessful, FString& ErrorMessage)
{
	FString ImageParam = "@" + UpdateParameter;
	FString PGImageParam = FString("$") + FString::FromInt(ParameterID);
	Query = Query.Replace(*ImageParam, *PGImageParam);

	string query(TCHAR_TO_UTF8(*Query));
	char* q = _strdup(query.c_str());

	if (DBConnection)
	{
		char* ImageChar = GetRawImageFromPath(ImagePath);
		int ImageCharLength = GetRawImageSize(ImagePath); 

		if (ImageChar)
		{
			try
			{
				const char* values[1] = { ImageChar };
				int lengths[1] = { ImageCharLength };
				int formats[1] = { 1 };  // binary
				PGresult* result = PQexecParams(DBConnection, q, 1, nullptr, values, lengths, formats, 1);
				
				if (PQresultStatus(result) != PGRES_COMMAND_OK)
				{
					ErrorMessage = FString(PQresultErrorMessage(result));
					IsSuccessful = false;
				}
			}
			catch (const exception& ex)
			{
				const char* emessage = ex.what();
				ErrorMessage = FString(UTF8_TO_TCHAR(emessage));
				IsSuccessful = false;
			}
		}
	}
}

void UPostgreSQLBPLibrary::CreateImageWrapperModule()
{
	if (!ImageWrapperModule)
	{
		ImageWrapperModule = &FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	}
}

char* UPostgreSQLBPLibrary::GetRawImageFromPath(FString ImagePath)
{
	CreateImageWrapperModule();
	void* imagebytes = nullptr;
	if (ImageWrapperModule)
	{
		FString Extension = FPaths::GetExtension(ImagePath);
		EImageFormat ImageFormat;

		if (Extension.Equals("jpeg", ESearchCase::IgnoreCase) || Extension.Equals("jpg", ESearchCase::IgnoreCase))
		{
			ImageFormat = EImageFormat::JPEG;
		}
		else if (Extension.Equals("png", ESearchCase::IgnoreCase))
		{
			ImageFormat = EImageFormat::PNG;
		}
		else
		{
			ImageFormat = EImageFormat::BMP;
		}

		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(ImageFormat);

		TArray<uint8> RawFileData;
		if (FFileHelper::LoadFileToArray(RawFileData, *ImagePath))
		{
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
			{
				TArray<uint8> UncompressedBGRA;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
				{
					const int64 RawNum = UncompressedBGRA.Num();
					imagebytes = FMemory::Malloc(RawNum);
					FMemory::Memcpy(imagebytes, UncompressedBGRA.GetData(), RawNum);

				}
			}
		}

	}

	return static_cast<char*>(imagebytes);
}

UTexture2D* UPostgreSQLBPLibrary::SelectImageFromQuery(FString Query, FString SelectParameter, int ParameterID, bool& IsSuccessful, FString& ErrorMessage)
{
	UTexture2D* Texture = nullptr;

	FString Param = "@" + SelectParameter;
	Query = Query.Replace(*Param, *FString("?"));

	string query(TCHAR_TO_UTF8(*Query));
	char* q = _strdup(query.c_str());

	if (DBConnection)
	{
		try
		{
			PGresult* result = PQexecParams(DBConnection, q, 0, NULL, NULL, NULL, NULL, 1);
            
			if (PQresultStatus(result) != PGRES_TUPLES_OK)
			{
				ErrorMessage = FString(PQresultErrorMessage(result));
				IsSuccessful = false;
			}
			else
			{
				// Access the image data
				// if(char* ImageChar = PQgetvalue(result, 0, ParameterID))
				// {
				// 	// Create a texture from the image data
				// 	Texture = LoadTexturefromCharData(ImageChar);
				// 	if(Texture)
				// 	{
				// 		IsSuccessful = true;
				// 	}
				// }
			
			}

			PQclear(result);
		}
		catch (const exception& ex)
		{
			const char* emessage = ex.what();
			ErrorMessage = FString(UTF8_TO_TCHAR(emessage));
			IsSuccessful = false;
		}
	}

	return Texture;
}
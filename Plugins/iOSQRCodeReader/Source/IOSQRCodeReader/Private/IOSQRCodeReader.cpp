//
//  IOOSQRCodeReader.cpp
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 1/16/23.
//  Copyright © 2023 Matthew Zane. All rights reserved.
//

#include "IOSQRCodeReader.h"

#define LOCTEXT_NAMESPACE "FIOSQRCodeReaderModule"

void FIOSQRCodeReaderModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FIOSQRCodeReaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FIOSQRCodeReaderModule, IOSQRCodeReader)

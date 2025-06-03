//
//  QRCodeReaderActor.cpp
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 1/16/23.
//  Copyright © 2023 Matthew Zane. All rights reserved.
//

#include "QRCodeReaderActor.h"

#if PLATFORM_IOS
#import <AVFoundation/AVFoundation.h>
#endif

#include "CameraEnumConverter.h"

// Sets default values
AQRCodeReaderActor::AQRCodeReaderActor()
{
	PrimaryActorTick.bCanEverTick = true;
    
    Texture = UTexture2D::CreateTransient(
        DEFAULT_TEXTURE_WIDTH,
        DEFAULT_TEXTURE_HEIGHT,
        PF_B8G8R8A8
    );
    
#if PLATFORM_IOS
	QRCodeReaderImpl = [[QRCodeReader alloc] init];
    
    QRCodeReaderImpl->Texture = Texture;
    QRCodeReaderImpl->TextureWidth = DEFAULT_TEXTURE_WIDTH;
    QRCodeReaderImpl->TextureHeight = DEFAULT_TEXTURE_HEIGHT;
#endif
}

// Called when the game starts or when spawned
void AQRCodeReaderActor::BeginPlay()
{
	Super::BeginPlay();
    
}

// Called every frame
void AQRCodeReaderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    Texture->UpdateResource();
}

void AQRCodeReaderActor::Init(
    bool QRCodeReaderEnabled,
    bool VideoEnabled,
    bool PortraitEnabled,
    bool PortraitUpsideDownEnabled,
    bool LandscapeLeftEnabled,
    bool LandscapeRightEnabled,
    bool AutoCameraRotateEnabled,
    CameraOrientation InitialOrientation
) {
#if PLATFORM_IOS
    QRCodeReaderImpl->qrCodeReaderEnabled = QRCodeReaderEnabled;
    QRCodeReaderImpl->videoEnabled = VideoEnabled;
    QRCodeReaderImpl->portraitEnabled = PortraitEnabled;
    QRCodeReaderImpl->portraitUpsideDownEnabled = PortraitUpsideDownEnabled;
    QRCodeReaderImpl->landscapeLeftEnabled = LandscapeLeftEnabled;
    QRCodeReaderImpl->landscapeRightEnabled = LandscapeRightEnabled;
    
    QRCodeReaderImpl->autoCameraRotateEnabled = AutoCameraRotateEnabled;
    
    AVCaptureVideoOrientation videoOrientation = UCameraEnumConverter::CameraOrientationToAVCaptureVideoOrientation(InitialOrientation);
    [QRCodeReaderImpl initOrientation: videoOrientation];
#endif
}


bool AQRCodeReaderActor::GetQRCodeReaderEnabled() {
#if PLATFORM_IOS
    return QRCodeReaderImpl->qrCodeReaderEnabled;
#else
    return false;
#endif
}

void AQRCodeReaderActor::SetQRCodeReaderEnabled(bool QRCodeReaderEnabled) {
#if PLATFORM_IOS
    QRCodeReaderImpl->qrCodeReaderEnabled = QRCodeReaderEnabled;
#endif
}

bool AQRCodeReaderActor::GetVideoEnabled() {
#if PLATFORM_IOS
    return QRCodeReaderImpl->videoEnabled;
#else
    return false;
#endif
}

void AQRCodeReaderActor::SetVideoEnabled(bool VideoEnabled) {
#if PLATFORM_IOS
    QRCodeReaderImpl->videoEnabled = VideoEnabled;
#endif
}

bool AQRCodeReaderActor::GetPortraitEnabled() {
#if PLATFORM_IOS
    return QRCodeReaderImpl->portraitEnabled;
#else
    return false;
#endif
}

void AQRCodeReaderActor::SetPortraitEnabled(bool PortraitEnabled) {
#if PLATFORM_IOS
    QRCodeReaderImpl->portraitEnabled = PortraitEnabled;
#endif
}

bool AQRCodeReaderActor::GetPortraitUpsideDownEnabled() {
#if PLATFORM_IOS
    return QRCodeReaderImpl->portraitUpsideDownEnabled;
#else
    return false;
#endif
}

void AQRCodeReaderActor::SetPortraitUpsideDownEnabled(bool PortraitUpsideDownEnabled) {
#if PLATFORM_IOS
     QRCodeReaderImpl->portraitUpsideDownEnabled = PortraitUpsideDownEnabled;
#endif
}

bool AQRCodeReaderActor::GetLandscapeLeftEnabled() {
#if PLATFORM_IOS
    return QRCodeReaderImpl->landscapeLeftEnabled;
#else
    return false;
#endif
}

void AQRCodeReaderActor::SetLandscapeLeftEnabled(bool LandscapeLeftEnabled) {
#if PLATFORM_IOS
    QRCodeReaderImpl->landscapeLeftEnabled = LandscapeLeftEnabled;
#endif
}

bool AQRCodeReaderActor::GetLandscapeRightEnabled() {
#if PLATFORM_IOS
    return QRCodeReaderImpl->landscapeRightEnabled;
#else
    return false;
#endif
}

void AQRCodeReaderActor::SetLandscapeRightEnabled(bool LandscapeRightEnabled) {
#if PLATFORM_IOS
    QRCodeReaderImpl->landscapeRightEnabled = LandscapeRightEnabled;
#endif
}

FString AQRCodeReaderActor::GetQRCodeUrl() {
    	const char *c = NULL;
#if PLATFORM_IOS
    	NSString* urlString = QRCodeReaderImpl.url;
    	c = [urlString UTF8String];
#endif
    	if (c != NULL) {
        	return FString(c);
    	}
    	else {
        	return FString("");
        }
}

bool AQRCodeReaderActor::IsCameraOn() {
#if PLATFORM_IOS
	return QRCodeReaderImpl.cameraOn;
#else
    	return false;
#endif
}

void AQRCodeReaderActor::SetCameraIsOn(bool cameraOn) {
#if PLATFORM_IOS
	if (cameraOn) {
		// Set camera ON
		if (!QRCodeReaderImpl.cameraOn) {
			[QRCodeReaderImpl startReading];
		}
	}
	else {
		// Set camera OFF
		if (QRCodeReaderImpl.cameraOn) {
			[QRCodeReaderImpl stopReading];
		}
	}
#endif
}

CameraPosition AQRCodeReaderActor::GetCameraPosition() {
#if PLATFORM_IOS
	return UCameraEnumConverter::AVCaptureDevicePositionToCameraPosition(
		QRCodeReaderImpl.cameraPosition
	);
#else
	return CameraPosition::UNSPECIFIED;
#endif
}

void AQRCodeReaderActor::SetCameraPosition(CameraPosition cameraPosition) {
#if PLATFORM_IOS
	QRCodeReaderImpl.cameraPosition =
		UCameraEnumConverter::CameraPositionToAVCaptureDevicePosition(
			cameraPosition
	);
    
	if (QRCodeReaderImpl.cameraOn) {
		[QRCodeReaderImpl stopReading];
		[QRCodeReaderImpl startReading];
	}
#endif
}

CameraOrientation AQRCodeReaderActor::GetCameraOrientation() {
#if PLATFORM_IOS
    return UCameraEnumConverter::AVCaptureVideoOrientationToCameraOrientation(
        QRCodeReaderImpl.deviceOrientation
    );
#else
    return CameraOrientation::UNSPECIFIED;
#endif
}

void AQRCodeReaderActor::SetCameraOrientation(CameraOrientation cameraOrientation) {
#if PLATFORM_IOS
    QRCodeReaderImpl.cameraOrientation =
        UCameraEnumConverter::CameraOrientationToAVCaptureVideoOrientation(
            cameraOrientation
    );
    
    if (QRCodeReaderImpl.cameraOn) {
        [QRCodeReaderImpl stopReading];
        [QRCodeReaderImpl startReading];
    }
#endif
}

bool AQRCodeReaderActor::GetAutoCameraRotateEnabled() {
#if PLATFORM_IOS
    return QRCodeReaderImpl->autoCameraRotateEnabled;
#else
    return false;
#endif
}

void AQRCodeReaderActor::SetAutoCameraRotateEnabled(bool AutoCameraRotateEnabled) {
#if PLATFORM_IOS
    QRCodeReaderImpl->autoCameraRotateEnabled = AutoCameraRotateEnabled;
#endif
}

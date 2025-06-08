//
//  QRCodeReaderActor.cpp
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 2/10/23.
//  Copyright © 2023 Matthew Zane. All rights reserved.
//

#include "QRCodeReader.h"

#include "TextureResource.h"

#if PLATFORM_IOS

#include "QRCodeReaderActor.h"

@interface QRCodeReader ()

@end

@implementation QRCodeReader

-(id)init {
    if (self = [super init]) {
        _cameraOn = NO;
        
        _url = @"";
        
        _cameraPosition = AVCaptureDevicePositionBack;
        
        _captureSession = nil;
        
        _videoConnection = nil;
        
        _deviceOrientation = AVCaptureVideoOrientationLandscapeRight;
        _cameraOrientation = AVCaptureVideoOrientationLandscapeRight;
        
        _videoOutput = nil;
        _metadataOutput = nil;
        
        autoCameraRotateEnabled = true;
        
        // Use a NSNotificationCenter to setup deviceOrientationDidChange() to
        // run whenever orientation changes
        NSNotificationCenter *notificationCenter = [NSNotificationCenter defaultCenter];
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        [notificationCenter addObserver:self
            selector:@selector(deviceOrientationDidChange)
            name:UIDeviceOrientationDidChangeNotification object:nil];
    }
    return self;
}

-(void)initOrientation:(AVCaptureVideoOrientation) initialOrientation {
    
    _deviceOrientation = initialOrientation;
    
    if (initialOrientation == AVCaptureVideoOrientationPortrait) {
        // Rotate counter-clockwise 90 degree from LandscapeLeft frames
        _cameraOrientation = AVCaptureVideoOrientationLandscapeLeft;
    }
    else if (initialOrientation == AVCaptureVideoOrientationPortraitUpsideDown) {
        // Rotate counter-clockwise 90 degree from Landscaperight frame
        _cameraOrientation = AVCaptureVideoOrientationLandscapeRight;
    }
    else if (initialOrientation == AVCaptureVideoOrientationLandscapeLeft){
        _cameraOrientation = AVCaptureVideoOrientationLandscapeRight;
    }
    else if (initialOrientation == AVCaptureVideoOrientationLandscapeRight) {
        _cameraOrientation = AVCaptureVideoOrientationLandscapeLeft;
    }
}

-(BOOL)startReading {
    _cameraOn = YES;
    return YES;
}

-(void)stopReading {
    _url = @"";
    _cameraOn = NO;
    dispatch_async(dispatch_get_main_queue(), ^{
        FEvent* fSemaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
        AsyncTask(ENamedThreads::GameThread, [self, fSemaphore]() {
            void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
            const int32 TextureDataSize = TextureWidth * TextureHeight * 4;
            TArray<FColor> BlackFrame;
            BlackFrame.Init(FColor(0,0,0,255), TextureWidth * TextureHeight);
            FMemory::Memcpy(TextureData, BlackFrame.GetData(), TextureDataSize);
            Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
            fSemaphore->Trigger();
        });
        fSemaphore->Wait();
        FGenericPlatformProcess::ReturnSynchEventToPool(fSemaphore);
    });
}


- (void)deviceOrientationDidChange {
    if (autoCameraRotateEnabled) {
        UIDeviceOrientation deviceOrientation = 
        [[UIDevice currentDevice] orientation];
        
        AVCaptureVideoOrientation newCameraOrientation = _cameraOrientation;
        
        // AVCapture and UIDevice have opposite meanings for landscape left and 
        // right
        if (deviceOrientation == UIDeviceOrientationPortrait &&
            portraitEnabled) {
            NSLog(@"deviceOrientationDidChange - Portrait");
            // Rotate counter-clockwise 90 degree from LandscapeLeft frames
            _deviceOrientation = AVCaptureVideoOrientationPortrait;
            newCameraOrientation = AVCaptureVideoOrientationLandscapeLeft;
        }
        else if (deviceOrientation == UIDeviceOrientationPortraitUpsideDown &&
                 portraitUpsideDownEnabled) {
            NSLog(@"deviceOrientationDidChange - PortraitUpsideDown");
            // Rotate counter-clockwise 90 degree from Landscaperight frames
            _deviceOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
            newCameraOrientation = AVCaptureVideoOrientationLandscapeRight;
        }
        else if (deviceOrientation == UIDeviceOrientationLandscapeLeft &&
                 landscapeRightEnabled){
            NSLog(@"deviceOrientationDidChange - LandscapeLeft");
            _deviceOrientation = AVCaptureVideoOrientationLandscapeRight;
            newCameraOrientation = AVCaptureVideoOrientationLandscapeRight;
        }
        else if (deviceOrientation == UIDeviceOrientationLandscapeRight
                 && landscapeLeftEnabled){
            NSLog(@"deviceOrientationDidChange - LandscapeRight");
            _deviceOrientation = AVCaptureVideoOrientationLandscapeLeft;
            newCameraOrientation = AVCaptureVideoOrientationLandscapeLeft;
        }
        
        if (newCameraOrientation != _cameraOrientation)
        {
            _cameraOrientation = newCameraOrientation;
        }
    }
}

-(void)resizeCameraFeedTexture:(int)width textureHeight:(int)height {
    FEvent* fSemaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
    AsyncTask(ENamedThreads::GameThread, [self, width, height, fSemaphore] () {
        Texture->ReleaseResource();

        // Allocate first mipmap.
        int32 NumBlocksX = width / GPixelFormats[PF_B8G8R8A8].BlockSizeX;
        int32 NumBlocksY = height / GPixelFormats[PF_B8G8R8A8].BlockSizeY;
        FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
        Mip.SizeX = width;
        Mip.SizeY = height;
        Mip.BulkData.Lock(LOCK_READ_WRITE);
        Mip.BulkData.Realloc(

-(void)processARFrame:(ARFrame*)frame {
    if (!_cameraOn) { return; }
    CVPixelBufferRef pixelBuffer = frame.capturedImage;
    CVOptionFlags lockFlags = 0;
    if (CVPixelBufferLockBaseAddress(pixelBuffer, lockFlags) != kCVReturnSuccess) {
        return;
    }
    int frameHeight;
    int frameWidth;
    if (_deviceOrientation == AVCaptureVideoOrientationPortrait ||
        _deviceOrientation == AVCaptureVideoOrientationPortraitUpsideDown) {
        frameWidth = (int)CVPixelBufferGetHeight(pixelBuffer);
        frameHeight = (int)CVPixelBufferGetWidth(pixelBuffer);
    } else {
        frameHeight = (int)CVPixelBufferGetHeight(pixelBuffer);
        frameWidth = (int)CVPixelBufferGetWidth(pixelBuffer);
    }
    void* SourceData = CVPixelBufferGetBaseAddress(pixelBuffer);
    if (TextureWidth != frameWidth || TextureHeight != frameHeight) {
        [self resizeCameraFeedTexture: frameWidth textureHeight:frameHeight];
    }
    TArray<FColor> RotatedPixels;
    if (_deviceOrientation == AVCaptureVideoOrientationPortrait ||
        _deviceOrientation == AVCaptureVideoOrientationPortraitUpsideDown) {
        FColor* SourcePixels = (FColor*)SourceData;
        RotatedPixels.SetNum(TextureWidth * TextureHeight);
        int SourceIndex = 0;
        for (int y = 0; y < TextureWidth; y++) {
            for (int x = 0; x < TextureHeight; x++) {
                int TargetIndex = y + TextureWidth * (TextureHeight - x - 1);
                RotatedPixels[TargetIndex] = SourcePixels[SourceIndex];
                SourceIndex++;
            }
        }
        SourceData = RotatedPixels.GetData();
    }
    FEvent* fSemaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
    AsyncTask(ENamedThreads::GameThread, [self, SourceData, frameWidth, frameHeight, fSemaphore]() {
        void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
        const int32 TextureDataSize = frameWidth * frameHeight * 4;
        if (TextureData != NULL && SourceData != NULL && _cameraOn) {
            FMemory::Memcpy(TextureData, SourceData, TextureDataSize);
        }
        Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
        fSemaphore->Trigger();
    });
    fSemaphore->Wait();
    FGenericPlatformProcess::ReturnSynchEventToPool(fSemaphore);
    VNDetectBarcodesRequest* request = [[VNDetectBarcodesRequest alloc] init];
    VNImageRequestHandler* handler = [[VNImageRequestHandler alloc] initWithCVPixelBuffer:pixelBuffer options:@{}];
    NSError* error = nil;
    [handler performRequests:@[request] error:&error];
    BOOL found = NO;
    if (!error) {
        for (VNBarcodeObservation* observation in request.results) {
            if (observation.payloadStringValue) {
                [_url release];
                _url = [[NSString alloc] initWithString:observation.payloadStringValue];
                found = YES;
                break;
            }
        }
    }
    if (!found) {
        _url = @"";
    }
    CVPixelBufferUnlockBaseAddress(pixelBuffer, lockFlags);
}

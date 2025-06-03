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
    // Pass in the cameraPosition so that the desired camera will be the first
    // camera in [captureDeviceDiscoverySession devices]
    AVCaptureDeviceDiscoverySession *captureDeviceDiscoverySession = [
            AVCaptureDeviceDiscoverySession discoverySessionWithDeviceTypes:
                @[AVCaptureDeviceTypeBuiltInWideAngleCamera]
                mediaType:AVMediaTypeVideo
                position:_cameraPosition
        ];
    
    // Obtain the device at the desired position
    NSArray *devices = [captureDeviceDiscoverySession devices];
    AVCaptureDevice *captureDevice = devices.firstObject;
    
    // Log which camera we are using
    if (captureDevice.position == AVCaptureDevicePositionBack) {
        NSLog(@"Using Back Camera");
    }
    else if (captureDevice.position == AVCaptureDevicePositionFront) {
        NSLog(@"Using Front Camera");
    }
    else {
        NSLog(@"Using Unknown Camera");
    }
    
    
    // Obtain the camera input device
    NSError * error;
    AVCaptureInput *input = [
            AVCaptureDeviceInput
                deviceInputWithDevice:captureDevice
                error:&error
        ];
    
    // Validate input
    if (!input) {
        NSLog(@"%@", [error localizedDescription]);
 
        return NO;
    }
    
    // Create the capture session
    _captureSession = [[AVCaptureSession alloc] init];
    
    // Add camera input to capture session
    [_captureSession addInput:input];
    
    if (videoEnabled) {
        // Make separate queues for video capture and metadata capture because, if
        // execution of one delegate takes a significant amount of time, the other
        // delegate may not execute
        dispatch_queue_t videoCaptureQueue = dispatch_queue_create(
            "videoQueue",
            DISPATCH_QUEUE_SERIAL
        );
        
        // Setup Video Output
        _videoOutput = [[AVCaptureVideoDataOutput alloc] init];

        // Set this class as the delegate for the videoOutput object
        [_videoOutput setSampleBufferDelegate: self queue: videoCaptureQueue];
        _videoOutput.alwaysDiscardsLateVideoFrames = YES;
        
        // Set video settings
        NSNumber * framePixelFormat = [NSNumber numberWithInt: kCVPixelFormatType_32BGRA];
        _videoOutput.videoSettings = [NSDictionary dictionaryWithObject: framePixelFormat forKey:(id)kCVPixelBufferPixelFormatTypeKey];
        
        [_captureSession addOutput: _videoOutput];

        // Must setup video orientation after adding video output to capture
        // session
        _videoConnection = [_videoOutput connectionWithMediaType:AVMediaTypeVideo];
        _videoConnection.videoOrientation = _cameraOrientation;
        
        // Mirror video if Front camera. Must execute after video output is
        // added to the capture session for video connection to support video
        // mirroring
        if(_videoConnection.supportsVideoMirroring &&  _cameraPosition == AVCaptureDevicePositionFront) {
            _videoConnection.automaticallyAdjustsVideoMirroring = NO;
            _videoConnection.videoMirrored = YES;
        }
    }
    
    if (qrCodeReaderEnabled) {
        // Setup QR Code Reader
        dispatch_queue_t metadataCaptureQueue = dispatch_queue_create(
            "metadataCaptureQueue",
            DISPATCH_QUEUE_SERIAL
        );
        
        // Setup Metadata Output
        _metadataOutput = [[AVCaptureMetadataOutput alloc] init];

        // Set this class as the delegate for the metadataOutput object
        [_metadataOutput setMetadataObjectsDelegate:self queue:metadataCaptureQueue];
        
        [_captureSession addOutput: _metadataOutput];
        
        // AVMetadataObjectTypeQRCode denotes the type of metadata to search for
        // Must be called after adding output to capture session
        [_metadataOutput setMetadataObjectTypes:[NSArray arrayWithObject:AVMetadataObjectTypeQRCode]];
    }
    
    
    
    // It is recommended to call startRunning from a background thread
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^ {
        [_captureSession startRunning];
        
        _cameraOn = YES;
    });

    return YES;
}

-(void)stopReading {
    // remove this class as delegates
    [_videoOutput setSampleBufferDelegate: nil queue:NULL];
    [_metadataOutput setMetadataObjectsDelegate:nil queue:NULL];
    
    // remove inputs from capture session
    for (AVCaptureInput *input in _captureSession.inputs) {
        [_captureSession removeInput:input];
    }
    for (AVCaptureOutput *output in _captureSession.outputs) {
        [_captureSession removeOutput:output];
    }

    _videoConnection = nil;
    
    [_captureSession stopRunning];
    _captureSession = nil;
    
    _url = @"";
    
    // This should be set to NO before texture is blackedout, so user can tell
    // when the texture is valid
    _cameraOn = NO;
    
    // Unreal AsycTask must be called from iOS main thread
    dispatch_async(dispatch_get_main_queue(), ^ {
        // Used to wait for main thread to finish
        FEvent* fSemaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
        // Texture must be updated on Unreal Game thread
        AsyncTask(ENamedThreads::GameThread, [
            self,
            fSemaphore
        ] () {
             void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(
             LOCK_READ_WRITE
             );
             
             const int32 TextureDataSize = TextureWidth * TextureHeight * 4;
             
             // Blackout texture
             TArray<FColor> BlackFrame;
             BlackFrame.Init(FColor(0, 0, 0, 255), TextureWidth * TextureHeight);
             FMemory::Memcpy(TextureData, BlackFrame.GetData(), TextureDataSize);
             
             Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
             fSemaphore->Trigger();
        });
        
        fSemaphore->Wait();
        FGenericPlatformProcess::ReturnSynchEventToPool(fSemaphore);
        

    });
}

-(void)captureOutput:(AVCaptureOutput *)captureOutput 
    didOutputSampleBuffer:(CMSampleBufferRef) sampleBuffer
    fromConnection:(AVCaptureConnection *)connection {
    // pixel buffer gives access to frame data
    CVPixelBufferRef pixelBuffer = 
        (CVPixelBufferRef)CMSampleBufferGetImageBuffer(sampleBuffer);
    CVOptionFlags lockFlags = 0;
    CVReturn status = CVPixelBufferLockBaseAddress(pixelBuffer, lockFlags);
    assert(kCVReturnSuccess == status);
    
    // if portrait
    int frameHeight;
    int frameWidth;
    if (_deviceOrientation == AVCaptureVideoOrientationPortrait ||
        _deviceOrientation == AVCaptureVideoOrientationPortraitUpsideDown) {
        // frame dimensions need to be flipped
        frameWidth = CVPixelBufferGetHeight(pixelBuffer);
        frameHeight = CVPixelBufferGetWidth(pixelBuffer);
    }
    else {
        frameHeight = CVPixelBufferGetHeight(pixelBuffer);
        frameWidth = CVPixelBufferGetWidth(pixelBuffer);
    }

    void* SourceData = CVPixelBufferGetBaseAddress(pixelBuffer);
    
    if (TextureWidth != frameWidth || TextureHeight != frameHeight) {
        [self resizeCameraFeedTexture: frameWidth textureHeight:frameHeight];
    }
    
    // Must defined outside of the if statement so the data pointer will be
    // valid if set to SourceData
    TArray<FColor> RotatedPixels;
    
    // if portrait mode
    if (_deviceOrientation == AVCaptureVideoOrientationPortrait ||
        _deviceOrientation == AVCaptureVideoOrientationPortraitUpsideDown) {
        FColor* SourcePixels = (FColor*)SourceData;
        RotatedPixels.SetNum(TextureWidth * TextureHeight);
        
        // Rotate frame counter clockwise 90 degrees
        int SourceIndex = 0;
        // TextureWidth is the height of the source frame
        for (int y = 0; y < TextureWidth; y++) {
            // TextureHeight is the width of the source frame
            for (int x = 0; x < TextureHeight; x++) {
                // Move pixel to rotated location
                // RotatedX = SourceY
                // RotatedY = TextureHeight - SourceX - 1
                // PixelArrayIndex = IndexX + IndexY * FrameWidth
                int TargetIndex = y + TextureWidth * (TextureHeight - x - 1);
                RotatedPixels[TargetIndex] = SourcePixels[SourceIndex];
                SourceIndex++;
            }
        }
        SourceData = RotatedPixels.GetData();
    }

    // Used to wait for main thread to finish
    FEvent* fSemaphore = FGenericPlatformProcess::GetSynchEventFromPool(false);
    // Texture must be updated on Unreal Game thread
    AsyncTask(ENamedThreads::GameThread, [
        self,
        SourceData,
        frameWidth,
        frameHeight,
        fSemaphore
    ] () {
        void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(
            LOCK_READ_WRITE
        );
        
        const int32 TextureDataSize = frameWidth * frameHeight * 4;
        
        if (TextureData != NULL && SourceData != NULL && _cameraOn) {
            FMemory::Memcpy(TextureData, SourceData, TextureDataSize);
        }
        
        Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
        
        fSemaphore->Trigger();
    });
    
    fSemaphore->Wait();
    FGenericPlatformProcess::ReturnSynchEventToPool(fSemaphore);
    
    CVOptionFlags unlockFlags = 0;
    CVPixelBufferUnlockBaseAddress(pixelBuffer, unlockFlags);
}

-(void)captureOutput:(AVCaptureOutput *)captureOutput
    didOutputMetadataObjects:(NSArray *)metadataObjects
    fromConnection:(AVCaptureConnection *)connection {
    BOOL qrCodeFound = NO;
    
    // Check if any metadata objects were been found
    if (metadataObjects != nil && [metadataObjects count] > 0) {
        // Check the metadata object found
        AVMetadataMachineReadableCodeObject *metadataObj = [
            metadataObjects objectAtIndex:0
        ];
        
        // Check if metadata object is a QR Code
        if ([[metadataObj type] isEqualToString:AVMetadataObjectTypeQRCode]) {
            // Set the url value to the QR Code's url
            NSString* urlString = [metadataObj stringValue];
            [_url release];
            _url = [[NSString alloc] initWithString:urlString];
            
            qrCodeFound = YES;
        }
    }
    
    // If no QR Code is found set the url to an empty string
    if (!qrCodeFound) {
        _url = @"";
    }
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
            
            if (_cameraOn) {
                _videoConnection.videoOrientation = _cameraOrientation;
            }
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
            NumBlocksX * NumBlocksY * GPixelFormats[PF_B8G8R8A8].BlockBytes
        );
        Mip.BulkData.Unlock();
        
        fSemaphore->Trigger();
    });
    
    fSemaphore->Wait();
    FGenericPlatformProcess::ReturnSynchEventToPool(fSemaphore);
    
    TextureWidth = width;
    TextureHeight = height;
}

@end

#endif

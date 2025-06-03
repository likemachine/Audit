//
//  QRCodeReader.h
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 1/16/23.
//  Copyright © 2023 Matthew Zane All rights reserved.
//

#pragma once

// Class is only defined on iOS platform
#if PLATFORM_IOS

#include "Engine/Texture2D.h"
#include "Async/Async.h"

// AVFoundation provides the camera and QR Code related types
#import <AVFoundation/AVFoundation.h>

/**
* Defined as a AVCaptureMetadataOutputObjectsDelegate to tell the class to run
* the captureOutput() function every tick when the camera is on.
*/
@interface QRCodeReader : NSObject<
    AVCaptureVideoDataOutputSampleBufferDelegate,
    AVCaptureMetadataOutputObjectsDelegate
> {
    @public
    // Texture of the camera feed
    UTexture2D* Texture;

    // Width of the texture.
    int TextureWidth;

    // Height of the texture.
    int TextureHeight;
    
    /// Whether the QR Code Reading functionality is enabled
    bool qrCodeReaderEnabled;

    /// Whether the video capture functionalilty is enabled
    bool videoEnabled;

    /// Whether the camera feed is enabled for Landscape Left mode
    bool landscapeLeftEnabled;

    /// Whether the camera feed is enabled for Landscape Right mode
    bool landscapeRightEnabled;

    /// Whether the camera feed is enabled for Portrait mode
    bool portraitEnabled;

    /// Whether the camera feed is enabled for Portrait Upside Down mode
    bool portraitUpsideDownEnabled;
    
    /// Whether the camera feed will automatically rotate when the device
    /// orientation changes.
    bool autoCameraRotateEnabled;
}

-(void)initOrientation:(AVCaptureVideoOrientation) initialOrientation;

/**
* Turns on the camera at cameraPosition. Return YES if successful, NO 
* otherwise.
*
* Will attempt to start a capture session with the camera position set by the
* cameraPosition variable and set the captureSession variable. If successful,
* cameraOn is set to YES. If an error occurs at any point, the capture session
* will to be started and NO will be returned.
*
* @return YES if successful, NO otherwise
*/
-(BOOL)startReading;

/**
* Turns off the current camera.
*
* Assumes that the camera is already on and captureSession is not null. Stops
* the capture session and sets captureSession to nil. Also sets url to an empty
* string and cameraOn to NO.
*/
-(void)stopReading;

/**
* Sets the deviceOrientation variable and videoConnection's videoOrientation
* variable to the device's current orientation.
* 
* Designed to be run whenever the device orientation changes. Reads the current
* UIDeviceOrientation value from the current device and sets deviceOrientation
* and videoConnection.videoOrientation to the corresponding 
* AVCaptureVideoOrientation value. deviceOrientation is only set if a valid
* videoConnection orientation is detected. videoConnection.videoOrientationare 
* only set if a valid videoConnection orientation is detected and 
* videoConnection is not nil. The following shows how UIDeviceOrientation 
* values are mapped to AVCaptureVideoOrientation values:
* 
*   UIDeviceOrientationLandscapeLeft -> AVCaptureVideoOrientationLandscapeRight
*   UIDeviceOrientationLandscapeRight -> AVCaptureVideoOrientationLandscapeLeft
*   UIDeviceOrientationUnknown -> AVCaptureVideoOrientationLandscapeRight
* 
* UIDeviceOrientation and AVCaptureVideoOrientation lanscape values are 
* switched. AVCaptureVideoOrientationLandscapeRight is set as the default value
* for the UIDeviceOrientationUnknown case.
*/
-(void)deviceOrientationDidChange;

/**
* Resizes the camera feed texture to the given width and height, with the 
* PF_B8G8R8A8 format. 
* 
* Releases the current texture and reallocates memory for the new texture
* dimensions. Run on the Unreal Engine GameThread.
*/
-(void)resizeCameraFeedTexture:(int)width textureHeight:(int)height;

/// The current on/off status of the camera.
@property (nonatomic, assign) BOOL cameraOn;

/// The url the QR Code Scanned in the last tick. Equals empty string if no QR
/// Code was scanned or the camera is off.
@property (nonatomic, copy) NSString* url;

/// The desired position of the camera.
@property (nonatomic, assign) AVCaptureDevicePosition cameraPosition;

/// The current capture session of the QR Code Reader. Equals nil if camera is
/// off.
@property (nonatomic, strong) AVCaptureSession *captureSession;

/// Represents the connection between the camera and video output. Used to
/// set the video orienation.
@property (nonatomic, strong) AVCaptureConnection* videoConnection;

/// The last enabled orientation of the device. If enabled, valid values include:
///  AVCaptureVideoOrientationPortrait
///  AVCaptureVideoOrientationPortraitUpsideDown
///  AVCaptureVideoOrientationLandscapeRight
///  AVCaptureVideoOrientationLandscapeLeft
@property (nonatomic, assign) AVCaptureVideoOrientation deviceOrientation;

/// The orientation of the camera. Valid values include:
///  AVCaptureVideoOrientationLandscapeRight
///  AVCaptureVideoOrientationLandscapeLeft
///  Use LandscapeLeft for Portrait and LandscapeRight for PortraitUpsideDown.
@property (nonatomic, assign) AVCaptureVideoOrientation cameraOrientation;

/// Represents the output for the video feed.
@property (nonatomic, strong) AVCaptureVideoDataOutput *videoOutput;

/// Represents the QR Code Reader output.
@property (nonatomic, strong) AVCaptureMetadataOutput *metadataOutput;

@end

#endif

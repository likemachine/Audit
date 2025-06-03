//
//  CameraEnumConverter.cpp
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 2/3/23.
//  Copyright © 2023 Matthew Zane All rights reserved.
//

#include "CameraEnumConverter.h"

FString UCameraEnumConverter::CameraPositionToFString(CameraPosition cameraPosition) {
    switch (cameraPosition) {
        case CameraPosition::FRONT:
            return FString("Front");
        case CameraPosition::BACK:
            return FString("Back");
        default:
            return FString("Unspecified");
    }
}

CameraPosition UCameraEnumConverter::FStringToCameraPosition(FString cameraPositionFString) {
    if (cameraPositionFString.Equals("Front")) {
        return CameraPosition::FRONT;
    }
    else if (cameraPositionFString.Equals("Back")) {
        return CameraPosition::BACK;
    }
    else {
        return CameraPosition::UNSPECIFIED;
    }
}

FString UCameraEnumConverter::CameraOrientationToFString(CameraOrientation cameraOrientation) {
    switch (cameraOrientation) {
        case CameraOrientation::LANDSCAPE_LEFT:
            return FString("Landscape Left");
        case CameraOrientation::LANDSCAPE_RIGHT:
            return FString("Landscape Right");
        case CameraOrientation::PORTRAIT:
            return FString("Portrait");
        case CameraOrientation::PORTRAIT_UPSIDE_DOWN:
            return FString("Portrait Upside Down");
        default:
            return FString("Unspecified");
    }
}
CameraOrientation UCameraEnumConverter::FStringToCameraOrientation(FString cameraOrientationFString) {
    if (cameraOrientationFString.Equals("Landscape Left")) {
        return CameraOrientation::LANDSCAPE_LEFT;
    }
    else if (cameraOrientationFString.Equals("Landscape Right")) {
        return CameraOrientation::LANDSCAPE_RIGHT;
    }
    else if (cameraOrientationFString.Equals("Portrait")) {
        return CameraOrientation::PORTRAIT;
    }
    else if (cameraOrientationFString.Equals("Portrait Upside Down")) {
        return CameraOrientation::PORTRAIT_UPSIDE_DOWN;
    }
    else {
        return CameraOrientation::UNSPECIFIED;
    }
}

#if PLATFORM_IOS
AVCaptureDevicePosition UCameraEnumConverter::CameraPositionToAVCaptureDevicePosition(
    CameraPosition cameraPosition
) {
    switch (cameraPosition) {
        case CameraPosition::FRONT:
            return AVCaptureDevicePositionFront;
        case CameraPosition::BACK:
            return AVCaptureDevicePositionBack;
        default:
            return AVCaptureDevicePositionUnspecified;
    }
}

CameraPosition UCameraEnumConverter::AVCaptureDevicePositionToCameraPosition(
    AVCaptureDevicePosition devicePosition
) {
    switch (devicePosition) {
        case AVCaptureDevicePositionFront:
            return CameraPosition::FRONT;
        case AVCaptureDevicePositionBack:
            return CameraPosition::BACK;
        default:
            return CameraPosition::UNSPECIFIED;
    }
}

AVCaptureVideoOrientation UCameraEnumConverter::CameraOrientationToAVCaptureVideoOrientation(
    CameraOrientation cameraOrientation
) {
    switch (cameraOrientation) {
        case CameraOrientation::LANDSCAPE_LEFT:
            return AVCaptureVideoOrientationLandscapeRight;
        case CameraOrientation::LANDSCAPE_RIGHT:
            return AVCaptureVideoOrientationLandscapeLeft;
        case CameraOrientation::PORTRAIT:
            return AVCaptureVideoOrientationPortrait;
        case CameraOrientation::PORTRAIT_UPSIDE_DOWN:
            return AVCaptureVideoOrientationPortraitUpsideDown;
        default:
            return AVCaptureVideoOrientationPortrait;
    }
}

CameraOrientation UCameraEnumConverter::AVCaptureVideoOrientationToCameraOrientation(
    AVCaptureVideoOrientation videoOrientation
) {
    switch (videoOrientation) {
        case AVCaptureVideoOrientationLandscapeLeft:
            return CameraOrientation::LANDSCAPE_RIGHT;
        case AVCaptureVideoOrientationLandscapeRight:
            return CameraOrientation::LANDSCAPE_LEFT;
        case AVCaptureVideoOrientationPortrait:
            return CameraOrientation::PORTRAIT;
        case AVCaptureVideoOrientationPortraitUpsideDown:
            return CameraOrientation::PORTRAIT_UPSIDE_DOWN;
        default:
            return CameraOrientation::LANDSCAPE_LEFT;
    }
}

#endif

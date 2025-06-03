//
//  CameraOrientation.h
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 2/3/23.
//  Copyright © 2023 Matthew Zane All rights reserved.
//

#pragma once

/**
* Represents a Blueprint exposed version of the AVCaptureDevicePosition type.
*/
UENUM(BlueprintType)
enum class CameraOrientation: uint8 {
    LANDSCAPE_LEFT,
    LANDSCAPE_RIGHT,
    PORTRAIT,
    PORTRAIT_UPSIDE_DOWN,
    UNSPECIFIED
};

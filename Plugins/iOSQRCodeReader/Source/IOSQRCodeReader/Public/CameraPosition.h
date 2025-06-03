//
//  CameraPosition.h
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 1/17/23.
//  Copyright © 2023 Matthew Zane All rights reserved.
//

#pragma once

/**
* Represents a Blueprint exposed version of the AVCaptureDevicePosition type.
*/
UENUM(BlueprintType)
enum class CameraPosition: uint8 {
    FRONT,
    BACK,
    UNSPECIFIED
};

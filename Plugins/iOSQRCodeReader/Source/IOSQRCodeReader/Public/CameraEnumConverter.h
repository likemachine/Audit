//
//  CameraEnumConverter.h
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 2/3/23.
//  Copyright © 2023 Matthew Zane All rights reserved.
//

#pragma once

#include "CameraOrientation.h"
#include "CameraPosition.h"

#if PLATFORM_IOS
#import <AVFoundation/AVFoundation.h>
#endif

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CameraEnumConverter.generated.h"

/**
 * Provide functions to convert the CameraPosition type into other Blueprint
 * types and Objective-C types.
 */
UCLASS()
class UCameraEnumConverter : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "IOS Camera Position")
    static FString CameraPositionToFString(CameraPosition cameraPosition);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "IOS Camera Position")
    static CameraPosition FStringToCameraPosition(FString cameraPositionFString);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "IOS Camera Orientation")
    static FString CameraOrientationToFString(CameraOrientation cameraOrientation);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "IOS Camera Orientation")
    static CameraOrientation FStringToCameraOrientation(FString cameraOrientationFString);
    
#if PLATFORM_IOS
    static AVCaptureDevicePosition CameraPositionToAVCaptureDevicePosition(
        CameraPosition cameraPosition
    );
    
    static CameraPosition AVCaptureDevicePositionToCameraPosition(
        AVCaptureDevicePosition devicePosition
    );
    
    static AVCaptureVideoOrientation CameraOrientationToAVCaptureVideoOrientation(
        CameraOrientation cameraOrientation
    );
    
    static CameraOrientation AVCaptureVideoOrientationToCameraOrientation(
        AVCaptureVideoOrientation videoOrientation
    );
#endif
};

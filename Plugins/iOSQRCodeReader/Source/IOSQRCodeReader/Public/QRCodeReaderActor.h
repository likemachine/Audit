//
//  QRCodeReaderActor.cpp
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 1/16/23.
//  Copyright © 2023 Matthew Zane. All rights reserved.
//

#pragma once
 
#include "CameraPosition.h"

#if PLATFORM_IOS
#include "QRCodeReader.h"
#include "QRCodeARSessionDelegate.h"
#endif

#include "Engine/Texture2D.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QRCodeReaderActor.generated.h"

/**
* The AQRCodeReaderActor is a C++ wrapper class that provides functions and
* variables which expose the output of the native iOS AVFoundation QRCode 
* to Unreal Engine.
*/
UCLASS()
class IOSQRCODEREADER_API AQRCodeReaderActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQRCodeReaderActor();
    
    // Texture of the camera feed
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "IOS QR Code Reader")
    UTexture2D* Texture;

protected:
#if PLATFORM_IOS
    // Objective-C QRCodeReader Object
    QRCodeReader* QRCodeReaderImpl;
    // ARSession delegate forwarding frames to the reader
    QRCodeARSessionDelegate* ARDelegate;
#endif
    
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    
private:
    // Default dimensions on the texture. These values should be overriden when
    // the camera is turned on.
    const int DEFAULT_TEXTURE_WIDTH = 1080;
    const int DEFAULT_TEXTURE_HEIGHT = 1920;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    
    /**
    * Initiaizes the QRCodeReaderActor's enabled flags.
    * 
    * Does not do anything if run on a non-iOS platform.
    * 
    * @param QRCodeReaderEnabled bool to set whether QR Code Reading
    *        functionality is enabled.
    * @param VideoEnabled bool to set whether Video functionality is enabled.
    * @param LandscapeLeftEnabled bool to set whether Landscape Left orientation
    *        is enabled.
    * @param LandscapeRightEnabled bool to set whether Landscape Right 
    *        orientation is enabled.
    * @param PortraitEnabled bool to set whether Portrait orientation is 
    *        enabled.
    * @param PortraitUpsideDownEnabled bool to set whether Portrait Upside Down
    *        orientation is enabled.
    * @param AutoCameraRotateEnabled bool to set whether Auto Camera Rotate 
    *        orientation is enabled.
    * @param InitialOrientation CameraOrientation to set the initial camera 
    *        orientation
    *        
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void Init(
        bool QRCodeReaderEnabled,
        bool VideoEnabled,
        bool LandscapeLeftEnabled,
        bool LandscapeRightEnabled,
        bool PortraitEnabled,
        bool PortraitUpsideDownEnabled,
        bool AutoCameraRotateEnabled,
        CameraOrientation InitialOrientation
    );
    
    /**
    * Returns whether QR Code Reading functionality is enabled.
    *
    * If true, the QR Codes will be scanned when the camera is on. If run on a
    * non-iOS platform, will always return false.
	*
	* @return bool representing whether QR Code Reading functionality is 
    *         enabled.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool GetQRCodeReaderEnabled();
    
    /**
    * Sets whether QR Code Reading functionality is enabled.
    *
    * The new status will not go into effect until the camera is turned on 
	* again. Does not do anything if run on a non-iOS platform.
    *
    * @param QRCodeReaderEnabled bool to set whether QR Code Reading
    *        functionality is enabled.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetQRCodeReaderEnabled(bool QRCodeReaderEnabled);
    
	/**
    * Returns whether Video functionality is enabled.
    *
    * If true, the texture will be updated to the video current video feed 
	* when the camera is turned on. If run on a non-iOS platform, will always
    * return false.
	*
	* @return bool representing whether Video functionality is enabled.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool GetVideoEnabled();

    /**
    * Sets whether Video functionality is enabled.
    *
    * The new status will not go into effect until the camera is turned on 
	* again. Does not do anything if run on a non-iOS platform.
    *
    * @param VideoEnabled bool to set whether Video functionality is enabled.
    */   
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetVideoEnabled(bool VideoEnabled);

    /**
    * Returns whether Landscape Left orientation for the Video texture is
    * enabled.
    *
    * If true, the Video Texture will automatically go into Landscape Left 
	* orientation when the device orientation changes to Landscape Left. 
    * Otherwise, the orientation will not change to Landscape Left. However,
    * SetCameraOrientation() can still set the Video Texture to Landscape Left.
    * If run on a non-iOS platform, will always return false.
	*
	* @return bool representing whether Landscape Left orientation for the Video
    *         texture is enabled.
    */  
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool GetLandscapeLeftEnabled();

    /**
    * Sets whether Portrait orientation for the Video texture is enabled.
    *
    * If set to true, the Video Texture will automatically go into Landscape 
    * Left orientation when the device orientation changes to Landscape Left. 
    * Otherwise, the orientation will not change to Landscape Left. However,
    * SetCameraOrientation() can still set the Video Texture to Landscape Left.
    * Does not do anything if run on a non-iOS platform.
    *
    * @param LandscapeLeftEnabled bool to set whether Landscape Left 
    *        orientation for the Video texture is enabled.
    */    
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetLandscapeLeftEnabled(bool LandscapeLeftEnabled);
    
	/**
    * Returns whether Landscape Right orientation for the Video texture is 
    * enabled.
    *
    * If true, the Video Texture will automatically go into Landscape Right 
    * orientation when the device orientation changes to Landscape Right. 
    * Otherwise, the orientation will not change to Landscape Right. However,
    * SetCameraOrientation() can still set the Video Texture to Landscape Right.
    * If run on a non-iOS platform, will always return false.
	*
	* @return bool representing whether Landscape Right orientation for 
    *         the Video texture is enabled.
    */
	UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool GetLandscapeRightEnabled();

    /**
    * Sets whether Landscape Right orientation for the Video texture is 
    * enabled.
    *
    * If set to true, the Video Texture will automatically go into Landscape 
    * Right orientation when the device orientation changes to Landscape Right. 
    * Otherwise, the orientation will not change to Landscape Right. However,
    * SetCameraOrientation() can still set the Video Texture to Landscape Right.
    * Does not do anything if run on a non-iOS platform.
    *
    * @param LandscapeRightEnabled bool to set whether Landscape Right 
    *        orientation for the Video texture is enabled.
    */    
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetLandscapeRightEnabled(bool LandscapeRightEnabled);
    
	/**
    * Returns whether Portrait orientation for the Video texture is enabled.
    *
    * If true, the Video Texture will automatically go into Portrait 
    * orientation when the device orientation changes to Portrait. Otherwise,
    * the orientation will not change to Portrait. However, 
    * SetCameraOrientation() can still set the Video Texture to Portrait. 
    * If run on a non-iOS platform, will always return false.
	*
	* @return bool representing whether Portrait orientation for the Video 
    *         texture is enabled.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool GetPortraitEnabled();

    /**
    * Sets whether Portrait orientation for the Video texture is enabled.
    *
    * If set to true, the Video Texture will automatically go into Portrait 
    * orientation when the device orientation changes to Portrait. Otherwise,
    * the orientation will not change to Portrait. However, 
    * SetCameraOrientation() can still set the Video Texture to Portrait. Does
    * not do anything if run on a non-iOS platform.
    *
    * @param PortraitEnabled bool to set whether Portrait orientation for the
	* Video texture is enabled.
    */    
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetPortraitEnabled(bool PortraitEnabled);
    
	/**
    * Returns whether Portrait Upside Down orientation for the Video texture 
	* is enabled.
    *
    * If true, the Video Texture will automatically go into Portrait Upside 
    * Down orientation when the device orientation changes to Portrait Upside
    * Down. Otherwise, the orientation will not change to Portrait Upside Down.
    * However, SetCameraOrientation() can still set the Video Texture to 
    * Portrait Upside Down. If run on a non-iOS platform, will always return 
    * false.
	*
	* @return bool representing whether Portrait Upside Down orientation for the
    *         Video texture is enabled.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool GetPortraitUpsideDownEnabled();

    /**
    * Sets whether Portrait Upside Down orientation for the Video texture is 
    * enabled.
    *
    * If set to true, the Video Texture will automatically go into Portrait 
    * Upside Down orientation when the device orientation changes to Landscape 
    * Right. Otherwise, the orientation will not change to Lanscape Right. 
    * However, SetCameraOrientation() can still set the Video Texture to 
    * Portrait Upside Down. Does not do anything if run on a non-iOS platform.
    *
    * @param PortraitUpsideDownEnabled bool to set whether Portrait Upside 
    *        Down orientation for the Video texture is enabled.
    */  
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetPortraitUpsideDownEnabled(bool PortraitUpsideDownEnabled);
    
    /**
    * Returns the string of the currently being scanned QR Code.
    *
    * This function will return an empty string if no QR Code is being scanned.
    * If run on a non-iOS platform, will always return an empty string.
	*
	* @return FString representing the scanned QR Code's Url.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    FString GetQRCodeString();
    
    /**
    * Returns whether the camera (front or back) is on. 
    * 
    * If run on a non-iOS platform, will always return false.
    *
    * @return bool representing whether camera is on.
    */    
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool IsCameraOn();
    
    /**
    * Sets whether camera is on.
    *
    * Starts or stops the camera, if the isCameraOn value is different from the
    * current state.
    *
    * @param isCameraOn bool to set whether camera is on.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetCameraIsOn(bool cameraIsOn);
    
    /**
    * Returns the camera's position .
    *
    * The position is represented by the CameraPosition enum. If run on a 
    * non-iOS platform, will always return UNSPECIFIED.
    *
    * @return CameraPosition representing the camera's position.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    CameraPosition GetCameraPosition();
    
    /**
    * Sets the camera position.
    *
    * Turns off the current camera before turning on the new camera, even if 
    * the camera did not actually change. Will not do anything if run on a
    * non-iOS platform.
    *    
    * @param cameraPosition CameraPosition to set the camera's position.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetCameraPosition(CameraPosition cameraPosition);
    
    /**
    * Returns the camera's orientation.
    *
    * The orientation is represented by the CameraOrientation enum. More
    * specifically references the QRCodeReader's Device Orientation value,
    * since portrait video orientation is rotated from a landscape orientation.
    * Returns the value UNSPECIFIED if run on a non-iOS platform.
    *
    * @return CameraOrientation representing the camera's orientation.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    CameraOrientation GetCameraOrientation();
    
    /**
    * Sets the camera orientation.
    *
    * Turns off the current camera before turning on the new camera, even if
    * the camera did not actually change. Does not do anything if run on a
    * non-iOS platform.
    *
    * @param cameraOrientation CameraOrientation to set the camera's 
    *        orientation.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetCameraOrientation(CameraOrientation cameraOrientation);
    
    /**
    * Returns whether the camera rotation is enabled.
    *
    * When this function returns true, the camera feed will automatically
    * rotate according to the device's orientation and the camera's enabled
    * orientations. Otherwise the camera must be rotated explicitly. If run on
    * a non-iOS platform, will always return false.
    *
    * @return bool representing whether the camera rotation is enabled.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    bool GetAutoCameraRotateEnabled();
    
    /**
    * Sets whether auto camera rotation is enabled.
    *
    * When set to true, the camera feed will automatically rotate according to
    * the device's orientation and the camera's enabled orientations.
    * Otherwise the camera must be rotated explicitly. If run on a non-iOS
    * platform, does nothing.
    *
    * @param cameraOrientation bool to set whether auto camera rotate is 
    *        enabled.
    */
    UFUNCTION(Blueprintcallable, Category = "IOS QR Code Reader")
    void SetAutoCameraRotateEnabled(bool AutoCameraRotateEnabled);

#if PLATFORM_IOS
    /**
    * Passes an ARKit frame to the QR code reader for processing.
    */
    void ProcessARFrame(void* ARFramePtr);
#endif
};

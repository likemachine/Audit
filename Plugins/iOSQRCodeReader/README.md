<a name="readme-top"></a>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader">
    <img src="Images/Thumbnail.png" width="300" height="300">
  </a>

  <h3 align="center">Unreal Engine iOS QR Code Reader Plugin</h3>

  <p align="center">
    An Unreal Engine plugin for QR Code Reading in iOS!
    <br />
    <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader">View Demo</a>
    ·
    <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader">Report Bug</a>
    ·
    <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li>
      <a href="#how-it-works">How It Works</a>
      <ul>
        <li><a href="#qrcodereader-objective-c">QRCodeReader (Objective-C)</a></li>
        <li><a href="#aqrcodereaderactor-c">AQRCodeReaderActor (C++)</a></li>
        <li><a href="#iosqrcodereaderbuildcs-c">IOSQRCodeReader.build.cs (C#)</a></li>
      </ul>
    </li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

<div align="center">
  <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/Images/Example.png">
    <img src="Images/Example.png" width="650" height="300">
  </a>
</div>

This project is an Unreal Engine 5 Plugin that utilizes the 
[AVFoundation](https://developer.apple.com/av-foundation/) framework to 
implement the native IOS QR Code Reading functionality in Unreal Engine, 
exposing them to Unreal Engine's C++ and Blueprints. It is possible to use 
Unreal Engine 4, but this repository has been tested only with Unreal Engine 5.

Please contact [matthewzane.unrealengine@gmail.com](matthewzane.unrealengine@gmail.com) for requests for additional functionality and for access to the [github repository](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With

This project is built with the following libraries. 

* [Unreal Engine 5](https://www.unrealengine.com/en-US/unreal-engine-5)
* [AVFoundation](https://developer.apple.com/av-foundation/)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these simple steps.

### Prerequisites

Please read the official Unreal Engine documentation for 
[iOS Game Development](https://docs.unrealengine.com/4.27/en-US/SharingAndReleasing/Mobile/iOS/)
to ensure you meet the prerequisites for developing for iOS Unreal Engine applications.

It is recommended to use a MacOS device to setup and build the project. It is possible to use a Win64 machine, but remote access to a MacOS machine will still be needed. An iOS device is required to build the project, otherwise most of the functionality will not work.


### Installation

_Below are instructions on how to setup the Unreal Engine IOS QR Code Reader plugin._

1. Create Unreal Engine 5 project with the following settings:
- Games -> Blank 
- C++
- Mobile
2. Ensure that there is a "Plugins" folder.
2. Copy the plugin into the "Plugins" folder. If you have access to the private repository and your project is also in a Git repository, the plugin can be added as a submodule.
```
git submodule add https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader.git Plugins/IOSQRCodeReader
```
4. In a Finder, right click the .uproject file and Generate XCode Project files. 
5. Open the project in XCode and build it.
6. Open the project in Unreal Engine 5.
7. Navigate to Platforms -> iOS -> Extra PList Data, in the "Additional Plist Data" field enter:
```
<key>NSCameraUsageDescription</key>\n<string>UE5 needs permission to use the camera in order to scan QR Codes.</string>\n
```
8. Now you are ready to utilize and develop with the plugin.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage
Note: There is a public repository which contains an 
[Example Unreal Engine Project](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReaderExampleProject/)
for this plugin. 


The plugin's functionality comes from the QRCodeReaderActor, which is a C++ class. Once the actor is spawned, the QR Code reading functionality can be accessed. Below is a simplified version of the QRCodeReaderActor's interface
```
// AQRCodeReaderActor.h

class AQRCodeReaderActor: public AActor 
{
    AQRCodeReaderActor();

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
    
    UFUNCTION(Blueprintcallable)
    bool GetQRCodeReaderEnabled();
    
    UFUNCTION(Blueprintcallable)
    void SetQRCodeReaderEnabled(bool QRCodeReaderEnabled);
    
    UFUNCTION(Blueprintcallable)
    bool GetVideoEnabled();
  
    UFUNCTION(Blueprintcallable)
    void SetVideoEnabled(bool VideoEnabled);

    UFUNCTION(Blueprintcallable)
    bool GetLandscapeLeftEnabled();
  
    UFUNCTION(Blueprintcallable)
    void SetLandscapeLeftEnabled(bool LandscapeLeftEnabled);
    
    UFUNCTION(Blueprintcallable)
    bool GetLandscapeRightEnabled();

    UFUNCTION(Blueprintcallable)
    void SetLandscapeRightEnabled(bool LandscapeRightEnabled);
    
    UFUNCTION(Blueprintcallable)
    bool GetPortraitEnabled();

    UFUNCTION(Blueprintcallable)
    void SetPortraitEnabled(bool PortraitEnabled);
    
    UFUNCTION(Blueprintcallable)
    bool GetPortraitUpsideDownEnabled();

    UFUNCTION(Blueprintcallable)
    void SetPortraitUpsideDownEnabled(bool PortraitUpsideDownEnabled);
    
    UFUNCTION(Blueprintcallable)
    FString GetQRCodeUrl();
     
    UFUNCTION(Blueprintcallable)
    bool IsCameraOn();
    
    UFUNCTION(Blueprintcallable)
    void SetCameraIsOn(bool cameraIsOn);
    
    UFUNCTION(Blueprintcallable)
    CameraPosition GetCameraPosition();
    
    UFUNCTION(Blueprintcallable)
    void SetCameraPosition(CameraPosition cameraPosition);
    
    UFUNCTION(Blueprintcallable)
    CameraOrientation GetCameraOrientation();
    
    UFUNCTION(Blueprintcallable)
    void SetCameraOrientation(CameraOrientation cameraOrientation);
    
    UFUNCTION(Blueprintcallable)
    bool GetAutoCameraRotateEnabled();
    
    UFUNCTION(Blueprintcallable)
    void SetAutoCameraRotateEnabled(bool AutoCameraRotateEnabled);
}
```

More detailed documentation is located in 
[QRCodeReaderActor.h](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/Source/Public/QRCodeReaderActor.h)

<div align="left">
  <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/Images/OrientationSettings.png">
    <img src="Images/WidgetVerticle.png" width="600" height="400">
  </a>
</div>

In the plugin's Content folder, there is an example map and widget to display the output of the QRCodeReaderActor. The widgets spawns a QRCodeReaderActor and displays the
following data:
- Scanned QR Code Url
- Camera On Status
- Camera Position (Front or Back)
- Camera Orientation (Left, Right, Portrait, or Portrait Upside Down)
- Camera Feed Texture

The BP_QRCodeReaderOutputWidget also includes three buttons widget toggle:
- Camera On Status
- Camera Position
- Camera Auto Rotate Enabled Status

The project Platform -> iOS -> Orientation settings are all checked. This may need to be modified to meet your specific needs.

<div align="left">
  <a href="https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/Images/OrientationSettings.png">
    <img src="Images/OrientationSettings.png" width="494" height="232">
  </a>
</div>

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- HOW IT WORKS -->
## How It Works

The plugin structure is comprised of Objective-C code, which enables the use of
the iOS framework, and Unreal Engine C++ code, which acts as a wrapper to the 
Objective-C code and exposes the functionality to Unreal Engine.

If you are not familiar with Objective-C or its syntax, please read this 
[tutorial](https://www.tutorialspoint.com/objective_c/index.html).

The [AVFoundation](https://developer.apple.com/av-foundation/) Camera and QR
Code functionality is accessed through the Objective-C QRCodeReader class. 

All of the Objective-C code is surrounded by
```
#if PLATFORM_IOS
...
#endif
```
blocks to ensure that the Objective-C code is only executed on iOS platforms.

The QR Code Reading Objective-C code is based on 
[this](https://www.appcoda.com/qr-code-ios-programming-tutorial/) tutorial. 
Much of the original code has been modified or stripped down into the
current version. The following is the definition for the Objective-C
QRCodeReader class along with explanations.

Additionally, much of the video output code has Unreal Engine C++ code included 
handle updating the Unreal Engine UTexture2D Texture value.

### QRCodeReader (Objective-C)

The QRCodeReader class is NSObject instead of a UIViewController object.

Here are the links to the 
[QRCodeReader.h](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/blob/main/Source/IOSQRCodeReader/Public/QRCodeReader.h)
and 
[QRCodeReader.cpp](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/blob/main/Source/IOSQRCodeReader/Private/QRCodeReader.cpp)
files which include comments.

QRCodeReader.cpp would usually have a .mm extension to signify the use of 
Objective-C++ code, which is simply a mix of Objective-C and C++ code. 
Since this plugin is compiled to the Engine, a .cpp extension is required.
However, compiling this plugin as a third party plugin, would allow you to use
the .mm extension, with no functionality change.

### AQRCodeReaderActor (C++)

The AQRCodeReaderActor parses QR Code images and provides a texture of the camera feed.

This class must be an actor. Making this class a UObject breaks the qr code
reading functions. The cause is possibly because of the relationship 
between an Actor's Tick() function and the QRCodeReader's captureOutput() 
function.

QRCodeReaderActor.cpp is given a .cpp extension to signify that it is the C++
wrapper for the Objective-C code, even though it also contains Objective-C 
code.

Here are the links to the 
[QRCodeReaderActor.h](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/blob/main/Source/IOSQRCodeReader/Public/QRCodeReaderActor.h)
add 
[QRCodeReaderActor.cpp](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/blob/main/Source/IOSQRCodeReader/Public/QRCodeReaderActor.h)
files which include comments.

### IOSQRCodeReader.build.cs (C#)

No modifications are necessary to the plugin's default build.cs file.

Here are the links to the
[IOSQRCodeReader.build.cs](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/blob/main/Source/IOSQRCodeReader/IOSQRCodeReader.Build.cs)
with comments.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ROADMAP -->
## Roadmap

See the [open issues](https://gitlab.nrp-nautilus.io/MatthewZane/CesiumNetBoxVisualization/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Please read the license [here](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/license.md).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

If you have any questions, issues, or requests, please contact me via my
LinkedIn or the email below.

Matthew Zane 
- [![LinkedIn][linkedin-shield]][linkedin-url]  [@matthewdzane](https://www.linkedin.com/in/matthewdzane/) 
- matthewzane.unrealengine@gmail.com

Plugin Project Link: [https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/)

Example Project Link: [https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReaderExampleProject/](https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReaderExampleProject/)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Thank you to [NSYNK {](https://nsynk.de/) for sponsoring this project!

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/MatthewDZane/UnrealEngineIOSQRCodeReaderPreview.svg?style=for-the-badge
[contributors-url]: https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/MatthewDZane/UnrealEngineIOSQRCodeReaderPreview.svg?style=for-the-badge
[forks-url]: https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/network/members
[stars-shield]: https://img.shields.io/github/stars/MatthewDZane/UnrealEngineIOSQRCodeReaderPreview.svg?style=for-the-badge
[stars-url]: https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/stargazers
[issues-shield]: https://img.shields.io/github/issues/MatthewDZane/UnrealEngineIOSQRCodeReaderPreview.svg?style=for-the-badge
[issues-url]: https://github.com/MatthewDZane/UnrealEngineIOSQRCodeReader/issues
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/matthewdzane

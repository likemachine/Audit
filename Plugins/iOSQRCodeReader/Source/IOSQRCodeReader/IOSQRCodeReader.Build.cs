//
//  IOSQRCodeReader.build.cs
//  IOSQRCodeReader
//
//  Created by Matthew Zane on 1/16/23.
//  Copyright © 2023 Matthew Zane. All rights reserved.
//

using System.IO;
using UnrealBuildTool;

public class IOSQRCodeReader : ModuleRules
{
	public IOSQRCodeReader(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
    
                PublicDependencyModuleNames.AddRange(
                    new string[] {
                        "Core",
                        "CoreUObject",
                        "Engine",
                        "InputCore",
                        "AugmentedReality",
                        "AppleARKit"
                    }
                );

                if (Target.Platform == UnrealTargetPlatform.IOS)
                {
                    PublicFrameworks.AddRange(new string[] { "ARKit", "Vision" });
                }
	}
}

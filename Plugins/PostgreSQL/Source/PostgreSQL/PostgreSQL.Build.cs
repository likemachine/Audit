// Copyright 2018-2023, Athian Games. All Rights Reserved. 

using UnrealBuildTool;
using System.IO;

public class PostgreSQL : ModuleRules
{

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/")); }
    }
    
  

    private void CopyToBinaries(string FilePath, ReadOnlyTargetRules Target)
    {
        string BinariesDir = Path.Combine(Target.ProjectFile!.Directory.FullName, "Binaries", Target.Platform.ToString());
        
        string Filename = Path.GetFileName(FilePath);
        string DestPath = Path.Combine(BinariesDir, Filename);

        if (!Directory.Exists(BinariesDir))
        {
            Directory.CreateDirectory(BinariesDir);
        }

        if (!File.Exists(DestPath))
        {
            File.Copy(FilePath, DestPath, true);
        }
        
        RuntimeDependencies.Add(DestPath);
        PublicDelayLoadDLLs.Add(Path.Combine(DestPath));

    }

    public PostgreSQL(ReadOnlyTargetRules Target) : base(Target)
    {

        PrivateIncludePaths.AddRange(new string[]
        {
            "PostgreSQL/Private"

        });

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableUndefinedIdentifierWarnings = false;
        bEnableExceptions = true;

        PublicDefinitions.Add("NTDDI_WIN7SP1");

        PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "RHI",
            "ImageWrapper", "RenderCore", "InputCore" , "Projects" });
        PrivateDependencyModuleNames.AddRange(new[] { "XmlParser", "Core", "ImageWrapper", "Engine" });

        var LibraryDirectory = Path.Combine(ThirdPartyPath, "libraries", "Windows");

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));
        
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            foreach (string FilePath in Directory.EnumerateFiles(LibraryDirectory, "*.lib", SearchOption.AllDirectories))
            {
                PublicAdditionalLibraries.Add(FilePath);
            }
        
            foreach (string FilePath in Directory.EnumerateFiles(LibraryDirectory, "*.dll", SearchOption.AllDirectories))
            {
                CopyToBinaries(FilePath, Target);
            }
        }
    }
}
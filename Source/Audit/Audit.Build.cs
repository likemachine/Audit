using UnrealBuildTool;
using System.IO;

public class Audit : ModuleRules
{
    public Audit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PublicIncludePaths.Add(ModuleDirectory);

        PublicDefinitions.Add("SQLITE_THREADSAFE=1");
        PublicDefinitions.Add("SQLITE_OMIT_LOAD_EXTENSION=1");
        if (Target.Platform == UnrealTargetPlatform.IOS || Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicSystemLibraries.Add("sqlite3");
        }
        RuntimeDependencies.Add("$(ProjectDir)/Content/Database/TestDB.db", StagedFileType.NonUFS);
    }
}

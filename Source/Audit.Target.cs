using UnrealBuildTool;
using System.Collections.Generic;

public class AuditTarget : TargetRules
{
    public AuditTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.AddRange(new string[] { "Audit" });
    }
}


using UnrealBuildTool;
using System.Collections.Generic;

public class AuditEditorTarget : TargetRules
{
    public AuditEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        ExtraModuleNames.AddRange(new string[] { "Audit" });
    }
}

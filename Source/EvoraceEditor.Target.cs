using UnrealBuildTool;

public class EvoraceEditorTarget : TargetRules
{
	public EvoraceEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Evorace");
		ExtraModuleNames.Add("EvoraceEditor");
	}
}

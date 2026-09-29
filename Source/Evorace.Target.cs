using UnrealBuildTool;

public class EvoraceTarget : TargetRules
{
	public EvoraceTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Evorace");
	}
}

// Liste des modules moteur dont notre code C++ a besoin.
using UnrealBuildTool;

public class Evorace : ModuleRules
{
	public Evorace(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "NetCore", "InputCore", "EnhancedInput", "GameplayTags", "AudioMixer" });
	}
}

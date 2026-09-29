// Module ÉDITEUR uniquement : n'est jamais inclus dans le jeu final.
// Il ajoute des outils MCP qui manquent pour construire les Blueprints (commentaires, infobulles).
using UnrealBuildTool;

public class EvoraceEditor : ModuleRules
{
	public EvoraceEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",
			"BlueprintGraph",
			"Kismet",
			"LevelEditor",
			"EnhancedInput",
			"InputCore",
			"AssetRegistry",
			"ToolsetRegistry",
		});
	}
}

// Enregistre nos outils éditeur auprès du serveur MCP au démarrage de l'éditeur.
#include "Modules/ModuleManager.h"
#include "ToolsetRegistry/UToolsetRegistry.h"
#include "VersusEditorToolset.h"

class FEvoraceEditorModule : public IModuleInterface
{
	virtual void StartupModule() override
	{
		UToolsetRegistry::RegisterToolsetClass(UVersusEditorToolset::StaticClass());
	}

	virtual void ShutdownModule() override
	{
		UToolsetRegistry::UnregisterToolsetClass(UVersusEditorToolset::StaticClass());
	}
};

IMPLEMENT_MODULE(FEvoraceEditorModule, EvoraceEditor)

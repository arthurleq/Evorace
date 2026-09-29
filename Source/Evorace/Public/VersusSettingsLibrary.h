// VersusSettingsLibrary : fonctions Blueprint pour le menu des réglages.
//
// Pourquoi en C++ : Unreal ne propose pas de nœud Blueprint pour le volume général,
// et la réassignation des touches (Enhanced Input "User Settings") est très verbeuse en Blueprint.
// Ces fonctions sont utilisées par WBP_PauseMenu et WBP_KeyRow.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InputCoreTypes.h"
#include "VersusSettingsLibrary.generated.h"

class APlayerController;
class UInputMappingContext;

UCLASS()
class EVORACE_API UVersusSettingsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ---- Son ---------------------------------------------------------------

	/** Règle le volume général (0 = muet, 1 = normal) et le sauvegarde dans GameUserSettings.ini. */
	UFUNCTION(BlueprintCallable, Category = "Versus|Reglages", meta = (WorldContext = "WorldContextObject"))
	static void SetMasterVolume(const UObject* WorldContextObject, float Volume);

	/** Volume général sauvegardé (1 par défaut). */
	UFUNCTION(BlueprintPure, Category = "Versus|Reglages")
	static float GetSavedMasterVolume();

	// ---- Touches -----------------------------------------------------------

	/**
	 * Prépare les touches d'un joueur local : enregistre les contextes auprès des réglages
	 * utilisateur (pour pouvoir les réassigner) et ajoute le contexte du jeu Versus.
	 */
	UFUNCTION(BlueprintCallable, Category = "Versus|Touches")
	static void InitPlayerInput(APlayerController* PlayerController, UInputMappingContext* VersusContext, UInputMappingContext* DefaultContext);

	/** Touche actuellement associée à une action (ex. "Versus_Jump"). */
	UFUNCTION(BlueprintCallable, Category = "Versus|Touches")
	static FKey GetMappedKey(APlayerController* PlayerController, FName MappingName);

	/** Associe une nouvelle touche à une action, applique et sauvegarde. Renvoie vrai si c'est accepté. */
	UFUNCTION(BlueprintCallable, Category = "Versus|Touches")
	static bool RemapKey(APlayerController* PlayerController, FName MappingName, FKey NewKey);

	/** Touche d'une colonne du menu Touches (Slot 0 = touche principale, 1 = touche secondaire). */
	UFUNCTION(BlueprintCallable, Category = "Versus|Touches")
	static FKey GetMappedKeyInSlot(APlayerController* PlayerController, FName MappingName, int32 Slot);

	/** Associe une touche à une action dans une colonne (0 = principale, 1 = secondaire), applique et sauvegarde. */
	UFUNCTION(BlueprintCallable, Category = "Versus|Touches")
	static bool RemapKeyInSlot(APlayerController* PlayerController, FName MappingName, FKey NewKey, int32 Slot);

	/** Supprime la touche d'une colonne (utile pour retirer la touche secondaire). */
	UFUNCTION(BlueprintCallable, Category = "Versus|Touches")
	static void ClearKeyInSlot(APlayerController* PlayerController, FName MappingName, int32 Slot);

	/** Remet toutes les touches par défaut. */
	UFUNCTION(BlueprintCallable, Category = "Versus|Touches")
	static void ResetAllKeys(APlayerController* PlayerController);
};

#include "VersusSettingsLibrary.h"

#include "AudioDevice.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "InputMappingContext.h"
#include "Misc/ConfigCacheIni.h"
#include "UserSettings/EnhancedInputUserSettings.h"

namespace
{
	const TCHAR* SettingsSection = TEXT("VersusSettings");

	UEnhancedInputLocalPlayerSubsystem* GetInputSubsystem(APlayerController* PlayerController)
	{
		ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
		return LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
	}

	UEnhancedInputUserSettings* GetUserSettings(APlayerController* PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(PlayerController);
		return Subsystem ? Subsystem->GetUserSettings() : nullptr;
	}
}

void UVersusSettingsLibrary::SetMasterVolume(const UObject* WorldContextObject, float Volume)
{
	Volume = FMath::Clamp(Volume, 0.f, 1.f);
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (World)
	{
		if (FAudioDeviceHandle AudioDevice = World->GetAudioDevice())
		{
			AudioDevice->SetTransientPrimaryVolume(Volume);
		}
	}
	GConfig->SetFloat(SettingsSection, TEXT("MasterVolume"), Volume, GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

float UVersusSettingsLibrary::GetSavedMasterVolume()
{
	float Volume = 1.f;
	GConfig->GetFloat(SettingsSection, TEXT("MasterVolume"), Volume, GGameUserSettingsIni);
	return FMath::Clamp(Volume, 0.f, 1.f);
}

void UVersusSettingsLibrary::InitPlayerInput(APlayerController* PlayerController, UInputMappingContext* VersusContext, UInputMappingContext* DefaultContext)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetInputSubsystem(PlayerController);
	if (!Subsystem)
	{
		return;
	}

	// Les contextes doivent être "enregistrés" pour que leurs touches soient réassignables.
	if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
	{
		if (DefaultContext)
		{
			Settings->RegisterInputMappingContext(DefaultContext);
		}
		if (VersusContext)
		{
			Settings->RegisterInputMappingContext(VersusContext);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VersusSettingsLibrary: Enhanced Input User Settings désactivés (bEnableUserSettings)."));
	}

	FModifyContextOptions Options;
	Options.bNotifyUserSettings = true;
	if (VersusContext && !Subsystem->HasMappingContext(VersusContext))
	{
		Subsystem->AddMappingContext(VersusContext, 1, Options);
	}
	Subsystem->RequestRebuildControlMappings(Options);
}

FKey UVersusSettingsLibrary::GetMappedKey(APlayerController* PlayerController, FName MappingName)
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings(PlayerController))
	{
		if (const FPlayerKeyMapping* Mapping = Settings->FindCurrentMappingForSlot(MappingName, EPlayerMappableKeySlot::First))
		{
			return Mapping->GetCurrentKey();
		}
	}
	return EKeys::Invalid;
}

bool UVersusSettingsLibrary::RemapKey(APlayerController* PlayerController, FName MappingName, FKey NewKey)
{
	UEnhancedInputUserSettings* Settings = GetUserSettings(PlayerController);
	if (!Settings || !NewKey.IsValid())
	{
		return false;
	}

	FMapPlayerKeyArgs Args;
	Args.MappingName = MappingName;
	Args.Slot = EPlayerMappableKeySlot::First;
	Args.NewKey = NewKey;

	FGameplayTagContainer FailureReason;
	Settings->MapPlayerKey(Args, FailureReason);
	Settings->ApplySettings();
	Settings->SaveSettings();
	return FailureReason.IsEmpty();
}

namespace
{
	EPlayerMappableKeySlot ToKeySlot(int32 Slot)
	{
		return Slot <= 0 ? EPlayerMappableKeySlot::First : EPlayerMappableKeySlot::Second;
	}
}

FKey UVersusSettingsLibrary::GetMappedKeyInSlot(APlayerController* PlayerController, FName MappingName, int32 Slot)
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings(PlayerController))
	{
		if (const FPlayerKeyMapping* Mapping = Settings->FindCurrentMappingForSlot(MappingName, ToKeySlot(Slot)))
		{
			return Mapping->GetCurrentKey();
		}
	}
	return EKeys::Invalid;
}

bool UVersusSettingsLibrary::RemapKeyInSlot(APlayerController* PlayerController, FName MappingName, FKey NewKey, int32 Slot)
{
	UEnhancedInputUserSettings* Settings = GetUserSettings(PlayerController);
	if (!Settings || !NewKey.IsValid())
	{
		return false;
	}

	FMapPlayerKeyArgs Args;
	Args.MappingName = MappingName;
	Args.Slot = ToKeySlot(Slot);
	Args.NewKey = NewKey;
	// La colonne secondaire n'existe pas encore tant qu'aucune touche n'y a été mise : on la crée.
	Args.bCreateMatchingSlotIfNeeded = true;

	FGameplayTagContainer FailureReason;
	Settings->MapPlayerKey(Args, FailureReason);
	Settings->ApplySettings();
	Settings->SaveSettings();
	return FailureReason.IsEmpty();
}

void UVersusSettingsLibrary::ClearKeyInSlot(APlayerController* PlayerController, FName MappingName, int32 Slot)
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings(PlayerController))
	{
		FMapPlayerKeyArgs Args;
		Args.MappingName = MappingName;
		Args.Slot = ToKeySlot(Slot);

		FGameplayTagContainer FailureReason;
		Settings->UnMapPlayerKey(Args, FailureReason);
		Settings->ApplySettings();
		Settings->SaveSettings();
	}
}

void UVersusSettingsLibrary::ResetAllKeys(APlayerController* PlayerController)
{
	if (UEnhancedInputUserSettings* Settings = GetUserSettings(PlayerController))
	{
		FGameplayTagContainer FailureReason;
		Settings->ResetKeyProfileIdToDefault(Settings->GetActiveKeyProfileId(), FailureReason);
		Settings->ApplySettings();
		Settings->SaveSettings();
	}
}

// VersusNetBridge : la "boîte aux lettres" réseau du jeu.
//
// Pourquoi ce fichier existe :
//   Les outils d'automatisation Blueprint ne savent pas cocher "Run on Server" /
//   "Multicast" sur un événement. Ce composant fournit donc ces trois canaux
//   réseau, une fois pour toutes, et TOUTE la logique du jeu reste en Blueprint.
//
// Comment l'utiliser en Blueprint :
//   1. Ajouter le composant "Versus Net Bridge" sur un acteur (PlayerController ou Character).
//   2. Envoyer   : appeler SendToServer / SendToOwningClient / SendToAll.
//   3. Recevoir  : dans le Blueprint, cliquer sur le composant puis "+" sur
//                  On Server Command / On Client Command / On Multicast Command.
//
// Chaque message = un nom de commande (ex. "BuyItem") + des valeurs libres :
//   IntValue    : un entier (index d'objet, d'évolution...)
//   VectorValue : une position (placement d'objet, direction de dash...)
//   RotatorValue: une rotation (orientation d'un objet placé...)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VersusNetBridge.generated.h"

// Signature commune des trois événements reçus côté Blueprint.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FVersusCommandSignature,
	FName, Command,
	int32, IntValue,
	FVector, VectorValue,
	FRotator, RotatorValue);

UCLASS(ClassGroup = (Versus), meta = (BlueprintSpawnableComponent, DisplayName = "Versus Net Bridge"))
class EVORACE_API UVersusNetBridge : public UActorComponent
{
	GENERATED_BODY()

public:
	UVersusNetBridge();

	// ---- Client -> Serveur ------------------------------------------------
	// Un joueur demande quelque chose au serveur ("je choisis le dash", "j'achète l'objet 2"...).
	// Ne fonctionne que sur un acteur possédé par ce joueur (son PlayerController ou son Character).
	UFUNCTION(BlueprintCallable, Category = "Versus|Reseau")
	void SendToServer(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue);

	// Déclenché SUR LE SERVEUR quand un client a appelé SendToServer.
	UPROPERTY(BlueprintAssignable, Category = "Versus|Reseau")
	FVersusCommandSignature OnServerCommand;

	// ---- Serveur -> le joueur propriétaire ---------------------------------
	// Le serveur parle à UN joueur précis (ex. "ouvre ton écran d'enchère").
	UFUNCTION(BlueprintCallable, Category = "Versus|Reseau")
	void SendToOwningClient(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue);

	// Déclenché CHEZ LE JOUEUR propriétaire de l'acteur.
	UPROPERTY(BlueprintAssignable, Category = "Versus|Reseau")
	FVersusCommandSignature OnClientCommand;

	// ---- Serveur -> tout le monde -----------------------------------------
	// Le serveur prévient toutes les machines (ex. effet visuel, son, annonce).
	UFUNCTION(BlueprintCallable, Category = "Versus|Reseau")
	void SendToAll(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue);

	// Déclenché SUR CHAQUE MACHINE (serveur + tous les clients).
	UPROPERTY(BlueprintAssignable, Category = "Versus|Reseau")
	FVersusCommandSignature OnMulticastCommand;

private:
	// Les vrais RPC réseau (invisibles en Blueprint, utilisés par les fonctions ci-dessus).
	UFUNCTION(Server, Reliable)
	void ServerReceive(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue);

	UFUNCTION(Client, Reliable)
	void ClientReceive(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReceive(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue);
};

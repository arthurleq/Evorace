// Implémentation de la boîte aux lettres réseau (voir VersusNetBridge.h pour le mode d'emploi).

#include "VersusNetBridge.h"

UVersusNetBridge::UVersusNetBridge()
{
	// Le composant doit être répliqué, sinon les RPC ne passent pas par le réseau.
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UVersusNetBridge::SendToServer(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue)
{
	// Appelé chez un client : part vers le serveur.
	// Appelé sur le serveur (joueur hôte) : s'exécute directement sur place.
	ServerReceive(Command, IntValue, VectorValue, RotatorValue);
}

void UVersusNetBridge::ServerReceive_Implementation(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue)
{
	OnServerCommand.Broadcast(Command, IntValue, VectorValue, RotatorValue);
}

void UVersusNetBridge::SendToOwningClient(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("VersusNetBridge: SendToOwningClient(%s) doit être appelé sur le serveur."), *Command.ToString());
		return;
	}
	ClientReceive(Command, IntValue, VectorValue, RotatorValue);
}

void UVersusNetBridge::ClientReceive_Implementation(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue)
{
	OnClientCommand.Broadcast(Command, IntValue, VectorValue, RotatorValue);
}

void UVersusNetBridge::SendToAll(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("VersusNetBridge: SendToAll(%s) doit être appelé sur le serveur."), *Command.ToString());
		return;
	}
	MulticastReceive(Command, IntValue, VectorValue, RotatorValue);
}

void UVersusNetBridge::MulticastReceive_Implementation(FName Command, int32 IntValue, FVector VectorValue, FRotator RotatorValue)
{
	OnMulticastCommand.Broadcast(Command, IntValue, VectorValue, RotatorValue);
}

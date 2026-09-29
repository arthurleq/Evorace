#include "VersusEditorToolset.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphNode_Comment.h"
#include "Engine/Blueprint.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_Knot.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Editor.h"
#include "LevelEditorSubsystem.h"
#include "ScopedTransaction.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "PlayerMappableKeySettings.h"
#include "UObject/Package.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VersusEditorToolset)

namespace
{
	// Les nœuds Blueprint n'ont pas de taille enregistrée (seul l'affichage la connaît) :
	// on l'estime à partir du titre et des pins pour pouvoir dessiner un cadre autour.
	FVector2D EstimateNodeSize(const UEdGraphNode* Node)
	{
		if (Node->NodeWidth > 0 && Node->NodeHeight > 0)
		{
			return FVector2D(Node->NodeWidth, Node->NodeHeight);
		}
		if (Node->IsA<UK2Node_Knot>())
		{
			return FVector2D(40, 24);
		}

		TArray<FString> TitleLines;
		Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString().ParseIntoArrayLines(TitleLines);
		int32 TitleLen = 0;
		for (const FString& Line : TitleLines)
		{
			TitleLen = FMath::Max(TitleLen, Line.Len());
		}

		int32 NumIn = 0, NumOut = 0, MaxIn = 0, MaxOut = 0;
		for (const UEdGraphPin* Pin : Node->Pins)
		{
			if (!Pin || Pin->bHidden)
			{
				continue;
			}
			int32 Len = Node->GetPinDisplayName(Pin).ToString().Len();
			if (Pin->Direction == EGPD_Input)
			{
				// Les valeurs saisies directement sur une pin prennent de la place.
				if (Pin->LinkedTo.Num() == 0)
				{
					Len += FMath::Min(Pin->GetDefaultAsString().Len(), 20) + 4;
				}
				++NumIn;
				MaxIn = FMath::Max(MaxIn, Len);
			}
			else
			{
				++NumOut;
				MaxOut = FMath::Max(MaxOut, Len);
			}
		}

		const float Width = FMath::Max3(120.f, TitleLen * 7.f + 70.f, (MaxIn + MaxOut) * 7.f + 90.f);
		const float Height = 36.f + 26.f * FMath::Max(NumIn, NumOut) + 12.f;
		return FVector2D(Width, Height);
	}

	void MarkModified(UEdGraph* Graph)
	{
		if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph))
		{
			FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
		}
	}
}

UEdGraphNode* UVersusEditorToolset::AddComment(UEdGraph* Graph, const FString& Text, int32 X, int32 Y, int32 Width, int32 Height, FLinearColor Color, int32 FontSize)
{
	if (!Graph)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("AddComment: Graph is null."));
		return nullptr;
	}

	const FScopedTransaction Transaction(NSLOCTEXT("VersusEditorToolset", "AddComment", "Add Comment"));
	Graph->Modify();

	UEdGraphNode_Comment* Comment = NewObject<UEdGraphNode_Comment>(Graph, NAME_None, RF_Transactional);
	Comment->CreateNewGuid();
	Comment->PostPlacedNewNode();
	Comment->NodePosX = X;
	Comment->NodePosY = Y;
	Comment->NodeWidth = FMath::Max(Width, 100);
	Comment->NodeHeight = FMath::Max(Height, 60);
	Comment->NodeComment = Text;
	Comment->CommentColor = Color;
	Comment->FontSize = FMath::Clamp(FontSize, 8, 72);
	Graph->AddNode(Comment, /*bFromUI*/ false, /*bSelectNewNode*/ false);

	MarkModified(Graph);
	return Comment;
}

UEdGraphNode* UVersusEditorToolset::CommentNodes(const TArray<UEdGraphNode*>& Nodes, const FString& Text, FLinearColor Color, int32 FontSize)
{
	UEdGraph* Graph = nullptr;
	FBox2D Bounds(ForceInit);
	for (const UEdGraphNode* Node : Nodes)
	{
		if (!Node || Node->IsA<UEdGraphNode_Comment>())
		{
			continue;
		}
		Graph = Graph ? Graph : Node->GetGraph();
		const FVector2D Pos(Node->NodePosX, Node->NodePosY);
		Bounds += Pos;
		Bounds += Pos + EstimateNodeSize(Node);
	}

	if (!Graph || !Bounds.bIsValid)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("CommentNodes: no valid node given."));
		return nullptr;
	}

	// Marge autour des nœuds + place en haut pour le texte du commentaire.
	const int32 Margin = 40;
	const int32 TitleSpace = FMath::Max(FontSize, 12) * 2 + 24;
	return AddComment(Graph, Text,
		FMath::RoundToInt(Bounds.Min.X) - Margin,
		FMath::RoundToInt(Bounds.Min.Y) - Margin - TitleSpace,
		FMath::RoundToInt(Bounds.GetSize().X) + Margin * 2,
		FMath::RoundToInt(Bounds.GetSize().Y) + Margin * 2 + TitleSpace,
		Color, FontSize);
}

UEdGraphNode* UVersusEditorToolset::CommentConnected(UEdGraphNode* StartNode, const FString& Text, FLinearColor Color, int32 FontSize)
{
	if (!StartNode)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("CommentConnected: StartNode is null."));
		return nullptr;
	}

	// Parcours en largeur de tous les nœuds reliés (exécution et données).
	TSet<UEdGraphNode*> Visited;
	TArray<UEdGraphNode*> Queue;
	Queue.Add(StartNode);
	Visited.Add(StartNode);
	while (Queue.Num() > 0)
	{
		UEdGraphNode* Node = Queue.Pop(EAllowShrinking::No);
		for (UEdGraphPin* Pin : Node->Pins)
		{
			if (!Pin)
			{
				continue;
			}
			for (UEdGraphPin* Linked : Pin->LinkedTo)
			{
				UEdGraphNode* Other = Linked ? Linked->GetOwningNode() : nullptr;
				if (Other && !Visited.Contains(Other))
				{
					Visited.Add(Other);
					Queue.Add(Other);
				}
			}
		}
	}

	return CommentNodes(Visited.Array(), Text, Color, FontSize);
}

int32 UVersusEditorToolset::RemoveAllComments(UEdGraph* Graph)
{
	if (!Graph)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("RemoveAllComments: Graph is null."));
		return 0;
	}

	const FScopedTransaction Transaction(NSLOCTEXT("VersusEditorToolset", "RemoveComments", "Remove Comments"));
	Graph->Modify();

	TArray<UEdGraphNode*> ToRemove;
	for (UEdGraphNode* Node : Graph->Nodes)
	{
		if (Node && Node->IsA<UEdGraphNode_Comment>())
		{
			ToRemove.Add(Node);
		}
	}
	for (UEdGraphNode* Node : ToRemove)
	{
		Node->Modify();
		Graph->RemoveNode(Node);
	}

	MarkModified(Graph);
	return ToRemove.Num();
}

void UVersusEditorToolset::SetNodeBubble(UEdGraphNode* Node, const FString& Text)
{
	if (!Node)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("SetNodeBubble: Node is null."));
		return;
	}

	const FScopedTransaction Transaction(NSLOCTEXT("VersusEditorToolset", "SetBubble", "Set Node Comment"));
	Node->Modify();
	Node->NodeComment = Text;
	Node->bCommentBubbleVisible = !Text.IsEmpty();
	Node->bCommentBubblePinned = !Text.IsEmpty();
	MarkModified(Node->GetGraph());
}

void UVersusEditorToolset::SetVariableTooltip(UBlueprint* Blueprint, const FString& VariableName, const FString& Tooltip)
{
	if (!Blueprint)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("SetVariableTooltip: Blueprint is null."));
		return;
	}
	if (FBlueprintEditorUtils::FindNewVariableIndex(Blueprint, FName(*VariableName)) == INDEX_NONE)
	{
		UKismetSystemLibrary::RaiseScriptError(FString::Printf(TEXT("SetVariableTooltip: variable '%s' not found."), *VariableName));
		return;
	}
	FBlueprintEditorUtils::SetBlueprintVariableMetaData(Blueprint, FName(*VariableName), nullptr, FBlueprintMetadata::MD_Tooltip, Tooltip);
}

void UVersusEditorToolset::SetFunctionTooltip(UEdGraph* FunctionGraph, const FString& Tooltip)
{
	if (!FunctionGraph)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("SetFunctionTooltip: FunctionGraph is null."));
		return;
	}
	for (UEdGraphNode* Node : FunctionGraph->Nodes)
	{
		if (UK2Node_FunctionEntry* Entry = Cast<UK2Node_FunctionEntry>(Node))
		{
			Entry->Modify();
			Entry->MetaData.ToolTip = FText::FromString(Tooltip);
			MarkModified(FunctionGraph);
			return;
		}
	}
	UKismetSystemLibrary::RaiseScriptError(TEXT("SetFunctionTooltip: no function entry node in this graph."));
}

bool UVersusEditorToolset::CreateLevel(const FString& AssetPath)
{
	ULevelEditorSubsystem* LevelEditor = GEditor ? GEditor->GetEditorSubsystem<ULevelEditorSubsystem>() : nullptr;
	if (!LevelEditor)
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("CreateLevel: LevelEditorSubsystem unavailable."));
		return false;
	}
	return LevelEditor->NewLevel(AssetPath, /*bIsPartitionedWorld*/ false);
}

void UVersusEditorToolset::ConfigurePIE(int32 NumPlayers, bool bListenServer, bool bNewWindows)
{
	ULevelEditorPlaySettings* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
	Settings->SetPlayNumberOfClients(FMath::Clamp(NumPlayers, 1, 8));
	Settings->SetPlayNetMode(bListenServer ? EPlayNetMode::PIE_ListenServer : EPlayNetMode::PIE_Standalone);
	Settings->SetRunUnderOneProcess(true);
	Settings->bLaunchSeparateServer = false;
	Settings->LastExecutedPlayModeType = bNewWindows ? EPlayModeType::PlayMode_InEditorFloating : EPlayModeType::PlayMode_InViewPort;
	Settings->PostEditChange();
	Settings->SaveConfig();
}

bool UVersusEditorToolset::ExecConsoleCommand(const FString& Command)
{
	if (!GEditor)
	{
		return false;
	}
	UWorld* World = GEditor->PlayWorld ? GEditor->PlayWorld.Get() : GEditor->GetEditorWorldContext().World();
	return GEditor->Exec(World, *Command);
}

namespace
{
	// Certains champs d'Enhanced Input sont "protected" : on les règle par réflexion.
	UPlayerMappableKeySettings* MakeKeySettings(UObject* Outer, const FString& MappingName, const FString& DisplayName, const FString& DisplayCategory)
	{
		UPlayerMappableKeySettings* Settings = NewObject<UPlayerMappableKeySettings>(Outer, NAME_None, RF_Transactional);
		Settings->Name = FName(*MappingName);
		Settings->DisplayName = FText::FromString(DisplayName);
		Settings->DisplayCategory = FText::FromString(DisplayCategory);
		return Settings;
	}

	void SetObjectField(UStruct* Type, void* Container, const TCHAR* FieldName, UObject* Value)
	{
		if (FObjectPropertyBase* Prop = FindFProperty<FObjectPropertyBase>(Type, FieldName))
		{
			Prop->SetObjectPropertyValue_InContainer(Container, Value);
		}
	}

	void SetSettingBehavior(FEnhancedActionKeyMapping& Mapping, EPlayerMappableKeySettingBehaviors Behavior)
	{
		if (FProperty* Prop = FEnhancedActionKeyMapping::StaticStruct()->FindPropertyByName(TEXT("SettingBehavior")))
		{
			*Prop->ContainerPtrToValuePtr<EPlayerMappableKeySettingBehaviors>(&Mapping) = Behavior;
		}
	}

	UPackage* MakeAssetPackage(const FString& FolderPath, const FString& AssetName)
	{
		const FString PackageName = FolderPath / AssetName;
		UPackage* Package = CreatePackage(*PackageName);
		Package->FullyLoad();
		return Package;
	}
}

UObject* UVersusEditorToolset::CreateInputAction(const FString& FolderPath, const FString& AssetName, const FString& MappingName, const FString& DisplayName, const FString& DisplayCategory)
{
	UPackage* Package = MakeAssetPackage(FolderPath, AssetName);
	UInputAction* Action = NewObject<UInputAction>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
	Action->ValueType = EInputActionValueType::Boolean;
	if (!MappingName.IsEmpty())
	{
		SetObjectField(UInputAction::StaticClass(), Action, TEXT("PlayerMappableKeySettings"), MakeKeySettings(Action, MappingName, DisplayName, DisplayCategory));
	}
	FAssetRegistryModule::AssetCreated(Action);
	Package->MarkPackageDirty();
	return Action;
}

UObject* UVersusEditorToolset::CreateMappingContext(const FString& FolderPath, const FString& AssetName)
{
	UPackage* Package = MakeAssetPackage(FolderPath, AssetName);
	UInputMappingContext* Context = NewObject<UInputMappingContext>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
	FAssetRegistryModule::AssetCreated(Context);
	Package->MarkPackageDirty();
	return Context;
}

int32 UVersusEditorToolset::AddKeyMapping(UObject* MappingContext, UObject* Action, const FString& KeyName, bool bPlayerMappable)
{
	UInputMappingContext* Context = Cast<UInputMappingContext>(MappingContext);
	UInputAction* InputAction = Cast<UInputAction>(Action);
	const FKey Key(*KeyName);
	if (!Context || !InputAction || !Key.IsValid())
	{
		UKismetSystemLibrary::RaiseScriptError(FString::Printf(TEXT("AddKeyMapping: invalid context, action or key '%s'."), *KeyName));
		return -1;
	}
	Context->Modify();
	FEnhancedActionKeyMapping& Mapping = Context->MapKey(InputAction, Key);
	if (!bPlayerMappable)
	{
		SetSettingBehavior(Mapping, EPlayerMappableKeySettingBehaviors::IgnoreSettings);
	}
	Context->MarkPackageDirty();
	return Context->GetMappings().Num() - 1;
}

bool UVersusEditorToolset::SetMappingPlayerSettings(UObject* MappingContext, int32 MappingIndex, const FString& MappingName, const FString& DisplayName, const FString& DisplayCategory)
{
	UInputMappingContext* Context = Cast<UInputMappingContext>(MappingContext);
	if (!Context || !Context->GetMappings().IsValidIndex(MappingIndex))
	{
		UKismetSystemLibrary::RaiseScriptError(TEXT("SetMappingPlayerSettings: invalid context or index."));
		return false;
	}
	Context->Modify();
	FEnhancedActionKeyMapping& Mapping = Context->GetMapping(MappingIndex);
	SetSettingBehavior(Mapping, EPlayerMappableKeySettingBehaviors::OverrideSettings);
	SetObjectField(FEnhancedActionKeyMapping::StaticStruct(), &Mapping, TEXT("PlayerMappableKeySettings"), MakeKeySettings(Context, MappingName, DisplayName, DisplayCategory));
	Context->MarkPackageDirty();
	return true;
}

TArray<FString> UVersusEditorToolset::DescribeMappings(UObject* MappingContext)
{
	TArray<FString> Lines;
	if (UInputMappingContext* Context = Cast<UInputMappingContext>(MappingContext))
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = Context->GetMappings();
		for (int32 Index = 0; Index < Mappings.Num(); ++Index)
		{
			const FEnhancedActionKeyMapping& Mapping = Mappings[Index];
			Lines.Add(FString::Printf(TEXT("%d | %s | %s | %s"), Index,
				Mapping.Action ? *Mapping.Action->GetName() : TEXT("None"),
				*Mapping.Key.ToString(),
				Mapping.IsPlayerMappable() ? *Mapping.GetMappingName().ToString() : TEXT("-")));
		}
	}
	return Lines;
}

TArray<FString> UVersusEditorToolset::DescribeNodes(UEdGraph* Graph)
{
	TArray<FString> Lines;
	if (!Graph)
	{
		return Lines;
	}
	for (const UEdGraphNode* Node : Graph->Nodes)
	{
		if (!Node)
		{
			continue;
		}
		const FVector2D Size = EstimateNodeSize(Node);
		Lines.Add(FString::Printf(TEXT("%s | %s | %s | %d,%d | %.0fx%.0f"),
			*Node->GetName(), *Node->GetClass()->GetName(),
			*Node->GetNodeTitle(ENodeTitleType::ListView).ToString(),
			Node->NodePosX, Node->NodePosY, Size.X, Size.Y));
	}
	return Lines;
}

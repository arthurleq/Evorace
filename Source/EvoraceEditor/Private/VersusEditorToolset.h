// Outils MCP maison (éditeur uniquement) pour documenter les Blueprints :
// blocs de commentaires, bulles de commentaire et infobulles.

#pragma once

#include "CoreMinimal.h"
#include "ToolsetRegistry/ToolsetDefinition.h"
#include "VersusEditorToolset.generated.h"

class UEdGraph;
class UEdGraphNode;
class UBlueprint;

/// Tools to document Blueprint graphs: comment boxes around nodes, comment bubbles and tooltips.
UCLASS(BlueprintType)
class UVersusEditorToolset : public UToolsetDefinition
{
	GENERATED_BODY()

public:
	/**
	 * Adds a comment box at an explicit position and size.
	 * @param Graph The graph to add the comment to.
	 * @param Text The comment text (shown in the title bar of the box).
	 * @param X Left position of the box in graph units.
	 * @param Y Top position of the box in graph units.
	 * @param Width Width of the box.
	 * @param Height Height of the box.
	 * @param Color Background color of the box (alpha is respected).
	 * @param FontSize Font size of the comment text (default in Unreal is 18).
	 * @return The created comment node.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static UEdGraphNode* AddComment(UEdGraph* Graph, const FString& Text, int32 X, int32 Y, int32 Width, int32 Height, FLinearColor Color, int32 FontSize);

	/**
	 * Adds a comment box that surrounds the given nodes (their size is estimated from titles and pins).
	 * @param Nodes The nodes to surround. They must all belong to the same graph.
	 * @param Text The comment text.
	 * @param Color Background color of the box.
	 * @param FontSize Font size of the comment text.
	 * @return The created comment node.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static UEdGraphNode* CommentNodes(const TArray<UEdGraphNode*>& Nodes, const FString& Text, FLinearColor Color, int32 FontSize);

	/**
	 * Adds a comment box around a node and every node connected to it (exec and data links, recursively).
	 * Typical use: pass an event or function entry node to frame its whole chain.
	 * @param StartNode The node to start from (e.g. an event node).
	 * @param Text The comment text.
	 * @param Color Background color of the box.
	 * @param FontSize Font size of the comment text.
	 * @return The created comment node.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static UEdGraphNode* CommentConnected(UEdGraphNode* StartNode, const FString& Text, FLinearColor Color, int32 FontSize);

	/**
	 * Removes every comment box from a graph (useful before re-commenting a rewritten graph).
	 * @param Graph The graph to clean.
	 * @return The number of comment boxes removed.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static int32 RemoveAllComments(UEdGraph* Graph);

	/**
	 * Shows a pinned comment bubble above a single node.
	 * @param Node The node to annotate.
	 * @param Text The bubble text. Empty text hides the bubble.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static void SetNodeBubble(UEdGraphNode* Node, const FString& Text);

	/**
	 * Sets the tooltip (description) of a Blueprint member variable.
	 * @param Blueprint The Blueprint owning the variable.
	 * @param VariableName The variable name.
	 * @param Tooltip The description shown when hovering the variable.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static void SetVariableTooltip(UBlueprint* Blueprint, const FString& VariableName, const FString& Tooltip);

	/**
	 * Sets the description (tooltip) of a Blueprint function graph.
	 * @param FunctionGraph The function graph.
	 * @param Tooltip The description shown when hovering the function.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static void SetFunctionTooltip(UEdGraph* FunctionGraph, const FString& Tooltip);

	/**
	 * Lists every node of a graph with its class, title, position and estimated size (debug helper).
	 * @param Graph The graph to describe.
	 * @return One line per node: "Name | Class | Title | X,Y | WxH".
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static TArray<FString> DescribeNodes(UEdGraph* Graph);

	/**
	 * Creates a new empty level asset and opens it in the editor (the current level must be saved first).
	 * @param AssetPath Long package path of the new level, e.g. "/Game/Versus/Level/Lvl_Versus".
	 * @return True if the level was created and opened.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static bool CreateLevel(const FString& AssetPath);

	/**
	 * Configures Play-In-Editor multiplayer settings.
	 * @param NumPlayers Number of players (windows) to launch.
	 * @param bListenServer True: the first window is a listen server and the others are clients.
	 *   False: standalone (no networking).
	 * @param bNewWindows True: each player plays in its own floating window.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static void ConfigurePIE(int32 NumPlayers, bool bListenServer, bool bNewWindows);

	/**
	 * Executes an editor console command (e.g. "stat fps"). Runs in the PIE world if one is running.
	 * @param Command The console command.
	 * @return True if the command was recognised.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static bool ExecConsoleCommand(const FString& Command);

	/**
	 * Creates a digital (bool) Input Action asset whose keys players can remap.
	 * @param FolderPath Content folder, e.g. "/Game/Versus/Input".
	 * @param AssetName Asset name, e.g. "IA_Ability".
	 * @param MappingName Unique remapping name, e.g. "Versus_Ability" (empty = not remappable).
	 * @param DisplayName Name shown to the player in the key settings.
	 * @param DisplayCategory Category shown to the player.
	 * @return The created Input Action (unsaved).
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static UObject* CreateInputAction(const FString& FolderPath, const FString& AssetName, const FString& MappingName, const FString& DisplayName, const FString& DisplayCategory);

	/**
	 * Creates an empty Input Mapping Context asset.
	 * @param FolderPath Content folder.
	 * @param AssetName Asset name, e.g. "IMC_Versus".
	 * @return The created mapping context (unsaved).
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static UObject* CreateMappingContext(const FString& FolderPath, const FString& AssetName);

	/**
	 * Adds a key mapping to an Input Mapping Context.
	 * @param MappingContext The Input Mapping Context.
	 * @param Action The Input Action to trigger.
	 * @param KeyName Key name, e.g. "LeftShift", "SpaceBar", "One", "LeftMouseButton", "Enter", "Tab", "Escape", "F10".
	 * @param bPlayerMappable False: this mapping ignores remapping settings (secondary / fixed key).
	 * @return The index of the new mapping, or -1 on failure.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static int32 AddKeyMapping(UObject* MappingContext, UObject* Action, const FString& KeyName, bool bPlayerMappable);

	/**
	 * Gives one existing mapping of a context its own remapping name (overrides the action settings).
	 * Useful when one action has several keys that must be remapped separately (e.g. movement W/A/S/D).
	 * @param MappingContext The Input Mapping Context.
	 * @param MappingIndex Index of the mapping in the context.
	 * @param MappingName Unique remapping name.
	 * @param DisplayName Name shown to the player.
	 * @param DisplayCategory Category shown to the player.
	 * @return True on success.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static bool SetMappingPlayerSettings(UObject* MappingContext, int32 MappingIndex, const FString& MappingName, const FString& DisplayName, const FString& DisplayCategory);

	/**
	 * Lists the mappings of an Input Mapping Context: "index | action | key | remap name".
	 * @param MappingContext The Input Mapping Context.
	 * @return One line per mapping.
	 */
	UFUNCTION(meta = (AICallable), Category = "VersusEditorToolset")
	static TArray<FString> DescribeMappings(UObject* MappingContext);
};

// All rights reserved Dominik Pavlicek 2022.


#include "ActorInventoryEditorUtilities.h"

#include "K2Node_Event.h"
#include "Factories/ActorInventoryClassViewerFilter.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/SClassPickerDialog.h"

bool FActorInventoryEditorUtilities::PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class)
{
	// Create filter
	TSharedPtr<FActorInventoryClassViewerFilter> Filter = MakeShareable(new FActorInventoryClassViewerFilter);
	Filter->AllowedChildrenOfClasses.Add(Class);

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.ClassFilter = Filter;
	Options.bShowUnloadedBlueprints = true;
	Options.bExpandRootNodes = true;
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::Dynamic;
	
	return SClassPickerDialog::PickClass(TitleText, Options, OutChosenClass, Class);
}

UK2Node_Event* FActorInventoryEditorUtilities::BlueprintGetOrAddEvent(UBlueprint* Blueprint, FName EventName,
	UClass* EventClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	// Find existing event
	if (UK2Node_Event* EventNode = BlueprintGetEvent(Blueprint, EventName, EventClassSignature))
	{
		return EventNode;
	}

	// Create a New Event
	if (Blueprint->UbergraphPages.Num())
	{
		int32 NodePositionY = 0;
		UK2Node_Event* NodeEvent = FKismetEditorUtilities::AddDefaultEventNode(
			Blueprint,
			Blueprint->UbergraphPages[0],
			EventName,
			EventClassSignature,
			NodePositionY
		);
		NodeEvent->SetEnabledState(ENodeEnabledState::Enabled);
		NodeEvent->NodeComment = "";
		NodeEvent->bCommentBubbleVisible = false;
		return NodeEvent;
	}

	return nullptr;
}

UK2Node_Event* FActorInventoryEditorUtilities::BlueprintGetEvent(UBlueprint* Blueprint, FName EventName,
	UClass* EventClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	TArray<UK2Node_Event*> AllEvents;
	FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_Event>(Blueprint, AllEvents);
	for (UK2Node_Event* EventNode : AllEvents)
	{
		if (EventNode->bOverrideFunction && EventNode->EventReference.GetMemberName() == EventName)
		{
			return EventNode;
		}
	}

	return nullptr;
}

UEdGraph* FActorInventoryEditorUtilities::BlueprintGetOrAddFunction(UBlueprint* Blueprint, FName FunctionName,
	UClass* FunctionClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	// Find existing function
	if (UEdGraph* GraphFunction = BlueprintGetFunction(Blueprint, FunctionName, FunctionClassSignature))
	{
		return GraphFunction;
	}

	// Create a new function
	UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, FunctionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
	FBlueprintEditorUtils::AddFunctionGraph(Blueprint, NewGraph, /*bIsUserCreated=*/ false, FunctionClassSignature);
	Blueprint->LastEditedDocuments.Add(NewGraph);
	return NewGraph;
}

UEdGraph* FActorInventoryEditorUtilities::BlueprintGetFunction(UBlueprint* Blueprint, FName FunctionName,
	UClass* FunctionClassSignature)
{
	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	// Find existing function
	for (UEdGraph* GraphFunction : Blueprint->FunctionGraphs)
	{
		if (FunctionName == GraphFunction->GetFName())
		{
			return GraphFunction;
		}
	}

	// Find in the implemented Interfaces Graphs
	for (const FBPInterfaceDescription& Interface : Blueprint->ImplementedInterfaces)
	{
		for (UEdGraph* GraphFunction : Interface.Graphs)
		{
			if (FunctionName == GraphFunction->GetFName())
			{
				return GraphFunction;
			}
		}
	}

	return nullptr;
}

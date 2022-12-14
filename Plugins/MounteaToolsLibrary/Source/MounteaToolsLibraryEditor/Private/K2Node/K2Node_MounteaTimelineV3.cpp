// All rights reserved Dominik Pavlicek 2022.


#include "K2Node/K2Node_MounteaTimelineV3.h"
#include "Engine/Blueprint.h"
#include "Engine/TimelineTemplate.h"

#include "Curves/CurveFloat.h"
#include "Curves/CurveLinearColor.h"
#include "Curves/CurveVector.h"

#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"

#include "K2Node_Composite.h"
#include "K2Node_VariableGet.h"

#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"

#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"

#include "DiffResults.h"

#include "KismetCompilerMisc.h"
#include "KismetCompiler.h"

#include "Helpers/MounteaTimeline.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_MounteaTimelineV3::UK2Node_MounteaTimelineV3(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bAutoPlay = false;
	bLoop = false;
	bReplicated = false;
	bIgnoreTimeDilation = false;
}

static FName PlayPinName(TEXT("Play"));
static FName PlayFromStartPinName(TEXT("PlayFromStart"));
static FName StopPinName(TEXT("Stop"));
static FName UpdatePinName(TEXT("Update"));
static FName ReversePinName(TEXT("Reverse"));
static FName ReverseFromEndPinName(TEXT("ReverseFromEnd"));
static FName FinishedPinName(TEXT("Finished"));
static FName NewTimePinName(TEXT("NewTime"));
static FName SetNewTimePinName(TEXT("SetNewTime"));
static FName DirectionPinName(TEXT("Direction"));

namespace 
{
	UEdGraphPin* GetPin (const UK2Node_MounteaTimelineV3* Timeline, const FName PinName, EEdGraphPinDirection DesiredDirection) 
	{
		UEdGraphPin* Pin = Timeline->FindPin(PinName);
		
		check(Pin);
		check(Pin->Direction == DesiredDirection);
		
		return Pin;
	}
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetPlayPin() const
{
	return GetPin(this, PlayPinName, EGPD_Input);
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetPlayFromStartPin() const
{
	return GetPin(this, PlayFromStartPinName, EGPD_Input);
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetStopPin() const
{
	return GetPin(this, StopPinName, EGPD_Input);
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetUpdatePin() const
{
	return GetPin(this, UpdatePinName, EGPD_Output);
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetReversePin() const
{
	return GetPin(this, ReversePinName, EGPD_Input);
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetReverseFromEndPin() const
{
	return GetPin(this, ReverseFromEndPinName, EGPD_Input);
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetFinishedPin() const
{
	return GetPin(this, FinishedPinName, EGPD_Output);
}

UEdGraphPin* UK2Node_MounteaTimelineV3::GetNewTimePin() const
{
	return GetPin(this, NewTimePinName, EGPD_Input);
}


UEdGraphPin* UK2Node_MounteaTimelineV3::GetDirectionPin() const
{
	// TODO: Remove TIMELINE class
	UEdGraphPin* Pin = FindPin(DirectionPinName);
	if (Pin)
	{
		const bool bIsOutput = (EGPD_Output == Pin->Direction);
		const bool bProperType = (UEdGraphSchema_K2::PC_Byte == Pin->PinType.PinCategory);
		const bool bProperSubCategoryObj = (Pin->PinType.PinSubCategoryObject == FTimeline::GetTimelineDirectionEnum());
		if(bIsOutput && bProperType && bProperSubCategoryObj)
		{
			return Pin;
		}
	}
	return NULL;
}

void UK2Node_MounteaTimelineV3::ExpandForPin(UEdGraphPin* TimelinePin, const FName PropertyName, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	// TODO: Remove TIMELINE class
	if (TimelinePin && TimelinePin->LinkedTo.Num() > 0)
	{
		UK2Node_VariableGet* GetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_VariableGet>(this, SourceGraph);
		GetVarNode->VariableReference.SetSelfMember(PropertyName);
		GetVarNode->AllocateDefaultPins();
		UEdGraphPin* ValuePin = GetVarNode->GetValuePin();
		if (NULL != ValuePin)
		{
			CompilerContext.MovePinLinksToIntermediate(*TimelinePin, *ValuePin);
		}
		else
		{
			CompilerContext.MessageLog.Error(*LOCTEXT("ExpandForPin_Error", "ExpandForPin error, no property found for @@").ToString(), TimelinePin);
		}
	}
}


#undef LOCTEXT_NAMESPACE

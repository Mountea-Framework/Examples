// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "K2Node.h"
#include "Textures/SlateIcon.h"
#include "K2Node_MounteaTimelineV3.generated.h"

class FBlueprintActionDatabaseRegistrar;
class FKismetCompilerContext;
class INameValidatorInterface;
class UEdGraph;
class UEdGraphPin;


/**
 * 
 */
UCLASS()
class MOUNTEATOOLSLIBRARYEDITOR_API UK2Node_MounteaTimelineV3 : public UK2Node
{
	GENERATED_UCLASS_BODY()

	/** The name of the timeline. Used to name ONLY the member variable (Component). To obtain the name of timeline template use UTimelineTemplate::TimelineVariableNameToTemplateName */
	UPROPERTY()
	FName TimelineName;

	/** If the timeline is set to autoplay */
	UPROPERTY(Transient)
	uint32 bAutoPlay:1;

	/** Unique ID for the template we use, required to indentify the timeline after a paste */
	UPROPERTY()
	FGuid TimelineGuid;

	/** If the timeline is set to loop */
	UPROPERTY(Transient)
	uint32 bLoop:1;

	/** If the timeline is set to replicate */
	UPROPERTY(Transient)
	uint32 bReplicated:1;

	/** If the timeline should ignore global time dilation */
	UPROPERTY(Transient)
	uint32 bIgnoreTimeDilation : 1;
/*
	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual void PreloadRequiredAssets() override;
	virtual void DestroyNode() override;
	virtual void PostPasteNode() override;
	virtual void PrepareForCopying() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	virtual void FindDiffs(class UEdGraphNode* OtherNode, struct FDiffResults& Results )  override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual TSharedPtr<class INameValidatorInterface> MakeNameValidator() const override;
	virtual FText GetTooltipText() const override;
	virtual FString GetDocumentationExcerptName() const override;
	virtual FName GetCornerIcon() const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface.
	virtual bool NodeCausesStructuralBlueprintChange() const override { return true; }
	virtual class FNodeHandlingFunctor* CreateNodeHandler(class FKismetCompilerContext& CompilerContext) const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetNodeAttributes( TArray<TKeyValuePair<FString, FString>>& OutNodeAttributes ) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	//~ End UK2Node Interface.
	*/
	
	/** Get the 'play' input pin */
	UEdGraphPin* GetPlayPin() const;

	/** Get the 'play from start' input pin */
	UEdGraphPin* GetPlayFromStartPin() const;

	/** Get the 'stop' input pin */
	UEdGraphPin* GetStopPin() const;
	
	/** Get the 'update' output pin */
	UEdGraphPin* GetUpdatePin() const;

	/** Get the 'reverse' input pin */
	UEdGraphPin* GetReversePin() const;

	/** Get the 'reverse from end' input pin */
	UEdGraphPin* GetReverseFromEndPin() const;

	/** Get the 'finished' output pin */
	UEdGraphPin* GetFinishedPin() const;

	/** Get the 'newtime' input pin */
	UEdGraphPin* GetNewTimePin() const;

	/** Get the 'Direction' output pin */
	UEdGraphPin* GetDirectionPin() const;

	/** Get the 'Direction' output pin */
	UEdGraphPin* GetTrackPin(const FName TrackName) const;

private:
	void ExpandForPin(UEdGraphPin* TimelinePin, const FName PropertyName, FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph);

};

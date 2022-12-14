// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_ConstructObjectFromClass.h"

#include "K2Node_MounteaTimelineV2.generated.h"

class FBlueprintActionDatabaseRegistrar;
class UEdGraph;
class UK2Node_TemporaryVariable;
class UK2Node_CustomEvent;
class UK2Node_CallFunction;

struct FAsyncTaskFromUClassPinRedirectMapInfo
{
	TMap<FName, TArray<UClass*> > OldPinToProxyClassMap;
};

/**
 * 
 */
UCLASS()
class MOUNTEATOOLSLIBRARYEDITOR_API UK2Node_MounteaTimelineV2 : public UK2Node_ConstructObjectFromClass
{
	GENERATED_BODY()

	UK2Node_MounteaTimelineV2(const FObjectInitializer& ObjectInitializer);

	//~ Begin UEdGraphNode Interface.
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void EarlyValidation(class FCompilerResultsLog& MessageLog) const override;
	//~ End UEdGraphNode Interface.

	virtual bool UseWorldContext() const override;
	virtual bool UseOuter() const override { return true; }
	
	// UEdGraphNode interface
	// virtual void AllocateDefaultPins() override;
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool IsCompatibleWithGraph(const UEdGraph* TargetGraph) const override;
	virtual void ValidateNodeDuringCompilation(class FCompilerResultsLog& MessageLog) const override;
	
	virtual void GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const override;
	// End of UEdGraphNode interface

	// UK2Node interface
	virtual bool HasExternalDependencies(TArray<class UStruct*>* OptionalOutput) const override;
	virtual FName GetCornerIcon() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	/** Whether or not two pins match for purposes of reconnection after reconstruction.  This allows pins that may have had their names changed via reconstruction to be matched to their old values on a node-by-node basis, if needed*/
	virtual ERedirectType DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const override;
	// End of UK2Node interface

protected:
	// Returns the factory function (checked)
	UFunction* GetFactoryFunction() const;

	/** Determines what the possible redirect pin names are **/
	virtual void GetRedirectPinNames(const UEdGraphPin& Pin, TArray<FString>& RedirectPinNames) const override;
	
protected:
	// The name of the function to call to create a proxy object
	UPROPERTY()
	FName ProxyFactoryFunctionName;

	// The class containing the proxy object functions
	UPROPERTY()
	UClass* ProxyFactoryClass;

	// The type of proxy object that will be created
	UPROPERTY()
	UClass* ProxyClass;

	// The name of the 'go' function on the proxy object that will be called after delegates are in place, can be NAME_None
	UPROPERTY()
	FName ProxyActivateFunctionName;

	struct MOUNTEATOOLSLIBRARYEDITOR_API FBaseAsyncTaskHelper
	{
		struct FOutputPinAndLocalVariable
		{
			UEdGraphPin* OutputPin;
			UK2Node_TemporaryVariable* TempVar;

			FOutputPinAndLocalVariable(UEdGraphPin* Pin, UK2Node_TemporaryVariable* Var) : OutputPin(Pin), TempVar(Var) {}
		};

		static bool ValidDataPin(const UEdGraphPin* Pin, EEdGraphPinDirection Direction);
		static bool CreateDelegateForNewFunction(UEdGraphPin* DelegateInputPin, FName FunctionName, UK2Node* CurrentNode, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext);
		static bool CopyEventSignature(UK2Node_CustomEvent* CENode, UFunction* Function, const UEdGraphSchema_K2* Schema);
		static bool HandleDelegateImplementation(
			FMulticastDelegateProperty* CurrentProperty, const TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable>& VariableOutputs,
			UEdGraphPin* ProxyObjectPin, UEdGraphPin*& InOutLastThenPin,
			UK2Node* CurrentNode, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext);

		static const FName GetAsyncTaskProxyName()
		{
			static const FName Name(TEXT("ReturnValue"));
			return Name;
		}
	};

	// Pin Redirector support
	static TMap<FName, FAsyncTaskFromUClassPinRedirectMapInfo> AsyncTaskPinRedirectMap;
	static bool bAsyncTaskPinRedirectMapInitialized;

private:
	/** Invalidates current pin tool tips, so that they will be refreshed before being displayed: */
	void InvalidatePinTooltips() { bPinTooltipsValid = false; }

	void ResetOutputDelegatePin();
	
	UEdGraphPin* GenerateAssignmentNodes(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph, UK2Node_CallFunction* CallBeginSpawnNode, UEdGraphNode* SpawnNode, UEdGraphPin* CallBeginResult, const UClass* ForClass ) const;
	/**
	* Creates hover text for the specified pin.
	*
	* @param   Pin				The pin you want hover text for (should belong to this node)
	*/
	void GeneratePinTooltip(UEdGraphPin& Pin) const;

	/** Flag used to track validity of pin tooltips, when tooltips are invalid they will be refreshed before being displayed */
	mutable bool bPinTooltipsValid;
};

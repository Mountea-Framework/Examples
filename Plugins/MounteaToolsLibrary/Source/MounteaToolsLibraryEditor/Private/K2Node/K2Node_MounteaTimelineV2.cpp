// All rights reserved Dominik Pavlicek 2022.


#include "K2Node/K2Node_MounteaTimelineV2.h"

#include "Kismet/GameplayStatics.h"
#include "K2Node_CallFunction.h"
#include "KismetCompilerMisc.h"
#include "KismetCompiler.h"
#include "EdGraph/EdGraphPin.h"
#include "UObject/UnrealType.h"
#include "EdGraphSchema_K2.h"
#include "Misc/ConfigCacheIni.h"
#include "Kismet/KismetSystemLibrary.h"
#include "K2Node_AddDelegate.h"
#include "K2Node_AssignmentStatement.h"
#include "K2Node_CreateDelegate.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_Self.h"
#include "K2Node_TemporaryVariable.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Engine/MemberReference.h"

#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintCompilationManager.h"
#include "K2Node_CallArrayFunction.h"
#include "K2Node_EnumLiteral.h"
#include "Helpers/MounteaTimeline.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_MounteaTimelineV2::UK2Node_MounteaTimelineV2(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ProxyFactoryFunctionName(NAME_None)
	, ProxyFactoryClass(nullptr)
	//, ProxyClass(nullptr)
	, ProxyClass(UMounteaTimelineObject::StaticClass())
	, ProxyActivateFunctionName("Active")
	, bPinTooltipsValid(false)
{
}

void UK2Node_MounteaTimelineV2::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	ProxyFactoryFunctionName = NAME_None;
	ProxyFactoryClass = UMounteaTimelineObject::StaticClass(); //GetClassToSpawn();
	ProxyClass = ProxyFactoryClass;

	UK2Node_CallFunction* CallCreateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallCreateNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UGameplayStatics, SpawnObject), UGameplayStatics::StaticClass());
	CallCreateNode->AllocateDefaultPins();

	// store off the class to spawn before we mutate pin connections:
	UClass* ClassToSpawn = UMounteaTimelineObject::StaticClass();// GetClassToSpawn();
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	bool bSucceeded = true;
	//connect exe
	{
		UEdGraphPin* SpawnExecPin = GetExecPin();
		UEdGraphPin* CallExecPin = CallCreateNode->GetExecPin();
		bSucceeded &= SpawnExecPin && CallExecPin && CompilerContext.MovePinLinksToIntermediate(*SpawnExecPin, *CallExecPin).CanSafeConnect();
	}
	
	//connect class
	{
		UEdGraphPin* SpawnClassPin = GetClassPin();
		UEdGraphPin* CallClassPin = CallCreateNode->FindPin(TEXT("ObjectClass"));
		bSucceeded &= SpawnClassPin && CallClassPin && CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallClassPin).CanSafeConnect();
	}

	//connect outer
	{
		UEdGraphPin* SpawnOuterPin = GetOuterPin();
		UEdGraphPin* CallOuterPin = CallCreateNode->FindPin(TEXT("Outer"));

    	if(!SpawnOuterPin || SpawnOuterPin->LinkedTo.Num()==0)
    	{
    		UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
    		SelfNode->AllocateDefaultPins();
    		Schema->TryCreateConnection(SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), SpawnOuterPin);
    	}
    	
		bSucceeded &= SpawnOuterPin && CallOuterPin && CompilerContext.MovePinLinksToIntermediate(*SpawnOuterPin, *CallOuterPin).CanSafeConnect();
	}

	UEdGraphPin* CallResultPin = nullptr;
	//connect result
	{
		UEdGraphPin* SpawnResultPin = GetResultPin();
		CallResultPin = CallCreateNode->GetReturnValuePin();

		// cast HACK. It should be safe. The only problem is native code generation.
		if (SpawnResultPin && CallResultPin)
		{
			CallResultPin->PinType = SpawnResultPin->PinType;
		}
		bSucceeded &= SpawnResultPin && CallResultPin && CompilerContext.MovePinLinksToIntermediate(*SpawnResultPin, *CallResultPin).CanSafeConnect();
	}

	//assign exposed values and connect then
	// {
		UEdGraphPin* LastThen = GenerateAssignmentNodes(CompilerContext, SourceGraph, CallCreateNode, this, CallResultPin, ClassToSpawn);
		// UEdGraphPin* SpawnNodeThen = GetThenPin();
		// bSucceeded &= SpawnNodeThen && LastThen && CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *LastThen).CanSafeConnect();
	// }

	// BreakAllNodeLinks();

	if (!bSucceeded)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("AsyncTaskFromUClass_Error", "ICE: AsyncTaskFromUClass error @@").ToString(), this);
	}

	
	check(SourceGraph && Schema);
	bool bIsErrorFree = true;

	// Create a call to factory the proxy object
	const UK2Node_CallFunction* const CallCreateProxyObjectNode = CallCreateNode;

	// bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(UEdGraphSchema_K2::PN_Execute), *CallCreateProxyObjectNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute)).CanSafeConnect();
	//
	// for (UEdGraphPin* CurrentPin : Pins)
	// {
	// 	if (FBaseAsyncTaskHelper::ValidDataPin(CurrentPin, EGPD_Input))
	// 	{
	// 		UEdGraphPin* DestPin = CallCreateProxyObjectNode->FindPin(CurrentPin->PinName); // match function inputs, to pass data to function from CallFunction node
	// 		bIsErrorFree &= DestPin && CompilerContext.MovePinLinksToIntermediate(*CurrentPin, *DestPin).CanSafeConnect();
	// 	}
	// }

	UEdGraphPin* const ProxyObjectPin = CallCreateProxyObjectNode->GetReturnValuePin();
	check(ProxyObjectPin);
	UEdGraphPin* OutputAsyncTaskProxy = FindPin(FBaseAsyncTaskHelper::GetAsyncTaskProxyName());
	bIsErrorFree &= !OutputAsyncTaskProxy || CompilerContext.MovePinLinksToIntermediate(*OutputAsyncTaskProxy, *ProxyObjectPin).CanSafeConnect();
	
	// GATHER OUTPUT PARAMETERS AND PAIR THEM WITH LOCAL VARIABLES
	TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable> VariableOutputs;
	bool bPassedFactoryOutputs = false;
	for (UEdGraphPin* CurrentPin : Pins)
	{
		if ((OutputAsyncTaskProxy != CurrentPin) && FBaseAsyncTaskHelper::ValidDataPin(CurrentPin, EGPD_Output))
		{
			if (!bPassedFactoryOutputs)
			{
				UEdGraphPin* DestPin = CallCreateProxyObjectNode->FindPin(CurrentPin->PinName);
				bIsErrorFree &= DestPin && CompilerContext.MovePinLinksToIntermediate(*CurrentPin, *DestPin).CanSafeConnect();
			}
			else
			{
				const FEdGraphPinType& PinType = CurrentPin->PinType;
				UK2Node_TemporaryVariable* TempVarOutput = CompilerContext.SpawnInternalVariable(
					this, PinType.PinCategory, PinType.PinSubCategory, PinType.PinSubCategoryObject.Get(), PinType.ContainerType, PinType.PinValueType);
				bIsErrorFree &= TempVarOutput->GetVariablePin() && CompilerContext.MovePinLinksToIntermediate(*CurrentPin, *TempVarOutput->GetVariablePin()).CanSafeConnect();
				VariableOutputs.Add(FBaseAsyncTaskHelper::FOutputPinAndLocalVariable(CurrentPin, TempVarOutput));
			}
		}
		else if (!bPassedFactoryOutputs && CurrentPin && CurrentPin->Direction == EGPD_Output)
		{
			// the first exec that isn't the node's then pin is the start of the asyc delegate pins
			// once we hit this point, we've iterated beyond all outputs for the factory function
			bPassedFactoryOutputs = (CurrentPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec) && (CurrentPin->PinName != UEdGraphSchema_K2::PN_Then);
		}
	}

	// FOR EACH DELEGATE DEFINE EVENT, CONNECT IT TO DELEGATE AND IMPLEMENT A CHAIN OF ASSIGMENTS
	UEdGraphPin* LastThenPin = LastThen;//CallCreateProxyObjectNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);

	UK2Node_CallFunction* IsValidFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	const FName IsValidFuncName = GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, IsValid);
	IsValidFuncNode->FunctionReference.SetExternalMember(IsValidFuncName, UKismetSystemLibrary::StaticClass());
	IsValidFuncNode->AllocateDefaultPins();
	UEdGraphPin* IsValidInputPin = IsValidFuncNode->FindPinChecked(TEXT("Object"));

	bIsErrorFree &= Schema->TryCreateConnection(ProxyObjectPin, IsValidInputPin);

	UK2Node_IfThenElse* ValidateProxyNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	ValidateProxyNode->AllocateDefaultPins();
	bIsErrorFree &= Schema->TryCreateConnection(IsValidFuncNode->GetReturnValuePin(), ValidateProxyNode->GetConditionPin());

	bIsErrorFree &= Schema->TryCreateConnection(LastThenPin, ValidateProxyNode->GetExecPin());
	LastThenPin = ValidateProxyNode->GetThenPin();

	for (TFieldIterator<FMulticastDelegateProperty> PropertyIt(ProxyClass); PropertyIt && bIsErrorFree; ++PropertyIt)
	{
		bIsErrorFree &= FBaseAsyncTaskHelper::HandleDelegateImplementation(*PropertyIt, VariableOutputs, ProxyObjectPin, LastThenPin, this, SourceGraph, CompilerContext);
	}

	if (CallCreateProxyObjectNode->FindPinChecked(UEdGraphSchema_K2::PN_Then) == LastThenPin)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("MissingDelegateProperties", "BaseAsyncTask: Proxy has no delegates defined. @@").ToString(), this);
		return;
	}

	// Create a call to activate the proxy object if necessary
	if (ProxyActivateFunctionName != NAME_None)
	{
		UK2Node_CallFunction* const CallActivateProxyObjectNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		CallActivateProxyObjectNode->FunctionReference.SetExternalMember(ProxyActivateFunctionName, ProxyClass);
		CallActivateProxyObjectNode->AllocateDefaultPins();

		// Hook up the self connection
		UEdGraphPin* ActivateCallSelfPin = Schema->FindSelfPin(*CallActivateProxyObjectNode, EGPD_Input);
		if(!ActivateCallSelfPin)
		{
			const auto Tip = ProxyClass->GetDisplayNameText().ToString() + " has no Active function or event defined. @@";
			CompilerContext.MessageLog.Error(*Tip);
			return;
		}
		check(ActivateCallSelfPin);

		bIsErrorFree &= Schema->TryCreateConnection(ProxyObjectPin, ActivateCallSelfPin);

		// Hook the activate node up in the exec chain
		UEdGraphPin* ActivateExecPin = CallActivateProxyObjectNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute);
		UEdGraphPin* ActivateThenPin = CallActivateProxyObjectNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);

		bIsErrorFree &= Schema->TryCreateConnection(LastThenPin, ActivateExecPin);

		LastThenPin = ActivateThenPin;
	}

	// Move the connections from the original node then pin to the last internal then pin

	UEdGraphPin* OriginalThenPin = FindPin(UEdGraphSchema_K2::PN_Then);

	if (OriginalThenPin)
	{
		bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*OriginalThenPin, *LastThenPin).CanSafeConnect();
	}
	bIsErrorFree &= CompilerContext.CopyPinLinksToIntermediate(*LastThenPin, *ValidateProxyNode->GetElsePin()).CanSafeConnect();

	if (!bIsErrorFree)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InternalConnectionError", "BaseAsyncTask: Internal connection error. @@").ToString(), this);
	}

	// Make sure we caught everything
	BreakAllNodeLinks();
}

void UK2Node_MounteaTimelineV2::EarlyValidation(FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);
	const UEdGraphPin* ClassPin = GetClassPin(&Pins);
	const bool bAllowAbstract = ClassPin && ClassPin->LinkedTo.Num();
	const UClass* ClassToSpawn = GetClassToSpawn();
	
	bool bCanSpawnObject = (nullptr != ClassToSpawn)
			&& (bAllowAbstract || !ClassToSpawn->HasAnyClassFlags(CLASS_Abstract))
			&& !ClassToSpawn->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists);

	// UObject is a special case where if we are allowing abstract we are going to allow it through even though it doesn't have BlueprintType on it
	if (bCanSpawnObject && (!bAllowAbstract || (ClassToSpawn != UObject::StaticClass())))
	{
		static const FName BlueprintTypeName(TEXT("BlueprintType"));
		static const FName NotBlueprintTypeName(TEXT("NotBlueprintType"));
		static const FName DontUseGenericSpawnObjectName(TEXT("DontUseGenericSpawnObject"));

		auto IsClassAllowedLambda = [](const UClass* InClass)
		{
			return InClass != AActor::StaticClass()
				&& InClass != UActorComponent::StaticClass();
		};

		// Exclude all types in the initial set by default.
		bCanSpawnObject = false;
		const UClass* CurrentClass = ClassToSpawn;

		// Climb up the class hierarchy and look for "BlueprintType." If "NotBlueprintType" is seen first, or if the class is not allowed, then stop searching.
		while (!bCanSpawnObject && CurrentClass != nullptr && !CurrentClass->GetBoolMetaData(NotBlueprintTypeName) && IsClassAllowedLambda(CurrentClass))
		{
			// Include any type that either includes or inherits 'BlueprintType'
			bCanSpawnObject = CurrentClass->GetBoolMetaData(BlueprintTypeName);

			// Stop searching if we encounter 'BlueprintType' with 'DontUseGenericSpawnObject'
			if (bCanSpawnObject && CurrentClass->GetBoolMetaData(DontUseGenericSpawnObjectName))
			{
				bCanSpawnObject = false;
				break;
			}

			CurrentClass = CurrentClass->GetSuperClass();
		}

		// If we validated the given class, continue walking up the hierarchy to make sure we exclude it if it's an Actor or ActorComponent derivative.
		while (bCanSpawnObject && CurrentClass != nullptr)
		{
			bCanSpawnObject &= IsClassAllowedLambda(CurrentClass);

			CurrentClass = CurrentClass->GetSuperClass();
		}
	}

	if(!bCanSpawnObject)
	{
		MessageLog.Error(*FText::Format(LOCTEXT("MounteaTimeline_WrongClassFmt", "Cannot construct objects of type '{0}' in @@"), FText::FromString(GetPathNameSafe(ClassToSpawn))).ToString(), this);

	}
}

bool UK2Node_MounteaTimelineV2::UseWorldContext() const
{
	UBlueprint* BP = GetBlueprint();
	const UClass* ParentClass = BP ? BP->ParentClass : nullptr;
	return ParentClass ? ParentClass->HasMetaDataHierarchical(FBlueprintMetadata::MD_ShowWorldContextPin) != nullptr : false;
}

void UK2Node_MounteaTimelineV2::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins)
{
	AllocateDefaultPins();
	if (UClass* UseSpawnClass = GetClassToSpawn(&OldPins))
	{
		CreatePinsForClass(UseSpawnClass);
		ProxyFactoryClass = UseSpawnClass;
		ProxyClass = UseSpawnClass;
	}
	
	ResetOutputDelegatePin();
	RestoreSplitPins(OldPins);
}

void UK2Node_MounteaTimelineV2::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	if (Pin == GetClassPin())
	{
		const auto UseSpawnClass = GetClassToSpawn();
		if (Pin->LinkedTo.Num() == 0 && ProxyFactoryClass != UseSpawnClass)
		{
			ProxyFactoryClass = UseSpawnClass;
			ProxyClass = UseSpawnClass;
			ResetOutputDelegatePin();
		}
	}
}

FText UK2Node_MounteaTimelineV2::GetTooltipText() const
{
	return FText::FromString("New Mountea Timeline V2");
}

FText UK2Node_MounteaTimelineV2::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Mountea Timeline V2");
}

bool UK2Node_MounteaTimelineV2::IsCompatibleWithGraph(const UEdGraph* TargetGraph) const
{
	const EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	const bool bIsCompatible = GraphType == EGraphType::GT_Ubergraph || GraphType == EGraphType::GT_Macro || GraphType == EGraphType::GT_Function;
	return bIsCompatible && Super::IsCompatibleWithGraph(TargetGraph);

}

void UK2Node_MounteaTimelineV2::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	Super::ValidateNodeDuringCompilation(MessageLog);
	if(UObject const* SourceObject = MessageLog.FindSourceObject(this))
	{
		// Lets check if it's a result of macro expansion, to give a helpful error
		if(UK2Node_MacroInstance const* MacroInstance = Cast<UK2Node_MacroInstance>(SourceObject))
		{
			// Since it's not possible to check the graph's type, just check if this is a ubergraph using the schema's name for it
			if(!(GetGraph()->HasAnyFlags(RF_Transient) && GetGraph()->GetName().StartsWith(UEdGraphSchema_K2::FN_ExecuteUbergraphBase.ToString())))
			{
				MessageLog.Error(*LOCTEXT("AsyncTaskInFunctionFromMacro", "@@ is being used in Function '@@' resulting from expansion of Macro '@@'").ToString(), this, GetGraph(), MacroInstance);
			}
		}
	}
}

void UK2Node_MounteaTimelineV2::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	if (!bPinTooltipsValid)
	{
		for (UEdGraphPin* P : Pins)
		{
			if (P->Direction == EGPD_Input)
			{
				P->PinToolTip.Reset();
				GeneratePinTooltip(*P);
			}
		}

		bPinTooltipsValid = true;
	}

	return UK2Node::GetPinHoverText(Pin, HoverTextOut);
}

bool UK2Node_MounteaTimelineV2::HasExternalDependencies(TArray<UStruct*>* OptionalOutput) const
{
	const UBlueprint* SourceBlueprint = GetBlueprint();

	const bool bProxyFactoryResult = (ProxyFactoryClass != NULL) && (ProxyFactoryClass->ClassGeneratedBy != SourceBlueprint);
	if (bProxyFactoryResult && OptionalOutput)
	{
		OptionalOutput->AddUnique(ProxyFactoryClass);
	}

	const bool bProxyResult = (ProxyClass != NULL) && (ProxyClass->ClassGeneratedBy != SourceBlueprint);
	if (bProxyResult && OptionalOutput)
	{
		OptionalOutput->AddUnique(ProxyClass);
	}

	const bool bSuperResult = Super::HasExternalDependencies(OptionalOutput);
	return bProxyFactoryResult || bProxyResult || bSuperResult;
}

FName UK2Node_MounteaTimelineV2::GetCornerIcon() const
{
	return TEXT("Graph.Latent.LatentIcon");
}

void UK2Node_MounteaTimelineV2::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

UK2Node::ERedirectType UK2Node_MounteaTimelineV2::DoPinsMatchForReconstruction(const UEdGraphPin* NewPin, int32 NewPinIndex, const UEdGraphPin* OldPin, int32 OldPinIndex) const
{
	if (GConfig && ProxyClass)
	{
		// Initialize remap table from INI
		if (!bAsyncTaskPinRedirectMapInitialized)
		{
			bAsyncTaskPinRedirectMapInitialized = true;
			FConfigSection* PackageRedirects = GConfig->GetSectionPrivate(TEXT("/Script/Engine.Engine"), false, true, GEngineIni);
			for (FConfigSection::TIterator It(*PackageRedirects); It; ++It)
			{
				if (It.Key() == TEXT("K2AsyncTaskPinRedirects"))
				{
					FString ProxyClassString;
					FString OldPinString;
					FString NewPinString;

					FParse::Value(*It.Value().GetValue(), TEXT("ProxyClassName="), ProxyClassString);
					FParse::Value(*It.Value().GetValue(), TEXT("OldPinName="), OldPinString);
					FParse::Value(*It.Value().GetValue(), TEXT("NewPinName="), NewPinString);

					UClass* RedirectProxyClass = FindObject<UClass>(ANY_PACKAGE, *ProxyClassString);
					if (RedirectProxyClass)
					{
						FAsyncTaskFromUClassPinRedirectMapInfo& PinRedirectInfo = AsyncTaskPinRedirectMap.FindOrAdd(*OldPinString);
						TArray<UClass*>& ProxyClassArray = PinRedirectInfo.OldPinToProxyClassMap.FindOrAdd(*NewPinString);
						ProxyClassArray.AddUnique(RedirectProxyClass);
					}
				}
			}
		}

		// See if these pins need to be remapped.
		if (FAsyncTaskFromUClassPinRedirectMapInfo* PinRedirectInfo = AsyncTaskPinRedirectMap.Find(OldPin->PinName))
		{
			if (TArray<UClass*>* ProxyClassArray = PinRedirectInfo->OldPinToProxyClassMap.Find(NewPin->PinName))
			{
				for (UClass* RedirectedProxyClass : *ProxyClassArray)
				{
					if (ProxyClass->IsChildOf(RedirectedProxyClass))
					{
						return UK2Node::ERedirectType_Name;
					}
				}
			}
		}
	}

	return Super::DoPinsMatchForReconstruction(NewPin, NewPinIndex, OldPin, OldPinIndex);
}

UFunction* UK2Node_MounteaTimelineV2::GetFactoryFunction() const
{
	if (ProxyFactoryClass == nullptr)
	{
		UE_LOG(LogBlueprint, Error, TEXT("ProxyFactoryClass null in %s. Was a class deleted or saved on a non promoted build?"), *GetFullName());
		return nullptr;
	}

	FMemberReference FunctionRef;
	FunctionRef.SetExternalMember(ProxyFactoryFunctionName, ProxyFactoryClass);

	UFunction* FactoryFunction = FunctionRef.ResolveMember<UFunction>(GetBlueprint());

	if (FactoryFunction == nullptr)
	{
		FactoryFunction = ProxyFactoryClass->FindFunctionByName(ProxyFactoryFunctionName);
		UE_CLOG(
			FactoryFunction == nullptr,
			LogBlueprint,
			Error,
			TEXT("FactoryFunction %s null in %s. Was a class deleted or saved on a non promoted build?"),
			*ProxyFactoryFunctionName.ToString(),
			*GetFullName());
	}

	return FactoryFunction;
}

void UK2Node_MounteaTimelineV2::GetRedirectPinNames(const UEdGraphPin& Pin, TArray<FString>& RedirectPinNames) const
{
	TMap<FString, FStringFormatArg> Args;

	if (ProxyClass)
	{
		Args.Add(TEXT("ProxyClass"), ProxyClass->GetName());
		Args.Add(TEXT("ProxyClassSeparator"), TEXT("."));
	}
	else
	{
		Args.Add(TEXT("ProxyClass"), TEXT(""));
		Args.Add(TEXT("ProxyClassSeparator"), TEXT(""));
	}
	if (ProxyFactoryFunctionName != NAME_None)
	{
		Args.Add(TEXT("ProxyFactoryFunction"), ProxyFactoryFunctionName.ToString());
		Args.Add(TEXT("ProxyFactoryFunctionSeparator"), TEXT("."));
	}
	else
	{
		Args.Add(TEXT("ProxyFactoryFunction"), TEXT(""));
		Args.Add(TEXT("ProxyFactoryFunctionSeparator"), TEXT(""));
	}

	Args.Add(TEXT("PinName"), Pin.PinName.ToString());

	FString FullPinName;
	FullPinName = FString::Format(TEXT("{ProxyClass}{ProxyClassSeparator}{ProxyFactoryFunction}{ProxyFactoryFunctionSeparator}{PinName}"), Args);

	RedirectPinNames.Add(FullPinName);
}

bool UK2Node_MounteaTimelineV2::FBaseAsyncTaskHelper::ValidDataPin(const UEdGraphPin* Pin, EEdGraphPinDirection Direction)
{
	const bool bValidDataPin = Pin
		&& !Pin->bOrphanedPin
		&& (Pin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec);

	const bool bProperDirection = Pin && (Pin->Direction == Direction);

	return bValidDataPin && bProperDirection;
}

bool UK2Node_MounteaTimelineV2::FBaseAsyncTaskHelper::CreateDelegateForNewFunction(UEdGraphPin* DelegateInputPin, FName FunctionName, UK2Node* CurrentNode, UEdGraph* SourceGraph, FKismetCompilerContext& CompilerContext)
{
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(DelegateInputPin && Schema && CurrentNode && SourceGraph && (FunctionName != NAME_None));
	bool bResult = true;

	// WORKAROUND, so we can create delegate from nonexistent function by avoiding check at expanding step
	// instead simply: Schema->TryCreateConnection(AddDelegateNode->GetDelegatePin(), CurrentCENode->FindPinChecked(UK2Node_CustomEvent::DelegateOutputName));
	UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(CurrentNode, SourceGraph);
	SelfNode->AllocateDefaultPins();

	UK2Node_CreateDelegate* CreateDelegateNode = CompilerContext.SpawnIntermediateNode<UK2Node_CreateDelegate>(CurrentNode, SourceGraph);
	CreateDelegateNode->AllocateDefaultPins();
	bResult &= Schema->TryCreateConnection(DelegateInputPin, CreateDelegateNode->GetDelegateOutPin());
	bResult &= Schema->TryCreateConnection(SelfNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), CreateDelegateNode->GetObjectInPin());
	CreateDelegateNode->SetFunction(FunctionName);

	return bResult;
}

bool UK2Node_MounteaTimelineV2::FBaseAsyncTaskHelper::CopyEventSignature(UK2Node_CustomEvent* CENode, UFunction* Function, const UEdGraphSchema_K2* Schema)
{
	check(CENode && Function && Schema);

	bool bResult = true;
	for (TFieldIterator<FProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		const FProperty* Param = *PropIt;
		if (!Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm))
		{
			FEdGraphPinType PinType;
			bResult &= Schema->ConvertPropertyToPinType(Param, /*out*/ PinType);
			bResult &= (nullptr != CENode->CreateUserDefinedPin(Param->GetFName(), PinType, EGPD_Output));
		}
	}
	return bResult;
}

bool UK2Node_MounteaTimelineV2::FBaseAsyncTaskHelper::HandleDelegateImplementation(
	FMulticastDelegateProperty* CurrentProperty,
	const TArray<FBaseAsyncTaskHelper::FOutputPinAndLocalVariable>& VariableOutputs, UEdGraphPin* ProxyObjectPin,
	UEdGraphPin*& InOutLastThenPin, UK2Node* CurrentNode, UEdGraph* SourceGraph,
	FKismetCompilerContext& CompilerContext)
{
	bool bIsErrorFree = true;
	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	check(CurrentProperty && ProxyObjectPin && InOutLastThenPin && CurrentNode && SourceGraph && Schema);

	UEdGraphPin* PinForCurrentDelegateProperty = CurrentNode->FindPin(CurrentProperty->GetFName());
	if (!PinForCurrentDelegateProperty || (UEdGraphSchema_K2::PC_Exec != PinForCurrentDelegateProperty->PinType.PinCategory))
	{
		FText ErrorMessage = FText::Format(LOCTEXT("WrongDelegateProperty", "BaseAsyncTask: Cannot find execution pin for delegate "), FText::FromString(CurrentProperty->GetName()));
		CompilerContext.MessageLog.Error(*ErrorMessage.ToString(), CurrentNode);
		return false;
	}

	UK2Node_CustomEvent* CurrentCENode = CompilerContext.SpawnIntermediateEventNode<UK2Node_CustomEvent>(CurrentNode, PinForCurrentDelegateProperty, SourceGraph);
	{
	UK2Node_AddDelegate* AddDelegateNode = CompilerContext.SpawnIntermediateNode<UK2Node_AddDelegate>(CurrentNode, SourceGraph);
	AddDelegateNode->SetFromProperty(CurrentProperty, false, CurrentProperty->GetOwnerClass());
	AddDelegateNode->AllocateDefaultPins();
		bIsErrorFree &= Schema->TryCreateConnection(AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Self), ProxyObjectPin);
		bIsErrorFree &= Schema->TryCreateConnection(InOutLastThenPin, AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute));
		InOutLastThenPin = AddDelegateNode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
		CurrentCENode->CustomFunctionName = *FString::Printf(TEXT("%s_%s"), *CurrentProperty->GetName(), *CompilerContext.GetGuid(CurrentNode));
		CurrentCENode->AllocateDefaultPins();

		bIsErrorFree &= FBaseAsyncTaskHelper::CreateDelegateForNewFunction(AddDelegateNode->GetDelegatePin(), CurrentCENode->GetFunctionName(), CurrentNode, SourceGraph, CompilerContext);
		bIsErrorFree &= FBaseAsyncTaskHelper::CopyEventSignature(CurrentCENode, AddDelegateNode->GetDelegateSignature(), Schema);
	}

	UEdGraphPin* LastActivatedNodeThen = CurrentCENode->FindPinChecked(UEdGraphSchema_K2::PN_Then);
	for (const FBaseAsyncTaskHelper::FOutputPinAndLocalVariable& OutputPair : VariableOutputs) // CREATE CHAIN OF ASSIGMENTS
	{
		UEdGraphPin* PinWithData = CurrentCENode->FindPin(OutputPair.OutputPin->PinName);
		if (PinWithData == nullptr)
		{
			continue;
		}

		UK2Node_AssignmentStatement* AssignNode = CompilerContext.SpawnIntermediateNode<UK2Node_AssignmentStatement>(CurrentNode, SourceGraph);
		AssignNode->AllocateDefaultPins();
		bIsErrorFree &= Schema->TryCreateConnection(LastActivatedNodeThen, AssignNode->GetExecPin());
		bIsErrorFree &= Schema->TryCreateConnection(OutputPair.TempVar->GetVariablePin(), AssignNode->GetVariablePin());
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetVariablePin());
		bIsErrorFree &= Schema->TryCreateConnection(AssignNode->GetValuePin(), PinWithData);
		AssignNode->NotifyPinConnectionListChanged(AssignNode->GetValuePin());

		LastActivatedNodeThen = AssignNode->GetThenPin();
	}

	bIsErrorFree &= CompilerContext.MovePinLinksToIntermediate(*PinForCurrentDelegateProperty, *LastActivatedNodeThen).CanSafeConnect();
	return bIsErrorFree;
}

void UK2Node_MounteaTimelineV2::ResetOutputDelegatePin()
{
InvalidatePinTooltips();

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	
	// CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	bool bExposeProxy = false;
	bool bHideThen = false;
	FText ExposeProxyDisplayName;
	for (const UStruct* TestStruct = ProxyClass; TestStruct; TestStruct = TestStruct->GetSuperStruct())
	{
		bExposeProxy |= true;// TestStruct->HasMetaData(TEXT("ExposedAsyncProxy"));
		bHideThen |= TestStruct->HasMetaData(TEXT("HideThen"));
		if (ExposeProxyDisplayName.IsEmpty())
		{
			ExposeProxyDisplayName = TestStruct->GetMetaDataText(TEXT("ExposedAsyncProxy"));
		}
	}

	// if (!bHideThen)
	// {
	// 	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	// }

	if (bExposeProxy)
	{
		UEdGraphPin* ProxyPin = FindPin( FBaseAsyncTaskHelper::GetAsyncTaskProxyName());
		if (!ExposeProxyDisplayName.IsEmpty())
		{
			ProxyPin->PinFriendlyName = ExposeProxyDisplayName;
		}
	}

	// UFunction* Function = GetFactoryFunction();
	// if (!bHideThen && Function)
	// {
	// 	for (TFieldIterator<FProperty> PropIt(Function); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	// 	{
	// 		FProperty* Param = *PropIt;
	// 		// invert the check for function inputs (below) and exclude the factory func's return param - the assumption is
	// 		// that the factory method will be returning the proxy object, and that other outputs should be forwarded along 
	// 		// with the 'then' pin
	// 		const bool bIsFunctionOutput = Param->HasAnyPropertyFlags(CPF_OutParm) && !Param->HasAnyPropertyFlags(CPF_ReferenceParm) && !Param->HasAnyPropertyFlags(CPF_ReturnParm);
	// 		if (bIsFunctionOutput)
	// 		{
	// 			UEdGraphPin* Pin = CreatePin(EGPD_Output, NAME_None, Param->GetFName());
	// 			K2Schema->ConvertPropertyToPinType(Param, /*out*/ Pin->PinType);
	// 		}
	// 	}
	// }

	UFunction* DelegateSignatureFunction = nullptr;
	for (TFieldIterator<FProperty> PropertyIt(ProxyClass); PropertyIt; ++PropertyIt)
	{
		if (FMulticastDelegateProperty* Property = CastField<FMulticastDelegateProperty>(*PropertyIt))
		{
			UEdGraphPin* ExecPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, Property->GetFName());
			ExecPin->PinToolTip = Property->GetToolTipText().ToString();
			ExecPin->PinFriendlyName = Property->GetDisplayNameText();

			if (!DelegateSignatureFunction)
			{
				DelegateSignatureFunction = Property->SignatureFunction;
			}
		}
	}

	if (DelegateSignatureFunction)
	{
		for (TFieldIterator<FProperty> PropIt(DelegateSignatureFunction); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
		{
			FProperty* Param = *PropIt;
			const bool bIsFunctionInput = !Param->HasAnyPropertyFlags(CPF_OutParm) || Param->HasAnyPropertyFlags(CPF_ReferenceParm);
			if (bIsFunctionInput)
			{
				UEdGraphPin* Pin = CreatePin(EGPD_Output, NAME_None, Param->GetFName());
				K2Schema->ConvertPropertyToPinType(Param, /*out*/ Pin->PinType);

				Pin->PinToolTip = Param->GetToolTipText().ToString();
			}
		}
	}
}

UEdGraphPin* UK2Node_MounteaTimelineV2::GenerateAssignmentNodes(FKismetCompilerContext& CompilerContext,
	UEdGraph* SourceGraph, UK2Node_CallFunction* CallBeginSpawnNode, UEdGraphNode* SpawnNode,
	UEdGraphPin* CallBeginResult, const UClass* ForClass) const
{
	static const FName ObjectParamName(TEXT("Object"));
	static const FName ValueParamName(TEXT("Value"));
	static const FName PropertyNameParamName(TEXT("PropertyName"));

	const UEdGraphSchema_K2* Schema = CompilerContext.GetSchema();
	UEdGraphPin* LastThen = CallBeginSpawnNode->GetThenPin();

	// Create 'set var by name' nodes and hook them up
	for (int32 PinIdx = 0; PinIdx < SpawnNode->Pins.Num(); PinIdx++)
	{
		// Only create 'set param by name' node if this pin is linked to something
		UEdGraphPin* OrgPin = SpawnNode->Pins[PinIdx];
		const bool bHasDefaultValue = !OrgPin->DefaultValue.IsEmpty() || !OrgPin->DefaultTextValue.IsEmpty() || OrgPin->DefaultObject;
		if (NULL == CallBeginSpawnNode->FindPin(OrgPin->PinName) &&
			(OrgPin->LinkedTo.Num() > 0 || bHasDefaultValue))
		{
			if( OrgPin->LinkedTo.Num() == 0 )
			{
				FProperty* Property = FindFProperty<FProperty>(ForClass, OrgPin->PinName);
				// NULL property indicates that this pin was part of the original node, not the 
				// class we're assigning to:
				if( !Property )
				{
					continue;
				}

				// We don't want to generate an assignment node unless the default value 
				// differs from the value in the CDO:
				FString DefaultValueAsString;
					
				if (FBlueprintCompilationManager::GetDefaultValue(ForClass, Property, DefaultValueAsString))
				{
					if (Schema->DoesDefaultValueMatch(*OrgPin, DefaultValueAsString))
					{
						continue;
					}
				}
				else if(ForClass->ClassDefaultObject)
				{
					FBlueprintEditorUtils::PropertyValueToString(Property, (uint8*)ForClass->ClassDefaultObject, DefaultValueAsString);

					if (DefaultValueAsString == OrgPin->GetDefaultAsString())
					{
						continue;
					}
				}
			}

			UFunction* SetByNameFunction = Schema->FindSetVariableByNameFunction(OrgPin->PinType);
			if (SetByNameFunction)
			{
				UK2Node_CallFunction* SetVarNode = nullptr;
				if (OrgPin->PinType.IsArray())
				{
					SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallArrayFunction>(SpawnNode, SourceGraph);
				}
				else
				{
					SetVarNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode, SourceGraph);
				}
				SetVarNode->SetFromFunction(SetByNameFunction);
				SetVarNode->AllocateDefaultPins();

				// Connect this node into the exec chain
				Schema->TryCreateConnection(LastThen, SetVarNode->GetExecPin());
				LastThen = SetVarNode->GetThenPin();

				// Connect the new actor to the 'object' pin
				UEdGraphPin* ObjectPin = SetVarNode->FindPinChecked(ObjectParamName);
				CallBeginResult->MakeLinkTo(ObjectPin);

				// Fill in literal for 'property name' pin - name of pin is property name
				UEdGraphPin* PropertyNamePin = SetVarNode->FindPinChecked(PropertyNameParamName);
				PropertyNamePin->DefaultValue = OrgPin->PinName.ToString();

				UEdGraphPin* ValuePin = SetVarNode->FindPinChecked(ValueParamName);
				if (OrgPin->LinkedTo.Num() == 0 &&
					OrgPin->DefaultValue != FString() &&
					OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Byte &&
					OrgPin->PinType.PinSubCategoryObject.IsValid() &&
					OrgPin->PinType.PinSubCategoryObject->IsA<UEnum>())
				{
					// Pin is an enum, we need to alias the enum value to an int:
					UK2Node_EnumLiteral* EnumLiteralNode = CompilerContext.SpawnIntermediateNode<UK2Node_EnumLiteral>(SpawnNode, SourceGraph);
					EnumLiteralNode->Enum = CastChecked<UEnum>(OrgPin->PinType.PinSubCategoryObject.Get());
					EnumLiteralNode->AllocateDefaultPins();
					EnumLiteralNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue)->MakeLinkTo(ValuePin);

					UEdGraphPin* InPin = EnumLiteralNode->FindPinChecked(UK2Node_EnumLiteral::GetEnumInputPinName());
					check( InPin );
					InPin->DefaultValue = OrgPin->DefaultValue;
				}
				else
				{
					// For non-array struct pins that are not linked, transfer the pin type so that the node will expand an auto-ref that will assign the value by-ref.
					if (OrgPin->PinType.IsArray() == false && OrgPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct && OrgPin->LinkedTo.Num() == 0)
					{
						ValuePin->PinType.PinCategory = OrgPin->PinType.PinCategory;
						ValuePin->PinType.PinSubCategory = OrgPin->PinType.PinSubCategory;
						ValuePin->PinType.PinSubCategoryObject = OrgPin->PinType.PinSubCategoryObject;
						CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
					}
					else if(OrgPin->Direction != EGPD_Output)
					{
						CompilerContext.MovePinLinksToIntermediate(*OrgPin, *ValuePin);
						SetVarNode->PinConnectionListChanged(ValuePin);
					}

				}
			}
		}
	}

	return LastThen;
}

void UK2Node_MounteaTimelineV2::GeneratePinTooltip(UEdGraphPin& Pin) const
{
	ensure(Pin.GetOwningNode() == this);

	UEdGraphSchema const* Schema = GetSchema();
	check(Schema);
	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(Schema);

	if (K2Schema == nullptr)
	{
		Schema->ConstructBasicPinTooltip(Pin, FText::GetEmpty(), Pin.PinToolTip);
		return;
	}
}

TMap<FName, FAsyncTaskFromUClassPinRedirectMapInfo> UK2Node_MounteaTimelineV2::AsyncTaskPinRedirectMap;
bool UK2Node_MounteaTimelineV2::bAsyncTaskPinRedirectMapInitialized = false;

#undef LOCTEXT_NAMESPACE

// All rights reserved Dominik Pavlicek 2022.


#include "Library/MounteaFunctionLibrary.h"

#include "Helpers/MounteaTimeline.h"

UObject* UMounteaFunctionLibrary::MakeMounteaTimeline(UObject* Outer, TSubclassOf<UObject> Class)
{
	//TSubclassOf<UObject> Class = UMounteaTimeline::StaticClass();
	if (IsValid(Outer) && Class && !Class->HasAnyClassFlags(CLASS_Abstract))
	{
		const FName TaskObjName = MakeUniqueObjectName(Outer, Class, Class->GetFName());
		const auto Task = NewObject<UObject>(Outer, Class, TaskObjName, RF_NoFlags);
		//if (Outer) Task->SetWorldContext(Outer);
		return Task;
	}
	return nullptr;
}

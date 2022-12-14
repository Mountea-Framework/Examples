// All rights reserved Dominik Pavlicek 2022.


#include "Helpers/MounteaTimeline.h"
#include "CoreMinimal.h"
#include "Stats/Stats.h"
#include "UObject/Class.h"
#include "UObject/CoreNet.h"
#include "UObject/UnrealType.h"
#include "Curves/CurveLinearColor.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveFloat.h"
#include "UObject/Package.h"
#include "GameFramework/WorldSettings.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "ProfilingDebugging/CsvProfiler.h"
#include "Misc/App.h"

#include "Helpers/MounteaToolsLibraryLog.h"

#pragma region DefinitionStructs
/*
UEnum* FMounteaTimeline::GetTimelineDirectionEnum()
{
	static UEnum* TimelineDirectionEnum = NULL;
	if(NULL == TimelineDirectionEnum)
	{
		FName TimelineDirectionEnumName(TEXT("EMounteaTimelineDirection::Forward"));
		UEnum::LookupEnumName(TimelineDirectionEnumName, &TimelineDirectionEnum);
		check(TimelineDirectionEnum);
	}
	return TimelineDirectionEnum;
}

void FMounteaTimeline::Play()
{
	bReversePlayback = false;
	bPlaying = true;
}

void FMounteaTimeline::PlayFromStart()
{
	SetPlaybackPosition(0.f, false);
	Play();
}

void FMounteaTimeline::Reverse()
{
	bReversePlayback = true;
	bPlaying = true;
}

void FMounteaTimeline::ReverseFromEnd()
{
	SetPlaybackPosition(GetTimelineLength(), false);
	Reverse();
}

void FMounteaTimeline::Stop()
{
	bPlaying = false;
}

bool FMounteaTimeline::IsPlaying() const
{
	return bPlaying;
}

bool FMounteaTimeline::IsReversing() const
{
	return bPlaying && bReversePlayback;
}

void FMounteaTimeline::SetPlaybackPosition(float NewPosition, bool bFireEvents, bool bFireUpdate)
{
	float OldPosition = Position;
	Position = NewPosition;

	UObject* PropSetObject = PropertySetObject.Get();

	// Iterate over each vector interpolation
	for(int32 InterpIdx=0; InterpIdx<InterpVectors.Num(); InterpIdx++)
	{
		FMounteaTimelineVectorTrack& VecEntry = InterpVectors[InterpIdx];
		if ( VecEntry.VectorCurve && (VecEntry.InterpFunc.IsBound() || VecEntry.VectorPropertyName != NAME_None || VecEntry.InterpFuncStatic.IsBound()) )
		{
			// Get vector from curve
			FVector const Vec = VecEntry.VectorCurve->GetVectorValue(Position);

			// Pass vec to specified function
			VecEntry.InterpFunc.ExecuteIfBound(Vec);

			// Set vector property
			if (PropSetObject)
			{
				if (VecEntry.VectorProperty == NULL)
				{
					VecEntry.VectorProperty = FindFProperty<FStructProperty>(PropSetObject->GetClass(), VecEntry.VectorPropertyName);
					if(VecEntry.VectorProperty == NULL)
					{
						MTL_LOG(Warning, TEXT("[Mountea Timeline] SetPlaybackPosition: No vector property '%s' in '%s'"), *VecEntry.VectorPropertyName.ToString(), *PropSetObject->GetName());
					}
				}
				if (VecEntry.VectorProperty)
				{
					*VecEntry.VectorProperty->ContainerPtrToValuePtr<FVector>(PropSetObject) = Vec;
				}
			}

			// Pass vec to non-dynamic version of the specified function
			VecEntry.InterpFuncStatic.ExecuteIfBound(Vec);
		}
	}

	// Iterate over each float interpolation
	for(int32 InterpIdx=0; InterpIdx<InterpFloats.Num(); InterpIdx++)
	{
		FMounteaTimelineFloatTrack& FloatEntry = InterpFloats[InterpIdx];
		if( FloatEntry.FloatCurve && (FloatEntry.InterpFunc.IsBound() || FloatEntry.FloatPropertyName != NAME_None || FloatEntry.InterpFuncStatic.IsBound()) )
		{
			// Get float from func
			const float Val = FloatEntry.FloatCurve->GetFloatValue(Position);

			// Pass float to specified function
			FloatEntry.InterpFunc.ExecuteIfBound(Val);

			// Set float property
			if (PropSetObject)
			{
				if (FloatEntry.FloatProperty == NULL)
				{
					FloatEntry.FloatProperty = FindFProperty<FFloatProperty>(PropSetObject->GetClass(), FloatEntry.FloatPropertyName);
					if(FloatEntry.FloatProperty == NULL)
					{
						MTL_LOG(Warning, TEXT("[Mountea Timeline] SetPlaybackPosition: No float property '%s' in '%s'"), *FloatEntry.FloatPropertyName.ToString(), *PropSetObject->GetName());
					}
				}
				if (FloatEntry.FloatProperty)
				{
					FloatEntry.FloatProperty->SetPropertyValue_InContainer(PropSetObject, Val);
				}
			}
			
			// Pass float to non-dynamic version of the specified function
			FloatEntry.InterpFuncStatic.ExecuteIfBound(Val);
		}
	}

	// Iterate over each color interpolation
	for(int32 InterpIdx=0; InterpIdx<InterpLinearColors.Num(); InterpIdx++)
	{
		FMounteaTimelineLinearColorTrack& ColorEntry = InterpLinearColors[InterpIdx];
		if ( ColorEntry.LinearColorCurve && (ColorEntry.InterpFunc.IsBound() || ColorEntry.LinearColorPropertyName != NAME_None || ColorEntry.InterpFuncStatic.IsBound()) )
		{
			// Get vector from curve
			const FLinearColor Color = ColorEntry.LinearColorCurve->GetLinearColorValue(Position);

			// Pass vec to specified function
			ColorEntry.InterpFunc.ExecuteIfBound(Color);

			// Set vector property
			if (PropSetObject)
			{
				if (ColorEntry.LinearColorProperty == NULL)
				{
					ColorEntry.LinearColorProperty = FindFProperty<FStructProperty>(PropSetObject->GetClass(), ColorEntry.LinearColorPropertyName);
					if(ColorEntry.LinearColorProperty == NULL)
					{
						MTL_LOG(Warning, TEXT("[Mountea Timeline] SetPlaybackPosition: No linear color property '%s' in '%s'"), *ColorEntry.LinearColorPropertyName.ToString(), *PropSetObject->GetName());
					}
				}
				if (ColorEntry.LinearColorProperty)
				{
					*ColorEntry.LinearColorProperty->ContainerPtrToValuePtr<FLinearColor>(PropSetObject) = Color;
				}
			}

			// Pass vec to non-dynamic version of the specified function
			ColorEntry.InterpFuncStatic.ExecuteIfBound(Color);
		}
	}

	if(DirectionPropertyName != NAME_None)
	{
		if (PropSetObject)
		{
			if (DirectionProperty == nullptr)
			{
				DirectionProperty = FindFProperty<FByteProperty>(PropSetObject->GetClass(), DirectionPropertyName);
				if (DirectionProperty == nullptr)
				{
					DirectionProperty = FindFProperty<FEnumProperty>(PropSetObject->GetClass(), DirectionPropertyName);
				}

				if (DirectionProperty == nullptr)
				{
					MTL_LOG(Warning, TEXT("[Mountea Timeline] SetPlaybackPosition: No direction property '%s' in '%s'"), *DirectionPropertyName.ToString(), *PropSetObject->GetName());
				}
			}
			if (DirectionProperty)
			{
				const EMounteaTimelineDirection::Type CurrentDirection = bReversePlayback ? EMounteaTimelineDirection::Backward : EMounteaTimelineDirection::Forward;
				TEnumAsByte<EMounteaTimelineDirection::Type> ValueAsByte(CurrentDirection);
				if (FByteProperty* ByteDirection = CastField<FByteProperty>(DirectionProperty))
				{
					ByteDirection->SetPropertyValue_InContainer(PropSetObject, ValueAsByte);
				}
				else
				{
					FEnumProperty* EnumProp = CastFieldChecked<FEnumProperty>(DirectionProperty);
					void* PropAddr = EnumProp->ContainerPtrToValuePtr<void>(PropSetObject);
					FNumericProperty* UnderlyingProp = EnumProp->GetUnderlyingProperty();
					UnderlyingProp->SetIntPropertyValue(PropAddr, (int64)ValueAsByte);
				}
			}
		}
	}


	// If we should be firing events for this track...
	if (bFireEvents)
	{
		// If playing sequence forwards.
		float MinTime, MaxTime;
		if (!bReversePlayback)
		{
			MinTime = OldPosition;
			MaxTime = Position;

			// Slight hack here.. if playing forwards and reaching the end of the sequence, force it over a little to ensure we fire events actually on the end of the sequence.
			if (MaxTime == GetTimelineLength())
			{
				MaxTime += (float)KINDA_SMALL_NUMBER;
			}
		}
		// If playing sequence backwards.
		else
		{
			MinTime = Position;
			MaxTime = OldPosition;

			// Same small hack as above for backwards case.
			if (MinTime == 0.f)
			{
				MinTime -= (float)KINDA_SMALL_NUMBER;
			}
		}

		// See which events fall into traversed region.
		for (int32 i = 0; i < Events.Num(); i++)
		{
			float EventTime = Events[i].Time;

			// Need to be slightly careful here and make behavior for firing events symmetric when playing forwards of backwards.
			bool bFireThisEvent = false;
			if (!bReversePlayback)
			{
				if (EventTime >= MinTime && EventTime < MaxTime)
				{
					bFireThisEvent = true;
				}
			}
			else
			{
				if (EventTime > MinTime && EventTime <= MaxTime)
				{
					bFireThisEvent = true;
				}
			}

			if (bFireThisEvent)
			{
				Events[i].EventFunc.ExecuteIfBound();
			}
		}
	}

	// Execute the delegate to say that all properties are updated
	if (bFireUpdate)
	{
		TimelinePostUpdateFunc.ExecuteIfBound();
	}
}

float FMounteaTimeline::GetPlaybackPosition() const
{
	return Position;
}

void FMounteaTimeline::SetLooping(bool bNewLooping)
{
	bLooping = bNewLooping;
}

bool FMounteaTimeline::IsLooping() const
{
	return bLooping;
}

void FMounteaTimeline::SetPlayRate(float NewRate)
{
	PlayRate = NewRate;
}

float FMounteaTimeline::GetPlayRate() const
{
	return PlayRate;
}

void FMounteaTimeline::SetNewTime(float NewTime)
{
	// Ensure value is sensible
	NewTime = FMath::Clamp<float>(NewTime, 0.0f, Length);
	SetPlaybackPosition(NewTime, false);
}

float FMounteaTimeline::GetTimelineLength() const
{
	switch (LengthMode)
	{
		case MTL_TimelineLength:
			return Length;
		case MTL_LastKeyFrame:
			return GetLastKeyframeTime();
		default:
			MTL_LOG(Error, TEXT("[Mountea Timeline] Invalid timeline length mode on timeline!"));
			return 0.f;
	}
}

void FMounteaTimeline::SetTimelineLengthMode(EMounteaTimelineLengthMode NewMode)
{
	LengthMode = NewMode;
}

void FMounteaTimeline::SetTimelineLength(float NewLength)
{
	Length = NewLength;
	if(Position > NewLength)
	{
		SetNewTime(NewLength-KINDA_SMALL_NUMBER);
	}
}

void FMounteaTimeline::SetFloatCurve(UCurveFloat* NewFloatCurve, FName FloatTrackName)
{
	bool bFoundTrack = false;
	if (FloatTrackName != NAME_None)
	{
		for (FMounteaTimelineFloatTrack& FloatTrack : InterpFloats)
		{
			if (FloatTrack.TrackName == FloatTrackName)
			{
				FloatTrack.FloatCurve = NewFloatCurve;
				bFoundTrack = true;
				break;
			}
		}
	}

	if(!bFoundTrack)
	{
		MTL_LOG(Log, TEXT("[Mountea Timeline] SetFloatCurve: No float track with name %s!"), *FloatTrackName.ToString());
	}
}

void FMounteaTimeline::SetVectorCurve(UCurveVector* NewVectorCurve, FName VectorTrackName)
{
	bool bFoundTrack = false;
	if (VectorTrackName != NAME_None)
	{
		for (FMounteaTimelineVectorTrack& VectorTrack : InterpVectors)
		{
			if (VectorTrack.TrackName == VectorTrackName)
			{
				VectorTrack.VectorCurve = NewVectorCurve;
				bFoundTrack = true;
				break;
			}
		}
	}

	if (!bFoundTrack)
	{
		MTL_LOG(Log, TEXT("[Mountea Timeline] SetVectorCurve: No vector track with name %s!"), *VectorTrackName.ToString());
	}
}

void FMounteaTimeline::SetLinearColorCurve(UCurveLinearColor* NewLinearColorCurve, FName LinearColorTrackName)
{
	bool bFoundTrack = false;
	if (LinearColorTrackName != NAME_None)
	{
		for (FMounteaTimelineLinearColorTrack& ColorTrack : InterpLinearColors)
		{
			if (ColorTrack.TrackName == LinearColorTrackName)
			{
				ColorTrack.LinearColorCurve = NewLinearColorCurve;
				bFoundTrack = true;
				break;
			}

		}
	}

	if (!bFoundTrack)
	{
		MTL_LOG(Log, TEXT("[Mountea Timeline] SetLinearColorCurve: No color track with name %s!"), *LinearColorTrackName.ToString());
	}
}

void FMounteaTimeline::SetPropertySetObject(UObject* NewPropertySetObject)
{
	PropertySetObject = NewPropertySetObject;
}

void FMounteaTimeline::SetTimelinePostUpdateFunc(FOnMounteaTimelineEvent NewTimelinePostUpdateFunc)
{
	TimelinePostUpdateFunc = NewTimelinePostUpdateFunc;
}

void FMounteaTimeline::SetTimelineFinishedFunc(FOnMounteaTimelineEvent NewTimelineFinishedFunc)
{
	TimelineFinishedFunc = NewTimelineFinishedFunc;
}

void FMounteaTimeline::SetTimelineFinishedFunc(FOnMounteaTimelineEventStatic NewTimelineFinishedFunc)
{
	TimelineFinishFuncStatic = NewTimelineFinishedFunc;
}

void FMounteaTimeline::AddEvent(float Time, FOnMounteaTimelineEvent EventFunc)
{
	FMounteaTimelineEventEntry NewEntry;
	NewEntry.Time = Time;
	NewEntry.EventFunc = EventFunc;

	Events.Add(NewEntry);
}

void FMounteaTimeline::AddInterpVector(UCurveVector* VectorCurve, FOnMounteaTimelineVector InterpFunc, FName PropertyName, FName TrackName)
{
	FMounteaTimelineVectorTrack NewEntry;
	NewEntry.VectorCurve = VectorCurve;
	NewEntry.InterpFunc = InterpFunc;
	NewEntry.TrackName = TrackName;
	NewEntry.VectorPropertyName = PropertyName;

	InterpVectors.Add(NewEntry);
}

void FMounteaTimeline::AddInterpVector(UCurveVector* VectorCurve, FOnMounteaTimelineVectorStatic InterpFunc)
{
	FMounteaTimelineVectorTrack NewEntry;
	NewEntry.VectorCurve = VectorCurve;
	NewEntry.InterpFuncStatic = InterpFunc;

	InterpVectors.Add(NewEntry);
}

void FMounteaTimeline::AddInterpFloat(UCurveFloat* FloatCurve, FOnMounteaTimelineFloat InterpFunc, FName PropertyName, FName TrackName)
{
	FMounteaTimelineFloatTrack NewEntry;
	NewEntry.FloatCurve = FloatCurve;
	NewEntry.InterpFunc = InterpFunc;
	NewEntry.TrackName = TrackName;
	NewEntry.FloatPropertyName = PropertyName;

	InterpFloats.Add(NewEntry);
}

void FMounteaTimeline::AddInterpFloat(UCurveFloat* FloatCurve, FOnMounteaTimelineFloatStatic InterpFunc)
{
	FMounteaTimelineFloatTrack NewEntry;
	NewEntry.FloatCurve = FloatCurve;
	NewEntry.InterpFuncStatic = InterpFunc;

	InterpFloats.Add(NewEntry);
}

void FMounteaTimeline::AddInterpLinearColor(UCurveLinearColor* LinearColorCurve, FOnMounteaTimelineLinearColor InterpFunc, FName PropertyName, FName TrackName)
{
	FMounteaTimelineLinearColorTrack NewEntry;
	NewEntry.LinearColorCurve = LinearColorCurve;
	NewEntry.InterpFunc = InterpFunc;
	NewEntry.TrackName = TrackName;
	NewEntry.LinearColorPropertyName = PropertyName;

	InterpLinearColors.Add(NewEntry);
}

void FMounteaTimeline::AddInterpLinearColor(UCurveLinearColor* LinearColorCurve, FOnMounteaTimelineLinearColorStatic InterpFunc)
{
	FMounteaTimelineLinearColorTrack NewEntry;
	NewEntry.LinearColorCurve = LinearColorCurve;
	NewEntry.InterpFuncStatic = InterpFunc;

	InterpLinearColors.Add(NewEntry);
}

void FMounteaTimeline::TickTimeline(float DeltaTime)
{
	bool bIsFinished = false;

	if(bPlaying)
	{
		const float TimelineLength = GetTimelineLength();
		float EffectiveDeltaTime = DeltaTime * (bReversePlayback ? (-PlayRate) : (PlayRate));

		float NewPosition = Position + EffectiveDeltaTime;

		if(EffectiveDeltaTime > 0.0f)
		{
			if(NewPosition > TimelineLength)
			{
				// If looping, play to end, jump to start, and set target to somewhere near the beginning.
				if(bLooping)
				{
					SetPlaybackPosition(TimelineLength, true, false);
					SetPlaybackPosition(0.f, false, false);

					if( TimelineLength > 0.f )
					{
						while(NewPosition > TimelineLength)
						{
							NewPosition -= TimelineLength;
						}
					}
					else
					{
						NewPosition = 0.f;
					}
				}
				// If not looping, snap to end and stop playing.
				else
				{
					NewPosition = TimelineLength;
					Stop();
					bIsFinished = true;
				}
			}
		}
		else
		{
			if(NewPosition < 0.f)
			{
				// If looping, play to start, jump to end, and set target to somewhere near the end.
				if(bLooping)
				{
					SetPlaybackPosition(0.f, true, false);
					SetPlaybackPosition(TimelineLength, false, false);

					if( TimelineLength > 0.f )
					{
						while(NewPosition < 0.f)
						{
							NewPosition += TimelineLength;
						}
					}
					else
					{
						NewPosition = 0.f;
					}
				}
				// If not looping, snap to start and stop playing.
				else
				{
					NewPosition = 0.f;
					Stop();
					bIsFinished = true;
				}
			}
		}

		SetPlaybackPosition(NewPosition, true);
	}

	// Notify user that timeline finished
	if (bIsFinished) 
	{
		TimelineFinishedFunc.ExecuteIfBound();
		TimelineFinishFuncStatic.ExecuteIfBound();
	}
}

void FMounteaTimeline::SetDirectionPropertyName(FName InDirectionPropertyName)
{
	DirectionPropertyName = InDirectionPropertyName;
}

void FMounteaTimeline::GetAllCurves(TSet<UCurveBase*>& InOutCurves) const
{
	for (auto& Track : InterpVectors)
	{
		InOutCurves.Add(Track.VectorCurve);
	}
	for (auto& Track : InterpFloats)
	{
		InOutCurves.Add(Track.FloatCurve);
	}
	for (auto& Track : InterpLinearColors)
	{
		InOutCurves.Add(Track.LinearColorCurve);
	}
}

float FMounteaTimeline::GetLastKeyframeTime() const
{
	switch (LengthMode)
	{
		case MTL_TimelineLength:
			return Length;
		case MTL_LastKeyFrame:
			return GetLastKeyframeTime();
		default:
			MTL_LOG(Error, TEXT("[Mountea Timeline] Invalid timeline length mode on timeline!"));
			return 0.f;
	}
}
*/
#pragma endregion

#pragma region DefinitionObjects

void UMounteaTimelineObject::Play()
{
	Activate();
	TheTimeline.Play();
}

void UMounteaTimelineObject::PlayFromStart()
{
	Activate();
	TheTimeline.PlayFromStart();
}

void UMounteaTimelineObject::Reverse()
{
	Activate();
	TheTimeline.Reverse();
}

void UMounteaTimelineObject::ReverseFromEnd()
{
	Activate();
	TheTimeline.ReverseFromEnd();
}

void UMounteaTimelineObject::Stop()
{
	TheTimeline.Stop();
}

bool UMounteaTimelineObject::IsPlaying() const
{
	return TheTimeline.IsPlaying();
}

bool UMounteaTimelineObject::IsReversing() const
{
	return TheTimeline.IsReversing();
}

void UMounteaTimelineObject::SetPlaybackPosition(float NewPosition, bool bFireEvents, bool bFireUpdate)
{
	if (!TheTimeline.IsPlaying())
	{
		// make sure a final update call occurs on the client for the final position
		// FIXME: this is incomplete, we need to compare vs the last simulated position for firing events and such
		TheTimeline.SetPlaybackPosition(TheTimeline.GetPlaybackPosition(), false, true);
	}
}

float UMounteaTimelineObject::GetPlaybackPosition() const
{
	return TheTimeline.GetPlaybackPosition();
}

void UMounteaTimelineObject::SetLooping(bool bNewLooping)
{
	TheTimeline.SetLooping(bNewLooping);
}

bool UMounteaTimelineObject::IsLooping() const
{
	return TheTimeline.IsLooping();
}

void UMounteaTimelineObject::SetPlayRate(float NewRate)
{
	TheTimeline.SetPlayRate(NewRate);
}

float UMounteaTimelineObject::GetPlayRate() const
{
	return TheTimeline.GetPlayRate();
}

void UMounteaTimelineObject::SetNewTime(float NewTime)
{
	TheTimeline.SetNewTime(NewTime);
}

float UMounteaTimelineObject::GetTimelineLength() const
{
	return TheTimeline.GetTimelineLength();
}

void UMounteaTimelineObject::SetTimelineLength(float NewLength)
{
	return TheTimeline.SetTimelineLength(NewLength);
}

void UMounteaTimelineObject::SetTimelineLengthMode(ETimelineLengthMode NewLengthMode)
{
	TheTimeline.SetTimelineLengthMode(NewLengthMode);
}

void UMounteaTimelineObject::SetIgnoreTimeDilation(bool bNewIgnoreTimeDilation)
{
	bIgnoreTimeDilation = bNewIgnoreTimeDilation;
}

bool UMounteaTimelineObject::GetIgnoreTimeDilation() const
{
	return bIgnoreTimeDilation;
}

void UMounteaTimelineObject::SetFloatCurve(UCurveFloat* NewFloatCurve, FName FloatTrackName)
{
	TheTimeline.SetFloatCurve(NewFloatCurve, FloatTrackName);
}

void UMounteaTimelineObject::SetVectorCurve(UCurveVector* NewVectorCurve, FName VectorTrackName)
{
	TheTimeline.SetVectorCurve(NewVectorCurve, VectorTrackName);
}

void UMounteaTimelineObject::SetLinearColorCurve(UCurveLinearColor* NewLinearColorCurve, FName LinearColorTrackName)
{
	TheTimeline.SetLinearColorCurve(NewLinearColorCurve, LinearColorTrackName);
}

void UMounteaTimelineObject::OnRep_Timeline()
{
	if (!TheTimeline.IsPlaying())
	{
		// make sure a final update call occurs on the client for the final position
		// FIXME: this is incomplete, we need to compare vs the last simulated position for firing events and such
		TheTimeline.SetPlaybackPosition(TheTimeline.GetPlaybackPosition(), false, true);
	}
}

UMounteaTimelineObject::UMounteaTimelineObject(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIsActive = true;
	bCanEverTick = true;
	bCanTick = false;

	TickableType = ETickableTickType::Always;
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bTickEvenWhenPaused = false;
	//PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void UMounteaTimelineObject::Tick(float DeltaTime)
{
	if (CanTick())
	{
		if (bIgnoreTimeDilation)
		{
			// Get the raw, undilated delta time.
			DeltaTime = FApp::GetDeltaTime();
			const UWorld* World = GetWorld();
			if (const AWorldSettings* WorldSettings = World ? World->GetWorldSettings() : nullptr)
			{
				// Clamp DeltaTime in the same way as before.
				// UWorld::Tick called AWorldSettings::FixupDeltaSeconds, which clamped between Min and MaxUndilatedFrameTime.
				DeltaTime = FMath::Clamp(DeltaTime, WorldSettings->MinUndilatedFrameTime, WorldSettings->MaxUndilatedFrameTime);
			}
		}

		TheTimeline.TickTimeline(DeltaTime);

		// Do not deactivate if we are done, since bActive is a replicated property and we shouldn't have simulating
		// clients touch replicated variables.
		if (!TheTimeline.IsPlaying())
		{
			Deactivate();
		}

		LastFrameNumberWeTicked = GFrameCounter;
	}
}

bool UMounteaTimelineObject::ShouldActivate() const
{
	return !bIsActive;
}

bool UMounteaTimelineObject::CanTick() const
{
	return bIsActive && bCanTick && LastFrameNumberWeTicked != GFrameCounter;
}

TStatId UMounteaTimelineObject::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT( UMounteaTimelineObject, STATGROUP_Tickables );
}

ETickableTickType UMounteaTimelineObject::GetTickableTickType() const
{
	return TickableType;
}

bool UMounteaTimelineObject::IsTickableWhenPaused() const
{
	return false;
}

bool UMounteaTimelineObject::IsTickableInEditor() const
{
	if (!GWorld) return false;
	return (!IsTemplate(RF_ClassDefaultObject)) && !(GIsEditor && !GWorld->HasBegunPlay());
}

void UMounteaTimelineObject::Activate(bool bReset)
{
	if (bReset || ShouldActivate()==true)
	{
		bCanTick = true;
		bIsActive = true;
	}
}

void UMounteaTimelineObject::Deactivate()
{
	if (ShouldActivate())
	{
		bCanTick = false;
		bIsActive = false;
	}
}

bool UMounteaTimelineObject::IsReadyForAutoDestroy() const
{
	return !IsPlaying();
}

bool UMounteaTimelineObject::IsPostLoadThreadSafe() const
{
	// WTF
	return true;
}

UFunction* UMounteaTimelineObject::GetTimelineEventSignature()
{
	UFunction* TimelineEventSig = FindObject<UFunction>(FindPackage(nullptr, TEXT("/Script/Engine")), TEXT("OnTimelineEvent__DelegateSignature"));
	check(TimelineEventSig != NULL);
	return TimelineEventSig;
}

UFunction* UMounteaTimelineObject::GetTimelineFloatSignature()
{
	UFunction* TimelineFloatSig = FindObject<UFunction>(FindPackage(nullptr, TEXT("/Script/Engine")), TEXT("OnTimelineFloat__DelegateSignature"));
	check(TimelineFloatSig != NULL);
	return TimelineFloatSig;
}

UFunction* UMounteaTimelineObject::GetTimelineVectorSignature()
{
	UFunction* TimelineVectorSig = FindObject<UFunction>(FindPackage(nullptr, TEXT("/Script/Engine")), TEXT("OnTimelineVector__DelegateSignature"));
	check(TimelineVectorSig != NULL);
	return TimelineVectorSig;
}

UFunction* UMounteaTimelineObject::GetTimelineLinearColorSignature()
{
	UFunction* TimelineVectorSig = FindObject<UFunction>(FindPackage(nullptr, TEXT("/Script/Engine")), TEXT("OnTimelineLinearColor__DelegateSignature"));
	check(TimelineVectorSig != NULL);
	return TimelineVectorSig;
}

ETimelineSigType UMounteaTimelineObject::GetTimelineSignatureForFunction(const UFunction* InFunc)
{
	if(InFunc != nullptr)
	{
		if(InFunc->IsSignatureCompatibleWith(GetTimelineEventSignature()))
		{
			return ETS_EventSignature;
		}
		else if(InFunc->IsSignatureCompatibleWith(GetTimelineFloatSignature()))
		{
			return ETS_FloatSignature;
		}
		else if(InFunc->IsSignatureCompatibleWith(GetTimelineVectorSignature()))
		{
			return ETS_VectorSignature;
		}
		else if(InFunc->IsSignatureCompatibleWith(GetTimelineLinearColorSignature()))
		{
			return ETS_LinearColorSignature;
		}
	}

	return ETS_InvalidSignature;
}

void UMounteaTimelineObject::AddEvent(float Time, FOnTimelineEvent EventFunc)
{
	TheTimeline.AddEvent(Time, EventFunc);
}

void UMounteaTimelineObject::AddInterpVector(UCurveVector* VectorCurve, FOnTimelineVector InterpFunc, FName PropertyName, FName TrackName)
{
	TheTimeline.AddInterpVector(VectorCurve, InterpFunc, PropertyName, TrackName);
}

void UMounteaTimelineObject::AddInterpFloat(UCurveFloat* FloatCurve, FOnTimelineFloat InterpFunc, FName PropertyName, FName TrackName)
{
	TheTimeline.AddInterpFloat(FloatCurve, InterpFunc, PropertyName, TrackName);
}

void UMounteaTimelineObject::AddInterpLinearColor(UCurveLinearColor* LinearColorCurve, FOnTimelineLinearColor InterpFunc, FName PropertyName, FName TrackName)
{
	TheTimeline.AddInterpLinearColor(LinearColorCurve, InterpFunc, PropertyName, TrackName);
}

void UMounteaTimelineObject::SetPropertySetObject(UObject* NewPropertySetObject)
{
	TheTimeline.SetPropertySetObject(NewPropertySetObject);
}

void UMounteaTimelineObject::SetTimelinePostUpdateFunc(FOnTimelineEvent NewTimelinePostUpdateFunc)
{
	TheTimeline.SetTimelinePostUpdateFunc(NewTimelinePostUpdateFunc);
}

void UMounteaTimelineObject::SetTimelineFinishedFunc(FOnTimelineEvent NewTimelineFinishedFunc)
{
	TheTimeline.SetTimelineFinishedFunc(NewTimelineFinishedFunc);
}

void UMounteaTimelineObject::SetTimelineFinishedFunc(FOnTimelineEventStatic NewTimelineFinishedFunc)
{
	TheTimeline.SetTimelineFinishedFunc(NewTimelineFinishedFunc);
}

void UMounteaTimelineObject::SetDirectionPropertyName(FName DirectionPropertyName)
{
	TheTimeline.SetDirectionPropertyName(DirectionPropertyName);
}

void UMounteaTimelineObject::GetAllCurves(TSet<UCurveBase*>& InOutCurves) const
{
	//TheTimeline.GetAllCurves(InOutCurves);
}

void UMounteaTimelineObject::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( UMounteaTimelineObject, TheTimeline );
}

#pragma endregion 

// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "MounteaTimeline.generated.h"

class UCurveVector;
struct FTimeline;

#pragma region DeclarationDelegates

/** Signature of function to handle a timeline 'event' */
DECLARE_DYNAMIC_DELEGATE( FOnMounteaTimelineEvent );
/** Signature of function to handle timeline float track */
DECLARE_DYNAMIC_DELEGATE_OneParam( FOnMounteaTimelineFloat, float, Output );
/** Signature of function to handle timeline vector track */
DECLARE_DYNAMIC_DELEGATE_OneParam( FOnMounteaTimelineVector, FVector, Output );
/** Signature of function to handle linear color track */
DECLARE_DYNAMIC_DELEGATE_OneParam( FOnMounteaTimelineLinearColor, FLinearColor, Output );

/** Static version of delegate to handle a timeline 'event' */
DECLARE_DELEGATE( FOnMounteaTimelineEventStatic );
/** Static version of timeline delegate for a float track */
DECLARE_DELEGATE_OneParam( FOnMounteaTimelineFloatStatic, float );
/** Static version of timeline delegate for a vector track */
DECLARE_DELEGATE_OneParam( FOnMounteaTimelineVectorStatic, FVector );
/** Static version of timeline delegate for a linear color track */
DECLARE_DELEGATE_OneParam( FOnMounteaTimelineLinearColorStatic, FLinearColor );

#pragma endregion 

#pragma region DeclarationEnums

/** Whether or not the timeline should be finished after the specified length, or the last keyframe in the tracks */
UENUM()
enum EMounteaTimelineLengthMode
{
	MTL_TimelineLength,
	MTL_LastKeyFrame
};

/** Does timeline play or reverse ? */
UENUM(BlueprintType)
namespace EMounteaTimelineDirection
{
	enum Type
	{
		Forward,
		Backward
	};
}

#pragma endregion 

#pragma region DeclarationStructs

/** Struct that contains one entry for an 'event' during the timeline */
USTRUCT()
struct FMounteaTimelineEventEntry
{
	GENERATED_USTRUCT_BODY()

	/** Time at which event should fire */
	UPROPERTY()
	float Time;

	/** Function to execute when Time is reached */
	UPROPERTY()
	FOnMounteaTimelineEvent EventFunc;


	FMounteaTimelineEventEntry()
		: Time(0)
	{
	}

};

/** Struct that contains one entry for each vector interpolation performed by the timeline */
USTRUCT()
struct FMounteaTimelineFloatTrack
{
	GENERATED_USTRUCT_BODY()

	/** Float curve to be evaluated */
	UPROPERTY()
	UCurveFloat* FloatCurve;

	/** Function that the output from ValueCurve will be passed to */
	UPROPERTY()
	FOnMounteaTimelineFloat InterpFunc;

	/** Name of track, usually set in Timeline Editor. Used by SetInterpFloatCurve function. */
	UPROPERTY()
	FName TrackName;

	/** Name of property that we should update from this curve */
	UPROPERTY()
	FName FloatPropertyName;

	/** Cached float property pointer */
	FFloatProperty* FloatProperty;

	/** Static version of FOnTimelineFloat, for use with non-UObjects */
	FOnMounteaTimelineFloatStatic InterpFuncStatic;

	FMounteaTimelineFloatTrack()
		: FloatCurve(NULL)
		, FloatPropertyName(NAME_None)
		, FloatProperty(NULL)
	{
	}

};

/** Struct that contains one entry for each vector interpolation performed by the timeline */
USTRUCT()
struct FMounteaTimelineVectorTrack
{
	GENERATED_USTRUCT_BODY()

	/** Vector curve to be evaluated */
	UPROPERTY()
	UCurveVector* VectorCurve;

	/** Function that the output from ValueCurve will be passed to */
	UPROPERTY()
	FOnMounteaTimelineVector InterpFunc;

	/** Name of track, usually set in Timeline Editor. Used by SetInterpVectorCurve function. */
	UPROPERTY()
	FName TrackName;

	/** Name of property that we should update from this curve */
	UPROPERTY()
	FName VectorPropertyName;

	/** Cached vector struct property pointer */
	FStructProperty* VectorProperty;

	/** Static version of FOnTimelineVector, for use with non-UObjects  */
	FOnMounteaTimelineVectorStatic InterpFuncStatic;

	FMounteaTimelineVectorTrack()
		: VectorCurve(NULL)
		, VectorPropertyName(NAME_None)
		, VectorProperty(NULL)
	{
	}

};

/** Struct that contains one entry for each linear color interpolation performed by the timeline */
USTRUCT()
struct FMounteaTimelineLinearColorTrack
{
	GENERATED_USTRUCT_BODY()

	/** Float curve to be evaluated */
	UPROPERTY()
	UCurveLinearColor* LinearColorCurve;

	/** Function that the output from ValueCurve will be passed to */
	UPROPERTY()
	FOnMounteaTimelineLinearColor InterpFunc;

	/** Name of track, usually set in Timeline Editor. Used by SetInterpLinearColorCurve function. */
	UPROPERTY()
	FName TrackName;

	/** Name of property that we should update from this curve */
	UPROPERTY()
	FName LinearColorPropertyName;

	/** Cached linear color struct property pointer */
	FStructProperty* LinearColorProperty;

	/** Static version of FOnTimelineFloat, for use with non-UObjects */
	FOnMounteaTimelineLinearColorStatic InterpFuncStatic;

	FMounteaTimelineLinearColorTrack()
		: LinearColorCurve(NULL)
		, LinearColorPropertyName(NAME_None)
		, LinearColorProperty(NULL)
	{
	}

};

USTRUCT()
struct FMounteaTimeline
{
	GENERATED_USTRUCT_BODY()
		
private:
	/** Specified how the timeline determines its own length (e.g. specified length, last keyframe) */
	UPROPERTY(NotReplicated)
	TEnumAsByte<EMounteaTimelineLengthMode> LengthMode;

	/** Whether timeline should loop when it reaches the end, or stop */
	UPROPERTY()
	uint8 bLooping:1;

	/** If playback should move the current position backwards instead of forwards */
	UPROPERTY()
	uint8 bReversePlayback:1;

	/** Are we currently playing (moving Position) */
	UPROPERTY()
	uint8 bPlaying:1;

	/** How long the timeline is, will stop or loop at the end */
 	UPROPERTY(NotReplicated)
 	float Length;

	/** How fast we should play through the timeline */
	UPROPERTY()
	float PlayRate;

	/** Current position in the timeline */
	UPROPERTY()
	float Position;

	/** Array of events that are fired at various times during the timeline */
	UPROPERTY(NotReplicated)
	TArray<struct FMounteaTimelineEventEntry> Events;

	/** Array of vector interpolations performed during the timeline */
	UPROPERTY(NotReplicated)
	TArray<FMounteaTimelineVectorTrack> InterpVectors;

	/** Array of float interpolations performed during the timeline */
	UPROPERTY(NotReplicated)
	TArray<struct FMounteaTimelineFloatTrack> InterpFloats;

	/** Array of linear color interpolations performed during the timeline */
	UPROPERTY(NotReplicated)
	TArray<struct FMounteaTimelineLinearColorTrack> InterpLinearColors;

	/** Called whenever this timeline is playing and updates - done after all delegates are executed and variables updated  */
	UPROPERTY(NotReplicated)
	FOnMounteaTimelineEvent TimelinePostUpdateFunc;

	/** Called whenever this timeline is finished. Is not called if 'stop' is used to terminate timeline early  */
	UPROPERTY(NotReplicated)
	FOnMounteaTimelineEvent TimelineFinishedFunc;

	/**	Optional. If set, Timeline will also set float/vector properties on this object using the PropertyName set in the tracks. */
	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UObject> PropertySetObject;

	/**	Optional. If set, Timeline will also set ETimelineDirection property on PropertySetObject using the name. */
	UPROPERTY(NotReplicated)
	FName DirectionPropertyName;

	/** Called whenever this timeline is finished. Is not called if 'stop' is used to terminate timeline early  */
	FOnMounteaTimelineEventStatic TimelineFinishFuncStatic;

	/** Cached property pointer for setting timeline direction */
	FProperty* DirectionProperty;

public:
	
	FMounteaTimeline()
	: LengthMode( MTL_LastKeyFrame )
	, bLooping( false )
	, bReversePlayback( false )
	, bPlaying( false )
	, Length( 5.f )
	, PlayRate( 1.f )
	, Position( 0.0f )	
	, PropertySetObject(nullptr)
	, DirectionProperty(nullptr)
	{
	}

	/** Helper function to get to the timeline direction enum */
	static UEnum* GetTimelineDirectionEnum();

	/** Start playback of timeline */
	void Play();

	/** Start playback of timeline from the start */
	void PlayFromStart();

	/** Start playback of timeline in reverse */
	void Reverse();

	/** Start playback of timeline in reverse from the end */
	void ReverseFromEnd();

	/** Stop playback of timeline */
	void Stop();

	/** Get whether this timeline is playing or not. */
	bool IsPlaying() const;

	/** Get whether we are reversing or not */
	bool IsReversing() const;

	/** Jump to a position in the timeline. If bFireEvents is true, event functions will fire, otherwise will not. */
	void SetPlaybackPosition(float NewPosition, bool bFireEvents, bool bFireUpdate = true);

	/** Get the current playback position of the Timeline */
	float GetPlaybackPosition() const;

	/** true means we whould loop, false means we should not. */
	void SetLooping(bool bNewLooping);

	/** Get whether we are looping or not */
	bool IsLooping() const;

	/** Sets the new play rate for this timeline */
	void SetPlayRate(float NewRate);

	/** Get the current play rate for this timeline */
	float GetPlayRate() const;

	/** Set the new playback position time to use */
	void SetNewTime(float NewTime);

	/** Get length of the timeline */
	float GetTimelineLength() const;

	/** Sets the timeline length mode */
	void SetTimelineLengthMode(EMounteaTimelineLengthMode NewMode);

	/** Set the length of the timeline */
	void SetTimelineLength(float NewLength);

	/** Update a certain float track's curve */
	void SetFloatCurve(UCurveFloat* NewFloatCurve, FName FloatTrackName);

	/** Update a certain vector track's curve */
	void SetVectorCurve(UCurveVector* NewVectorCurve, FName VectorTrackName);

	/** Update a certain linear color track's curve */
	void SetLinearColorCurve(UCurveLinearColor* NewLinearColorCurve, FName LinearColorTrackName);

	/** Optionally provide an object to automatically update properties on */
	void SetPropertySetObject(UObject* NewPropertySetObject);

	/** Set the delegate to call after each timeline tick */
	void SetTimelinePostUpdateFunc(FOnMounteaTimelineEvent NewTimelinePostUpdateFunc);

	/** Set the delegate to call when timeline is finished */
	void SetTimelineFinishedFunc(FOnMounteaTimelineEvent NewTimelineFinishedFunc);

	/** Set the static delegate to call when timeline is finished */
	void SetTimelineFinishedFunc(FOnMounteaTimelineEventStatic NewTimelineFinishedFunc);

	/** Add a callback event to the timeline */
	void AddEvent(float Time, FOnMounteaTimelineEvent EventFunc);

	/** Add a vector interpolation to the timeline */
	void AddInterpVector(UCurveVector* VectorCurve, FOnMounteaTimelineVector InterpFunc, FName PropertyName = NAME_None, FName TrackName = NAME_None);

	/** Add a vector interpolation to the timeline. Use a non-serializeable delegate. */
	void AddInterpVector(UCurveVector* VectorCurve, FOnMounteaTimelineVectorStatic InterpFunc);

	/** Add a float interpolation to the timeline */
	void AddInterpFloat(UCurveFloat* FloatCurve, FOnMounteaTimelineFloat InterpFunc, FName PropertyName = NAME_None, FName TrackName = NAME_None);

	/** Add a float interpolation to the timeline. Use a non-serializeable delegate. */
	void AddInterpFloat(UCurveFloat* FloatCurve, FOnMounteaTimelineFloatStatic InterpFunc );

	/** Add a linear color interpolation to the timeline */
	void AddInterpLinearColor(UCurveLinearColor* LinearColorCurve, FOnMounteaTimelineLinearColor InterpFunc, FName PropertyName = NAME_None, FName TrackName = NAME_None);

	/** Add a linear color interpolation to the timeline. Use a non-serializeable delegate. */
	void AddInterpLinearColor(UCurveLinearColor* LinearColorCurve, FOnMounteaTimelineLinearColorStatic InterpFunc);


	/** Advance the timeline, if playing, firing delegates */
	void TickTimeline(float DeltaTime);

	/** Set the delegate to call when timeline is finished */
	void SetDirectionPropertyName(FName InDirectionPropertyName);

	/** Get all curves used by the Timeline */
	void GetAllCurves(TSet<class UCurveBase*>& InOutCurves) const;
	
private:
	
	/** Returns the time value of the last keyframe in any of the timeline's curves */
	float GetLastKeyframeTime() const;
};

#pragma endregion 

#pragma region DeclarationObjects

/**
 * Simplified Float Curve based Timeline.
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class MOUNTEATOOLSLIBRARY_API UMounteaTimelineObject : public UObject, public FTickableGameObject
{
public:
	 
	GENERATED_BODY()

	UMounteaTimelineObject(const FObjectInitializer& ObjectInitializer);

	/** The actual timeline structure */
	UPROPERTY(ReplicatedUsing=OnRep_Timeline)
	FTimeline	TheTimeline;

	/** True if global time dilation should be ignored by this timeline, false otherwise. */
	UPROPERTY()
	uint32 bIgnoreTimeDilation : 1;

public:

	/** Start playback of timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void Play();

	/** Start playback of timeline from the start */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void PlayFromStart();

	/** Start playback of timeline in reverse */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void Reverse();

	/** Start playback of timeline in reverse from the end */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void ReverseFromEnd();

	/** Stop playback of timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void Stop();

	/** Get whether this timeline is playing or not. */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	bool IsPlaying() const;

	/** Get whether we are reversing or not */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	bool IsReversing() const;

	/** Jump to a position in the timeline. 
	  * @param bFireEvents If true, event functions that are between current position and new playback position will fire. 
	  * @param bFireUpdate If true, the update output exec will fire after setting the new playback position.
	 */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline", meta=(AdvancedDisplay="bFireUpdate"))
	void SetPlaybackPosition(float NewPosition, bool bFireEvents, bool bFireUpdate = true);

	/** Get the current playback position of the Timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	float GetPlaybackPosition() const;

	/** true means we would loop, false means we should not. */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void SetLooping(bool bNewLooping);

	/** Get whether we are looping or not */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	bool IsLooping() const;

	/** Sets the new play rate for this timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void SetPlayRate(float NewRate);

	/** Get the current play rate for this timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	float GetPlayRate() const;

	/** Set the new playback position time to use */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void SetNewTime(float NewTime);

	/** Get length of the timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	float GetTimelineLength() const;

	/** Set length of the timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void SetTimelineLength(float NewLength);

	/** Sets the length mode of the timeline */
	UFUNCTION(BlueprintCallable, Category="Components|Timeline")
	void SetTimelineLengthMode(ETimelineLengthMode NewLengthMode);

	/** Set whether to ignore time dilation. */
	UFUNCTION(BlueprintCallable, Category = "Components|Timeline")
	void SetIgnoreTimeDilation(bool bNewIgnoreTimeDilation);

	/** Get whether to ignore time dilation. */
	UFUNCTION(BlueprintCallable, Category = "Components|Timeline")
	bool GetIgnoreTimeDilation() const;

	/** Update a certain float track's curve */
	UFUNCTION(BlueprintCallable, Category = "Components|Timeline")
	void SetFloatCurve(UCurveFloat* NewFloatCurve, FName FloatTrackName);

	/** Update a certain vector track's curve */
	UFUNCTION(BlueprintCallable, Category = "Components|Timeline")
	void SetVectorCurve(UCurveVector* NewVectorCurve, FName VectorTrackName);

	/** Update a certain linear color track's curve */
	UFUNCTION(BlueprintCallable, Category = "Components|Timeline")
	void SetLinearColorCurve(UCurveLinearColor* NewLinearColorCurve, FName LinearColorTrackName);

	UFUNCTION()
	void OnRep_Timeline();

	//~ Begin ActorComponent Interface.
	virtual void Tick(float DeltaTime) override;
	void Activate(bool bReset=false); //override;
	void Deactivate(); //override;
	bool IsReadyForAutoDestroy() const; //override;
	//~ End ActorComponent Interface.

	//~ Begin UObject Interface. 
	virtual bool IsPostLoadThreadSafe() const override;
	//~ End UObject Interface

	/** Get the signature function for Timeline event functions */
	static UFunction* GetTimelineEventSignature();
	/** Get the signature function for Timeline float functions */
	static UFunction* GetTimelineFloatSignature();
	/** Get the signature function for Timeline vector functions */
	static UFunction* GetTimelineVectorSignature();
	/** Get the signature function for Timeline linear color functions */
	static UFunction* GetTimelineLinearColorSignature();

	/** Get the signature type for a specified function */
	static ETimelineSigType GetTimelineSignatureForFunction(const UFunction* InFunc);

	/** Add a callback event to the timeline */
	void AddEvent(float Time, FOnTimelineEvent EventFunc);
	
	/** Add a vector interpolation to the timeline */
	void AddInterpVector(UCurveVector* VectorCurve, FOnTimelineVector InterpFunc, FName PropertyName = NAME_None, FName TrackName = NAME_None);
	
	/** Add a float interpolation to the timeline */
	void AddInterpFloat(UCurveFloat* FloatCurve, FOnTimelineFloat InterpFunc, FName PropertyName = NAME_None, FName TrackName = NAME_None);

	/** Add a linear color interpolation to the timeline */
	void AddInterpLinearColor(UCurveLinearColor* LinearColorCurve, FOnTimelineLinearColor InterpFunc, FName PropertyName = NAME_None, FName TrackName = NAME_None);

	/** Optionally provide an object to automatically update properties on */
	void SetPropertySetObject(UObject* NewPropertySetObject);

	/** Set the delegate to call after each timeline tick */
	void SetTimelinePostUpdateFunc(FOnTimelineEvent NewTimelinePostUpdateFunc);

	/** Set the delegate to call when timeline is finished */
	void SetTimelineFinishedFunc(FOnTimelineEvent NewTimelineFinishedFunc);
	/** Set the static delegate to call when timeline is finished */
	void SetTimelineFinishedFunc(FOnMounteaTimelineEventStatic NewTimelineFinishedFunc);

	/** Set the delegate to call when timeline is finished */
	void SetDirectionPropertyName(FName DirectionPropertyName);

	/** Get all curves used by the Timeline */
	void GetAllCurves(TSet<UCurveBase*>& InOutCurves) const;

	/** Return true if this object is in a state where it can be activated normally. */
	bool ShouldActivate() const;

	/** Return true if this object can tick. */
	bool CanTick() const;

	// FTickableGameObject Begin
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	// FTickableGameObject End

private:

	ETickableTickType TickableType;
	
	UPROPERTY(transient) //, ReplicatedUsing=OnRep_IsActive)
	uint8 bIsActive : 1;
	uint8 bCanTick : 1;
	uint8 bCanEverTick : 1;

	// The last frame number we were ticked.
	// We don't want to tick multiple times per frame 
	uint32 LastFrameNumberWeTicked = INDEX_NONE;
};

#pragma endregion 

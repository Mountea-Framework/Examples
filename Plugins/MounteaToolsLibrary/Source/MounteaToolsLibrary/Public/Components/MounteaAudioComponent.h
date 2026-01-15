// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MounteaAudioComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAudioComponentStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAudioComponentCompleted);

UENUM(BlueprintType)
enum class EMounteaAudioComponentState : uint8
{
	EMA_Inactive UMETA(DisplayName="Inactive"),
	EMA_Playing UMETA(DisplayName="Playing")
};

USTRUCT(BlueprintType)
struct FMounteaAudioComponentBase
{
	GENERATED_BODY()

	/**
	 * Sound to be played.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category="Mountea")
	USoundBase* SoundBase = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category="Mountea")
	EMounteaAudioComponentState ComponentState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category="Mountea")
	uint8 bDoesLoop : 1;
};


/**
 * Mountea Audio Component.
 * Helper component responsible for Client side spawning audio.
 *
 * Can be preset or can updated in runtime.
 */
UCLASS(ClassGroup=(MounteaFramework), meta=(BlueprintSpawnableComponent, DisplayName=" Mountea Audio Component"))
class MOUNTEATOOLSLIBRARY_API UMounteaAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UMounteaAudioComponent();

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea")
	FORCEINLINE USoundBase* GetSoundBase() const
	{ return AudioComponentBase.SoundBase; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea")
	FORCEINLINE EMounteaAudioComponentState GetComponentState() const
	{ return AudioComponentBase.ComponentState; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea")
	FORCEINLINE bool CanBeInterrupted() const
	{ return GetComponentState() != EMounteaAudioComponentState::EMA_Playing; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea")
	FORCEINLINE bool IsLooping() const
	{ return AudioComponentBase.bDoesLoop; };
	
	UFUNCTION(BlueprintCallable, Category="Mountea", Client, Unreliable)
	void PlaySound(USoundBase* SoundBase);

protected:

	void SetSoundBase(USoundBase* NewSound);
	void SetComponentState(const EMounteaAudioComponentState NewState);

protected:

	UFUNCTION() void OnSoundCompleted_Callback();

protected:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly,  Category="Mountea")
	FMounteaAudioComponentBase AudioComponentBase;

	UPROPERTY(BlueprintAssignable, Category="Mounta")
	FAudioComponentStarted AudioComponentStarted;
	
	UPROPERTY(BlueprintAssignable, Category="Mounta")
	FAudioComponentCompleted AudioComponentCompleted;
	
	FTimerHandle TimerHandle_PlaySound;
};

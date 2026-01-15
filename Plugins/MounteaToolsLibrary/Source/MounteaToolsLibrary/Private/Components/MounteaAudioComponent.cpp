// Copyright Dominik Pavlicek 2022. All Rights Reserved.


#include "Components/MounteaAudioComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UMounteaAudioComponent::UMounteaAudioComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	SetComponentState(EMounteaAudioComponentState::EMA_Inactive);
}

void UMounteaAudioComponent::SetSoundBase(USoundBase* NewSound)
{
	if (CanBeInterrupted()) AudioComponentBase.SoundBase = NewSound;
}

void UMounteaAudioComponent::SetComponentState(const EMounteaAudioComponentState NewState)
{
	AudioComponentBase.ComponentState = NewState;
}

void UMounteaAudioComponent::OnSoundCompleted_Callback()
{
	SetComponentState(EMounteaAudioComponentState::EMA_Inactive);
	SetSoundBase(nullptr);
	
	AudioComponentCompleted.Broadcast();
}

void UMounteaAudioComponent::PlaySound_Implementation(USoundBase* SoundBase /*= nullptr*/)
{
	if (!GetSoundBase()) return;
	if (!GetWorld()) return;
	if (!GetOwner()) return;
	if (GetComponentState() == EMounteaAudioComponentState::EMA_Playing) return;
	if (!SoundBase) return;

	SetComponentState(EMounteaAudioComponentState::EMA_Playing);
	SetSoundBase(SoundBase);

	const float Duration = GetSoundBase()->GetDuration();

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UMounteaAudioComponent::OnSoundCompleted_Callback);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlaySound, Delegate, Duration, false);

	AudioComponentStarted.Broadcast();

	UGameplayStatics::PlaySoundAtLocation(GetOwner(), SoundBase, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
}

void UMounteaAudioComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMounteaAudioComponent, AudioComponentBase);
}
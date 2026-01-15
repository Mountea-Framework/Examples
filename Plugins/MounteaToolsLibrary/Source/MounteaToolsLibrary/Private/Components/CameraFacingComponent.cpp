// All rights reserved Dominik Pavlicek 2022.


#include "Components/CameraFacingComponent.h"

#include "Kismet/KismetMathLibrary.h"

UCameraFacingComponent::UCameraFacingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	AngleLimit = 60.f;
	Interval = 0.2;
	InterpolationSpeed = 1000.f;
	FacingType = ERotationFacingType::ERFT_ZOnlyLimited;
	bUseRotationInterpolation = true;

	EnableTick();
}

void UCameraFacingComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentActivated.AddUniqueDynamic(this, &UCameraFacingComponent::RotationActivated);
	OnComponentDeactivated.AddUniqueDynamic(this, &UCameraFacingComponent::RotationDeactivated);
	OnFacingTypeChanged.AddUniqueDynamic(this, &UCameraFacingComponent::OnFacingTypeChangedEvent);
	OnLookAtActorChanged.AddUniqueDynamic(this, &UCameraFacingComponent::OnLookAtActorChangedEvent);

	CachedRotation = GetComponentRotation();
	EnableTick();
}

void UCameraFacingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CanRotate())
	{
		RotateComponent(DeltaTime);
	}
}

void UCameraFacingComponent::OnFacingTypeChangedEvent()
{
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
		case ERotationFacingType::ERFT_ZOnly:
		case ERotationFacingType::ERFT_ZOnlyLimited:
			Activate();
			OnFacingTypeChangedEventBP();
			break;
		case ERotationFacingType::ERFT_None:
			Deactivate();
			OnFacingTypeChangedEventBP();
			break;
		case ERotationFacingType::ERFT_Default: 
		default:
			Deactivate();
			break;
	}
}

void UCameraFacingComponent::OnLookAtActorChangedEvent(AActor* Actor)
{
	if (LookAtActor)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
	OnLookAtActorChangedEventBP(Actor);
}

void UCameraFacingComponent::RotationActivated(UActorComponent* ActorComponent, bool bArg)
{
	EnableTick();
}

void UCameraFacingComponent::RotationDeactivated(UActorComponent* ActorComponent)
{
	SetRelativeRotation(CachedRotation);
	
	EnableTick();
}

void UCameraFacingComponent::EnableTick()
{
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
		case ERotationFacingType::ERFT_ZOnly:
		case ERotationFacingType::ERFT_ZOnlyLimited:
			PrimaryComponentTick.TickInterval = Interval;
			PrimaryComponentTick.SetTickFunctionEnable(true);
			break;
		case ERotationFacingType::ERFT_None:
		case ERotationFacingType::ERFT_Default: 
		default:
			PrimaryComponentTick.SetTickFunctionEnable(false);
			break;
	}
}

bool UCameraFacingComponent::CanRotate() const
{
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
		case ERotationFacingType::ERFT_ZOnly:
		case ERotationFacingType::ERFT_ZOnlyLimited:
			return LookAtActor != nullptr;
		case ERotationFacingType::ERFT_None:
		case ERotationFacingType::ERFT_Default: 
		default:
			return false;
	}

	return false;
}

void UCameraFacingComponent::RotateComponent(float DeltaTime)
{
	if (!LookAtActor)
	{
		OnLookAtActorChanged.Broadcast(nullptr);
		return;
	}

	LastRotation = GetComponentRotation();
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), LookAtActor->GetActorLocation());
		
	switch (FacingType)
	{
		case ERotationFacingType::ERFT_AllAxes:
			break;
		case ERotationFacingType::ERFT_ZOnly:
			NewRotation.Roll = 0.f;
			NewRotation.Pitch = 0.f;
			break;
		case ERotationFacingType::ERFT_ZOnlyLimited:
			NewRotation.Roll = 0.f;
			NewRotation.Pitch = 0.f;
			{
				NewRotation.Yaw = FMath::ClampAngle(NewRotation.Yaw, (-1 * AngleLimit), AngleLimit);
			}
			break;
	}

	SetWorldRotation
	(
		bUseRotationInterpolation ?
		FMath::RInterpConstantTo(LastRotation, NewRotation, GetWorld()->GetDeltaSeconds(), InterpolationSpeed) :
		NewRotation
	);
}

void UCameraFacingComponent::SetFacingType(const ERotationFacingType& NewType)
{
	FacingType = NewType;

	OnFacingTypeChanged.Broadcast();
}

void UCameraFacingComponent::SetLookAtActor(AActor* Actor)
{
	if (LookAtActor != Actor)
	{
		LookAtActor = Actor;
		OnLookAtActorChanged.Broadcast(LookAtActor);
	}
}

AActor* UCameraFacingComponent::GetLookAtActor() const
{ return LookAtActor; };


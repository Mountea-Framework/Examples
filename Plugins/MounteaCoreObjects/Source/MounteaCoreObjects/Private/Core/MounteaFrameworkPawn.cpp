// All rights reserved Dominik Pavlicek 2022.


#include "Core/MounteaFrameworkPawn.h"
#include "Core/MounteaFrameworkPlayerController.h"

#include "UObject/ConstructorHelpers.h"

#include "Engine/World.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerInput.h"


FName AMounteaFrameworkPawn::MovementComponentName(TEXT("MovementComponent0"));
FName AMounteaFrameworkPawn::CollisionComponentName(TEXT("CollisionComponent0"));
FName AMounteaFrameworkPawn::MeshComponentName(TEXT("MeshComponent0"));


AMounteaFrameworkPawn::AMounteaFrameworkPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetCanBeDamaged(true);

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;

	BaseEyeHeight = 0.0f;
	bCollideWhenPlacing = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(AMounteaFrameworkPawn::CollisionComponentName);
	CollisionComponent->InitSphereRadius(35.0f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = true;

	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(AMounteaFrameworkPawn::MovementComponentName);
	MovementComponent->UpdatedComponent = CollisionComponent;

	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh;
		FConstructorStatics()
			: SphereMesh(TEXT("/Engine/EngineMeshes/Sphere")) {}
	};

	static FConstructorStatics ConstructorStatics;

	MeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(AMounteaFrameworkPawn::MeshComponentName);
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(ConstructorStatics.SphereMesh.Object);
		MeshComponent->AlwaysLoadOnClient = true;
		MeshComponent->AlwaysLoadOnServer = true;
		MeshComponent->bOwnerNoSee = true;
		MeshComponent->bCastDynamicShadow = true;
		MeshComponent->bAffectDynamicIndirectLighting = false;
		MeshComponent->bAffectDistanceFieldLighting = false;
		MeshComponent->bVisibleInRayTracing = false;
		MeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		MeshComponent->SetupAttachment(RootComponent);
		MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
		const float Scale = CollisionComponent->GetUnscaledSphereRadius() / 160.f;
		MeshComponent->SetRelativeScale3D(FVector(Scale));
		MeshComponent->SetGenerateOverlapEvents(false);
		MeshComponent->SetCanEverAffectNavigation(false);
	}

	// This is the default pawn class, we want to have it be able to move out of the box.
	bAddDefaultMovementBindings = true;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
}

void InitializeDefaultPawnInputBindings()
{
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::S, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Up, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Down, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveForward", EKeys::Gamepad_LeftY, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_MoveRight", EKeys::Gamepad_LeftX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Gamepad_RightX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Left, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_TurnRate", EKeys::Right, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_Turn", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_LookUpRate", EKeys::Gamepad_RightY, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("DefaultPawn_LookUp", EKeys::MouseY, -1.f));
	}
}

void AMounteaFrameworkPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	if (bAddDefaultMovementBindings)
	{
		InitializeDefaultPawnInputBindings();

		PlayerInputComponent->BindAxis("DefaultPawn_MoveForward", this, &AMounteaFrameworkPawn::MoveForward);
		PlayerInputComponent->BindAxis("DefaultPawn_MoveRight", this, &AMounteaFrameworkPawn::MoveRight);
		PlayerInputComponent->BindAxis("DefaultPawn_Turn", this, &AMounteaFrameworkPawn::AddControllerYawInput);
		PlayerInputComponent->BindAxis("DefaultPawn_TurnRate", this, &AMounteaFrameworkPawn::TurnAtRate);
		PlayerInputComponent->BindAxis("DefaultPawn_LookUp", this, &AMounteaFrameworkPawn::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("DefaultPawn_LookUpRate", this, &AMounteaFrameworkPawn::LookUpAtRate);
	}
}

void AMounteaFrameworkPawn::UpdateNavigationRelevance()
{
	if (CollisionComponent)
	{
		CollisionComponent->SetCanEverAffectNavigation(bCanAffectNavigationGeneration);
	}
}

void AMounteaFrameworkPawn::MoveRight(float Val)
{
	if (const AMounteaFrameworkPlayerController* PC = Cast<AMounteaFrameworkPlayerController>(GetController()))
	{
		if (Val != 0.f && PC->IsInputEnabled())
		{
			if (Controller)
			{
				FRotator const ControlSpaceRot = Controller->GetControlRotation();

				// transform to world space and add it
				AddMovementInput( FRotationMatrix(ControlSpaceRot).GetScaledAxis( EAxis::Y ), Val );
			}
		}
	}
}

void AMounteaFrameworkPawn::MoveForward(float Val)
{
	if (const AMounteaFrameworkPlayerController* PC = Cast<AMounteaFrameworkPlayerController>(GetController()))
	{
		if (Val != 0.f && PC->IsInputEnabled())
		{
			if (Controller)
			{
				FRotator const ControlSpaceRot = Controller->GetControlRotation();

				// transform to world space and add it
				AddMovementInput( FRotationMatrix(ControlSpaceRot).GetScaledAxis( EAxis::X ), Val );
			}
		}
	}
}

void AMounteaFrameworkPawn::TurnAtRate(float Rate)
{
	if (const AMounteaFrameworkPlayerController* PC = Cast<AMounteaFrameworkPlayerController>(GetController()))
	{
		if (PC->IsInputEnabled())
		{
			AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
		}
	}
}

void AMounteaFrameworkPawn::LookUpAtRate(float Rate)
{
	if (const AMounteaFrameworkPlayerController* PC = Cast<AMounteaFrameworkPlayerController>(GetController()))
	{
		if (PC->IsInputEnabled())
		{
			AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
		}
	}
}

UPawnMovementComponent* AMounteaFrameworkPawn::GetMovementComponent() const
{
	return MovementComponent;
}
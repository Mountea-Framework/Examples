// All rights reserved Dominik Pavlicek 2022.


#include "Core/MounteaFrameworkCharacter.h"

// Sets default values
AMounteaFrameworkCharacter::AMounteaFrameworkCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMounteaFrameworkCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMounteaFrameworkCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMounteaFrameworkCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


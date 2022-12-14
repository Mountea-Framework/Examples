// All rights reserved Dominik Pavlicek 2022.


#include "Core/MounteaFrameworkGameMode.h"

#include "Core/MounteaFrameworkGameState.h"
#include "Core/MounteaFrameworkHUD.h"
#include "Core/MounteaFrameworkPawn.h"
#include "Core/MounteaFrameworkPlayerController.h"
#include "Core/MounteaFrameworkPlayerState.h"

AMounteaFrameworkGameMode::AMounteaFrameworkGameMode()
{
	DefaultPawnClass = AMounteaFrameworkPawn::StaticClass();
	GameStateClass = AMounteaFrameworkGameState::StaticClass();
	PlayerControllerClass = AMounteaFrameworkPlayerController::StaticClass();
	PlayerStateClass = AMounteaFrameworkPlayerState::StaticClass();
	HUDClass = AMounteaFrameworkHUD::StaticClass();
	
	/*
	static ConstructorHelpers::FClassFinder<AMounteaFrameworkPawn>DefaultPawnClassBP(TEXT("/MounteaCoreObjects/Blueprints/Core/BP_MounteaPawn"));
	if (DefaultPawnClassBP.Class.Get() != nullptr)
	{
		DefaultPawnClass = DefaultPawnClassBP.Class;
	}
	else
	{
		DefaultPawnClass = AMounteaFrameworkPawn::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<AMounteaFrameworkGameState>DefaultGameStateClassBP(TEXT("/MounteaCoreObjects/Blueprints/Core/BP_MounteaGS"));
	if (DefaultGameStateClassBP.Class.Get() != nullptr)
	{
		GameStateClass = DefaultGameStateClassBP.Class;
	}
	else
	{
		GameStateClass = AMounteaFrameworkGameState::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<AMounteaFrameworkPlayerController>DefaultPlayerControllerClassBP(TEXT("/MounteaCoreObjects/Blueprints/Core/BP_MounteaPC"));
	if (DefaultPlayerControllerClassBP.Class.Get() != nullptr)
	{
		PlayerControllerClass = DefaultPlayerControllerClassBP.Class;
	}
	else
	{
		PlayerControllerClass = AMounteaFrameworkPlayerController::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<AMounteaFrameworkPlayerState>DefaultPlayerStateClassBP(TEXT("/MounteaCoreObjects/Blueprints/Core/BP_MounteaPS"));
	if (DefaultPlayerStateClassBP.Class.Get() != nullptr)
	{
		PlayerStateClass = DefaultPlayerStateClassBP.Class;
	}
	else
	{
		PlayerStateClass = AMounteaFrameworkPlayerState::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<AMounteaFrameworkHUD>DefaultHUDClassBP(TEXT("/MounteaCoreObjects/Blueprints/Core/BP_MounteaHUD"));
	if (DefaultHUDClassBP.Class.Get() != nullptr)
	{
		HUDClass = DefaultHUDClassBP.Class;
	}
	else
	{
		HUDClass = AMounteaFrameworkHUD::StaticClass();
	}
	*/
}

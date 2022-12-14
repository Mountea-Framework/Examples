// All rights reserved Dominik Pavlicek 2022.


#include "Core/MounteaFrameworkPlayerController.h"

#include "MounteaToolsLibrary/Public/Library/MounteaFunctionLibrary.h"

AMounteaFrameworkPlayerController::AMounteaFrameworkPlayerController()
{
	bInputEnabled = true;
	MeasurementPeriod = 120.f;
}

void AMounteaFrameworkPlayerController::TravelToLevel(const FString& LevelURL)
{
	ClientTravel(LevelURL, ETravelType::TRAVEL_Partial);
}

void AMounteaFrameworkPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (bMeasureFPS)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_FPSMeasuring, this, &AMounteaFrameworkPlayerController::ToggleMeasureFPS, MeasurementPeriod, false);
	}
}

void AMounteaFrameworkPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(ShouldMeasureFPS())
	{
		CalculateAverageFPS();
	}
}

void AMounteaFrameworkPlayerController::ToggleMeasureFPS()
{
	if (bMeasureFPS)
	{
		TotalFPS = 0;
		FPSCounter.Empty();

		OnMeasuringStopped.Broadcast();
	}
	else
	{
		TotalFPS = 0;
		FPSCounter.Empty();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_FPSMeasuring, this, &AMounteaFrameworkPlayerController::ToggleMeasureFPS, MeasurementPeriod, false);

		OnMeasuringStopped.Broadcast();
	}

	bMeasureFPS = !bMeasureFPS;
}

void AMounteaFrameworkPlayerController::CalculateAverageFPS()
{
	const float Frame = UMounteaFunctionLibrary::GetFPS();
	TotalFPS += Frame;
	FPSCounter.Add(Frame);
	
	AverageFPS = TotalFPS / FPSCounter.Num();
}
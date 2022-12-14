// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MounteaFrameworkPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeasuringStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMeasuringStopped);

/**
 * 
 */
UCLASS()
class MOUNTEACOREOBJECTS_API AMounteaFrameworkPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	AMounteaFrameworkPlayerController();
	
protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:

	UFUNCTION(BlueprintCallable, Category="Mountea")
	void TravelToLevel(const FString& LevelURL);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea")
	FORCEINLINE bool IsInputEnabled() const
	{ return bInputEnabled; };
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea")
	FORCEINLINE bool ShouldMeasureFPS() const
	{ return bMeasureFPS; };

	UFUNCTION(BlueprintCallable, Category="Mountea")
	void ToggleMeasureFPS();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea")
	FORCEINLINE float GetAverageFPS() const
	{ return AverageFPS; };
	
	void CalculateAverageFPS();
	
protected:

	UPROPERTY(BlueprintAssignable, Category="Mountea")
	FOnMeasuringStarted OnMeasuringStarted;

	UPROPERTY(BlueprintAssignable, Category="Mountea")
	FOnMeasuringStopped OnMeasuringStopped;

protected:
	
	UPROPERTY(EditAnywhere, Category="Mountea")
	uint8 bInputEnabled : 1;

	UPROPERTY(EditAnywhere, Category="Mountea")
	uint8 bMeasureFPS : 1;

	UPROPERTY(VisibleAnywhere, Category="Mountea")
	float AverageFPS;
	UPROPERTY(VisibleAnywhere, Category="Mountea")
	float TotalFPS;

	UPROPERTY(VisibleAnywhere, Category="Mountea")
	TArray<float> FPSCounter;

	UPROPERTY(EditAnywhere, Category="Mountea")
	float MeasurementPeriod;
	
	UPROPERTY()
	FTimerHandle TimerHandle_FPSMeasuring;
};

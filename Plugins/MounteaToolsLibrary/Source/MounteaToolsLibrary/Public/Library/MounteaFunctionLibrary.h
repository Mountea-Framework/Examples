// All rights reserved Dominik Pavlicek 2022.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GenericPlatform/GenericApplication.h"

#include "MounteaFunctionLibrary.generated.h"

#pragma region DisplayDetails

/**
 * Helper structure which contains all detailed information about Display.
 */
USTRUCT(BlueprintType)
struct FDisplayDetails
{
	GENERATED_BODY()

	FDisplayDetails();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Name;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString ID;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FIntPoint NativeResolution = FIntPoint(ForceInitToZero);
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsPrimary = false;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 DPI = 0;
};

FORCEINLINE FDisplayDetails::FDisplayDetails() {};

#pragma endregion 

enum class EPlaybackResult : uint8;
enum class ETimelinePlayback : uint8;

class UMounteaTimeline;

/**
 * 
 */
UCLASS()
class MOUNTEATOOLSLIBRARY_API UMounteaFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

#pragma region Data

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Performance", meta=(CompactNodeTitle="Get Stats: FPS"))
	static float GetFPS()
	{
		extern ENGINE_API float GAverageFPS;
		return GAverageFPS;
	};

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Performance", meta=(CompactNodeTitle="Get Stats: MS"))
	static float GetMS()
	{
		extern ENGINE_API float GAverageMS;;
		return GAverageMS;
	};
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Platform", meta=(CompactNodeTitle="IsPlatformDesktop"))
	static bool IsPlatformDesktop()
	{
#if PLATFORM_DESKTOP
		return true;
#else
		return false;
#endif
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Platform", meta=(CompactNodeTitle="CPU Brand"))
	static FString GetCPUBrand()
	{
		return FPlatformMisc::GetCPUBrand();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Platform", meta=(CompactNodeTitle="GPU Brand"))
	static FString GetPrimaryGPUBrand()
	{
		return FPlatformMisc::GetPrimaryGPUBrand();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Platform", meta=(CompactNodeTitle="OS Version"))
	static FString GetOSVersion()
	{
		return FPlatformMisc::GetOSVersion();
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Display", meta=(CompactNodeTitle="Monitor Info"))
	static FDisplayDetails GetMonitorInfo(const int32 Index = 0)
	{
		FDisplayMetrics* DisplayMetrics = new(FDisplayMetrics);

		FDisplayDetails DisplayDetails;
		if (DisplayMetrics)
		{
			DisplayMetrics->RebuildDisplayMetrics(*DisplayMetrics);
			
			DisplayDetails.Name = DisplayMetrics->MonitorInfo[Index].Name;
			DisplayDetails.ID = DisplayMetrics->MonitorInfo[Index].ID;
			DisplayDetails.NativeResolution = FIntPoint(DisplayMetrics->MonitorInfo[Index].NativeWidth, DisplayMetrics->MonitorInfo[Index].NativeHeight);
			DisplayDetails.bIsPrimary = DisplayMetrics->MonitorInfo[Index].bIsPrimary;
			DisplayDetails.DPI = DisplayMetrics->MonitorInfo[Index].DPI;
			delete DisplayMetrics;
		}
		
		return DisplayDetails;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Display", meta=(CompactNodeTitle="Attached Monitors"))
	static int32 GetNAttachedMonitors()
	{
		FDisplayMetrics* DisplayMetrics = new(FDisplayMetrics);

		if (DisplayMetrics)
		{
			DisplayMetrics->RebuildDisplayMetrics(*DisplayMetrics);
			const int32 Num = DisplayMetrics->MonitorInfo.Num();
			delete DisplayMetrics;
			return Num;
		}
		
		return -1;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Mountea|Hardware", meta=(CompactNodeTitle="CPU Utilizattion"))
	static float GetCPUUtilization()
	{
		return 	FPlatformTime::GetCPUTime().CPUTimePct;
	}

#pragma endregion Data

#pragma region Create

	//Construct Mountea Timeline
	UFUNCTION(BlueprintCallable, Category = "Mountea|Timeline",
		meta = (
			DefaultToSelf = "Outer",
			DisplayName = "Make Mountea Timeline",
			BlueprintInternalUseOnly = "TRUE", 
			//DeterminesOutputType = "Class",
			Keywords = "Spawn Create Construct Mountea Timeline"
		))
		static UObject* MakeMounteaTimeline(UObject* Outer, TSubclassOf<UObject> Class);

#pragma endregion Create

};

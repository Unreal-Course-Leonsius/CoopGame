// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameStateBase.generated.h"



UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,

	WaveInProgress,

	/// No longer spawning new bots waiting for player to kill remaining bots
	WaitingToComplete,

	WaveComplete,

	CameOver,
};


/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(ReplicatedUsing = OnRep_WaveState, BlueprintReadOnly, Category = "Wave State")
	EWaveState WaveState;
	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave State")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

public:

	
	void SetWaveState(EWaveState NewState);
	
	
};

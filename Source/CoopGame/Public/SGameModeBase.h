// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameModeBase.generated.h"

enum class EWaveState : uint8;
class ASPlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerControler);

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	/// Spawn Timer
	FTimerHandle TimerHandle_BotSpawner;

	/// Wave Timer
	FTimerHandle TimerHandle_NextWavesStart;

	/*FTimerHandle TimerHandle_PlayerRespon;
	FTimerDelegate TimerDel;*/

	/// Bots to Spawn in current wave
	int32 NrOfBotsToSpawn;
	
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Object")
	float TimeBetweenWaves;

	TArray<ASPlayerController*> PlayerControllers;

protected:

	/// Hock for BP to Spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn Object")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	/// Start Spawning Bots
	void StartWave();

	/// Stop Spawn Bots
	void EndWave();

	/// Set Timer for next startwave
	void PrepareForNextWave();

	void CheckWaveSate();

	//void CheckAnyPlayerActive();
	void CheckAllPlayerLive();


	void GameOVer();

	void SetWaveState(EWaveState NewState);

	UFUNCTION()
	void RestartDeadPlayers(ASPlayerController* PC);

	void FillPlayerControllerArray(APlayerController* NewPlayer);

public:

	ASGameModeBase();

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer);

	virtual void Tick(float DeltaSecondes) override;

	UPROPERTY(BlueprintAssignable, Category = "Player Score")
	FOnActorKilled OnActorKilled;

	void ResurrectionPlayer(ASPlayerController* PC);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTickPowerup();


protected:

	/* Time between Powerup tick */
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float PowerupInterval;

	/* Total Time we Apply the Powerup Effect */
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float TotalNrOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	/* Total number of Ticks applied */
	int32 TicksPorcessed;

	/*UPROPERTY(ReplicatedUsing=OnRep_Activated)
	bool bIsActivated;
	UFUNCTION()
	void OnRep_Activated();*/

public:	

	void ActivatePowerup(class ASCharacter *ActivatedCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated(class ASCharacter *ActivatedCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();
	
	
};

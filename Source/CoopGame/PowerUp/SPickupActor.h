// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"


class USphereComponent;
class UDecalComponent;
class ASPowerupActor;
class ASCharacter;

UCLASS()
class COOPGAME_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Respawn();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USphereComponent *SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UDecalComponent *DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "Powerup Actor")
	TSubclassOf<ASPowerupActor> PowerupBlueprintClass;

	ASPowerupActor *PowerupInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Powerup Actor")
	float CoolDownDuration;

	FTimerHandle TimerHandle_RespawnPowerupActor;

	
public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


private:

	TArray<ASCharacter*> Players;

	ASCharacter *Player;

};

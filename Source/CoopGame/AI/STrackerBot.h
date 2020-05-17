// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class USoundCue;

UCLASS()
class COOPGAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent *MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USHealthComponent *HealthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USphereComponent *SpherComp;

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TargetPoint")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TargetPoint")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TargetPoint")
	float RequiredDistanceToTarget;


	UMaterialInstanceDynamic *MaterialDynamic;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	UParticleSystem *ExplosionEffect;

	bool bExploded;

	bool bStartSelfDestroyted;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float SelfDamageInterval;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	USoundCue *SelfDestroySound;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	USoundCue *ExplosionSound;


	FTimerHandle TimerHandle_SelfDamage;
	FTimerHandle TimerHandle_CheckPowerLevel;


	int32 PowerLevel = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextPathPoint();

	UFUNCTION()
	void HandleTakeDamge(USHealthComponent* OwnerHealtComp, float Health, float HealthDelta, 
		const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void ExplodeTracker();

	void DamageSelf();

	void OnCheckNeabyBots();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:

	class ACharacter *PlayerCharacter;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBarrel.generated.h"

class URadialForceComponent;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASBarrel();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent *Barrel = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UParticleSystemComponent *ExplosionEffect = nullptr;

	/* radial component doesn't need to be replicated 
	- just existing on both sides (which it will) should be sufficient -- Tom's remark */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent *RadialForceComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Barrel Material")
	UMaterialInterface* ExplodedMaterial;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ExplodeBarrel(USHealthComponent* OwnHealtComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionDamage;

private:

	bool bExploded;
	float ExplosionImpulse;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	UFUNCTION(BlueprintCallable, Category = "Fire") //  Blueprintprotected specifier not working
	virtual void Fire();

	void StartFire();

	void StopFire();

private:

	FTimerHandle TimeBetweenFire;
	

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MeshWeapon")
	USkeletalMeshComponent *Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	FName MuzzleSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UParticleSystem *MuzzleEffect;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UParticleSystem *DefualtImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UParticleSystem *FleshImpactEffect;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	FName TraceTargetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UParticleSystem *TraceEffect;


	float LineTraceLength = 1000.f;

	/// Fire mechanizm
	float LastFireTime;

	/* RPM -- Bulltets per minute Fire by Weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float RateOfFire;

	/* Derived from RateOfFire */
	float TimeBetweenShots;
	//AActor *MyOwner;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetProperlyPartilce(FHitResult &Hit);

private:

	void PlayFireEffect(FVector TraceEnd);


	
};

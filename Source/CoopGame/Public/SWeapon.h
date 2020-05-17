// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"


class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

/// Contains Infromation a single hitscan weapon linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

};


/// USTRUCT() with NewSerializa() function
//USTRUCT()
//struct FMyCustomNetSerializableStruct
//{
//	GENERATED_USTRUCT_BODY()
//
//public:
//
//	UPROPERTY()
//		float SomeProperty;
//
//	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
//	{
//		return true;
//	}
//
//
//};
//
//template<>
//struct TStructOpsTypeTraits<FMyCustomNetSerializableStruct> : public TStructOpsTypeTraitsBase2<FMyCustomNetSerializableStruct>
//{
//	enum
//	{
//		WithNetSerializer = true
//	};
//};

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

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

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

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float LineTraceLength = 1000.f;

	/// Fire mechanizm
	float LastFireTime;

	/* RPM -- Bulltets per minute Fire by Weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float RateOfFire;

	/* Derived from RateOfFire */
	float TimeBetweenShots;
	//AActor *MyOwner;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;
	UFUNCTION()
	void OnRep_HitScanTrace();
		

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//void GetProperlyPartilce(FHitResult &Hit);

	void PlayImpactEffects(EPhysicalSurface surfacetype, FVector ImpactPoint);


private:

	void PlayFireEffect(FVector TraceEnd);


	
};

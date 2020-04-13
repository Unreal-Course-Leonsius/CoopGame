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

private:

	float LineTraceLength = 1000.f;

protected:

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void Fire();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MeshWeapon")
	USkeletalMeshComponent *Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UDamageType> DamageType;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	FName MuzzleSocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UParticleSystem *MuzzleEffect;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UParticleSystem *ImpactEffect;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	FName TraceTargetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	UParticleSystem *TraceEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

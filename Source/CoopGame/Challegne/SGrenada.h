// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SGrenada.generated.h"

class ASProjectile;

/**
 * 
 */
UCLASS()
class COOPGAME_API ASGrenada : public ASWeapon
{
	GENERATED_BODY()
	
	
protected:

	virtual void BeginPlay() override;

	/// We need not UFUNCTION() properties because it's virtual
	virtual void Fire() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Firing")
	float LaunchSpeed = 10000; // TODO find sensible default


private:

	FVector GetSightRayHitLocation() const;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Setup") // we need this class to get Projectile_BP in C++
	TSubclassOf<ASProjectile> ProjectileBlueprint;
	
};

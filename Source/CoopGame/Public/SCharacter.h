// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class APlayerController;
class USHealthComponent;


UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	UPROPERTY(BlueprintReadWrite, Category = "Jump")
	bool bCanJump;
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UCameraComponent *CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	USpringArmComponent *SpringArmComp;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	/* Default FOV set during beginPlay */
	float DefaultFOV;

	UPROPERTY(Replicated)
	ASWeapon *CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASWeapon> BPWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponSocket;


	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> BPCameraShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	USHealthComponent* HealthComp;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
	bool bDead;


protected:

	void MoveForward(float Val);
	void MoveRight(float Val);

	void BigenCrouch();
	void EndCrouch();

	void BeginZoom();
	void EndZoom();

	UPROPERTY(ReplicatedUsing=OnRep_Speed, BlueprintReadWrite, Category = "Powerup")
	float MaxSpeed;
	UFUNCTION()
	void OnRep_Speed();


	UPROPERTY(ReplicatedUsing = OnRep_HealthWidget)
	bool bIsHealthWidget = false;
	UFUNCTION()
	void OnRep_HealthWidget();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwnHealtComp, float Health, 
		float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Powerup")
	void ChangeSpeed(float SuperSpeed);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual FVector GetPawnViewLocation() const override;

	USHealthComponent* GetHealthComponent() { return HealthComp; }

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void StopFire();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	bool IsAlive();


private:

	class ASPlayerController *OwnController;

	ENetRole RoleState;
	
};

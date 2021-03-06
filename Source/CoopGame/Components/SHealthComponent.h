// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Widget/SHealthWidget.h"
#include "SHealthComponent.generated.h"

class ASCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHeathChangedSignature, USHealthComponent*, HealtComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float HealthAmount);

protected:
	
	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health;
	UFUNCTION()
	void OnRep_Health(float oldHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;
		
public:

	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnHeathChangedSignature OnHealthChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	uint8 TeamNum;

public:

	void CreatePlayerHealthWidget(ASCharacter* OwningPlayer); // ASCharacter* OwningPlayer

	void DeletePlayerHealthWidget();

	void DeleteHealthWidget();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);


private:

	bool bIsDead;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<USHealthWidget> HealthClass;  // USHealthWidget

	USHealthWidget* HealthIndicator = nullptr;

	ASCharacter* Player;
	
};

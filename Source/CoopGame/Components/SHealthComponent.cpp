// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	
	DefaultHealth = 100.f;
	
	bReplicates = true;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor *OwnerActor = GetOwner();
		if (OwnerActor)
		{
			OwnerActor->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
			//DrawDebugString(GetWorld(), FVector(200, 0, 150.f), FString("Health Component"), OwnerActor, FColor::White, 10.f);
		}
	}
	

	Health = DefaultHealth;

}


void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (Damage <= 0.f)
		return;

	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	//UE_LOG(LogTemp, Warning, TEXT("Health Changed = %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

}



void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}
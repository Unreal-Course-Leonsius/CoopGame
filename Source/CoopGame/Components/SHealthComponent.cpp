// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

#include "../Public/SGameModeBase.h"
#include "../Public/SCharacter.h"
#include "../Widget/SHealthWidget.h"



// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	
	/*ConstructorHelpers::FClassFinder<USHealthWidget> HealthBPClass (TEXT("/Game/UI/WBP_Health"));
	if (!ensure(HealthBPClass.Class != nullptr)) return;
	HealthClass = HealthBPClass.Class;*/

	//UE_LOG(LogTemp, Warning, TEXT("HealthWidget Create in HealthComponent Constructor"));

	DefaultHealth = 100.f;
	bIsDead = false;

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

	Player = Cast<ASCharacter>(GetOwner());
	//if (Player && Player->IsLocallyControlled() && HealthClass)
	//{
	//	//CreatePlayerHealthWidget();
	//	UE_LOG(LogTemp, Warning, TEXT("HealthWidget Name = %s"), *HealthIndicator->GetName());
	//}
	//
	//if (Player)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("HealthWidget BeginPlay Player = %s"), *Player->GetName());
	//}

	//if (HealthClass && Player)
	//{
	//	//UE_LOG(LogTemp, Warning, TEXT("HealthWidget BeginPlay HealthIndicator = %s"), *HealthIndicator->GetName());
	//}
	

	Health = DefaultHealth;

}


void USHealthComponent::CreatePlayerHealthWidget(ASCharacter* OwningPlayer)
{
	if (HealthClass)
	{
		HealthIndicator = CreateWidget<USHealthWidget>(GetOwner()->GetGameInstance(), HealthClass);
		HealthIndicator->CreateHealthBar(OwningPlayer);
		HealthIndicator->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("HealthWidget Name = %s"), *HealthIndicator->GetName());
	}
	
}

void USHealthComponent::DeletePlayerHealthWidget()
{

	FTimerHandle TimerHandle_DeleteHealthWidget;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_DeleteHealthWidget, this, &USHealthComponent::DeleteHealthWidget, 3.f, false, 3.f);
		
}

void USHealthComponent::DeleteHealthWidget()
{
	if (HealthIndicator)
	{
		HealthIndicator->RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("HealthWidget delete Name = %s"), *HealthIndicator->GetName());
	}
}


void USHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (Damage <= 0.f || bIsDead)
		return;

	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);
	bIsDead = Health <= 0;

	UE_LOG(LogTemp, Warning, TEXT("Health Changed = %s, bIsDead = %i"), *FString::SanitizeFloat(Health), bIsDead);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead && InstigatedBy != nullptr)
	{
		ASGameModeBase *GM = Cast<ASGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
	}


}

void USHealthComponent::Heal(float HealthAmount)
{

	if (HealthAmount <= 0 || Health <= 0)
		return;

	Health = FMath::Clamp(Health + HealthAmount, 0.f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, -HealthAmount, nullptr, nullptr, nullptr);

	UE_LOG(LogTemp, Warning, TEXT("Health Powerup = %f"), HealthAmount);

}

void USHealthComponent::OnRep_Health(float oldHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("oldHealth = %f   Health = %f"), oldHealth, Health);
	float Damage = oldHealth - Health;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}



void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}
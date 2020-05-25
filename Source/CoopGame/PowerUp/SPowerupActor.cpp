// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"

#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"


// Sets default values
ASPowerupActor::ASPowerupActor()
{
	PowerupInterval = 0.f;
	TotalNrOfTicks = 0.f;

	//bIsActivated = false;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupActor::OnTickPowerup()
{
	TicksPorcessed++;

	OnPowerupTicked();

	if (TicksPorcessed >= TotalNrOfTicks)
	{

		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}

	
}

//void ASPowerupActor::OnRep_Activated()
//{
//	OnActivated();
//}

void ASPowerupActor::ActivatePowerup(class ASCharacter *ActivatedCharacter)
{

	//bIsActivated = true;
	OnActivated(ActivatedCharacter);

	if (PowerupInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
		DrawDebugString(GetWorld(), GetActorLocation(), "Health Healing", nullptr, FColor::Red, 5.f);
	}
	else
	{
		OnTickPowerup();
	}

}


//void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//
//	DOREPLIFETIME(ASPowerupActor, bIsActivated);
//
//}

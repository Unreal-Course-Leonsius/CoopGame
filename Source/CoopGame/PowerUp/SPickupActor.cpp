// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickupActor.h"

#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

#include "SPowerupActor.h"
#include "../Public/SCharacter.h"


// Sets default values
ASPickupActor::ASPickupActor()
{
 
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->SetSphereRadius(75.f);
	RootComponent = SphereComp;


	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComp->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	DecalComp->DecalSize = FVector(64.f, 75.f, 75.f);
	DecalComp->SetupAttachment(RootComponent);

	CoolDownDuration = 5.f;

	SetReplicates(true);

}

// Called when the game starts or when spawned
void ASPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if(Role == ROLE_Authority)
		Respawn();
}

void ASPickupActor::Respawn()
{

	if (PowerupBlueprintClass == nullptr) return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupBlueprintClass, GetTransform(), SpawnParameters);
}

void ASPickupActor::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (PowerupInstance == nullptr) return;

	// TODO: Grant Powerup to Player if available

	Player = Cast<ASCharacter>(OtherActor);

	if (Role == ROLE_Authority && Player)
	{
		PowerupInstance->ActivatePowerup(Player);
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnPowerupActor, this, &ASPickupActor::Respawn, CoolDownDuration);
	}

}




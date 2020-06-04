// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"

#include "Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

#include "../Components/SHealthComponent.h"
#include "../Public/SCharacter.h"

#define OUT

// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCanEverAffectNavigation(false);

	SpherComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SpherComp->SetSphereRadius(200);
	SpherComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SpherComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SpherComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SpherComp->SetupAttachment(RootComponent);


	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamge);

	bUseVelocityChange = false;
	MovementForce = 10000.f;
	RequiredDistanceToTarget = 150.f;

	bExploded = false;
	bStartSelfDestroyted = false;
	ExplosionDamage = 100;
	ExplosionRadius = 500;
	SelfDamageInterval = 0.25f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	
	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();
		GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &ASTrackerBot::OnCheckNeabyBots, 1.f, true, 0.f);
	}

}

FVector ASTrackerBot::GetNextPathPoint()
{
	

	UNavigationPath *NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerCharacter);

	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Num PointMass = %i"), NavPath->PathPoints.Num());
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();

}

void ASTrackerBot::HandleTakeDamge(USHealthComponent * OwnerHealtComp, float Health, float HealthDelta, 
	const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

	if (MaterialDynamic == nullptr)
		MaterialDynamic = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));

	if (MaterialDynamic)
		MaterialDynamic->SetScalarParameterValue("DamageTaken", GetWorld()->TimeSeconds);

	UE_LOG(LogTemp, Log, TEXT("Health = %f, TimeSecond = %f"), Health, GetWorld()->TimeSeconds);

	if (Health <= 0)
		ExplodeTracker();

}

void ASTrackerBot::ExplodeTracker()
{
	if (bExploded)
		return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	

	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);

		float ActualDamage = ExplosionDamage + (ExplosionDamage * PowerLevel);

		UGameplayStatics::ApplyRadialDamage(
			this,
			ActualDamage,
			GetActorLocation(),
			ExplosionRadius,
			nullptr,
			IgnoreActors,
			this,
			GetInstigatorController(),
			true
		);

		SetLifeSpan(0.5f);
	}
	
	MeshComp->SetVisibility(false);


	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 3.f, 0.f, 1.f);


}



// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/// if Tracket Exploded need not Executed this
	if (Role == ROLE_Authority && !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		/*bool bEquale = (int32(NextPathPoint.X * NextPathPoint.Y)) ==
			(int32(PlayerCharacter->GetActorLocation().X * PlayerCharacter->GetActorLocation().Y))
			? true : false;*/

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
			//UE_LOG(LogTemp, Error, TEXT("Set Path Point   NextPathPoint = %s   PlayerCharacter Loc = %s"), *NextPathPoint.ToString(), *PlayerCharacter->GetActorLocation().ToString());
		}
		else
		{

			//UE_LOG(LogTemp, Warning, TEXT("Next Path Point  = %s"), *NextPathPoint.ToString());
			// Keep Moving Next Target Point
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;
			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

			DrawDebugSphere(GetWorld(), NextPathPoint, 20.f, 12, FColor::Yellow, false, 0.f, 1.f);

		}
	}

	

}

void ASTrackerBot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASTrackerBot, NrOfBots);
	DOREPLIFETIME(ASTrackerBot, PowerLevel);
	DOREPLIFETIME(ASTrackerBot, MaxPowerLevel);
	
	
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (bStartSelfDestroyted)
		return;

	ASCharacter *Player = Cast<ASCharacter>(OtherActor);
	if (Player)
	{
		if(Role == ROLE_Authority)
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.f);

		UGameplayStatics::SpawnSoundAttached(SelfDestroySound, RootComponent);
		UE_LOG(LogTemp, Warning, TEXT("NotifyOverlap Function"));
		bStartSelfDestroyted = true;
	}

}

void ASTrackerBot::DamageSelf()
{
	/// Damage Self
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTrackerBot::OnCheckNeabyBots()
{
	//UE_LOG(LogTemp, Error, TEXT("TrackBot =========================="));
	const float Radius = 600;

	FCollisionShape CollisionSphere;
	CollisionSphere.SetSphere(Radius);

	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	TArray<FOverlapResult> Overalaps;
	GetWorld()->OverlapMultiByObjectType(OUT Overalaps, GetActorLocation(), FQuat::Identity, QueryParams, CollisionSphere);

	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, FColor::Yellow, false, 1.f);

	NrOfBots = 0;
	for (FOverlapResult Result : Overalaps)
	{
		ASTrackerBot *Bot = Cast<ASTrackerBot>(Result.GetActor());
		if (Bot && Bot != this)
		{
			NrOfBots++;
		}
	}


	SetCollectBotEffect();
	//UE_LOG(LogTemp, Warning, TEXT("TrackBot Server Function PowerLevel = %i"), PowerLevel);
	//NrOfBots = 0;
	//UE_LOG(LogTemp, Log, TEXT("TrackBot Server Function NrOfBots = %i"), NrOfBots);

}



void ASTrackerBot::SetCollectBotEffect()
{
	PowerLevel = FMath::Clamp(NrOfBots, 0, MaxPowerLevel);
	float Alpha = PowerLevel / (float)MaxPowerLevel;

	if (MaterialDynamic == nullptr)
		MaterialDynamic = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));

	if (MaterialDynamic)
		MaterialDynamic->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	//UE_LOG(LogTemp, Warning, TEXT("TrackBot SetCollectBotEffect NrOfBots = %i"), NrOfBots);

}

void ASTrackerBot::OnRep_NeabyBots()
{
	SetCollectBotEffect();
	//UE_LOG(LogTemp, Warning, TEXT("TrackBot OnRep Function PowerLevel = %i"), PowerLevel);
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
//#include "Components/SkeletalMeshComponent.h"

#include "../CoopGame.h"

#define OUT

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Lines for Weapons"),
	ECVF_Cheat
	);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	RootComponent = Weapon;

	MuzzleSocket = "MuzzleFlashSocket";
	TraceTargetName = "BeamEnd";

	BaseDamage = 20.f;
	RateOfFire = 600.f;

	bReplicates = true;   // == SetReplicated(true)

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}


// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	/*MyOwner = GetOwner();
	if (MyOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponOwner = %s"), *MyOwner->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponOwner Is NULL"));
	}*/

	TimeBetweenShots = 60 / RateOfFire;
}


void ASWeapon::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("FireFunction"));
	
	if (Role < ROLE_Authority)
		ServerFire();


	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Weapon Owner = %s"), *MyOwner->GetName());
		FVector StartLocation;
		FRotator StartRotation;
		MyOwner->GetActorEyesViewPoint(OUT StartLocation, OUT StartRotation);

		UE_LOG(LogTemp, Warning, TEXT("CameraLocation & StartLocation = %s"), *StartLocation.ToString());

		FVector Direction = StartRotation.Vector();
		FVector EndLocation = StartLocation + (StartRotation.Vector() * LineTraceLength);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TraceEndPoint = EndLocation;

		FHitResult Hit;
		bool bIsDamage = GetWorld()->LineTraceSingleByChannel(
			OUT Hit,
			OUT StartLocation,
			OUT EndLocation,
			COLLISION_WEAPON,
			QueryParams
			);

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		if (bIsDamage)
		{

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			auto CurrentDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVUNERABLE)
			{
				CurrentDamage = CurrentDamage * 4;
			}


			AActor *DamageActor = Hit.GetActor();
			UGameplayStatics::ApplyPointDamage(
				DamageActor,
				CurrentDamage,
				Direction,
				Hit,
				MyOwner->GetInstigatorController(),
				this,
				DamageType
			);
			UE_LOG(LogTemp, Warning, TEXT("Damage"));

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);


			TraceEndPoint = Hit.ImpactPoint;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0.f, 1.f);
		}


		PlayFireEffect(TraceEndPoint);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TraceEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->TimeSeconds;

	}

	

}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}

void ASWeapon::OnRep_HitScanTrace()
{
	//UE_LOG(LogTemp, Warning, TEXT("Update HitScanTrace"));
	PlayFireEffect(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}


void ASWeapon::PlayImpactEffects(EPhysicalSurface surfacetype, FVector ImpactPoint)
{

	UParticleSystem * SelectedEffect = nullptr;
	switch (surfacetype)
	{
	case SURFACE_FLESHDEFUALT:
	case SURFACE_FLESHVUNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefualtImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzelLocation = Weapon->GetSocketLocation(MuzzleSocket);

		FVector ShotDirection = ImpactPoint - MuzzelLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASWeapon::ServerFire_Validate()
{
	return true;
}


void ASWeapon::StartFire()
{
	// Clamp. Max() choose maximum between two values
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.f);

	// Timer is Loop and ever TimeBetweenShots rate it's called Fire() function
	GetWorldTimerManager().SetTimer(TimeBetweenFire, this, &ASWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	// if Last Parameter FirstDelay will be less then 0 function changed it by default and Default is TireRate Parameter
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimeBetweenFire);
}




void ASWeapon::PlayFireEffect(FVector TraceEnd)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, Weapon, MuzzleSocket);
	}

	if (TraceEffect)
	{
		FVector MuzzleLocation = Weapon->GetSocketLocation(MuzzleSocket);

		UParticleSystemComponent *TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);
		if (TraceComp)
		{
			TraceComp->SetVectorParameter(TraceTargetName, TraceEnd);
		}
	}
}

//FString GetSurface(EPhysicalSurface surface)
//{
//	switch (surface)
//	{
//	case SURFACE_FLESHDEFUALT:
//		return "Surface 1";
//	case SURFACE_FLESHVUNERABLE:
//		return "Surface 2";
//	default:
//		return "Default Surface";
//	}
//}


//void ASWeapon::GetProperlyPartilce(FHitResult &Hit)
//{
//	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
//
//
//	UParticleSystem * SelectedEffect = nullptr;
//	switch (SurfaceType)
//	{
//	case SURFACE_FLESHDEFUALT:
//	case SURFACE_FLESHVUNERABLE:
//		SelectedEffect = FleshImpactEffect;
//		break;
//	default:
//		SelectedEffect = DefualtImpactEffect;
//		break;
//	}
//
//	if (SelectedEffect)
//	{
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
//	}
//}
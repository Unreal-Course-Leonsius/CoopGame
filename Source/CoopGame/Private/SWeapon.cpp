// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
//#include "Components/SkeletalMeshComponent.h"

#define OUT

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	RootComponent = Weapon;

	MuzzleSocket = "MuzzleFlashSocket";
	TraceTargetName = "BeamEnd";

}

void ASWeapon::Fire()
{
	AActor *MyOwner = GetOwner();
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

		FVector TraceEndPoint = EndLocation;

		FHitResult Hit;
		bool bIsDamage = GetWorld()->LineTraceSingleByChannel(
			OUT Hit,
			OUT StartLocation,
			OUT EndLocation,
			ECC_Visibility,
			QueryParams
			);

		if (bIsDamage)
		{
			AActor *DamageActor = Hit.GetActor();
			UGameplayStatics::ApplyPointDamage(
				DamageActor,
				20.f,
				Direction,
				Hit,
				MyOwner->GetInstigatorController(),
				this,
				DamageType
			);
			UE_LOG(LogTemp, Warning, TEXT("Damage"));

			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TraceEndPoint = Hit.ImpactPoint;
		}

		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0.f, 1.f);

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
				TraceComp->SetVectorParameter(TraceTargetName, TraceEndPoint);
			}
		}

	}

	

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


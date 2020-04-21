// Fill out your copyright notice in the Description page of Project Settings.

#include "SGrenada.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"

#include "SProjectile.h"


#define  OUT

void ASGrenada::BeginPlay()
{
	Super::BeginPlay();
}

void ASGrenada::Fire()
{

	FVector MuzzleLocation = Weapon->GetSocketLocation(MuzzleSocket);
	FRotator MuzzleRotation = Weapon->GetSocketRotation(MuzzleSocket);
	FVector EndLocation = GetSightRayHitLocation();
	UE_LOG(LogTemp, Error, TEXT("MuzzleLocation = %s"), *MuzzleLocation.ToString());

	FVector LaunchVelocity;
	bool SPV = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OUT LaunchVelocity,
		MuzzleLocation,
		EndLocation,
		LaunchSpeed,
		false,    // we can't missed this and following two parametrs  // false = Not Higharc
		0.f,      // and after that declar ESuggestProjVelocityTraceOption parameter's  // gravity = 0
		0.f,      // and even thouge Ben's code compile that because ::DoNotTrace parameter's appropriate matching Default false paramter's  // radiuse = 0
		ESuggestProjVelocityTraceOption::DoNotTrace
		//FCollisionResponseParams::DefaultResponseParam,
		//ActorsToIgnor,
		//true
	);
	
	
	UE_LOG(LogTemp, Warning, TEXT("LaunchVeloctiy = %s"), *LaunchVelocity.ToString());
	UE_LOG(LogTemp, Error, TEXT("After MuzzleLocation = %s"), *MuzzleLocation.ToString());
	UE_LOG(LogTemp, Error, TEXT("LaunchVeloctiy Normal = %s"), *LaunchVelocity.GetSafeNormal().ToString());
	UE_LOG(LogTemp, Error, TEXT("LaunchVeloctiy Size = %f"), LaunchVelocity.Size());
	/// EndLocation's direction is not same as LaunchVelocity
	FRotator Direction = LaunchVelocity.GetSafeNormal().Rotation();
	ASProjectile *Projectile = GetWorld()->SpawnActor<ASProjectile>(
		ProjectileBlueprint,
		MuzzleLocation,
		Direction
		);

	DrawDebugSphere(GetWorld(), LaunchVelocity.GetSafeNormal(), 40.f, 10.f, FColor::Yellow, false, 10.f);

	Projectile->LaunchProjectile(LaunchSpeed);


}

FVector ASGrenada::GetSightRayHitLocation() const
{
	AActor* MyOwner = GetOwner();

	if(!MyOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyOwner is NULL"));
		return FVector::ZeroVector;
	}

	FVector StartLocation;
	FRotator StartRotation;
	MyOwner->GetActorEyesViewPoint(OUT StartLocation, OUT StartRotation);

	//UE_LOG(LogTemp, Warning, TEXT("CameraLocation & StartLocation = %s"), *StartLocation.ToString());

	FVector Direction = StartRotation.Vector();
	FVector EndLocation = StartLocation + (StartRotation.Vector() * LineTraceLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	//FVector TraceEndPoint = EndLocation;

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
		UE_LOG(LogTemp, Warning, TEXT("Hit Object"));
		return Hit.Location;
	}
	else
	{

		return EndLocation;
	}

}

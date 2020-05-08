// Fill out your copyright notice in the Description page of Project Settings.

#include "SBarrel.h"

#include "../Components/SHealthComponent.h"

#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASBarrel::ASBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Barrel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel"));
	Barrel->SetSimulatePhysics(true);
	Barrel->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = Barrel;

	ExplosionEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	ExplosionEffect->SetupAttachment(RootComponent);
	ExplosionEffect->bAutoActivate = false;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComp->SetupAttachment(RootComponent);
	RadialForceComp->bIgnoreOwningActor = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->Radius = 300;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASBarrel::ExplodeBarrel);

	ExplosionDamage = 100;
	ExplosionImpulse = 400;
	bExploded = false;

	bReplicates = true;
	SetReplicateMovement(true);
}



// Called when the game starts or when spawned
void ASBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	/// AI Pawn Spawn Random Material 
	//check(BarrelMaterials.Num());
	//UMaterialInterface* BarelMaterial = BarrelMaterials[0];
	//Barrel->SetMaterial(0, BarelMaterial);

}

void ASBarrel::ExplodeBarrel(USHealthComponent * OwnHealtComp, float Health, float HealthDelta,
	const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

	if (bExploded)
		return;

	UE_LOG(LogTemp, Error, TEXT("ExplodeBarrel Event == Health = %f"), Health);
	if (Health <= 0 && ExplosionEffect && RadialForceComp && ExplodedMaterial)
	{
		bExploded = true;

		RadialForceComp->FireImpulse();

		FVector BoostIntesity = FVector::UpVector * ExplosionImpulse;
		Barrel->AddImpulse(BoostIntesity, NAME_None, true);

		bool IsDamege = UGameplayStatics::ApplyRadialDamage(
			Barrel,
			ExplosionDamage,
			GetActorLocation(),
			RadialForceComp->Radius,
			DamageType->GetClass(),
			TArray<AActor*>(),
			DamageCauser,
			nullptr,
			true
		);

		ExplosionBarrel();

		/*DamageData.rDamageType = DamageType;
		DamageData.rDamageCauser = DamageCauser;*/

	}
}

void ASBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// need not Condition Because Variable must be update for Client and Simulated Proxy
	// and therefor its ReplicatedUsing update will not be executed for Server
	// So it will not be run twice for Server
	DOREPLIFETIME(ASBarrel, bExploded);
}

void ASBarrel::OnRep_Exploded()
{
	UE_LOG(LogTemp, Error, TEXT("Update BarrelDamageData"));
	ExplosionBarrel();
}

void ASBarrel::ExplosionBarrel()
{
	ExplosionEffect->Activate(true);
	Barrel->SetMaterial(0, ExplodedMaterial);
	//UE_LOG(LogTemp, Error, TEXT("ExplodeBarrel Event Damage = %i"), IsDamege);

}

// Called every frame
void ASBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


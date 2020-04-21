// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectile.h"

#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASProjectile::ASProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh")); // Crete StaticMesh
	SetRootComponent(CollisionMesh);                      // Make CollisionMesh as a RootComponent
	CollisionMesh->SetNotifyRigidBodyCollision(true);     // Switch Simulation Generates Hit Event
	CollisionMesh->SetVisibility(false);

	LaunchBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Launch Blast"));
	LaunchBlast->SetupAttachment(RootComponent); // AttachTo(RootComponent); gets Error at Last

	ImpactBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("Impact Blast"));
	ImpactBlast->SetupAttachment(RootComponent);
	ImpactBlast->bAutoActivate = false;

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Exlosion Force"));
	ExplosionForce->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false; // It's responsible not be activeted while there will not be relevant order

}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentHit.AddDynamic(this, &ASProjectile::OnHit);
}

// Called every frame
void ASProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ASProjectile::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{

	LaunchBlast->Deactivate();
	ImpactBlast->Activate();

	SetRootComponent(ImpactBlast);
	CollisionMesh->DestroyComponent();

	ExplosionForce->FireImpulse();

	bool IsDamage = false;

	//~~~~~~~~ IT"S ABOUT TSubclassOf<UDamageType>() CLASS ~~~~~~~~~~\\
	// any TSuclassOf<>() returns UClass reference
	// UClass Damage; It's impossible to create UClass Objcet here
	// UClass *Damage = nullptr; // = UClass::StatiCClass(); in ApplyRadialDamage() function we can use it
	// TSubclassOf<UDamageType> Damage; // we can such create UDamageType=UClass pointer and we can use it in ApplyRadialDamage()
	// TSuclassOf<>() we can use through the Unreal Editor

	IsDamage = UGameplayStatics::ApplyRadialDamage(
		this,
		ProjectilDamage,
		GetActorLocation(),
		ExplosionForce->Radius,
		UDamageType::StaticClass(),  // it's return UCLass
		TArray<AActor *>()         // 0bjcet which must be damage Only need AActor
	);

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &ASProjectile::OnTimerExpire, DestroyDelay, false);

}

void ASProjectile::LaunchProjectile(float speed)
{
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * speed); // ForwardVector is the same as Barrel SocketRotation's Look direction thats why we need not calculat this parameter
	ProjectileMovement->Activate(true);  // we need it anywhere will be or not bAutoActivate = false. It's responsible moving
}

void ASProjectile::OnTimerExpire()
{
	Destroy(true);
}
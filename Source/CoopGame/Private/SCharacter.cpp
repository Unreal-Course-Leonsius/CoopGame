// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "../Public/SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

#include "../Public/SWeapon.h"
#include "CoopGame.h"
#include "../Components/SHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));

	ZoomedFOV = 65.f;
	ZoomInterpSpeed = 20.f;

	WeaponSocket = "WeaponSocket";
	MaxSpeed = 0;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	/// Spawn Default weapon
	
	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(BPWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		}
	}

	/*FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(BPWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	}*/

	OwnController = Cast<APlayerController>(GetController());

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDead);
	DOREPLIFETIME(ASCharacter, MaxSpeed);
}

void ASCharacter::ChangeSpeed(float SuperSpeed)
{
	MaxSpeed = SuperSpeed;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	UE_LOG(LogTemp, Warning, TEXT("Speed Changed = %f"), GetCharacterMovement()->MaxWalkSpeed)
}

void ASCharacter::OnRep_Speed()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
	UE_LOG(LogTemp, Warning, TEXT("OnRep Speed Changed = %f"), GetCharacterMovement()->MaxWalkSpeed)
}


void ASCharacter::OnHealthChanged(USHealthComponent* OwnHealtComp, float Health, float HealthDelta, 
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Health Changed = %s"), *FString::SanitizeFloat(Health));
	//DrawDebugString(GetWorld(), GetActorLocation(), "Health Damage", DamageCauser, FColor::Red, 5.f);
	if (Health <= 0 && !bDead)
	{
		bDead = true;
		//UE_LOG(LogTemp, Warning, TEXT("bDead = %i"), bDead);
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.f);
	}
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Lookup", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);


	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BigenCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("CharacterJump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
}

void ASCharacter::MoveForward(float Val)
{
	AddMovementInput(GetActorForwardVector() * Val);
}

void ASCharacter::MoveRight(float Val)
{
	AddMovementInput(GetActorRightVector() * Val);
}

void ASCharacter::BigenCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::StartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("StartFire"));
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
		OwnController->ClientPlayCameraShake(BPCameraShake);
	}
}

void ASCharacter::StopFire()
{
	UE_LOG(LogTemp, Warning, TEXT("StopFire"));
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}




FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Update Pawns Viewpont FV = %s"), *CameraComp->GetComponentLocation().ToString());
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}


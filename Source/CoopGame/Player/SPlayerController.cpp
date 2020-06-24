// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"
#include "../Public/SCharacter.h"
#include "../Components/SHealthComponent.h"

#include "DrawDebugHelpers.h"

#define OUT

static int32 DebugCrosshairPosition = 0;
FAutoConsoleVariableRef CVARDebugCrosshairPosition(
	TEXT("COOP.DebugCrosshair"),
	DebugCrosshairPosition,
	TEXT("Draw Lines for Weapons"),
	ECVF_Cheat
);

ASPlayerController::ASPlayerController()
{
	PlayerLive = 2;
}

bool ASPlayerController::IsPlayerAlive()
{
	return PlayerLive >= 0;
}

ASCharacter * ASPlayerController::GetControlCharater()
{
	return OwnPlayer;
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	UE_LOG(LogTemp, Warning, TEXT("LineTrace End GetViewPortSize = %s"), *ViewportSize.ToString());

	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

}

void ASPlayerController::Possess(APawn * aPawn)
{
	Super::Possess(aPawn);

	OwnPlayer = Cast<ASCharacter>(GetPawn());

	/*if (IsLocalController())
	{
		OwnPlayer->GetHealthComponent()->CreatePlayerHealthWidget(OwnPlayer);
		UE_LOG(LogTemp, Warning, TEXT("PlayerController Create Health Widget"));
	}*/
	
}

void ASPlayerController::UnPossess()
{
	Super::UnPossess();
	
	/*if (IsLocalController())
	{
		OwnPlayer->GetHealthComponent()->DeletePlayerHealthWidget();
		UE_LOG(LogTemp, Warning, TEXT("PlayerController delete Health Widget"));
	}*/
	
}


FVector2D ASPlayerController::GetPositionCrosshair()
{
	FVector StartLocation;
	FRotator StartRotation;
	OwnPlayer->GetActorEyesViewPoint(OUT StartLocation, OUT StartRotation);

	FVector ShotDirection = StartRotation.Vector();
	ShotDirection.Normalize();
	FVector EndLocation = StartLocation + (ShotDirection  * 10000.f);
	UE_LOG(LogTemp, Error, TEXT("LineTrace End StartPositon_1 = %s"), *StartLocation.ToString());
	//UE_LOG(LogTemp, Error, TEXT("LineTrace End = %s"), *EndLocation.ToString());

	/*FHitResult Hit;
	bool bIsDamage = GetWorld()->LineTraceSingleByChannel(
		OUT Hit,
		OUT StartLocation,
		OUT EndLocation,
		ECollisionChannel::ECC_Camera
	);*/

	FVector2D CrosshairPositoin;
	ProjectWorldLocationToScreen(EndLocation, OUT CrosshairPositoin);
	UE_LOG(LogTemp, Error, TEXT("LineTrace End  CrosshairPosition = %s"), *CrosshairPositoin.ToString());

	if (DebugCrosshairPosition)
		DrawDebugSphere(GetWorld(), EndLocation, 100, 12, FColor::Yellow, false, 2.f, 0, 1.f);

	return CrosshairPositoin;
}

FVector2D ASPlayerController::GetPositionAnchor()
{
	FVector StartLocation;
	FRotator StartRotation;
	OwnPlayer->GetActorEyesViewPoint(OUT StartLocation, OUT StartRotation);

	if (DebugCrosshairPosition)
		DrawDebugSphere(GetWorld(), StartLocation, 100, 12, FColor::Red, false, 2.f, 0, 1.f);


	FVector2D AnchorPositoin;
	ProjectWorldLocationToScreen(StartLocation, OUT AnchorPositoin);
	UE_LOG(LogTemp, Error, TEXT("LineTrace End StartPositon = %s"), *StartLocation.ToString());
	UE_LOG(LogTemp, Error, TEXT("LineTrace End AnchorPosition = %s"), *AnchorPositoin.ToString());
	return AnchorPositoin;
}

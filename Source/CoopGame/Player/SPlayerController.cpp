// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"
#include "../Public/SCharacter.h"
#include "../Components/SHealthComponent.h"


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

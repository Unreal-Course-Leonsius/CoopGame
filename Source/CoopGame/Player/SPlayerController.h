// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

class ASCharacter;

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:

	ASPlayerController();


	/* For GameMode */
	UPROPERTY(EditDefaultsOnly, Category = Live)
	int32 PlayerLive;
	
	bool IsPlayerAlive();

	ASCharacter* GetControlCharater();

	virtual void BeginPlay() override;

	virtual void Possess(APawn* aPawn) override;

	virtual void UnPossess() override;

	/*  Call this function after Character Possessed */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crosshair")
	FVector2D GetPositionCrosshair();

	/*  Call this function after Character Possessed */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Crosshair")
	FVector2D GetPositionAnchor();

protected:

	ASCharacter* OwnPlayer;

	
};

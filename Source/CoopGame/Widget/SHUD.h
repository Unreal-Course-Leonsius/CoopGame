// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SHUD.generated.h"

class UTexture2D;

/**
 * 
 */
UCLASS()
class COOPGAME_API ASHUD : public AHUD
{
	GENERATED_BODY()
	
protected:

	/** Crosshair asset pointer */
	UTexture2D* CrosshairTex;

public:

	ASHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "ScreenSize", meta = (ClampMin = 0, ClampMax = 1))
	float DivideScreenX;

	UPROPERTY(EditDefaultsOnly, Category = "ScreenSize", meta = (ClampMin = 0, ClampMax = 1))
	float DivideScreenY;
	
};

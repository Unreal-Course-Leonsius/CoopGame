// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SHealthWidget.generated.h"

class ASCharacter;

/**
 * 
 */
UCLASS()
class COOPGAME_API USHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	USHealthWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Create")
	void CreateHealthBar(ASCharacter* OwnerPlayer);

	
};

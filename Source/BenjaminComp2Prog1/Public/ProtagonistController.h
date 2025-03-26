// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FroggyCharacter.h"
#include "ProtagonistController.generated.h"

/**
 * 
 */
UCLASS()
class BENJAMINCOMP2PROG1_API AProtagonistController : public APlayerController
{
	GENERATED_BODY()

public:
	static void BindInputs(AFroggyCharacter* FroggyCharacter, UEnhancedInputComponent* EnhancedInputComponent);
	virtual void BeginPlay() override;
	
};

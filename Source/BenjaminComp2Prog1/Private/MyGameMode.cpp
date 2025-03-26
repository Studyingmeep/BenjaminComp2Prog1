// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "FroggyCharacter.h"
#include "ProtagonistController.h"
#include "UObject/ConstructorHelpers.h"

AMyGameMode::AMyGameMode()
{
	// Set the default pawn class to my pure C++ character Froggy
	DefaultPawnClass = AFroggyCharacter::StaticClass();

	// Set the default controller class to my own controller for the enhanced input system
	PlayerControllerClass = AProtagonistController::StaticClass();

	/*
	// Blueprint version
	// If I were to use a Blueprint class, I'd have to load it like this:
	
	// Finding and locating the exact blueprint
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_FroggyCharacter"));
	
	if (PlayerPawnBPClass.Succeeded())
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	*/
}

void AMyGameMode::StartPlay()
{
	Super::StartPlay();
	check(GEngine != nullptr);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("Hello World, this is myGameMode!"));
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtagonistController.h"
#include "FroggyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Now why do this in BeginPlay? Why not in SetupPlayerInputComponent?
// ...I learned of the SetupPlayerInputComponent after doing the below code, and I'm too lazy.
// I'll fix it when I reuse this for my group project - move it over and optimize the code.
// Which is exactly why it's nice to have this Comp_1 assignment, to do bad first-time code, and improve next time. :3
void AProtagonistController::BeginPlay()
{
	Super::BeginPlay();

	// Get the Local Player Subsystem - which manages input mappings per local player (used to add or remove input mappings dynamically)
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!InputSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to get UEnhancedInputLocalPlayerSubsystem! Input mapping will not be applied."));
		return;
	}

	// Get the controlled FroggyCharacter
	AFroggyCharacter* FroggyCharacter = Cast<AFroggyCharacter>(GetPawn());
	if (!FroggyCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Froggy Character not found inside ProtagonistController.cpp!"));
		return;
	}

	// Ensure IA_* actions are loaded before binding inputs
	if (!FroggyCharacter->GetIA_Move() || !FroggyCharacter->GetIA_Sit() || 
		!FroggyCharacter->GetIA_Interact() || !FroggyCharacter->GetIA_Look())
	{
		UE_LOG(LogTemp, Error, TEXT("❌ One or more IA_* actions are NULL!"));
		return;
	}

	// Get the Enhanced Input Component
	UEnhancedInputComponent* EnhancedInputComponent = FroggyCharacter->GetEnhancedInputComponent();
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ EnhancedInputComponent not found! Input binding will fail."));
		return;
	}
	
	BindInputs(FroggyCharacter, EnhancedInputComponent);

	// Ensure the character has a valid Input Mapping Context
	UInputMappingContext* MappingContext = FroggyCharacter->GetMappingContext();
	if (MappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("🐸 Mapping Context Added!"));
		InputSubsystem->AddMappingContext(MappingContext, 0);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Mapping Context NOT Added inside ProtagonistController.cpp!"));
	}
}

void AProtagonistController::BindInputs(AFroggyCharacter* FroggyCharacter, UEnhancedInputComponent* EnhancedInputComponent)
{
	// Bind Move action
	if (FroggyCharacter->GetIA_Move())
		EnhancedInputComponent->BindAction(FroggyCharacter->GetIA_Move(), ETriggerEvent::Triggered, FroggyCharacter, &AFroggyCharacter::Move);
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Move is NULL!")); }

	// Bind Look action
	if (FroggyCharacter->GetIA_Look())
		EnhancedInputComponent->BindAction(FroggyCharacter->GetIA_Look(), ETriggerEvent::Triggered, FroggyCharacter, &AFroggyCharacter::Look);
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Look is NULL!")); }

	// Bind Sit actions
	if (FroggyCharacter->GetIA_Sit())
	{
		EnhancedInputComponent->BindAction(FroggyCharacter->GetIA_Sit(), ETriggerEvent::Started, FroggyCharacter, &AFroggyCharacter::Sit);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Sit is NULL!")); }

	// Bind Interact actions
	if (FroggyCharacter->GetIA_Interact())
	{
		EnhancedInputComponent->BindAction(FroggyCharacter->GetIA_Interact(), ETriggerEvent::Started, FroggyCharacter, &AFroggyCharacter::StartInteract);
		EnhancedInputComponent->BindAction(FroggyCharacter->GetIA_Interact(), ETriggerEvent::Completed, FroggyCharacter, &AFroggyCharacter::StopInteract);
	}
	else { UE_LOG(LogTemp, Error, TEXT("❌ IA_Interact is NULL!")); }
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "FroggyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InteractableItem.h"
#include "ProtagonistController.h"

/**
	* Overview and Execution Order of the code:
	* 1. Constructor AFroggyCharacter() - When actor is instantiated (before Play mode).
	* 2. NotifyControllerChanged() - When the character gets a controller. (And when you exit Playmode)
	* 3. SetupPlayerInputComponent() - Runs when Unreal sets up input for the character.
	*
	* not added, but if desired 4. BeginPlay() - Character is now fully in the world, and Play Mode is started.
	* not added, but if desired 5. Tick(DeltaTime) - Every frame and runs 60+ times per second.
	*
	* Move(), Interact(), etc. - Only runs when buttons are pressed.
 */

// Sets default values, initializing the values for better, robust code.
AFroggyCharacter::AFroggyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Create the camera boom (spring arm) and attach it to the root
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // Distance between camera and the player it follows
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create the follow camera and attach it to the boom's socket
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to the arm

	// Create the SphereCollision for pickups
	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);
	PickupSphere->InitSphereRadius(PickupRadius);

	// Just to test and practice logging:
	// Being mindful that floats have to be limited due too many decimal spaces: %.2f = 2 decimals, %.1f = 1 decimal.
	// And strings need a * in front of them, otherwise no print for you.
	// unsigned 32-bit ints would be %u.

	/*
	int32 Score = 125;
	float Health = 87.5f;
	FString PlayerName = "Froggy";

	UE_LOG(LogTemp, Warning, TEXT("Just doing some test logging, to try format specifiers!"));
	UE_LOG(LogTemp, Warning, TEXT("Froggy %s has %d points and %.1f health left!"), *PlayerName, Score, Health);
	
	UE_LOG(LogTemp, Warning, TEXT("🐸 FroggyCharacter Constructed!"));
	*/
}

void AFroggyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Run timer for checking pickup items in given interval; and run the corresponding function CheckForNearbyPickups.
	GetWorld()->GetTimerManager().SetTimer(PickupTimerHandle, this, &AFroggyCharacter::CheckForNearbyItems, PickupCheckTimeInterval, true);
}

void AFroggyCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// This nasty piece of log (sneaky conditional statement) actually finds the controller that triggers NotifyControllerChanged.
	// If GetController returns a name, we print that name, else we print nothing.
	UE_LOG(LogTemp, Warning, TEXT("🐸 NotifyControllerChanged Called! Controller: %s"), 
	       (GetController() ? *GetController()->GetName() : TEXT("None")));
}

void AFroggyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Ensuring everything is initialized, then trying to get the ProtagonistController
	ProtagonistController = Cast<AProtagonistController>(GetController());
	
	if (ProtagonistController)
	{
		UE_LOG(LogTemp, Warning, TEXT("🐸 Found ProtagonistController!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ ProtagonistController is NULL!"));
	}

	/**
	 *	Just want to highlight that the method of loading assets dynamically in runtime via BeginPlay() and StaticLoadObject,
	 *	is different from the ConstructorHelper::FObjectFinder, which can only be done inside constructors:
	 *
	 *	Just leaving the example here for how you would do it in the constructor with ConstructorHelper.
	 * 
	 *  static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveFinder(TEXT("InputAction'/Game/Input/IA_Move.IA_Move'"));
	 *  if (IA_MoveFinder.Succeeded()) { IA_Move = IA_MoveFinder.Object; }
	 *  else
	 *  {
	 * 	 UE_LOG(LogTemp, Error, TEXT("Failed to load IA_Move!"));
	 *  }
	 */
	
	// Cast to EnhancedInputComponent and set-up action bindings, if not nullptr
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// If IMC_Player null, assign it the InputMappingContext in the described path.
		if (!IMC_Player)
		{
			IMC_Player = Cast<UInputMappingContext>(StaticLoadObject(UInputMappingContext::StaticClass(), nullptr, TEXT("InputMappingContext'/Game/Input/IMC_Player.IMC_Player'")));
			if (IMC_Player) { UE_LOG(LogTemp, Warning, TEXT("🐸 Successfully loaded InputMappingContext")); }
			else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load InputMappingContext")); }
		}

		if (!IA_Move)
		{
			IA_Move = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Move.IA_Move'")));
			if (IA_Move) { UE_LOG(LogTemp, Warning, TEXT("🐸 Successfully loaded IA_Move")); }
			else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Move!")); }
		}
	
		if (!IA_Sit)
		{
			IA_Sit = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Sit.IA_Sit'")));
			if (IA_Sit) { UE_LOG(LogTemp, Warning, TEXT("🐸 Successfully loaded IA_Sit")); }
			else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Sit!")); }
		}
	
		if (!IA_Interact)
		{
			IA_Interact = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Interact.IA_Interact'")));
			if (IA_Interact) { UE_LOG(LogTemp, Warning, TEXT("🐸 Successfully loaded IA_Interact")); }
			else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Interact!")); }
		}
	
		if (!IA_Look)
		{
			IA_Look = Cast<UInputAction>(StaticLoadObject(UInputAction::StaticClass(), nullptr, TEXT("InputAction'/Game/Input/IA_Look.IA_Look'")));
			if (IA_Look) { UE_LOG(LogTemp, Warning, TEXT("🐸 Successfully loaded IA_Look")); }
			else { UE_LOG(LogTemp, Error, TEXT("❌ Failed to load IA_Look!")); }
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent not found!"))
	}
}

void AFroggyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (ProtagonistController)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFroggyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (ProtagonistController != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * LookSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * LookSensitivity);
	}
}

void AFroggyCharacter::Sit(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("FroggyCharacter.cpp Sit() called!"));
	
	if (bool bPressed = Value.Get<bool>())
	{
		bIsSitting = !bIsSitting;
		
		UE_LOG(LogTemp, Display, TEXT("Froggy is now sitting"));
		// TODO: Perform sitting logic here. :)
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Froggy is no longer sitting"));
		// TODO: Handle standing up logic here. :)
	}
}

void AFroggyCharacter::CheckForNearbyItems()
{
	UE_LOG(LogTemp, Display, TEXT("Checking for nearby items..."));
	TArray<AActor*> NearbyItems;
	GetOverlappingActors(NearbyItems, AInteractableItem::StaticClass());

	for (AActor* Actor : NearbyItems)
	{
		AInteractableItem* Item = Cast<AInteractableItem>(Actor);
		if (Item && FVector::Dist(Item->GetActorLocation(), GetActorLocation()) <= PickupRadius)
		{
			// Gotta cast(?) the AActor found (which is filtered) to be AInteractableItem for the function.
			PickupAnItem(Item);
		}
	}
}

void AFroggyCharacter::PickupAnItem(AInteractableItem* Item)
{
	UE_LOG(LogTemp, Display, TEXT("PickupAnItem() from Froggy called to: %s"), *Item->GetName());
	Item->PickupItem();
}

// When Interact Input is received.
void AFroggyCharacter::Interact()
{
	// If... I have to manually find the playerLocation.
	// FVector PlayerLocation = GetActorLocation();
	
	TArray<AActor*> OverlappingActors;
	
	// The below functions just detects if the PlayerCollision overlaps with an AInteractableItem.
	GetOverlappingActors(OverlappingActors, AInteractableItem::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		AInteractableItem* Item = Cast<AInteractableItem>(Actor);
		if (Item)
		{
			Item->Interact();
			UE_LOG(LogTemp, Warning, TEXT("Froggy is interacting with %s!"), *Item->GetName());
			return;
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Froggy is interacting - but no interactable nearby!"));
}

// For hold interactions:
void AFroggyCharacter::StartInteract(const FInputActionValue& Value)
{
	InteractHoldTime = 0.0f; // Reset timer
	GetWorld()->GetTimerManager().SetTimer(InteractHoldTimerHandle, this, &AFroggyCharacter::UpdateHoldTime, 0.05f, true);

	UE_LOG(LogTemp, Display, TEXT("Froggy started holding interact!"));
}

void AFroggyCharacter::StopInteract(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().ClearTimer(InteractHoldTimerHandle); // stop tracking timer

	if (InteractHoldTime >= InteractHoldTimeThreshold) // For example, hold for 1.5 seconds = Long Interact, else Short Interact
	{
		PerformLongInteract();
	}
	else
	{
		PerformShortInteract();
	}

	UE_LOG(LogTemp, Display, TEXT("Froggy stopped interacting after %.2f seconds!"), InteractHoldTime);
}

void AFroggyCharacter::UpdateHoldTime()
{
	InteractHoldTime += 0.05f; // Increase hold time by timer interval
}

void AFroggyCharacter::PerformShortInteract()
{
	UE_LOG(LogTemp, Display, TEXT("Froggy does a quick interact!"));
	// Calling the function again to run the "tap" interact event again. Can also be swapped with OnInteract(false);
	Interact(); 
}

void AFroggyCharacter::PerformLongInteract()
{
	UE_LOG(LogTemp, Display, TEXT("Froggy does a long interact!"));
	OnInteract(true);
}

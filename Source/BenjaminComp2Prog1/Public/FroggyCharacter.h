// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "Logging/LogMacros.h"
#include "InputActionValue.h"
#include "InteractableItem.h"
#include "FroggyCharacter.generated.h"

/**
 * Declaring forward these classes to be utilized ahead of time, without the full definition.
 * Why? - It speeds up compilation and reduces unnecessary dependencies.
 * However, these classes are only usable when I want to store a pointer to the class, like USpringArmComponent* CameraBoom.
 * I HAVE TO #include the respective header when I want to use a class, or call its methods / functions.
 *
 * The struct FInputActionValue forward declares just the struct, not the entire header it is from with other headers.
 * It's like forwarding a class, but a struct is public and can be accessed. The compiler doesn't care to about what's inside
 * FInputActionValue until later, only when compiling the .cpp file.
*/
class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UInputMappingContext;
class UInputAction;

class AProtagonistController;

struct FInputActionValue;
/**
 * UCLASS(Config=Game) means that default values are being stored in DefaultGame.ini, letting us tweak settings directly
 * in the .ini files. It means we can change the values in .ini without having to recompile the game every time.
 * 
 * "Config=" doesn't always have to lead to "Game", it can lead to =Engine, =Editor, =Input, etc.
 * Config only loads once at start-up though, so don't use it with variables that need to change in run-time.
 *
 * For dynamic values, use USTRUCT: e.g. USTRUCT(BlueprintType) struct FPlayerStats { GENERATED_BODY() ... }
 * and store it in your character class: UPROPERTY(EditAnywhere, BlueprintReadWrite, ...) FPlayerStats PlayerStats;
 * If you need dynamic values saved: use UCharacterSaveGame : public USaveGame, with functions ::SaveGame and ::LoadGame
 * 
 * Properties that should always be visible in Blueprints, should utilize UPROPERTY and "EditAnywhere" instead.
 * But... for C++ only coders, Config= and .ini files are great too. :)
*/

UCLASS(Config=Game)
class BENJAMINCOMP2PROG1_API AFroggyCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Sphere Collision (Pick-up Radius) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USphereComponent* PickupSphere;

	/** Custom Protagonist Controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller", meta = (AllowPrivateAccess = "true"))
	AProtagonistController* ProtagonistController;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Player;

	// The radius in which the player can interact/pick up items
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	float PickupRadius = 110.0f;

	/** Interact Hold Threshold, before long Interact happens */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = "true"))
	float InteractHoldTimeThreshold = 1.0f; // How long it takes for LongInteract to happen

	/** Look Sensitivity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character", meta = (AllowPrivateAccess = "true"))
	float LookSensitivity = 1.0f; // How much to multiply the mouse-look input with

	/** Is the Froggy sitting? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	bool bIsSitting = false; // Tracks if the player is sitting.
	
	FTimerHandle InteractHoldTimerHandle; // Timer Handle to help track hold duration with Interact
	float InteractHoldTime = 0.0f; // Stores how long the Interact button is held;
	
	FTimerHandle PickupTimerHandle; // Timer Handle for often the player checks for nearby pick-ups.
	float PickupCheckTimeInterval = 0.10f; // The actual time between checks in float value. 

public:
	// Sets default values for this character's properties
	AFroggyCharacter();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void CheckForNearbyItems();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void PickupAnItem(AInteractableItem* Item);
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Interact();

	// Blueprint event that designers or others can override to define custom behaviour
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteract(bool bWasLongInteract);

	/** Called for movement input */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void Move(const FInputActionValue& Value);

	/** Called for camera input */
	void Look(const FInputActionValue& Value);

	/** Called for sit input */
	void Sit(const FInputActionValue& Value);

	/** Called for Interact input */
	void StartInteract(const FInputActionValue& Value);

	/** Called for Interact input */
	void StopInteract(const FInputActionValue& Value);
	
protected:
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Move;

	/** Sit Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Sit;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Interact;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Look;

	/** Called when Interacting; has to do with short or long interact */
	void UpdateHoldTime();
	void PerformShortInteract();
	void PerformLongInteract();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void NotifyControllerChanged() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:
	
	/**
	 * FORCEINLINE forces the compiler to inline the function, but it increases the size of it.
	 * Ideal for very small, frequently called getters, to reduce function call overhead. These just return a pointer.
	 * Big functions would leave copies of inlined code everywhere...
	 * For example:
	 * Normal Function: 1x 20 bytes = 20 bytes total (just one function in memory).
	 * 10 Inline Functions: 10x 20 bytes = 200 bytes total (10 separate copies!).
	*/

	// Getter methods to access IA_* actions
	UInputAction* GetIA_Move() const { return IA_Move; }
	UInputAction* GetIA_Sit() const { return IA_Sit; }
	UInputAction* GetIA_Interact() const { return IA_Interact; }
	UInputAction* GetIA_Look() const { return IA_Look; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool GetIsSitting() const { return bIsSitting; }
	
	UFUNCTION(BlueprintCallable, Category = Input)
	UEnhancedInputComponent* GetEnhancedInputComponent() const { return Cast<UEnhancedInputComponent>(InputComponent); }
	
	// Getter function for the mapping context
	UFUNCTION(BlueprintCallable, Category = Input)
	FORCEINLINE class UInputMappingContext* GetMappingContext() const { return IMC_Player; }
	
	/** Returns CameraBoom subObject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subObject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

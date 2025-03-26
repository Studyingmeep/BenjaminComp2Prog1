// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class USoundBase;

UCLASS()
class BENJAMINCOMP2PROG1_API AInteractableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Function to handle interaction
	UFUNCTION(BlueprintCallable, Category = "Bools & Interaction")
	void Interact();

	// Function to handle Pick-up
	UFUNCTION(BlueprintCallable, Category = "Bools & Interaction")
	void PickupItem();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Components")
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ObjectMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Object Components", meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* PointLight;

	// Interaction Properties
	// Add your given interaction sound to play when the object is interacted with. (on the object itself in Content Browser)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bools & Interaction")
	USoundBase* InteractionSound;

	// Toggle whether the object destroys itself on interact (editable from Details panel, thanks to EditAnywhere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools & Interaction")
	bool bDestroyOnInteract = false;

	// Toggle whether the light should turn on/off when interacted with
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools & Interaction")
	bool bToggleLight = false;

	// Is this object a pick-up? If so, it will run only pick-up functionality.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bools & Interaction")
	bool bIsAPickup = false;

private:
	bool bLightOn = true;
};

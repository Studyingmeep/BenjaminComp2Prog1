// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableItem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AInteractableItem::AInteractableItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Create Root SceneComponent
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// Setting up the RootComponent the other components attach to. Always needs to be one, and should be SceneComponent.
	SetRootComponent(Root);

	// Create StaticMeshComponent, and attaching it to the root (is done with all components after sceneComponent)
	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	ObjectMesh->SetupAttachment(Root);
	
	// Using ConstructorHelper to help find the right mesh to set for our object, via the header "UObject/ConstructorHelpers.h".
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("Static Mesh'/Engine/BasicShapes/Cube.Cube'"));
    if (MeshAsset.Succeeded())
    {
    	UE_LOG(LogTemp, Warning, TEXT("👍 Loading object mesh %s!"), *MeshAsset.Object->GetName());
	    ObjectMesh->SetStaticMesh(MeshAsset.Object);
    	// Adjusting scale
    	ObjectMesh->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));
    }
    else
    {
	    UE_LOG(LogTemp, Warning, TEXT("❌ Failed to load object mesh"));
    }

	// Ensure correct relative transform of ObjectMesh
	ObjectMesh->SetRelativeLocation(FVector(0, 0, 0));

	// Create CollisionSphere Component (to enable interactions)
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(Root);
	
	InteractionSphere->SetSphereRadius(80.0f);
	InteractionSphere->SetGenerateOverlapEvents(true);
	InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Create PointLight Component
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(Root);
	
	PointLight->SetVisibility(true);
	PointLight->SetAttenuationRadius(100.0f);
	PointLight->SetIntensity(200.0f);
}

// Called when the game starts or when spawned
void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractableItem::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("%s was interacted with by %s"), *GetName(), *UGameplayStatics::GetPlayerPawn(this, 0)->GetName());

	// Play sound if assigned
	if (InteractionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());
	}

	// Toggle the light, if enabled on the object
	if (bToggleLight)
	{
		bLightOn = !bLightOn;
		PointLight->SetVisibility(bLightOn);

		if (bLightOn)
		{
			UE_LOG(LogTemp, Warning, TEXT("Light toggled: %s"), TEXT("ON"));
			check(GEngine != nullptr);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Lights going on."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Light toggled: %s"), TEXT("OFF"));
			check(GEngine != nullptr);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Silver, TEXT("Lights going off."));
		}
	}

	// Destroy object if set to do so
	if (bDestroyOnInteract)
	{
		FString objectName = this->GetName();
		
		check(GEngine != nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Goodbye World! ...but remember me as " + objectName));
		
		Destroy();
	}
}

// Since we're adding pick-up functionality to an interactable item, this really should have been split into
// two separate classes. One for Interactables and one for pickups, but this works ok for this tiny project. :3
void AInteractableItem::PickupItem()
{
	if (!bIsAPickup) return;
	
	// TODO: If desired, run HUD code or other features. : )
	
	FString objectName = this->GetName();
		
	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("I'm being picked up! ... remember me as " + objectName));
		
	Destroy();
}


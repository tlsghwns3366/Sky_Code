// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkySeoulCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Inventory/InventoryComponent.h"
#include "InteractionInterface.h"
#include "Character/RobotCharacter.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "ProjectPlayerState.h"
#include "Controller/PlayerHanController.h"
#include "Character/Abilities/CharacterGameplayAbility.h"
#include "Character/Abilities/Attributes/RobotAttributeSetBase.h"
#include "Character/Abilities/AbilitySetData.h"
#include "Character/ActorComponent/EquipmentComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASkySeoulCharacter

ASkySeoulCharacter::ASkySeoulCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	FocusedActor = nullptr;
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	AIControllerClass = APlayerController::StaticClass();
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EquipComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipComponent"));

}

void ASkySeoulCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->GetRelativeLocation();
}

void ASkySeoulCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ForwardTrace();
}


void ASkySeoulCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	if (PS)
	{
		InitializeStartingValues(PS);
	}
}


void ASkySeoulCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	if (PS)
	{
		RobotAttributeSetBase = PS->GatRobotAttributeSetBase();
	}
}

void ASkySeoulCharacter::ForwardTrace()
{
	FVector Loc;
	FRotator Rot;
	FHitResult HitResult;
	GetController()->GetPlayerViewPoint(Loc, Rot);

	FVector Start = Loc;
	FVector End = Start + (Rot.Vector() * 1000.f);

	FCollisionQueryParams TraceParams;
	bool Result = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel2, TraceParams);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false, 2.0f);
	if (Result)
	{
		AActor* Interactable = HitResult.GetActor();
		if (Interactable)
		{
			if (FocusedActor != Interactable)
			{
				if (FocusedActor)
				{
					EndFocus(FocusedActor);
				}
				StartFocus(Interactable);
				FocusedActor = Interactable;
			}
		}
	}
	else
	{
		if (FocusedActor)
		{
			EndFocus(FocusedActor);
		}
		FocusedActor = nullptr;
	}
}

void ASkySeoulCharacter::LocationTrace()
{
	FVector Loc;
	FRotator Rot;
	FHitResult HitResult;
	GetController()->GetPlayerViewPoint(Loc, Rot);
	FVector Start = Loc + (Rot.Vector() * 500.f);
	FVector End = Start + (Rot.Vector() * 3000.f);
	FCollisionQueryParams TraceParams;
	bool Result = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, TraceParams);
	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0f);
	if (Result)
	{
		RobotLocation = HitResult.Location;
	}
	else
	{
		RobotLocation = End;
	}
}

void ASkySeoulCharacter::Interaction()
{
	if (!FocusedActor)
		return;

	if (IInteractionInterface* Interface = Cast<IInteractionInterface>(FocusedActor))
	{
		Interface->Execute_OnInteract(FocusedActor, this);
	}
}

void ASkySeoulCharacter::StartFocus(AActor* TraceActor)
{
	IInteractionInterface* Interface = Cast<IInteractionInterface>(TraceActor);
	if (Interface)
	{
		Interface->Execute_StartFocus(TraceActor);
	}
}

void ASkySeoulCharacter::EndFocus(AActor* TraceActor)
{
	IInteractionInterface* Interface = Cast<IInteractionInterface>(TraceActor);
	if (Interface)
	{
		Interface->Execute_EndFocus(TraceActor);
	}
}

bool ASkySeoulCharacter::RobotCostCheck(int32 RobotCost)
{
	int32 CurrentRobotCost = RobotAttributeSetBase->GetRobotCost();
	if (RobotAttributeSetBase->GetMaxRobotCost() < CurrentRobotCost + RobotCost)
		return true;

	return false;
}

bool ASkySeoulCharacter::AddRobot(ARobotCharacter* RobotCharacter)
{
	int32 CurrentRobotCost = RobotAttributeSetBase->GetRobotCost();
	RobotAttributeSetBase->SetRobotCost(CurrentRobotCost + RobotCharacter->RobotCost);
	RobotArray.AddUnique(RobotCharacter);
	ChangeRobot(RobotArray.Num() - 1);
	return true;
}

void ASkySeoulCharacter::RemoveRobot(int32 Value)
{
	int32 CurrentRobotCost = RobotAttributeSetBase->GetRobotCost();
	int32 MaxCost = RobotAttributeSetBase->GetMaxRobotCost();
	RobotAttributeSetBase->SetRobotCost(FMath::Clamp(CurrentRobotCost - RobotArray[SelectRobotNumber]->RobotCost, 0, MaxCost));
	RobotArray.RemoveAt(SelectRobotNumber);
	ChangeRobotRight();
}

float ASkySeoulCharacter::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector ASkySeoulCharacter::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}

void ASkySeoulCharacter::ChangeRobotLeft()
{
	if (RobotArray.Num() == 0)
	{
		OnRobotUpdated.Broadcast();
		return;
	}

	if (RobotArray.Num() > SelectRobotNumber)
		RobotArray[SelectRobotNumber]->SetSelect(false);

	SelectRobotNumber--;
	if (SelectRobotNumber < 0)
		SelectRobotNumber = RobotArray.Num() - 1;

	if (RobotArray.Num() > SelectRobotNumber)
		RobotArray[SelectRobotNumber]->SetSelect(true);
	OnRobotUpdated.Broadcast();
}

void ASkySeoulCharacter::ChangeRobotRight()
{
	if (RobotArray.Num() == 0)
	{
		OnRobotUpdated.Broadcast();
		return;
	}

	if (RobotArray.Num() > SelectRobotNumber)
		RobotArray[SelectRobotNumber]->SetSelect(false);

	SelectRobotNumber++;
	if (SelectRobotNumber >= RobotArray.Num())
		SelectRobotNumber = 0;

	if (RobotArray.Num() > SelectRobotNumber)
		RobotArray[SelectRobotNumber]->SetSelect(true);
	OnRobotUpdated.Broadcast();
}

void ASkySeoulCharacter::ChangeRobot(const int32 Value)
{
	if (RobotArray.Num() > SelectRobotNumber)
		RobotArray[SelectRobotNumber]->SetSelect(false);

	SelectRobotNumber = Value;
	if (SelectRobotNumber >= RobotArray.Num())
		SelectRobotNumber = 0;

	if (RobotArray.Num() > SelectRobotNumber)
		RobotArray[Value]->SetSelect(true);
	OnRobotUpdated.Broadcast();
}

void ASkySeoulCharacter::SetAbilityData(UAbilitySetData* Data)
{
	RemoveAbility();
	if (Data == nullptr)
	{
		AbilityActionData = nullptr;
	}
	else
	{
		AbilityActionData = Data;
		AddAbility();
	}
}

void ASkySeoulCharacter::AddAbility()
{
	if (AbilityActionData == nullptr)
		return;

	FAbilitySetData_GrantedHandles& NewHandle  = AbilityHandles.AddDefaulted_GetRef();
	AbilityActionData->GiveToAbilitySystem(AbilitySystemComponent, &NewHandle);
}

void ASkySeoulCharacter::RemoveAbility()
{
	for (auto It = AbilityHandles.CreateIterator(); It; It++)
	{
		FAbilitySetData_GrantedHandles& Entry = *It;
		if (AbilitySystemComponent)
		{
			Entry.TakeFromAbilitySystem(AbilitySystemComponent);
		}
		It.RemoveCurrent();
	}
}

void ASkySeoulCharacter::RequestSelectAction(const int32 Num)
{
	SelectNumber = Num;
}
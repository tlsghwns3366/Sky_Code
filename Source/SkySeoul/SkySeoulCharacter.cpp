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

#include "InteractionInterface.h"
#include "Character/RobotCharacter.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "ProjectPlayerState.h"
#include "Controller/PlayerHanController.h"
#include "Character/Abilities/CharacterGameplayAbility.h"

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

void ASkySeoulCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	BindASCInput();
}

void ASkySeoulCharacter::InitializeStartingValues(AProjectPlayerState* PS)
{
	AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSetBase = PS->GatAttributeSetBase();
	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	InitializeAttributes();
	SetHealth(GetMaxHealth());
}

void ASkySeoulCharacter::BindASCInput()
{
	//사용안되는걸로 판단됨
	if (!ASCInputBound && AbilitySystemComponent.IsValid() && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"), FString("CancelTarget"), FString("SkySeoulAbilityID"), static_cast<int32>(SkySeoulAbilityID::Confirm), static_cast<int32>(SkySeoulAbilityID::Cancel)));
		ASCInputBound = true;
	}
}

void ASkySeoulCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	if (PS)
	{
		InitializeStartingValues(PS);

		AddStartupEffects();
		AddCharacterAbilities();
	}
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

bool ASkySeoulCharacter::AddRobot(ARobotCharacter* RobotCharacter)
{
	if (RobotArray.Num() >= MaxRobotCount)
		return false;
	RobotArray.AddUnique(RobotCharacter);
	ChangeRobot(RobotArray.Num()-1);
	return true;
}

float ASkySeoulCharacter::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector ASkySeoulCharacter::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}

void ASkySeoulCharacter::SelectRobotAction()
{
	if (RobotArray.Num() > SelectRobotNumber)
	{
		switch (SelectNumber)
		{
		case 1:
			RobotArray[SelectRobotNumber]->PlayerSelectState(1);
			break;
		case 2:
			RobotLocation.Z = 0.f;
			RobotArray[SelectRobotNumber]->MoveAction(RobotLocation);
			break;
		case 3:
			RobotArray[SelectRobotNumber]->PlayerSelectState(3);
			RobotArray[SelectRobotNumber]->SetSelect(false);
			RobotArray.RemoveAt(SelectRobotNumber);
			ChangeRobot();
			break;
		case 4:
			RobotArray[SelectRobotNumber]->RobotExplosion();
			RobotArray[SelectRobotNumber]->SetSelect(false);
			RobotArray.RemoveAt(SelectRobotNumber);
			ChangeRobot();
			break;
		default:
			break;
		}
	}
}

void ASkySeoulCharacter::ChangeRobot()
{
	if(RobotArray.Num() > SelectRobotNumber)
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

void ASkySeoulCharacter::RequestSelectAction(const int32 Num)
{
	SelectNumber = Num;
}
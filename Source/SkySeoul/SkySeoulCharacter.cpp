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

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASkySeoulCharacter

ASkySeoulCharacter::ASkySeoulCharacter()
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
}

void ASkySeoulCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ASkySeoulCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ForwardTrace();
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
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0f);
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
	return true;
}

void ASkySeoulCharacter::SelectRobotAction()
{
	if (RobotArray.Num() > SelectRobotNumber)
	{

		GetWorld()->GetTimerManager().ClearTimer(RobotLocationTimer);
		RobotLocation.Z = 0.f;
		RobotArray[SelectRobotNumber]->MoveAction(RobotLocation);

		/*
		switch (SelectNumber)
		{
		case 1:
			GetWorld()->GetTimerManager().SetTimer(RobotLocationTimer, this, &ASkySeoulCharacter::LocationTrace, 0.01f, true);
			break;
		case 2:
			GetWorld()->GetTimerManager().ClearTimer(RobotLocationTimer);
			RobotArray[SelectRobotNumber]->MoveAction(RobotLocation);
			break;
		default:
			break;
		}*/
	}
}

void ASkySeoulCharacter::RequestSelectAction(const FInputActionValue& Value)
{
	int32 Num = Value.Get<float>();
	SelectNumber = Num;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASkySeoulCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASkySeoulCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASkySeoulCharacter::Look);

		//Interaction
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &ASkySeoulCharacter::Interaction);

		//Select
		//EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Triggered, this, &ASkySeoulCharacter::SelectRobotAction);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Triggered, this, &ASkySeoulCharacter::LocationTrace);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Completed, this, &ASkySeoulCharacter::SelectRobotAction);
		
		// ** SelectNumberAction ** //
		EnhancedInputComponent->BindAction(SelectNumberAction, ETriggerEvent::Triggered, this, &ASkySeoulCharacter::RequestSelectAction);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASkySeoulCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASkySeoulCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
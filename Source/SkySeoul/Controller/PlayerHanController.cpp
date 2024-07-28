// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PlayerHanController.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include "Character/SkySeoulCharacter.h"
#include "ProjectPlayerState.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include	"ProjectPlayerState.h"

void APlayerHanController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<ASkySeoulCharacter>(GetCharacter());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	DefaultInitialization();
}

void APlayerHanController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	if (PS)
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void APlayerHanController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	//프레임에 한번 실행된다.
	UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetCharacterAbilitySystemComponent());
	if (ASC)
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void APlayerHanController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerHanController::ControllerJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerHanController::ControllerStopJump);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerHanController::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerHanController::Look);

		//Interaction
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &APlayerHanController::Interaction);

		//Select
		//EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Completed, this, &APlayerHanController::Select);
		//EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Triggered, this, &APlayerHanController::LocationTrace);

		// ** SelectNumberAction ** //
		EnhancedInputComponent->BindAction(SelectNumberAction, ETriggerEvent::Triggered, this, &APlayerHanController::SelectNumber);

	}
}

void APlayerHanController::Move(const FInputActionValue& Value)
{
	if (!PlayerCharacter)
		return;
	if (PlayerCharacter->IsAlive())
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		PlayerCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
		PlayerCharacter->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerHanController::Look(const FInputActionValue& Value)
{
	if (PlayerCharacter->IsAlive())
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		// add yaw and pitch input to controller
		PlayerCharacter->AddControllerYawInput(LookAxisVector.X);
		PlayerCharacter->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerHanController::ControllerJump()
{
	if (!PlayerCharacter)
		return;
	if (PlayerCharacter->IsAlive())
	{
		PlayerCharacter->Jump();
	}
}

void APlayerHanController::ControllerStopJump()
{
	if (!PlayerCharacter)
		return;
	if (PlayerCharacter->IsAlive())
	{
		PlayerCharacter->StopJumping();
	}
}

void APlayerHanController::Interaction()
{
	if (!PlayerCharacter)
		return;
	if (PlayerCharacter->IsAlive())
	{
		PlayerCharacter->Interaction();
	}
}

void APlayerHanController::SelectNumber(const FInputActionValue& Value)
{
	if (!PlayerCharacter)
		return;
	if (PlayerCharacter->IsAlive())
	{
		int32 Num = Value.Get<float>();
		PlayerCharacter->RequestSelectAction(Num);
	}
}

void APlayerHanController::LocationTrace()
{
	if (!PlayerCharacter)
		return;

	if (PlayerCharacter->IsAlive()) 
	{
		PlayerCharacter->LocationTrace();
	}
}
void APlayerHanController::DefaultInitialization()
{
	if (!PlayerCharacter)
		return;
	if (const USkySeoulInputConfig* InputConfig = PlayerCharacter->InputConfig)
	{
		TArray<uint32> BindHandles;
		BindAbilityActions(InputConfig,this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	}
}

void APlayerHanController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetCharacterAbilitySystemComponent()))
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void APlayerHanController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetCharacterAbilitySystemComponent()))
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

UAbilitySystemComponent* APlayerHanController::GetCharacterAbilitySystemComponent() const
{
	const AProjectPlayerState* PPS = CastChecked<AProjectPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
	if (PPS)
		return PPS->GetAbilitySystemComponent();
	else
		return nullptr;
}

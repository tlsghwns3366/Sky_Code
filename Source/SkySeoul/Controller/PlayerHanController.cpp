// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PlayerHanController.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include "SkySeoulCharacter.h"

void APlayerHanController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<ASkySeoulCharacter>(GetCharacter());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
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
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Completed, this, &APlayerHanController::Select);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Triggered, this, &APlayerHanController::LocationTrace);

		// ** SelectNumberAction ** //
		EnhancedInputComponent->BindAction(SelectNumberAction, ETriggerEvent::Triggered, this, &APlayerHanController::SelectNumber);

		// ** TabAction ** //
		EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Triggered, this, &APlayerHanController::ChangeRobotNumber);

	}
}

void APlayerHanController::Move(const FInputActionValue& Value)
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

void APlayerHanController::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	// add yaw and pitch input to controller
	PlayerCharacter->AddControllerYawInput(LookAxisVector.X);
	PlayerCharacter->AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerHanController::ControllerJump()
{
	if (!PlayerCharacter)
		return;
	PlayerCharacter->Jump();
}

void APlayerHanController::ControllerStopJump()
{
	if (!PlayerCharacter)
		return;
	PlayerCharacter->StopJumping();
}

void APlayerHanController::Select()
{
	if (!PlayerCharacter)
		return;
	PlayerCharacter->SelectRobotAction();
}

void APlayerHanController::Interaction()
{
	if (!PlayerCharacter)
		return;
	PlayerCharacter->Interaction();
}

void APlayerHanController::SelectNumber(const FInputActionValue& Value)
{
	if (!PlayerCharacter)
		return;
	PlayerCharacter->RequestSelectAction(Value);
}

void APlayerHanController::LocationTrace()
{
	if (!PlayerCharacter)
		return;
	PlayerCharacter->LocationTrace();
}

void APlayerHanController::ChangeRobotNumber()
{
	if (!PlayerCharacter)
		return;
	PlayerCharacter->ChangeRobot();

}

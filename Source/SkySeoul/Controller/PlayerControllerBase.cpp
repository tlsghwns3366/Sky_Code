// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PlayerControllerBase.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include "MyGameInstance.h"
#include "Character/SkySeoulCharacter.h"
#include "ProjectPlayerState.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include	"ProjectPlayerState.h"



void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<ASkySeoulCharacter>(GetCharacter());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	DefaultInitialization();

	mpGameInstance = Cast<UMyGameInstance>(GetGameInstance());
}

void APlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	if (PS)
	{
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void APlayerControllerBase::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	//프레임에 한번 실행된다.
	UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetCharacterAbilitySystemComponent());
	if (ASC)
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerControllerBase::ControllerJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APlayerControllerBase::ControllerStopJump);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerControllerBase::Look);
	}
}

void APlayerControllerBase::Move(const FInputActionValue& Value)
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

		// Photon Event
		mpGameInstance->SendMove(Rotation.Yaw, MovementVector);
	}
}

void APlayerControllerBase::Look(const FInputActionValue& Value)
{
	if (PlayerCharacter->IsAlive())
	{
		if (UAbilitySystemComponent* ASC = Cast<ACharacterBase>(PlayerCharacter)->GetAbilitySystemComponent())
		{
			const FGameplayTagContainer& Container = ASC->GetOwnedGameplayTags();
			if (Container.HasTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.CameraLock"))))
				return;
		}
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		// add yaw and pitch input to controller
		PlayerCharacter->AddControllerYawInput(LookAxisVector.X);
		PlayerCharacter->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerControllerBase::ControllerJump()
{
	if (!PlayerCharacter)
		return;
	if (PlayerCharacter->IsAlive())
	{
		//PlayerCharacter->Jump();

		// Photon Event
		mpGameInstance->SendJump(true);
	}
}

void APlayerControllerBase::ControllerStopJump()
{
	if (!PlayerCharacter)
		return;
	if (PlayerCharacter->IsAlive())
	{
		PlayerCharacter->StopJumping();

		// Photon Event
		mpGameInstance->SendJump(false);
	}
}

void APlayerControllerBase::DefaultInitialization()
{
	if (!PlayerCharacter)
		return;
	if (const USkySeoulInputConfig* InputConfig = PlayerCharacter->InputConfig)
	{
		TArray<uint32> BindHandles;
		BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
	}
}

void APlayerControllerBase::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetCharacterAbilitySystemComponent()))
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void APlayerControllerBase::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UCharacterAbilitySystemComponent* ASC = Cast<UCharacterAbilitySystemComponent>(GetCharacterAbilitySystemComponent()))
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

UAbilitySystemComponent* APlayerControllerBase::GetCharacterAbilitySystemComponent() const
{
	const AProjectPlayerState* PPS = CastChecked<AProjectPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
	if (PPS)
		return PPS->GetAbilitySystemComponent();
	else
		return nullptr;
}

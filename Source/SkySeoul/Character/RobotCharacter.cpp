// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RobotCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"

#include "SkySeoulCharacter.h"
#include "AI/RobotAIController.h"

// Sets default values
ARobotCharacter::ARobotCharacter(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(GetMesh());
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetVisibility(false);

	ButtonWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ButtonWidgetComponent"));
	ButtonWidgetComponent->SetupAttachment(GetMesh());
	ButtonWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	ButtonWidgetComponent->SetVisibility(false);

}

// Called when the game starts or when spawned
void ARobotCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARobotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARobotCharacter::HackingStart(AActor* Caller)
{
	GetWorld()->GetTimerManager().ClearTimer(HackingAbortTimer);
	if (HackingPercent >= 1.0f)
	{
		HackingEnd(Caller);
	}
	else
	{
		OwnerCharacter = Cast<ACharacter>(Caller);
		ButtonWidgetComponent->SetVisibility(true);
		CurrentHackingPercent = FMath::Clamp(FMath::Lerp(CurrentHackingPercent, CurrentHackingPercent + 1.f, 1.f), 0.f, MaxHackingPercent);
		HackingPercent = FMath::Clamp(CurrentHackingPercent / MaxHackingPercent, 0.f, 1.0f);
		GetWorld()->GetTimerManager().SetTimer(HackingAbortTimer, this, &ARobotCharacter::HackingAbort, 1.0f, true);
	}
	HackingUpdate.Broadcast();
}

void ARobotCharacter::HackingAbort()
{
	CurrentHackingPercent = FMath::Clamp( FMath::Lerp(CurrentHackingPercent, CurrentHackingPercent - 10.f, 1.f),0.f,MaxHackingPercent);
	HackingPercent = FMath::Clamp(CurrentHackingPercent / MaxHackingPercent, 0.f, 1.0f);
	if (HackingPercent <= 0.f)
		GetWorld()->GetTimerManager().ClearTimer(HackingAbortTimer);
	HackingUpdate.Broadcast();
}

void ARobotCharacter::HackingEnd(AActor* Caller)
{
	ActionState = ERobotActionState::E_FollowCharacter;
	InteractionWidgetComponent->SetVisibility(false);
	ASkySeoulCharacter* PlayerCharacter = Cast<ASkySeoulCharacter>(Caller);
	if (PlayerCharacter)
	{
		PlayerCharacter->AddRobot(this);
	}

	GetWorld()->GetTimerManager().ClearTimer(HackingAbortTimer);
	ARobotAIController* RobotController = Cast< ARobotAIController>(GetController());
	if (RobotController)
	{
		RobotController->SetRobotGoalLocation(Caller);
	}
}

void ARobotCharacter::MoveAction(FVector GoalLocation)
{
	ARobotAIController* RobotController = Cast< ARobotAIController>(GetController());
	if (RobotController)
	{
		RobotController->SetRobotGoalLocation(GoalLocation);
	}
}

void ARobotCharacter::SetRobotActionState(ERobotActionState Value)
{
	ActionState = Value;
}

void ARobotCharacter::RobotExplosion()
{
	SetSelect(false);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetMesh()->SetSimulatePhysics(true);
	ActionState = ERobotActionState::E_Explosion;
	AAIController* RobotController = Cast<AAIController>(GetController());
	if (RobotController)
	{
		RobotController->GetBrainComponent()->StopLogic(FString("Die"));
		RobotController->Destroy();
	}
	UCapsuleComponent* RobotCapsule = GetCapsuleComponent();
	if (RobotCapsule)
	{
		RobotCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ARobotCharacter::RobotIsFree()
{
	SetSelect(false);
	ActionState = ERobotActionState::E_Idle;
	GetWorld()->GetTimerManager().SetTimer(HackingAbortTimer, this, &ARobotCharacter::HackingAbort, 1.0f, true);
	ARobotAIController* RobotController = Cast<ARobotAIController>(GetController());
	if (RobotController)
	{
		RobotController->SetOwnerCharacter(nullptr);
	}
}

void ARobotCharacter::SetSelect(bool Value)
{
	IsSelect = Value;
	PlayerSelectUpdate.Broadcast();
}

void ARobotCharacter::StateChange(ERobotState State)
{
	RobotState = State;
}

void ARobotCharacter::OnInteract_Implementation(AActor* Caller)
{
	ASkySeoulCharacter* PlayerCharacter = Cast<ASkySeoulCharacter>(Caller);
	if (PlayerCharacter)
	{
		if (PlayerCharacter->RobotCostCheck(RobotCost))
			return;
	}

	if (ActionState == ERobotActionState::E_Idle)
		HackingStart(Caller);
}

void ARobotCharacter::StartFocus_Implementation()
{
	if(ActionState == ERobotActionState::E_Idle)
		InteractionWidgetComponent->SetVisibility(true);
}

void ARobotCharacter::EndFocus_Implementation()
{
	if (ActionState == ERobotActionState::E_Idle)
	{
		if (HackingPercent > 0.f)
			GetWorld()->GetTimerManager().SetTimer(HackingAbortTimer, this, &ARobotCharacter::HackingAbort, 1.0f, true);
		InteractionWidgetComponent->SetVisibility(false);
	}
}

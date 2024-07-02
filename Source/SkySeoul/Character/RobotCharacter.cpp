// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RobotCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "SkySeoulCharacter.h"
#include "AI/RobotAIController.h"

// Sets default values
ARobotCharacter::ARobotCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComponent"));
	InteractionWidgetComponent->SetupAttachment(GetMesh());
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetVisibility(false);

}

// Called when the game starts or when spawned
void ARobotCharacter::BeginPlay()
{
	Super::BeginPlay();
	HackingUpdate.BindUObject(this, &ARobotCharacter::HackingEnd);
	
}

// Called every frame
void ARobotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ARobotCharacter::HackingStart(AActor* Caller)
{
	HackingUpdate.Execute(Caller);
}

void ARobotCharacter::HackingEnd(AActor* Caller)
{
	RobotState = ERobotState::E_FollowCharacter;
	InteractionWidgetComponent->SetVisibility(false);
	OwnerCharacter = Cast<ACharacter>(Caller);
	Cast<ASkySeoulCharacter>(Caller)->AddRobot(this);
	

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

void ARobotCharacter::PlayerSelect(int32 Value)
{
}

void ARobotCharacter::OnInteract_Implementation(AActor* Caller)
{
	switch (RobotState)
	{
	case ERobotState::E_Idle:
		HackingStart(Caller);
		break;
	case ERobotState::E_FollowCharacter:
		break;
	case ERobotState::E_MoveLocation:
		break;
	default:
		break;
	}
}

void ARobotCharacter::StartFocus_Implementation()
{
	if(RobotState == ERobotState::E_Idle)
		InteractionWidgetComponent->SetVisibility(true);
}

void ARobotCharacter::EndFocus_Implementation()
{
	if (RobotState == ERobotState::E_Idle)
		InteractionWidgetComponent->SetVisibility(false);
}

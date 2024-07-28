// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RobotAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AIPerceptionComponent.h"

#include "Character/CharacterBase.h"

ARobotAIController::ARobotAIController()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SetPerceptionSystem();
}

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!BehaviorTree)
		return;
	RunBehaviorTree(BehaviorTree);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ARobotAIController::SenseDetected);
}

void ARobotAIController::SetRobotGoalLocation(AActor* Caller)
{
	if (!GetBlackboardComponent())
		return;
	SetOwnerCharacter(Caller);
}

void ARobotAIController::SetRobotGoalLocation(FVector Location)
{
	if (!GetBlackboardComponent())
		return;
	GetBlackboardComponent()->SetValueAsVector("GoalLocation", Location);
}

void ARobotAIController::SetOwnerCharacter(AActor* OwnerCharacterActor)
{
	OwnerCharacter = Cast<ACharacterBase>(OwnerCharacterActor);
	GetBlackboardComponent()->SetValueAsObject("OwnerCharacter", OwnerCharacterActor);
}

void ARobotAIController::SetPerceptionSystem()
{
	if (SightConfig)
	{
		SightConfig->SightRadius = 1000.f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f;
		SightConfig->PeripheralVisionAngleDegrees = 45.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;
		SightConfig->PointOfViewBackwardOffset = 150.f;
		SightConfig->NearClippingRadius = 150.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		AIPerceptionComponent->ConfigureSense(*SightConfig);
	}
	if (DamageConfig)
	{
		DamageConfig->SetMaxAge(5.f);
		AIPerceptionComponent->ConfigureSense(*DamageConfig);
	}
}

void ARobotAIController::SenseDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	auto DetectedSence = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
	if (DetectedSence == UAISense_Sight::StaticClass())
	{
		SightDetected(Actor, Stimulus);
	}
	else if (DetectedSence == UAISense_Damage::StaticClass())
	{
		DamageDetected(Actor, Stimulus);
	}
}

void ARobotAIController::SightDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (OwnerCharacter == Cast<ACharacterBase>(Actor))
		return;
	if (Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsObject("SightTarget", Actor);
	}
	else
	{
		GetBlackboardComponent()->SetValueAsObject("SightTarget", nullptr);

	}
}

void ARobotAIController::DamageDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (OwnerCharacter == Cast<ACharacterBase>(Actor))
		return;
	if (Stimulus.WasSuccessfullySensed())
	{
		GetBlackboardComponent()->SetValueAsObject("AttackTarget", Actor);
	}
}

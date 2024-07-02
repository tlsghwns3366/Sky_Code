// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/RobotAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

void ARobotAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!BehaviorTree)
		return;
	RunBehaviorTree(BehaviorTree);
}

void ARobotAIController::SetRobotGoalLocation(AActor* Caller)
{
	if (!GetBlackboardComponent())
		return;
	GetBlackboardComponent()->SetValueAsObject("Target", Caller);
}

void ARobotAIController::SetRobotGoalLocation(FVector Location)
{
	if (!GetBlackboardComponent())
		return;
	GetBlackboardComponent()->SetValueAsVector("GoalLocation", Location);
}

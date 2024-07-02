// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RobotAIController.generated.h"

/**
 * 
 */
UCLASS()
class SKYSEOUL_API ARobotAIController : public AAIController
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBehaviorTree* BehaviorTree;

public:
	virtual void OnPossess(APawn* InPawn) override;

	void SetRobotGoalLocation(AActor* Caller);
	void SetRobotGoalLocation(FVector Location);
};

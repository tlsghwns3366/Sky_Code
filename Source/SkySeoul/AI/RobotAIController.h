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
	ARobotAIController();
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(EditAnywhere)
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditAnywhere)
	class UAISenseConfig_Damage* DamageConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SenseLocation;

	UPROPERTY(EditAnywhere)
	class ACharacterBase* OwnerCharacter;

public:
	virtual void OnPossess(APawn* InPawn) override;

	void SetRobotGoalLocation(AActor* Caller);
	void SetRobotGoalLocation(FVector Location);
	void SetOwnerCharacter(AActor* OwnerCharacterActor);

	void SetPerceptionSystem();

	UFUNCTION()
	void SenseDetected(AActor* Actor, FAIStimulus const Stimulus);

	void SightDetected(AActor* Actor, FAIStimulus const Stimulus);
	void DamageDetected(AActor* Actor, FAIStimulus const Stimulus);

};

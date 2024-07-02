// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "InteractionInterface.h"
#include "RobotCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FHackingUpdate, AActor*)

UENUM(BlueprintType)
enum class ERobotState : uint8
{
	E_Idle UMETA(DisplayName = "Idle"),
	E_FollowCharacter UMETA(DisplayName = "FollowCharacter"),
	E_MoveLocation UMETA(DisplayName = "MoveLocation"),
};


UCLASS()
class SKYSEOUL_API ARobotCharacter : public ACharacterBase, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARobotCharacter();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ACharacter* OwnerCharacter;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetComponent* InteractionWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERobotState RobotState;

	FHackingUpdate HackingUpdate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void HackingStart(AActor* Caller);
	void HackingEnd(AActor* Caller);

	void MoveAction(FVector GoalLocation);
	void PlayerSelect(int32 Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Caller);
	virtual void OnInteract_Implementation(AActor* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void StartFocus();
	virtual void StartFocus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EndFocus();
	virtual void EndFocus_Implementation();
};

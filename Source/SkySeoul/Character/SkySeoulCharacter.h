// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Logging/LogMacros.h"
#include "Character/Abilities/AbilitySetData.h"
#include "SkySeoulCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAbilitySetData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRobotUpdated);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASkySeoulCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	ASkySeoulCharacter(const class FObjectInitializer& ObjectInitializer);

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	TWeakObjectPtr<class URobotAttributeSetBase> RobotAttributeSetBase;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Camera")
	float StartingCameraBoomArmLength;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Camera")
	FVector StartingCameraBoomLocation;

	bool ASCInputBound = false;

	FGameplayTag DeadTag;

	virtual void OnRep_PlayerState() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interection")
	AActor* FocusedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	int32 MaxRobotCost = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	int32 SelectRobotNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Select")
	int32 SelectNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	FVector RobotLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	TArray<class ARobotCharacter*> RobotArray;

	UPROPERTY(BlueprintAssignable)
	FOnRobotUpdated OnRobotUpdated;

	FTimerHandle RobotLocationTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Action")
	UAbilitySetData* AbilityActionData;

	TArray<FAbilitySetData_GrantedHandles> AbilityHandles;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInventoryComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UEquipmentComponent*	EquipComponent;
	
protected:	
	// To add mapping context
	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:

	virtual void PossessedBy(AController* NewController) override;

	void ForwardTrace();

	UFUNCTION(BlueprintCallable)
	void LocationTrace();

	void Interaction();
	void StartFocus(AActor* TraceActor);
	void EndFocus(AActor* TraceActor);

	bool RobotCostCheck(int32 RobotCost);
	bool AddRobot(ARobotCharacter* RobotCharacter);

	UFUNCTION(BlueprintCallable)
	void RemoveRobot(int32 Value);

	UFUNCTION(BlueprintCallable)
	void ChangeRobotLeft();
	UFUNCTION(BlueprintCallable)
	void ChangeRobotRight();
	void ChangeRobot(const int32 Value);

	void SetAbilityData(UAbilitySetData* Data);

	UFUNCTION(BlueprintCallable)
	void AddAbility();

	UFUNCTION(BlueprintCallable)
	void RemoveAbility();


	void RequestSelectAction(const int32 Num);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	FVector GetStartingCameraBoomLocation();

};
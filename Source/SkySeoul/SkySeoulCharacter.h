// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Logging/LogMacros.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	int32 MaxRobotCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	int32 SelectRobotNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot")
	int32 SelectNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	FVector RobotLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	TArray<class ARobotCharacter*> RobotArray;

	UPROPERTY(BlueprintAssignable)
	FOnRobotUpdated OnRobotUpdated;

	FTimerHandle RobotLocationTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Robot")
	UAbilitySetData* RobotAbility;
	
protected:	
	// To add mapping context
	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	void InitializeStartingValues(class AProjectPlayerState* PS);
	void BindASCInput();

public:

	virtual void PossessedBy(AController* NewController) override;

	void ForwardTrace();

	UFUNCTION(BlueprintCallable)
	void LocationTrace();

	void Interaction();
	void StartFocus(AActor* TraceActor);
	void EndFocus(AActor* TraceActor);

	UFUNCTION(BlueprintCallable)
	void SelectRobotAction();
	void ChangeRobot();
	void ChangeRobot(const int32 Value);
	void RequestSelectAction(const int32 Num);
	bool AddRobot(ARobotCharacter* RobotCharacter);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	float GetStartingCameraBoomArmLength();

	UFUNCTION(BlueprintCallable, Category = "Character|Camera")
	FVector GetStartingCameraBoomLocation();

};
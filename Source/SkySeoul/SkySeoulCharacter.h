// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "SkySeoulCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ASkySeoulCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASkySeoulCharacter();

public:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	int32 MaxRobotCount = 3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	int32 SelectRobotNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	int32 SelectNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interection")
	AActor* FocusedActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	FVector RobotLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot")
	TArray<class ARobotCharacter*> RobotArray;


	FTimerHandle RobotLocationTimer;
	
protected:	
	// To add mapping context
	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	void ForwardTrace();
	void LocationTrace();

	void Interaction();
	void StartFocus(AActor* TraceActor);
	void EndFocus(AActor* TraceActor);

	void SelectRobotAction();
	void ChangeRobot();
	void ChangeRobot(int32 Value);
	void RequestSelectAction(const FInputActionValue& Value);

public:
	bool AddRobot(ARobotCharacter* RobotCharacter);
};


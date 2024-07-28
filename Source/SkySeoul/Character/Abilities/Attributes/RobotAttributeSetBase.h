// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RobotAttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class SKYSEOUL_API URobotAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_RobotLevel)
	FGameplayAttributeData RobotLevel;
	ATTRIBUTE_ACCESSORS(URobotAttributeSetBase, RobotLevel)

	UPROPERTY(BlueprintReadOnly, Category = "Cost", ReplicatedUsing = OnRep_RobotCost)
	FGameplayAttributeData RobotCost;
	ATTRIBUTE_ACCESSORS(URobotAttributeSetBase, RobotCost)
		
	UPROPERTY(BlueprintReadOnly, Category = "Cost", ReplicatedUsing = OnRep_MaxRobotCost)
	FGameplayAttributeData MaxRobotCost;
	ATTRIBUTE_ACCESSORS(URobotAttributeSetBase, MaxRobotCost)


public:

	UFUNCTION()
	virtual void OnRep_RobotLevel(const FGameplayAttributeData& OldRobotLevel);
	UFUNCTION()
	virtual void OnRep_RobotCost(const FGameplayAttributeData& OldRobotCost);
	UFUNCTION()
	virtual void OnRep_MaxRobotCost(const FGameplayAttributeData& OldMaxRobotCost);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

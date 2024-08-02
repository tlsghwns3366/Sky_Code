// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharacterAttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class SKYSEOUL_API UCharacterAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Level)

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Sp", ReplicatedUsing = OnRep_Sp)
	FGameplayAttributeData Sp;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Sp)

	UPROPERTY(BlueprintReadOnly, Category = "Sp", ReplicatedUsing = OnRep_MaxSp)
	FGameplayAttributeData MaxSp;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, MaxSp)

	//Temp Damage
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UCharacterAttributeSetBase, Damage)


	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

public:
	UFUNCTION()
	virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_Sp(const FGameplayAttributeData& OldSp);
	UFUNCTION()
	virtual void OnRep_MaxSp(const FGameplayAttributeData& OldMaxSp);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

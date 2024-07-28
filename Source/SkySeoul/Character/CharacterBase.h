// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, CharacterBase);

class UAbilitySetData;
class AProjectPlayerState;

UCLASS()
class SKYSEOUL_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase(const class FObjectInitializer& ObjectInitializer);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void Attack();

	UPROPERTY(BlueprintAssignable)
	FCharacterDiedDelegate OnCharacterDied;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Level")
	float GetCharacterLevel() const;
	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetSp() const;
	UFUNCTION(BlueprintCallable, Category = "Character")
	float GetMaxSp() const;

	virtual void PossessedBy(AController* NewController) override;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Input")
	TObjectPtr<class USkySeoulInputConfig> InputConfig;

protected:

	TObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Abilities")
	TArray<TObjectPtr<UAbilitySetData>> AbilitySets;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	virtual void InitializeStartingValues(AProjectPlayerState* PS);

	virtual void SetHealth(float Health);
	virtual void SetSp(float Sp);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

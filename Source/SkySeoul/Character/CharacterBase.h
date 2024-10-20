// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayTags.h"
#include "CharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, CharacterBase);

class UAbilitySetData;
class AProjectPlayerState;
class UStaticMeshComponent;
class UBoxComponent;
class UInventoryComponent;

UCLASS()
class SKYSEOUL_API ACharacterBase : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Input")
	TObjectPtr<class USkySeoulInputConfig> InputConfig;

	UFUNCTION(BlueprintCallable, Category = "Damage Event")
	void OnHitEvent(AActor* DamageAttacker, float DamageAmount = 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UInventoryComponent* InventoryComponent;

protected:

	TObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Abilities")
	TArray<TObjectPtr<UAbilitySetData>> AbilitySets;

	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	virtual void InitializeStartingValues(AProjectPlayerState* PS);

	virtual void SetHealth(float Health);
	virtual void SetSp(float Sp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer GameplayTagContainer;

	// Gameplay Tag Interface implementation
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override { TagContainer = GameplayTagContainer; return; }

	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override { return GameplayTagContainer.HasTag(TagToCheck); }

	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override { return GameplayTagContainer.HasAll(TagContainer); }

	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override { return GameplayTagContainer.HasAny(TagContainer); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

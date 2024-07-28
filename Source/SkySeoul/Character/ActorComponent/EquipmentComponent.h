// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Character/Abilities/AbilitySetData.h"
#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipUpdated);

class UItemObject;
class ASkySeoulCharacter;
class UInventoryComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYSEOUL_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TMap<FGameplayTag, UItemObject*> WeaponMap;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability|Action")
	UAbilitySetData* EquipAbilityActionData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UItemObject* SelectedWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FGameplayTag SelectedTag;

	UPROPERTY(EditAnywhere)
	UInventoryComponent* CharacterInvnetoryComponent;


	UPROPERTY(BlueprintAssignable)
	FOnEquipUpdated OnEquipUpdated;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TObjectPtr<class UCharacterAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UCharacterAttributeSetBase> AttributeSetBase;

	TArray<FAbilitySetData_GrantedHandles> AbilityHandles;

	UPROPERTY(EditDefaultsOnly)
	ASkySeoulCharacter* OwnerCharacter;


	void SetAbilityData(UAbilitySetData* Data);
	void AddAbility();
	void RemoveAbility();

public:

	UFUNCTION(BlueprintCallable)
	UItemObject* FindEquipItemObject(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	void OnEquip(FGameplayTag Tag, UItemObject* Object);

	UFUNCTION(BlueprintCallable)
	void UnEquip(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	void WeaponChangeUp();

	UFUNCTION(BlueprintCallable)
	void WeaponChangeDown();


	UFUNCTION(BlueprintCallable)
	UAbilitySetData* GetWeaponAbilityData();

};
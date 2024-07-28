// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ActorComponent/EquipmentComponent.h"

#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/AbilitySetData.h"
#include "ProjectPlayerState.h"
#include "Character/SkySeoulCharacter.h"
#include "Inventory/ItemObject.h"
#include "Inventory/InventoryComponent.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCharacter = Cast<ASkySeoulCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		CharacterInvnetoryComponent = OwnerCharacter->InventoryComponent;
		AProjectPlayerState* PS = OwnerCharacter->GetPlayerState<AProjectPlayerState>();
		if (PS)
		{
			AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			AttributeSetBase = PS->GatAttributeSetBase();
		}
	}
}

UItemObject* UEquipmentComponent::FindEquipItemObject(FGameplayTag Tag)
{
	return WeaponMap[Tag];
}

void UEquipmentComponent::OnEquip(FGameplayTag Tag, UItemObject* Object)
{
	CharacterInvnetoryComponent->RemoveItem(Object);
	WeaponMap.Add(Tag, Object);
	OnEquipUpdated.Broadcast();
}

void UEquipmentComponent::UnEquip(FGameplayTag Tag)
{
	if(WeaponMap.Contains(Tag))
		CharacterInvnetoryComponent->AddItem(WeaponMap[Tag]);

	WeaponMap.Remove(Tag);
	OnEquipUpdated.Broadcast();
}

void UEquipmentComponent::WeaponChangeUp()
{
	auto Inter = WeaponMap.begin();
	while (Inter != WeaponMap.end())
	{
		if (Inter.Key() == SelectedTag)
		{
			++Inter;
			if (Inter == WeaponMap.end())
			{
				SelectedWeapon = WeaponMap.begin().Value();
				SelectedTag = WeaponMap.begin().Key();
			}
			else
			{
				SelectedWeapon = Inter.Value();
				SelectedTag = Inter.Key();
			}
			break;
		}
		++Inter;
	}
	SetAbilityData(SelectedWeapon->GetAbilityData());
}

void UEquipmentComponent::WeaponChangeDown()
{
	auto Inter = WeaponMap.begin();
	auto BackInter = WeaponMap.begin();
	while (Inter != WeaponMap.end())
	{
		++Inter;
		if (Inter.Key() == SelectedTag)
		{
			SelectedWeapon = BackInter.Value();
			SelectedTag = BackInter.Key();
			SetAbilityData(SelectedWeapon->GetAbilityData());
			return;
		}
		if (Inter == WeaponMap.end())
			break;
		++BackInter;
	}
	SelectedWeapon = BackInter.Value();
	SelectedTag = BackInter.Key();
	SetAbilityData(SelectedWeapon->GetAbilityData());
}

UAbilitySetData* UEquipmentComponent::GetWeaponAbilityData()
{
	return EquipAbilityActionData;
}

void UEquipmentComponent::SetAbilityData(UAbilitySetData* Data)
{
	RemoveAbility();
	if (Data == nullptr)
	{
		EquipAbilityActionData = nullptr;
	}
	else
	{
		EquipAbilityActionData = Data;
		AddAbility();
	}
}

void UEquipmentComponent::AddAbility()
{
	if (EquipAbilityActionData == nullptr)
		return;

	FAbilitySetData_GrantedHandles& NewHandle = AbilityHandles.AddDefaulted_GetRef();
	EquipAbilityActionData->GiveToAbilitySystem(AbilitySystemComponent, &NewHandle);
}

void UEquipmentComponent::RemoveAbility()
{
	for (auto It = AbilityHandles.CreateIterator(); It; It++)
	{
		FAbilitySetData_GrantedHandles& Entry = *It;
		if (AbilitySystemComponent)
		{
			Entry.TakeFromAbilitySystem(AbilitySystemComponent);
		}
		It.RemoveCurrent();
	}
}

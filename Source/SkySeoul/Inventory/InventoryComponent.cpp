// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "ItemObject.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	for (UItemObject* Item : DefalutItemInventory)
	{
		AddItem(Item);
	}
	// ...
	
}

bool UInventoryComponent::AddItem(UItemObject* Item)
{
	if (Item == nullptr)
		return false;
	if (Item->ItemInfo.ItemCategory != EItemType::E_Item_Equip)
	{
		for (UItemObject* InventoryItem : ItemInventory)
		{
			if (InventoryItem->ItemInfo.GetItemName() == Item->ItemInfo.GetItemName())
			{
				InventoryItem->ItemInfo.ItemCount += Item->ItemInfo.ItemCount;
				OnInventoryUpdated.Broadcast();
				return true;
			}
		}
	}
	Item->World = GetWorld();
	Item->Inventory = this;
	ItemInventory.AddUnique(Item);
	OnInventoryUpdated.Broadcast();
	return true;
}

bool UInventoryComponent::RemoveItem(UItemObject* Item)
{
	if (Item == nullptr)
		return false;

	Item->World = nullptr;
	Item->Inventory = nullptr;
	ItemInventory.RemoveSingle(Item);
	OnInventoryUpdated.Broadcast();
	return true;
}

UItemObject* UInventoryComponent::FindItem(FGameplayTag Tag) const
{
	for (auto TargetItem : ItemInventory)
	{
		if (TargetItem->ItemInfo.ItemTag == Tag)
			return TargetItem;
	}

	return nullptr;
}

void UInventoryComponent::UseItem(UItemObject* Item)
{
	Item->ItemUse();
	if (Item->ItemInfo.GetItemCount() == 0)
		RemoveItem(Item);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::UseItem(UItemObject* Item, int32 Value)
{
	int32 Count = Item->ItemInfo.GetItemCount();
	if (Count - Value >= 0)
	{
		Item->ItemUse(Value);
		Count = Value;
	}
	else
		Item->ItemUse(Count);

	if (Item->ItemInfo.GetItemCount() == 0)
		RemoveItem(Item);
	OnInventoryUpdated.Broadcast();

	return Count;
}

void UInventoryComponent::UseItem(FGameplayTag Tag)
{
	UItemObject* Item = FindItem(Tag);
	UseItem(Item);
}

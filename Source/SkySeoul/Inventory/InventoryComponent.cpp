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
	for (UItemObject* InventoryItem : ItemInventory)
	{
		if (InventoryItem->ItemInfo.GetItemName() == Item->ItemInfo.GetItemName())
		{
			InventoryItem->ItemInfo.ItemCount += Item->ItemInfo.ItemCount;
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}
	Item->World = GetWorld();
	Item->Inventory = this;
	ItemInventory.Add(Item);
	OnInventoryUpdated.Broadcast();
	return true;
}

bool UInventoryComponent::RemoveItem(UItemObject* Item)
{
	if (Item == nullptr)
		return false;

	for (UItemObject* InventoryItem : ItemInventory)
	{
		if (InventoryItem->ItemInfo.GetItemName() == Item->ItemInfo.GetItemName())
		{
			InventoryItem->ItemInfo.ItemCount -= Item->ItemInfo.ItemCount;
			if (InventoryItem->ItemInfo.ItemCount <= 0)
				break;
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	Item->World = nullptr;
	Item->Inventory = nullptr;
	ItemInventory.RemoveSingle(Item);
	OnInventoryUpdated.Broadcast();
	return true;
}

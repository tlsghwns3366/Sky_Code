// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GunWeaponObject.h"
#include "Inventory/InventoryComponent.h"

void UGunWeaponObject::UseBullet()
{
	if(CurrentMagazine >0)
		CurrentMagazine--;
	BulletUpdated.Broadcast();
}

void UGunWeaponObject::SetCurrentMagazine(UInventoryComponent* TargetInventory)
{
	if (TargetInventory == nullptr)
		return;
	UItemObject* BulletItem = TargetInventory->FindItem(BulletTag);
	if (BulletItem == nullptr)
		return;

	int32 Count = BulletItem->ItemInfo.GetItemCount();
	int32 Value = MaxMagazine - CurrentMagazine;
	if (BulletItem->UseItemCheck())
		CurrentMagazine += TargetInventory->UseItem(BulletItem, Value);
	BulletUpdated.Broadcast();
}
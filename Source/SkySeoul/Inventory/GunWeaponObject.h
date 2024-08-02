// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/EquipmentObject.h"
#include "GunWeaponObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBulletUpdated);
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class SKYSEOUL_API UGunWeaponObject : public UEquipmentObject
{
	GENERATED_BODY()
	

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magazine")
	FGameplayTag BulletTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Magazine")
	int32 CurrentMagazine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Magazine")
	int32 MaxMagazine;


	UPROPERTY(BlueprintAssignable)
	FBulletUpdated BulletUpdated;
public:

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentMagazine() const{
		return CurrentMagazine;
	};
	UFUNCTION(BlueprintCallable)
	int32 GetMaxMagazine() const {
		return MaxMagazine;
	};

	void UseBullet();

	UFUNCTION(BlueprintCallable)
	void SetCurrentMagazine(UInventoryComponent* TargetInventory);
};

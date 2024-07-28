// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Styling/SlateBrush.h"
#include "GameplayTagContainer.h"
#include "ItemObject.generated.h"

struct FSlateBrush;
class UAbilitySetData;
class UStaticMesh;
/**
 * 
 */

UENUM(BlueprintType)
enum class EItemType : uint8
{
	E_None UMETA(DisplayName = "None"),
	E_Item_Equip UMETA(DisplayName = "Equip"),
	E_Item_Consumable UMETA(DisplayName = "Consumable"),
	E_Item_QuestItem UMETA(DisplayName = "QuestItem"),
	E_Item_Miscellaneous UMETA(DisplayName = "Miscellaneous"),
	E_Item_Material UMETA(DisplayName = "Material"),
};

USTRUCT(BlueprintType)
struct FItemInfoData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FString ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FSlateBrush ItemImage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	TArray<FString> ItemDescription;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0.0))
	int32 ItemCount = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	EItemType ItemCategory = EItemType::E_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0.0))
	float DropChance;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FGameplayTag ItemTag;

	FString GetItemName() { return ItemName; };
};

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class SKYSEOUL_API UItemObject : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Transient)
	class UWorld* World;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FItemInfoData ItemInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UInventoryComponent* Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UAbilitySetData* ItemAbilityData;

	UAbilitySetData* GetAbilityData();
};

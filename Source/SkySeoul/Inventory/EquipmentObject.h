// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemObject.h"
#include "EquipmentObject.generated.h"

/**
 * 
 */
UCLASS()
class SKYSEOUL_API UEquipmentObject : public UItemObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachTransform;

};

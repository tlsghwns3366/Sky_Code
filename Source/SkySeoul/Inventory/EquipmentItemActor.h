// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemActor.h"
#include "EquipmentItemActor.generated.h"

class USkeletalMeshComponent;
class UEquipmentObject;
/**
 * 
 */
UCLASS()
class SKYSEOUL_API AEquipmentItemActor : public AItemActor
{
	GENERATED_BODY()
public:
	AEquipmentItemActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USkeletalMeshComponent* SkeletalMeshComponent;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SKYSEOUL_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxInventorySize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<class UItemObject*> ItemInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<class UItemObject*> DefalutItemInventory;


	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;

public:	

	UFUNCTION(BlueprintCallable)
	virtual bool AddItem(class UItemObject* Item);
	UFUNCTION(BlueprintCallable)
	virtual bool RemoveItem(class UItemObject* Item);

		
};

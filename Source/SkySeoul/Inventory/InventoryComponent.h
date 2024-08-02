// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItemObject;

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
	TArray<UItemObject*> ItemInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TArray<UItemObject*> DefalutItemInventory;


	UPROPERTY(BlueprintAssignable)
	FOnInventoryUpdated OnInventoryUpdated;

public:	

	UFUNCTION(BlueprintCallable)
	virtual bool AddItem(UItemObject* Item);
	UFUNCTION(BlueprintCallable)
	virtual bool RemoveItem(UItemObject* Item);

	UFUNCTION(BlueprintCallable)
	UItemObject* FindItem(FGameplayTag Tag) const ;

	UFUNCTION(BlueprintCallable)
	void UseItem(UItemObject* Item);
	int32 UseItem(UItemObject* Item, int32 Value);
	void UseItem(FGameplayTag Tag);
};

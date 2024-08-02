// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/EquipmentItemActor.h"
#include "Inventory/EquipmentObject.h"

AEquipmentItemActor::AEquipmentItemActor()
{
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	RootComponent = SkeletalMeshComponent;
}

void AEquipmentItemActor::BeginPlay()
{
	Super::BeginPlay();
}
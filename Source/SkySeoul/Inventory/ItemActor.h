// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "ItemActor.generated.h"

class UItemObject;
class UWidgetComponent;
class UUserWidget;

UCLASS()
class SKYSEOUL_API AItemActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemObject* ItemObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* WidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> Widget;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Iteminitialization(UItemObject* item);

	void AddInventory(AActor* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Caller);
	virtual void OnInteract_Implementation(AActor* Caller);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void StartFocus();
	virtual void StartFocus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void EndFocus();
	virtual void EndFocus_Implementation();
};

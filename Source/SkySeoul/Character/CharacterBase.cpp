// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/Attributes/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/InventoryComponent.h"

#include "ProjectPlayerState.h"
#include "Abilities/AbilitySetData.h"
#include "Actor/ProjectileBase.h"

#include "Perception/AISense.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterBase::ACharacterBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;

	//GetCapsuleComponent()->

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*MeleeArea = CreateDefaultSubobject<UBoxComponent>
		(TEXT("Melee Attack Area"));
	MeleeArea->SetupAttachment(GetMesh(),
		TEXT("Melee Attack Area"));*/

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>
		(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->SetupAttachment(GetMesh(),
		TEXT("ProjectileSpawnPoint"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	CanAction = true;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool ACharacterBase::IsAlive() const
{
	return GetHealth() > 0.f;
}

float ACharacterBase::GetCharacterLevel() const
{
	if (AttributeSetBase.IsValid())
		return AttributeSetBase->GetLevel();
	return 0.0f;
}

float ACharacterBase::GetHealth() const
{
	if (AttributeSetBase.IsValid())
		return AttributeSetBase->GetHealth();
	return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (AttributeSetBase.IsValid())
		return AttributeSetBase->GetMaxHealth();
	return 0.0f;
}

float ACharacterBase::GetSp() const
{
	if (AttributeSetBase.IsValid())
		return AttributeSetBase->GetSp();
	return 0.0f;
}

float ACharacterBase::GetMaxSp() const
{
	if (AttributeSetBase.IsValid())
		return AttributeSetBase->GetMaxSp();
	return 0.0f;
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AProjectPlayerState* PS = GetPlayerState<AProjectPlayerState>();
	if (PS)
	{
		InitializeStartingValues(PS);
		for (const UAbilitySetData* AbilitySet : AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
			}
		}
		SetHealth(GetMaxHealth());
		SetSp(GetMaxSp());
	}
}

void ACharacterBase::InitializeStartingValues(AProjectPlayerState* PS)
{
	AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSetBase = PS->GatAttributeSetBase();
	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	AbilitySystemComponent->AddLooseGameplayTags(GameplayTagContainer);
}

void ACharacterBase::SetHealth(float Health)
{
	if (AttributeSetBase.IsValid())
		return AttributeSetBase->SetHealth(Health);
}

void ACharacterBase::SetSp(float Sp)
{
	if (AttributeSetBase.IsValid())
		return AttributeSetBase->SetSp(Sp);
}
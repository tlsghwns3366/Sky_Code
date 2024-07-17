// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/Attributes/CharacterAttributeSetBase.h"
#include "Character/Abilities/CharacterGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"


#include "ProjectPlayerState.h"


// Sets default values
ACharacterBase::ACharacterBase(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;
	
	//GetCapsuleComponent()->

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("State.RemoveOnDeath"));

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

void ACharacterBase::Attack()
{
}

bool ACharacterBase::IsAlive() const
{
	return GetHealth() > 0.f;
}

int32 ACharacterBase::GetAbilityLevel(enum SkySeoulAbilityID AbilityID) const
{
	return 1;
}

void ACharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven)
		return;

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}
	AbilitySystemComponent->CharacterAbilitiesGiven = false;
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

		AddStartupEffects();
		AddCharacterAbilities();
	}
}

void ACharacterBase::InitializeStartingValues(AProjectPlayerState* PS)
{
	AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSetBase = PS->GatAttributeSetBase();
	AbilitySystemComponent->SetTagMapCount(DeadTag, 0);
	InitializeAttributes();
	SetHealth(GetMaxHealth());
}

void ACharacterBase::AddCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->CharacterAbilitiesGiven)
		return;
	for (TSubclassOf<UCharacterGameplayAbility>& StartUpAbility : CharacterAbilities)
	{
		FGameplayAbilitySpec Spec(StartUpAbility, 0);
		Spec.DynamicAbilityTags.AddTag(StartUpAbility->GetDefaultObject<UCharacterGameplayAbility>()->InputTag);
		AbilitySystemComponent->GiveAbility(Spec);
	}
	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void ACharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent.IsValid())
		return;
	if (!DefaultAttributes)
		return;
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	FGameplayEffectSpecHandle NewHendle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(),EffectContext);
	if (NewHendle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHendle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHendle.Data.Get(),AbilitySystemComponent.Get());
	}
}

void ACharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent.IsValid() || AbilitySystemComponent->StartupEffectsApplied)
		return;
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHendle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(),EffectContext);
		if (NewHendle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHendle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHendle.Data.Get(), AbilitySystemComponent.Get());
		}
	}
	AbilitySystemComponent->StartupEffectsApplied = true;

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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/AbilitySetData.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterGameplayAbility.h"

void FAbilitySetData_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FAbilitySetData_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FAbilitySetData_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FAbilitySetData_GrantedHandles::TakeFromAbilitySystem(UCharacterAbilitySystemComponent* CharacterASC)
{
	check(CharacterASC);
	if (CharacterASC->IsOwnerActorAuthoritative())
		return;
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			CharacterASC->ClearAbility(Handle);
		}
	}
	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			CharacterASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		CharacterASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UAbilitySetData::UAbilitySetData(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UAbilitySetData::GiveToAbilitySystem(UCharacterAbilitySystemComponent* CharacterASC, FAbilitySetData_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(CharacterASC);
	if (CharacterASC->IsOwnerActorAuthoritative())
		return;
	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FAbilitySetData_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
			continue;

		UCharacterGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UCharacterGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = CharacterASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FAbilitySetData_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
			continue;

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = CharacterASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, CharacterASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FAbilitySetData_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
			continue;

		UAttributeSet* NewSet = NewObject<UAttributeSet>(CharacterASC->GetOwner(), SetToGrant.AttributeSet);
		CharacterASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

int32 UAbilitySetData::GetAbilityCount()
{
	return GrantedGameplayAbilities.Num();
}

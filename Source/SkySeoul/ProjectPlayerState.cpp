// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectPlayerState.h"
#include "Character/Abilities/CharacterAbilitySystemComponent.h"
#include "Character/Abilities/Attributes/CharacterAttributeSetBase.h"


AProjectPlayerState::AProjectPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UCharacterAttributeSetBase>(TEXT("AttributeSetbase"));

	//기본적으로 매우 낮고 지연이 발생할 수 있기 때문에 설정
	NetUpdateFrequency = 100.f;

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AProjectPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UCharacterAttributeSetBase* AProjectPlayerState::GatAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AProjectPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AProjectPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	//HUD 나중에
}

float AProjectPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AProjectPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AProjectPlayerState::GetSp() const
{
	return AttributeSetBase->GetSp();
}

float AProjectPlayerState::GetMaxSp() const
{
	return AttributeSetBase->GetMaxSp();
}

int32 AProjectPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetLevel();
}

void AProjectPlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystemComponent)
	{
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AProjectPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AProjectPlayerState::MaxHealthChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetLevelAttribute()).AddUObject(this, &AProjectPlayerState::CharacterLevelChanged);
		SpChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetSpAttribute()).AddUObject(this, &AProjectPlayerState::SpChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")),EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AProjectPlayerState::StunTagChanged);
	}
}

void AProjectPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	OnStateUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Health Changed"));
}

void AProjectPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnStateUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Max Health Changed"));
}

void AProjectPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	OnStateUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("CharacterLevel Changed"));
}

void AProjectPlayerState::SpChanged(const FOnAttributeChangeData& Data)
{
	OnStateUpdated.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("Sp Changed"));
}

void AProjectPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancle;
		AbilityTagsToCancle.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));
		//모든 실행중이던 태그가 취소된다
		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCancelByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancle, &AbilityTagsToIgnore);
	}
}

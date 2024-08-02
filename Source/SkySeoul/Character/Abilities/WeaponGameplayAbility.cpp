// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/WeaponGameplayAbility.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Character/CharacterBase.h"
#include "Character/ActorComponent/EquipmentComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/GunWeaponObject.h"
#include "Character/SkySeoulCharacter.h"

FHitResult UWeaponGameplayAbility::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, TArray<FHitResult>& OutHitResults) const
{
	TArray<FHitResult> HitResults;

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ GetAvatarActorFromActorInfo());
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		TArray<AActor*> AttachedActors;
		Avatar->GetAttachedActors(/*out*/ AttachedActors);
		TraceParams.AddIgnoredActors(AttachedActors);
	}
	const ECollisionChannel TraceChannel = ECollisionChannel::ECC_GameTraceChannel2;
	if (SweepRadius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);
	}

	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
				{
					return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
				};

			if (!OutHitResults.ContainsByPredicate(Pred))
			{
				OutHitResults.Add(CurHitResult);
			}
		}
		Hit = OutHitResults.Last();
	}
	else
	{
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}
	return Hit;
}

void UWeaponGameplayAbility::OnFire()
{
	ACharacterBase* Character = GetCharacterFromActorInfo();
	check(Character);

	UEquipmentComponent* EquipComponent = Character->FindComponentByClass<UEquipmentComponent>();
	check(EquipComponent);

	AActor* EquipActor = EquipComponent->GetEquipActor();
	check(EquipActor);

	FVector StartLocation = EquipActor->GetActorLocation();
	FVector EndLocation = StartLocation + (1000.f*Character->GetControlRotation().Vector());
	TArray<FHitResult> OutResults;

	FGameplayAbilityTargetDataHandle TargetData;
	FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit();
	HitData->HitResult = WeaponTrace(StartLocation, EndLocation, 0.f, true, OutResults);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red,false,3.f);


	TargetData.Add(HitData);
	TargetHit(TargetData);
}

void UWeaponGameplayAbility::SetWeaponObject(const FGameplayAbilityActorInfo* ActorInfo)
{
	ASkySeoulCharacter* Character = Cast<ASkySeoulCharacter>(ActorInfo->AvatarActor);
	WeaponObject = Cast<UGunWeaponObject>(Character->EquipComponent->GetSelectedItem());
}

bool UWeaponGameplayAbility::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags))
	{
		return false;
	}
	if (WeaponObject == nullptr)
		SetWeaponObject(ActorInfo);
	if (WeaponObject != nullptr)
	{
		if (WeaponObject->GetCurrentMagazine() <= 0)
			return false;
	}
	return true;
}

bool UWeaponGameplayAbility::CommitAbilityCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	Super::CommitAbilityCost(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);

	check(ActorInfo);
	if (WeaponObject != nullptr)
	{
		WeaponObject->UseBullet();
	}
	return true;
}
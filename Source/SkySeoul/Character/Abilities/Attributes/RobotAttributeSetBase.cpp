// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/Attributes/RobotAttributeSetBase.h"
#include "Net/UnrealNetwork.h"

void URobotAttributeSetBase::OnRep_RobotLevel(const FGameplayAttributeData& OldRobotLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URobotAttributeSetBase, RobotLevel, OldRobotLevel);
}

void URobotAttributeSetBase::OnRep_RobotCost(const FGameplayAttributeData& OldRobotCost)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URobotAttributeSetBase, RobotCost, OldRobotCost);
}

void URobotAttributeSetBase::OnRep_MaxRobotCost(const FGameplayAttributeData& OldMaxRobotCost)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URobotAttributeSetBase, MaxRobotCost, OldMaxRobotCost);
}

void URobotAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(URobotAttributeSetBase, RobotLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URobotAttributeSetBase, RobotCost, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URobotAttributeSetBase, MaxRobotCost, COND_None, REPNOTIFY_Always);
}

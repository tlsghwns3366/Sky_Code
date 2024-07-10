// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Abilities/Execution/SkySeoulEffectExecution.h"
#include "SkySeoulEffectExecution.h"
#include "Character/Abilities/Attributes/CharacterAttributeSetBase.h"

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition BaseHealthDef;

	FDamageStatics()
	{
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UCharacterAttributeSetBase::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseHealthDef = FGameplayEffectAttributeCaptureDefinition(UCharacterAttributeSetBase::GetHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}



USkySeoulEffectExecution::USkySeoulEffectExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseHealthDef);
}

void USkySeoulEffectExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

	const float DamageDone = BaseDamage;
	//계산된 데미지가 어디에 적용될 것인지 GetHealthAttribute
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UCharacterAttributeSetBase::GetHealthAttribute(), EGameplayModOp::Additive, -DamageDone));
}

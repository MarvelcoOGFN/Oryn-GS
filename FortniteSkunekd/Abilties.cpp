#include "framework.h"
#include "util.h"
#include "Abilites.h"

FGameplayAbilitySpec* Abilities::GiveAbility(AFortPlayerControllerAthena* PC, UClass* GameplayAbilityClass, UObject* CertainDef)
{
	if (!PC || !GameplayAbilityClass || !PC->MyFortPawn) {
		return nullptr; // null
	}

	auto AbilitySystemComponent = PC->MyFortPawn->AbilitySystemComponent; // gulp

	FGameplayAbilitySpec Spec{};
	// Contructs the Spec
	CombineStruct(&Spec, (UGameplayAbility*)GameplayAbilityClass->DefaultObject, 1, -1, CertainDef);
	//std::cout << "TEST " << std::endl;
	GiveAbilityAndActivateOnce(AbilitySystemComponent, &Spec.Handle, Spec);

	return &Spec;
}

// ill redo this function in the future but string is like useless
void Abilities::GiveAbilitySet(AFortPlayerControllerAthena* PC, std::string AbilitySetFileName) {
	if (!PC || AbilitySetFileName.empty()) { return; }
	static UFortAbilitySet* AbilitySet = Utils::StaticFindObject<UFortAbilitySet>(L"/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer");

	if (AbilitySet) {
		for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
		{
			UClass* AbilityClass = AbilitySet->GameplayAbilities[i].Get();
			std::cout << "AB: " << AbilityClass->GetName() << std::endl;
			if (AbilityClass) {
				GiveAbility(PC, AbilityClass);
			}
		}
	}
}

FGameplayAbilitySpec* Abilities::FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle) {

	for (FGameplayAbilitySpec& Spec : AbilitySystemComponent->ActivatableAbilities.Items) {
		if (Spec.Handle.Handle == Handle.Handle) {
			if (!Spec.PendingRemove) {
				return &Spec;
			}
		}
	}

	return nullptr;
}

void Abilities::InternalServerTryActivateAbility(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, const struct FPredictionKey& InPredictionKey, FGameplayEventData* TriggerEventData)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySystemComponent, Handle);
	if (!Spec) {
		std::cout << "%hs: Rejecting ClientActivation of ability with invalid SpecHandle!";
		return AbilitySystemComponent->ClientActivateAbilityFailed(Handle, InPredictionKey.Current);

	}
	UGameplayAbility* AbilityToActivate = Spec->Ability;

	if (!AbilityToActivate)
	{
		std::cout << "InternalTryActivateAbility called with invalid Ability";
		return AbilitySystemComponent->ClientActivateAbilityFailed(Handle, InPredictionKey.Current);
	}

	UGameplayAbility* InstancedAbility = nullptr;
	Spec->InputPressed = true;

	if (InternalTryActivateAbility(AbilitySystemComponent, Handle, InPredictionKey, &InstancedAbility, nullptr, TriggerEventData)) {
		std::cout << "hs: Accepted ClientActivation of %s with PredictionKey %s." << std::endl;
	}
	else {
		std::cout << "hs: Rejecting ClientActivation of %s with PredictionKey %s. InternalTryActivateAbility failed: %s" << std::endl;

		AbilitySystemComponent->ClientActivateAbilityFailed(Handle, InPredictionKey.Current);
		Spec->InputPressed = false;

		AbilitySystemComponent->ActivatableAbilities.MarkItemDirty(*Spec);
	}
}
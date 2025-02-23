#pragma once


class Abilities {
private:
	Abilities() = default;
public:

	// custom functions
	static FGameplayAbilitySpec* GiveAbility(AFortPlayerControllerAthena* PlayerController, UClass* GameplayAbilityClass, UObject* CertainDef = nullptr);
	static void GiveAbilitySet(AFortPlayerControllerAthena* PlayerController, std::string AbilitySetFileName);

	//https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L251
	// isnt stripped so we dont need a custom func
	// you are looking for GiveAbility NOT the function it self
	static inline void (*GiveAbilityAndActivateOnce)(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle* Spec, FGameplayAbilitySpec GameplayEventData);

	// idk cant say much
	//https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Public/GameplayAbilitySpec.h#L224
	static inline __int64 (*CombineStruct)(void* FGameplayAbilitySpec, UGameplayAbility* InAbility, char InLevel, int InInputID, void* InSourceObject);

	//https://github.com/EpicGames/UnrealEngine/blob/585df42eb3a391efd295abd231333df20cddbcf3/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L1675
	// we dont need to recreate this function since the whole function is still in IDA/fortnite
	static inline bool (*InternalTryActivateAbility)(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void*, const FGameplayEventData* TriggerEventData);
	//0x6B33F0
	
	//https://github.com/EpicGames/UnrealEngine/blob/585df42eb3a391efd295abd231333df20cddbcf3/Engine/Plugins/Runtime/GameplayAbilities/Source/GameplayAbilities/Private/AbilitySystemComponent_Abilities.cpp#L888
	static FGameplayAbilitySpec* FindAbilitySpecFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);
	static void InternalServerTryActivateAbility(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, const struct FPredictionKey& InPredictionKey, FGameplayEventData* TriggerEventData);
	// best way to find this is from the ServerTryActivateAbilityWithEventData
	// not going over how to find it bc thats like pain
};
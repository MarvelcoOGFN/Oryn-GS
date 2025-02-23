#pragma once

//NetMulticast_Athena_BatchedDamageCues(const struct FAthenaBatchedDamageGameplayCues_Shared& SharedData, const struct FAthenaBatchedDamageGameplayCues_NonShared& NonSharedData);

// btw i dont use this file name how its supposed to be used i'm just doing it bc too much in player like normal

class Miscs {
private:
	Miscs() = default;
public:
	static inline void (*RemoveAlivePlayers)(AFortGameModeAthena* a1, AFortPlayerControllerAthena* a2, AFortPlayerStateAthena* a3, AFortPlayerPawnAthena* a4, UFortWeaponItemDefinition* a5, EDeathCause a6, char a7);

	static inline void (*OriginalOnReload)(AFortWeapon* Weapon, int Count);
	static void OnReload(AFortWeapon* Weapon, int Count);

	static inline void (*OldrNetMulticast_Athena_BatchedDamageCues)(AFortPawn* Pawn, const FAthenaBatchedDamageGameplayCues_Shared& SharedData, const FAthenaBatchedDamageGameplayCues_NonShared& NonSharedData);
	static void NetMulticast_Athena_BatchedDamageCues(AFortPlayerPawnAthena* Pawn, const FAthenaBatchedDamageGameplayCues_Shared& SharedData, const FAthenaBatchedDamageGameplayCues_NonShared& NonSharedData);

	//ABuildingActor::OnDamageServer(float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, class AController* InstigatedBy, class AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext)
	//AFortPawn::OnDamageServer....
	static inline void (*OriginalOnDamageServer)(ABuildingActor* Pawn, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);
	static void OnDamageServer(ABuildingActor* Pawn, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext);
};
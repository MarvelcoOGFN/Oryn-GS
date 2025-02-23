#include "framework.h"
#include "Inventory.h"
#include "Miscs.h"
#include "util.h"

void Miscs::OnReload(AFortWeapon* Weapon, int Count)
{
	OriginalOnReload(Weapon, Count);
	if (!Weapon)
		return;

	AFortPlayerPawnAthena* Pawn = (AFortPlayerPawnAthena*)Weapon->Owner;
	if (!Pawn)
		return;

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->GetController();
	if (!PC || PC->IsInAircraft() || PC->bInfiniteAmmo)
		return;

	FFortItemEntry ItemEntry = Inventory::GetInventoryItem(PC, Weapon->ItemEntryGuid);
	if (!ItemEntry.ItemDefinition) return;
	
	auto AmmoDef = Weapon->WeaponData->GetAmmoWorldItemDefinition_BP();
	AFortInventory* WorldInventory = PC->WorldInventory;

	for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++) {
		auto& Item = WorldInventory->Inventory.ItemInstances[i];
		if (Item->ItemEntry.ItemGuid.Equals(ItemEntry.ItemGuid)) {
			if (Inventory::GetMaxStackSize(Item->ItemEntry.ItemDefinition) > 1)
			{
				Inventory::RemoveItem(PC, Item->ItemEntry.ItemGuid, Count, false);
			}
		}
		else if (AmmoDef) {
			if (Item->ItemEntry.ItemDefinition == AmmoDef) {
				Inventory::RemoveItem(PC, Item->ItemEntry.ItemGuid, Count, false);
			}
		}
	}

	// REload WE update the count
	// reload ranked in chapter 2 when
	Inventory::UpdateLoadedAmmo(PC, Weapon); // cooked
}

void Miscs::NetMulticast_Athena_BatchedDamageCues(AFortPlayerPawnAthena* Pawn, const FAthenaBatchedDamageGameplayCues_Shared& SharedData, const FAthenaBatchedDamageGameplayCues_NonShared& NonSharedData)
{
	if (!Pawn)
		return OldrNetMulticast_Athena_BatchedDamageCues(Pawn, SharedData, NonSharedData);

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->GetController();
	if (!PC) 
		return OldrNetMulticast_Athena_BatchedDamageCues(Pawn, SharedData, NonSharedData);

	std::cout << "SIGMA" << std::endl;
	//NonSharedData ~ is actor we hit
	// we could check damange or what ever but really just useful to ujpdate the users ammo count
	if (Pawn->CurrentWeapon)
	{
		// could be pickaxe but shouldnt matter
		if (Pawn->CurrentWeapon->WeaponData->bCanBeDropped)
		{
			Inventory::UpdateLoadedAmmo(PC, Pawn->CurrentWeapon);
		}
	}

	return OldrNetMulticast_Athena_BatchedDamageCues(Pawn, SharedData, NonSharedData);
}
/*
*   v20[3] = v16;
  return (*(__int64 (__fastcall **)(__int64 *, _OWORD *, __int128 *))(v11 + 0x8C8))(a1, v20, &v19);
}
*/

void Miscs::OnDamageServer(ABuildingActor* Actor, float Damage, const struct FGameplayTagContainer& DamageTags, const struct FVector& Momentum, const struct FHitResult& HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, const struct FGameplayEffectContextHandle& EffectContext)
{
	auto& OG = OriginalOnDamageServer;
	if (!Actor || Actor->bPlayerPlaced || !InstigatedBy || !DamageCauser)
		return OG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

	ABuildingContainer* BuildingContainer = (ABuildingContainer*)Actor;
	if(!BuildingContainer)
		return OG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);

	if (InstigatedBy->IsA(AFortPlayerControllerAthena::StaticClass()) 
		&& Damage > 0 
		&& DamageCauser->IsA(AFortWeapon::StaticClass())) {
		AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)InstigatedBy;
		AFortWeapon* Weapon = (AFortWeapon*)DamageCauser;
		UFortWeaponMeleeItemDefinition* PickaxeDefinition = InstigatedBy->CosmeticLoadoutPC.Pickaxe->WeaponDefinition;
		if (!PickaxeDefinition) return;

		if (InstigatedBy->MyFortPawn && InstigatedBy->MyFortPawn->CurrentWeapon && Weapon->WeaponData == PickaxeDefinition)
		{
			UCurveTable* CurveTable = BuildingContainer->BuildingResourceAmountOverride.CurveTable;
			FName RowName = BuildingContainer->BuildingResourceAmountOverride.RowName;
			float OutVal = 0;

			if (CurveTable)
			{
				UDataTableFunctionLibrary::GetDefaultObj()->EvaluateCurveTableRow(CurveTable, RowName, 0, new EEvaluateCurveTableResult(), &OutVal, FString());
			
				float GetHealth = BuildingContainer->GetMaxHealth() / Damage;
				float NewCount = std::round(OutVal / GetHealth);
				bool WeakSpots = BuildingContainer->bAllowWeakSpots && Damage == 100;

				UFortResourceItemDefinition* ItemDef = UFortKismetLibrary::K2_GetResourceItemDefinition(BuildingContainer->ResourceType);

				if (ItemDef)
				{
					if (NewCount > 0)
					{
						InstigatedBy->ClientReportDamagedResourceBuilding(BuildingContainer, BuildingContainer->ResourceType, NewCount, false, WeakSpots);

						Inventory::GiveItem(PC, ItemDef, NewCount);
					}
				}
			}
		}
	}

	return OG(Actor, Damage, DamageTags, Momentum, HitInfo, InstigatedBy, DamageCauser, EffectContext);
}
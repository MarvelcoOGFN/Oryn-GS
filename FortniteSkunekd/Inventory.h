#pragma once

class Inventory {
private:
	Inventory() = default;
public:
	static void InitInventory(AFortPlayerControllerAthena* PC);
	static float GetMaxStackSize(UFortItemDefinition* ItemDef);
	static int32 GetMaxAmmo(UFortItemDefinition* ItemDef);
	static void Update(AFortPlayerController* PC, FFortItemEntry* Entry = nullptr);
	static FFortItemEntry GetInventoryItem(AFortPlayerControllerAthena* PC, FGuid ItemGuid);
	static bool GiveItemForce(
		AFortPlayerControllerAthena* PC,
		UFortItemDefinition* ItemDefinition,
		int Count = 1,
		int LoadedAmmo = -1
	);
	static bool GiveItem(
		AFortPlayerControllerAthena* PC, 
		UFortItemDefinition* ItemDef,
		int Count,
		AFortPickup* Pickup = nullptr,
		int LoadedAmmo = -1
	);
	static bool RemoveItem(
		AFortPlayerControllerAthena* PC,
		FGuid ItemGuid,
		int Count = 1,
		bool bDropping = false
	);
	static void SpawnPickup(
		UFortItemDefinition* Item, 
		FVector Loc, 
		int Count, 
		int32 LoadedAmmo = -1, 
		EFortPickupSourceTypeFlag PickupSourceType = EFortPickupSourceTypeFlag::Container,
		EFortPickupSpawnSource SpawnSource = EFortPickupSpawnSource::ItemSpawner,
		AFortPawn* Pawn = nullptr
	);
	static EFortQuickBars GetQuickBars(UFortItemDefinition* ItemDef);
	static bool IsInvFull(AFortPlayerControllerAthena* PC);
	static void UpdateLoadedAmmo(AFortPlayerControllerAthena* PC, AFortWeapon* Weapon);
};
#include "framework.h"
#include "Inventory.h"
#include "util.h"

void Inventory::InitInventory(AFortPlayerControllerAthena* PC)
{
	PC->WorldInventory->Inventory.ItemInstances.Free();
	PC->WorldInventory->Inventory.ReplicatedEntries.Free();

	AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

	UAthenaPickaxeItemDefinition* PickaxeItemDef;
	FFortAthenaLoadout& CosmecticLoadoutPC = PC->CosmeticLoadoutPC;

	PickaxeItemDef = CosmecticLoadoutPC.Pickaxe != nullptr ? CosmecticLoadoutPC.Pickaxe : UObject::FindObjectContain<UAthenaPickaxeItemDefinition>("DefaultPickaxe.DefaultPickaxe");
	Inventory::GiveItemForce(PC, PickaxeItemDef->WeaponDefinition);

	for (size_t i = 0; i < GameMode->StartingItems.Num(); i++)
	{
		if (GameMode->StartingItems[i].Count > 0) { // fixes no wood
			std::cout << GameMode->StartingItems[i].Item->GetFullName() << std::endl;
			Inventory::GiveItemForce(PC, GameMode->StartingItems[i].Item, GameMode->StartingItems[i].Count);
		}
	}
}

float Inventory::GetMaxStackSize(UFortItemDefinition* ItemDef)
{
	// FANTUm wh
	if (!ItemDef->MaxStackSize.Curve.CurveTable)
		return ItemDef->MaxStackSize.Value;

	EEvaluateCurveTableResult OutResult;
	float OutXY;
	UDataTableFunctionLibrary::EvaluateCurveTableRow(ItemDef->MaxStackSize.Curve.CurveTable, ItemDef->MaxStackSize.Curve.RowName, 0, &OutResult, &OutXY, FString());

	return OutXY;
}

// this should only be called on item spawn AND never again
int32 Inventory::GetMaxAmmo(UFortItemDefinition* ItemDef)
{
	/*
	 "WeaponStatHandle": {
        "DataTable": {
          "ObjectName": "DataTable'AthenaRangedWeapons'",
          "ObjectPath": "FortniteGame/Content/Athena/Items/Weapons/AthenaRangedWeapons.0"
        },
        "RowName": "Pistol_SemiAuto_Athena_VR_Ore_T03"
      },
	*/
	if (ItemDef->IsA(UFortWeaponItemDefinition::StaticClass())) {
		//30
		UFortWeaponItemDefinition* WeaponDef = (UFortWeaponItemDefinition*)ItemDef;
		if (!WeaponDef) return 0;
		for (TPair<FName, uint8*>& Map : WeaponDef->WeaponStatHandle.DataTable->RowMap) {
		//	std::cout << Map.First.ToString() << std::endl;
			//Pistol_SemiAuto_Athena_VR_Ore_T03
			if (WeaponDef->WeaponStatHandle.RowName == Map.Key() && Map.Value()) {
				//std::cout << std::to_string(((FFortBaseWeaponStats*)Map.Value())->ClipSize) << std::endl;
				if (((FFortBaseWeaponStats*)Map.Value())->ClipSize == 0)
					return 1;
				return ((FFortBaseWeaponStats*)Map.Value())->ClipSize;
				break;
			}
		}
	}

	return 0;
}

void Inventory::Update(AFortPlayerController* PC, FFortItemEntry* Entry) {
	PC->WorldInventory->HandleInventoryLocalUpdate();

	if (Entry) {
		PC->WorldInventory->Inventory.MarkItemDirty(*Entry);
	}
	else {
		PC->WorldInventory->Inventory.MarkArrayDirty();
	}
}

FFortItemEntry Inventory::GetInventoryItem(AFortPlayerControllerAthena* PC, FGuid ItemGuid)
{
	AFortInventory* WorldInventory = PC->WorldInventory;
	for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++) {
		auto Item = WorldInventory->Inventory.ItemInstances[i];
		if (Item->ItemEntry.ItemGuid.Equals(ItemGuid))
			return Item->ItemEntry;
	}
	return {};
}

void Inventory::SpawnPickup(UFortItemDefinition* Item, FVector Loc, int Count, int32 LoadedAmmo, EFortPickupSourceTypeFlag PickupSourceType, EFortPickupSpawnSource SpawnSource, AFortPawn* Pawn) {
	FRotator Rotator = {};

	//std::cout << "SPAWNED FLOOR LOOT ?!?!?" << std::endl;
	AFortPickupAthena* NewPickup = Utils::SpawnActor<AFortPickupAthena>(Loc, Rotator, nullptr);
	NewPickup->PrimaryPickupItemEntry.ItemDefinition = Item;
	NewPickup->PrimaryPickupItemEntry.Count = Count;
	if (LoadedAmmo == -1)
		LoadedAmmo = GetMaxAmmo(Item);

	//std::cout << "PICKUP" << std::to_string(LoadedAmmo) << std::endl;

	//NewPickup->PickupLocationData.CombineTarget ~ this is for combining ammo and yap n the floor ill do this at some point

	NewPickup->PrimaryPickupItemEntry.LoadedAmmo = LoadedAmmo;
	NewPickup->OnRep_PrimaryPickupItemEntry();

	if(Pawn)
		NewPickup->PawnWhoDroppedPickup = Pawn;
	
	//5 might need to be true but idk if we need to check if its not a gun? unless its just not proper
	NewPickup->TossPickup(Loc, nullptr, -1, true, false, PickupSourceType, SpawnSource);
}
//const struct FVector& FinalLocation, class AFortPawn* ItemOwner, int32 OverrideMaxStackCount, bool bToss, bool bShouldCombinePickupsWhenTossCompletes, const EFortPickupSourceTypeFlag InPickupSourceTypeFlags, const EFortPickupSpawnSource InPickupSpawnSource

EFortQuickBars Inventory::GetQuickBars(UFortItemDefinition* ItemDef) {
	if (ItemDef->IsA(UFortBuildingItemDefinition::StaticClass()) ||
		ItemDef->IsA(UFortResourceItemDefinition::StaticClass()) ||
		ItemDef->IsA(UFortEditToolItemDefinition::StaticClass()) ||
		ItemDef->IsA(UFortTrapItemDefinition::StaticClass()) ||
		ItemDef->IsA(UFortWeaponMeleeItemDefinition::StaticClass()) ||
		ItemDef->IsA(UFortAmmoItemDefinition::StaticClass())) return EFortQuickBars::Secondary;

	return EFortQuickBars::Primary;
}


bool Inventory::IsInvFull(AFortPlayerControllerAthena* PC)
{
	auto WorldInventory = PC->WorldInventory;
	int EnoughIsEnoughISaidEnoughIsEnough = 0;
	for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++) {
		auto Item = WorldInventory->Inventory.ItemInstances[i];

		if (GetQuickBars(Item->ItemEntry.ItemDefinition) == EFortQuickBars::Primary) {
			std::cout << std::to_string(Item->GetNumberOfSlotsTaken()) << std::endl;
			EnoughIsEnoughISaidEnoughIsEnough += Item->GetNumberOfSlotsTaken(); // items can be more then 1
		}
	}

	return EnoughIsEnoughISaidEnoughIsEnough >= 5;
}

bool Inventory::GiveItemForce(AFortPlayerControllerAthena* PC,
	UFortItemDefinition* ItemDefinition,
	int Count,
	int LoadedAmmo)
{
	// this func legit gives the items without any checks
	UFortWorldItem* Item = (UFortWorldItem*)ItemDefinition->CreateTemporaryItemInstanceBP(Count, 0);
	if (Item) {
		Item->ItemEntry.LoadedAmmo = LoadedAmmo;
		Item->ItemEntry.Count = Count; // pretty sure this inst actually needed
		Item->SetOwningControllerForTemporaryItem(PC);
		PC->WorldInventory->Inventory.ItemInstances.Add(Item);
		PC->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		Update(PC, &Item->ItemEntry);
	}

	return true;
}

bool Inventory::GiveItem(AFortPlayerControllerAthena* PC,
	UFortItemDefinition* ItemDef,
	int Count,
	AFortPickup* Pickup,
	int LoadedAmmo)
{
	if (!PC || !ItemDef)
		return false;

	EFortQuickBars QuickBar = GetQuickBars(ItemDef);
	auto WorldInventory = PC->WorldInventory;
	//bool bStackable = false;
	bool bSwappable = false;
	bool TempBool = false;
	AFortWeapon* CurrWeapon = ((AFortPlayerPawnAthena*)PC->Pawn)->CurrentWeapon;
	int NumToDrop = 0;
	std::vector<std::pair<FGuid, int32>> ItemGuid; // for stacking
	//int NewCount = 0;
	// BEFORE we check max if the item is stackable you just stack it else just drop or smth idc
	for(UFortWorldItem*& Item : WorldInventory->Inventory.ItemInstances)
	{
		if (!Item) continue; // weird if this ever happens

		if (CurrWeapon->ItemEntryGuid.Equals(Item->ItemEntry.ItemGuid))
		{
			if (!bSwappable && !Item->ItemEntry.ItemDefinition->IsA(UAthenaPickaxeItemDefinition::StaticClass()))
			{
				bSwappable = true;
				TempBool = true;
			}
		}

		//std::cout << Item->ItemEntry.ItemDefinition->MaxStackSize.Value << std::endl;
		//Item->ItemEntry.ItemGuid.Equals(Pickup->PrimaryPickupItemEntry.ItemGuid)
		// if itemguid == pickupguid with maxstacked then give BUT if itemdef is also then throwerror
		if (Item->ItemEntry.ItemDefinition == ItemDef
			/*Item->ItemEntry.ItemGuid.Equals(Pickup->PrimaryPickupItemEntry.ItemGuid)*/
			&& GetMaxStackSize(Item->ItemEntry.ItemDefinition) > 1) {
			//if (NumToDrop == 0) {
				//bStackable = true;
			std::cout << "ITEM SHOULD STACK" << std::endl;
			float MaxStackSize = GetMaxStackSize(Item->ItemEntry.ItemDefinition);
			int tempChange = (Item->ItemEntry.Count += Count);
			if (tempChange >= MaxStackSize)
			{
				tempChange = MaxStackSize;
			}
			// shouldnt even try to change this :/
			//else tempChange = (Item->ItemEntry.Count += Count);
			// need to actually check if its over max size

			if (tempChange > MaxStackSize) {
				NumToDrop = tempChange - MaxStackSize;
				tempChange = (Item->ItemEntry.Count = MaxStackSize);
				if (NumToDrop > 0) {
					if (QuickBar == EFortQuickBars::Primary)
					{
						//if (TempBool)
						//	bSwappable = true;
						//bStackable = false; // ggs we want to try to give the item, 
						// items should fall back and spawn on the floor
						Count = NumToDrop;
					}
					else {
						Count = 0;
						bSwappable = false;
						SpawnPickup(
							Item->ItemEntry.ItemDefinition,
							PC->Pawn->K2_GetActorLocation() + PC->Pawn->GetActorForwardVector(),
							NumToDrop,
							Pickup ? Pickup->PrimaryPickupItemEntry.LoadedAmmo : LoadedAmmo, // skunked dev ngl
							EFortPickupSourceTypeFlag::Player,
							EFortPickupSpawnSource::Unset
						);
					}
				}
				else
				{
					Count = 0;
					bSwappable = false;
				}
				//ItemGuid.push_back({ Item->ItemEntry.ItemGuid, tempChange });
			}
			else // NOTHING more is needed
			{
				Count = 0;
				bSwappable = false;
			}
			//else // ik this code is bad but better then my last inv
				//ItemGuid.push_back({ Item->ItemEntry.ItemGuid, tempChange });

			ItemGuid.push_back({ Item->ItemEntry.ItemGuid, tempChange });
			/*}
			else {
				std::cout << "trying to over flow the item may fail" << std::endl;
				Count = (Item->ItemEntry.Count += Count);
			}*/
		}

		
	}

	if (ItemGuid.size() > 0) {
		for (const auto& Guid : ItemGuid) {
			for (FFortItemEntry& Item : WorldInventory->Inventory.ReplicatedEntries)
			{
				if (/*Item.ItemDefinition == ItemDef*/
					Item.ItemGuid.Equals(Guid.first)) {
					Item.Count = Guid.second;
					Update(PC, &Item);
					break;
				}
			}
		}
		//return true; ~ breaks with mroe then 1 item!
	}

	if (QuickBar == EFortQuickBars::Primary && IsInvFull(PC))
	{
		// ik this looks skunked asf but ggs ig

		if (Pickup && bSwappable) {
			// so proepr
			// it isnt calling it by default gotta loot at it
			((AFortPlayerPawnAthena*)PC->Pawn)->ServerHandlePickupWithSwap(Pickup, CurrWeapon->ItemEntryGuid, 0.4, Pickup->K2_GetActorLocation(), true);
		}
		else if (NumToDrop > 0) {
			SpawnPickup(
				ItemDef,
				PC->Pawn->K2_GetActorLocation() + PC->Pawn->GetActorForwardVector(),
				NumToDrop,
				Pickup ? Pickup->PrimaryPickupItemEntry.LoadedAmmo : LoadedAmmo,
				EFortPickupSourceTypeFlag::Player,
				EFortPickupSpawnSource::Unset
			);
		}
		else {
		//	return true; // bc yes
		}
		
		// shouldnt be implemented here
		return true;
	}

	if (Count <= 0) return true;

	UFortWorldItem* Item = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count, 0);
	if (Item) {
		Item->ItemEntry.LoadedAmmo = Pickup ? Pickup->PrimaryPickupItemEntry.LoadedAmmo : LoadedAmmo;
		Item->ItemEntry.Count = Count; // pretty sure this inst actually needed
		Item->SetOwningControllerForTemporaryItem(PC);
		if (QuickBar == EFortQuickBars::Secondary) {
			FFortItemEntryStateValue Value{};
			Value.IntValue = 1;
			Value.StateType = EFortItemEntryState::ShouldShowItemToast;
			Item->ItemEntry.StateValues.Add(Value);
		}
		PC->WorldInventory->Inventory.ItemInstances.Add(Item);
		PC->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		Update(PC, &Item->ItemEntry);
	}

	return true;
}


bool Inventory::RemoveItem(AFortPlayerControllerAthena* PC, FGuid ItemGuid, int Count, bool bDropping)
{
	bool bIsCertain = false;
	//for (UFortWorldItem*& Item : PC->WorldInventory->Inventory.ItemInstances)
	//{
	for (int i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++) // only bc i cant get index without this
	{
		UFortWorldItem*& Item = PC->WorldInventory->Inventory.ItemInstances[i];
		if (Item->ItemEntry.ItemGuid.Equals(ItemGuid) 
			&& Item->CanBeDropped()) { // hoping this is set to false on them gamemodes

			if (Count > 0) {
				//
				Item->ItemEntry.Count -= Count;
				if (Item->ItemEntry.Count > 0)
					bIsCertain = true;
				else
				{
					int DroppedAmount = Count + Item->ItemEntry.Count;
					Count = UKismetMathLibrary::Max(0, DroppedAmount);
					Item->ItemEntry.Count = 0;
				}
			}
			else 
				Count = Item->ItemEntry.Count;

			if (bDropping)
				SpawnPickup(
					Item->ItemEntry.ItemDefinition,
					PC->Pawn->K2_GetActorLocation() + PC->Pawn->GetActorForwardVector(), // pos might be weird but it should never be behind
					Count,//Item->ItemEntry.Count,
					Item->ItemEntry.LoadedAmmo,
					EFortPickupSourceTypeFlag::Player,
					EFortPickupSpawnSource::Unset,
					(AFortPawn*)PC->Pawn
				);

			if (!bIsCertain)
				PC->WorldInventory->Inventory.ItemInstances.Remove(i);
		}
	}

	for (int i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++) 
	{
		FFortItemEntry& Item = PC->WorldInventory->Inventory.ReplicatedEntries[i];
		if (Item.ItemGuid.Equals(ItemGuid))
		{
			if (bIsCertain) {
				Item.Count -= Count;
				Update(PC, &Item);
				return true;
			}
			else
			    PC->WorldInventory->Inventory.ReplicatedEntries.Remove(i);
		}
	}

	// i dont think anything is is needed
	Update(PC);

	return true; // uh i need to make it false but eh
}

void Inventory::UpdateLoadedAmmo(AFortPlayerControllerAthena* PC, AFortWeapon* Weapon)
{
	if (!PC) return;

	AFortInventory* WorldInventory = PC->WorldInventory;
	if (WorldInventory && Weapon) 
	{
		for (int i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++) // only bc i cant get index without this
		{
			UFortWorldItem*& Item = PC->WorldInventory->Inventory.ItemInstances[i];
			if (Item->ItemEntry.ItemGuid.Equals(Weapon->ItemEntryGuid))
			{
				Item->ItemEntry.LoadedAmmo = Weapon->AmmoCount;
			}
		}

		for (int i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++) {
			auto& Item = WorldInventory->Inventory.ReplicatedEntries[i];

			if (Item.ItemGuid.Equals(Weapon->ItemEntryGuid)) {
				Item.LoadedAmmo = Weapon->AmmoCount;
				std::cout << "RELOADED" << std::endl;
				Update(PC, &Item);
			}
		}
	}

	Update(PC); // shouldnt be needed
}
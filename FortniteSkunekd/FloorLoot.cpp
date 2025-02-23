#include "framework.h"
#include "util.h"
#include "Inventory.h"
#include "FloorLoot.h"

void FloorLoot::SearchPackages(FFortLootPackageData* PackageData, FString PackageString, AActor* ggs) {
	if (!PackageData) return;
	if (PackageString.ToString().empty()) { std::cout << "empty what the flip" << std::endl; }
	else {
		// not empty
		// loop again!
		

		if (UKismetStringLibrary::Conv_NameToString(PackageData->LootPackageID) == PackageString) {
			if (UKismetStringLibrary::Conv_NameToString(PackageData->ItemDefinition.ObjectID.AssetPathName).ToString() != "None") {
				if (PackageData->Weight != 0) {
					//std::cout << "SPAWNING FLOOR LOOT " << PackageData->ItemDefinition.Get()->GetName() << std::endl;
						
					UFortItemDefinition* ItemDef = Utils::StaticLoadObject<UFortItemDefinition>(UKismetStringLibrary::Conv_NameToString(PackageData->ItemDefinition.ObjectID.AssetPathName).ToString());
					if (ItemDef) {
						Inventory::SpawnPickup(ItemDef, ggs->K2_GetActorLocation(), PackageData->Count, -1);
		
					}
					else {
						std::cout << "FAILED :( " << std::endl;
					}
				}
				else { }
			}

			//std::cout << UKismetStringLibrary::Conv_NameToString(PackageData->ItemDefinition.ObjectID.AssetPathName).ToString() << std::endl;
		}
	}
}

FFortLootTierData* FloorLoot::FindRandomTierGroup(std::vector<FFortLootTierData*> FloorArray)
{
	float TotalWeight = 0.0f;

	for (const auto& Entry : FloorArray)
	{
		TotalWeight += Entry->Weight;
	}
	// Weights are floats e.g 1.0, 0.75, 20.2
	float RandomFloat = UKismetMathLibrary::RandomFloat() * TotalWeight;

	float CumulativeWeight = 0.0f;
	FFortLootTierData* Returned = nullptr;

	for (const auto& Entry : FloorArray)
	{
		CumulativeWeight += Entry->Weight;
		if (RandomFloat <= CumulativeWeight)
		{
			Returned = Entry;
			break;
		}
	}

	return Returned;
}


FFortLootPackageData* FloorLoot::FindRandomPackageGroup(std::vector<FFortLootPackageData*> FloorArray)
{
	float TotalWeight = 0.0f;

	for (const auto& Entry : FloorArray)
	{
		TotalWeight += Entry->Weight;
	}
	// Weights are floats e.g 1.0, 0.75, 20.2
	float RandomFloat = UKismetMathLibrary::RandomFloat() * TotalWeight;

	float CumulativeWeight = 0.0f;
	FFortLootPackageData* Returned = nullptr;

	for (const auto& Entry : FloorArray)
	{
		CumulativeWeight += Entry->Weight;
		if (RandomFloat <= CumulativeWeight)
		{
			Returned = Entry;
			break;
		}
	}

	return Returned;
}

void FloorLoot::LootTier(FName SearchLootTierGroup, FVector Location) {
	// looting so we need to get the playlist
	// goal with this is the loot would change per playlist.. if the playlist isnt supposed to have certain items it should be auto!
	// took a while but yeah ggs
	// this is also me learning how DataTables work + a bit more info on blueprints bc i'm skunked
	
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
	
	UDataTable* LootTierData = Utils::StaticLoadObject<UDataTable>(UKismetStringLibrary::Conv_NameToString(GameState->CurrentPlaylistInfo.BasePlaylist->LootTierData.ObjectID.AssetPathName).ToString());
	UDataTable* LootPackages = Utils::StaticLoadObject<UDataTable>(UKismetStringLibrary::Conv_NameToString(GameState->CurrentPlaylistInfo.BasePlaylist->LootPackages.ObjectID.AssetPathName).ToString());

	std::vector<FFortLootTierData*> FloorArray;

	//TArray<FFortLootTierData*> FloorArray;
	for (auto Tiers : LootTierData->RowMap) {
		// each loop is a FortLootTierData
		FFortLootTierData* TierData = (FFortLootTierData*)Tiers.Second;
		if (!TierData) continue;
		//   "TierGroup": "Loot_AthenaFloorLoot", ~ loottier
		//  "SearchLootTierGroup": "Loot_AthenaFloorLoot", ~ floorloot
		if (TierData->TierGroup == SearchLootTierGroup
			&& TierData->Weight != 0) // 0 is like never spawns
		{
			FloorArray.push_back(TierData);
		}
	}

	// So now we have a list of floorloot 

	// It's a array basically and we would need to find a random one in that array,
	// we already filtered the ones that are 0, and not in the correct SearchGroup,
	// but now we need to find a random Object in the array
	FFortLootTierData* TierData = FindRandomTierGroup(FloorArray);

	if (TierData) 
	{
		// Now we will look for the stuff in Package Table
		std::vector<std::vector<FFortLootPackageData*>> PackageArray; // uh idrk how to do this but idc

		std::vector<FFortLootPackageData*> WeaponLoot;
		std::vector<FFortLootPackageData*> SameAmmo; // like wtf how skunked could you be

		//int IdrkHowToDoThis = 0;
		std::vector<FFortLootPackageData*> TempList;
		//int MaxValue = -1;

		// I SPENT SO LONG ON THIS AND THE LEFT OVER LIST IS DATA THAT WE WNAT TO GENRATE LOL OMFG

		for (auto Packages : LootPackages->RowMap) 
		{
			FFortLootPackageData* PackageData = (FFortLootPackageData*)Packages.Second;
			if (!PackageData) continue;

			//if (IdrkHowToDoThis == -1)
				//TempList = {};

			
			float PackageDrops = std::floor(TierData->NumLootPackageDrops);
			// both should be the same normally located at the top of the file

			// sooo its less so new list?
			//if (TempList.size() > 0 && PackageData->LootPackageCategory <= IdrkHowToDoThis) {
			//	PackageArray.push_back(TempList);
			//	MaxValue++;
			//	TempList.clear();
			//}

			if (PackageData->LootPackageID == TierData->LootPackage
				&& PackageData->Weight != 0  // 0 is like never spawns
				&& (PackageData->LootPackageCategory) <= PackageDrops) // some filtering bc some stuff should become auto ~ we + 1 bc yeah
			{
				//PackageData->LootPackageCategory
				// skunked but eh if it works it works
				/*if (PackageData->LootPackageCategory == 0 && 
					PackageData->LootPackageCall.ToString().contains(".Weapon")) {
					WeaponLoot.push_back(PackageData);
					continue;
				}

				if (PackageData->LootPackageCategory == 1 &&
					PackageData->LootPackageCall.ToString().contains(".Ammo")) {
					SameAmmo.push_back(PackageData);
					continue;
				}*/
				
				TempList.push_back(PackageData);
				//IdrkHowToDoThis = PackageData->LootPackageCategory;
			}


		}
		//if (TempList.size() > 0) {
		//	PackageArray.push_back(TempList);
		//	MaxValue++;
		//}
		//TempList.clear();

		// like again we want to filter this list

		//LootPackageCategory ~ (0 = gun) (1 = Ammo) (2 = Consumables) (3 = Resources)
		//NumLootPackageDrops
		//TierData->NumLootPackageDrops ~ depending on the item!!!
		//LootPackageCall is only ever empty when def asset name isnt empty

		// we will just look for a random for each in this list, yes its proper i swear
		for (FFortLootPackageData* Item : TempList) {
			//FFortLootPackageData& PackageData = FindRandomPackageGroup()
			// we really just need to find the other package thingy in the list
			std::vector<FFortLootPackageData*> LootPackageCalls;

			if (Item->LootPackageCall.ToString().empty()) {
				//std::cout << "TEST TO SEE IF THIS ACTUALLY CALLS" << std::endl;
				LootPackageCalls.push_back(Item);
			}
			else {
				for (TPair<FName, uint8*> Map : LootPackages->RowMap) {
					FFortLootPackageData* LootPackageData = (FFortLootPackageData*)Map.Second;
					std::string LootPackageDataString = UKismetStringLibrary::Conv_NameToString(LootPackageData->LootPackageID).ToString();
					if (LootPackageDataString == Item->LootPackageCall.ToString() && LootPackageData->Weight != 0.0) {
						LootPackageCalls.push_back(LootPackageData);
					}
				}
			}

			FFortLootPackageData* PackageData = FindRandomPackageGroup(LootPackageCalls);
			if (!PackageData) {
				std::cout << "GGS" << std::endl;
				continue;
			}

			UFortItemDefinition* ItemDef = nullptr;
			FString ConvAsset = UKismetStringLibrary::Conv_NameToString(PackageData->ItemDefinition.ObjectID.AssetPathName);
			if (ConvAsset.ToString() != "None") {
				ItemDef = Utils::StaticLoadObject<UFortItemDefinition>(ConvAsset.ToString());
			}

			if (!ItemDef) {
				std::cout << "FAILED TO FIND ITEM" << std::endl;
				continue;
			}

			FVector Temp = Location;
			Temp.Z += 50;
			if (PackageData->Count > 0) {
				Inventory::SpawnPickup(ItemDef, Temp, PackageData->Count, -1, EFortPickupSourceTypeFlag::FloorLoot, EFortPickupSpawnSource::ItemSpawner);
			}
		}
	
		PackageArray.clear();
	}
	else 
	{
		std::cout << "Loot NUllptr but might be okAY" << std::endl; // makes the user think its okay clearly isnt
	}


	//FFortLootTierData* ChosenLootTierData = SelectLootTierData(FloorArray);
	//if (!ChosenLootTierData) {
	//	std::cout << "COOKED" << std::endl;
	//	return;
	//}


	// Loot Packages should have the same "FIRST" id since its like "WorldPKG.AthenaLoot.Weapon.HighShotgun.01":

	// ill recode this in the future
	//TArray<FFortLootTierData*> LetOverArray; // get it array0x hahaaaaaaaaaaaaaaaaaaaaaaaaa

	

	//for (auto Packages : FloorArray) {

	//	//  "ObjectName": "ScriptStruct'FortLootPackageData'",
	//	// we loop and loopahah
	//	for (auto Loots : LootPackages->RowMap) {
	//		//if (Loots.First == Packages->LootPackage) {
	//		FFortLootPackageData* PackageData = (FFortLootPackageData*)Loots.Second;

	//		if (!PackageData) continue;
	//		if (PackageData->Weight == 0) continue; // this item cant spawn since 0 weight

	//		if (PackageData->LootPackageID == Packages->LootPackage) {
	//			if (UKismetStringLibrary::Conv_NameToString(PackageData->ItemDefinition.ObjectID.AssetPathName).ToString() != "None") {
	//				// itemdef isnt none!
	//				//std::cout << "what da flip!?!?" << std::endl;
	//				//Inventory::SpawnPickup(PackageData->ItemDefinition.Get(), BuildingContainer->K2_GetActorLocation(), PackageData->Count, -1);
	//				continue;
	//			}

	//			
	//			LetOverArray.push_back(PackageData);

	//			// hasnt spawned? normal!
	//			// LetOverArray
	//			//LootPackageCall

	//			//if (PackageData->LootPackageCall == UKismetStringLibrary::Conv_NameToString(Packages->LootPackage)) {
	//			//	std::cout << "YES" << std::endl;

	//			//	// redo it again
	//			//	//for (auto& Loots : LootPackages->RowMap) {
	//			//		//FFortLootPackageData* PackageData2 = (FFortLootPackageData*)Loots.Second;
	//			//	//}
	//			//}
	//			//WorldList.AthenaLoot.Weapon.Shotgun
	//		}



	//		//}
	//		//FFortLootPackageData* PackageData = (FFortLootPackageData*)Loots.Second;
	//	}
	//}

	// left over data....
	//for (auto Loots : LootPackages->RowMap) {
	//	//if (Loots.First == Packages->LootPackage) {
	//	FFortLootPackageData* PackageData = (FFortLootPackageData*)Loots.Second;

	//for (auto Loots : LootPackages->RowMap) {
	//	//if (Loots.First == Packages->LootPackage) {
	//	FFortLootPackageData* PackageData = (FFortLootPackageData*)Loots.Second;

	//	if (!PackageData) continue;
	//	if (PackageData->Weight == 0) continue;

	//	//std::cout << PackageData->LootPackageCall.ToString() << std::endl;
	//	for (auto Packages : LetOverArray) {
	//		std::cout << UKismetStringLibrary::Conv_NameToString(PackageData->LootPackageID) << std::endl;
	//		if (Packages->LootPackageCall == UKismetStringLibrary::Conv_NameToString(PackageData->LootPackageID)) {
	//			std::cout << Packages->LootPackageCall << std::endl;
	//	
	//			continue;
	//			
	//			if (PackageData->LootPackageCall.ToString().empty()) {
	//				//std::cout << "what da flip!?!?" << std::endl;
	//				Inventory::SpawnPickup(PackageData->ItemDefinition.Get(), BuildingContainer->K2_GetActorLocation(), PackageData->Count, -1);
	//				continue;
	//			}
	//			/*	if (UKismetStringLibrary::Conv_NameToString(PackageData->LootPackageID) == PackageString) {
	//					if (UKismetStringLibrary::Conv_NameToString(PackageData->ItemDefinition.ObjectID.AssetPathName).ToString() != "None") {
	//		*/



	//			//FloorLoot::SearchPackages(Packages, PackageData->LootPackageCall, BuildingContainer);
	//		}
	//		
	//	}
	//}
	//}

	//FloorLoot::SearchPackages(LootPackages, PackageData->LootPackageCall, BuildingContainer);

	FloorArray.clear();
	//FloorArray.Free();
}
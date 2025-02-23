#include "framework.h"
#include "util.h"
#include "Vehicle.h"
#include "Inventory.h"
#include "FloorLoot.h"
#include "GameMode.h"

bool GameMode::ReadyToStartMatch(AFortGameModeAthena* GameMode) {
	static bool StartedMatch = false;
	
	OriginalRTSM(GameMode);

	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

	if (!GameMode || !GameState) return false;

	if (!StartedMatch) {
		static bool LoadedPL = false;

		if (!LoadedPL) {
			LoadedPL = true;

			UFortPlaylistAthena* Playlist = UObject::FindObjectContain<UFortPlaylistAthena>(std::format("{}.{}", Config::PlaylistID, Config::PlaylistID));

			if (!Playlist)
				return false;

			GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
			GameState->CurrentPlaylistInfo.MarkArrayDirty();

			GameState->CurrentPlaylistId = Playlist->PlaylistId;
		

			// fixes the crash
			AFortGameSessionDedicatedAthena* GameSession = Utils::SpawnActor<AFortGameSessionDedicatedAthena>();
			GameSession->SessionName = UKismetStringLibrary::Conv_StringToName(FString(L"SigmaSigma"));
			GameSession->MaxPlayers = Playlist->MaxPlayers;//100;
			std::cout << Playlist->MaxSocialPartySize << std::endl;
			GameSession->MaxPartySize = Playlist->MaxSocialPartySize;
			GameMode->GameSession = GameSession;
			GameMode->FortGameSession = GameSession;

			GameSession->MaxSplitscreensPerConnection = 2;

			//GameSession->MMSVersionCompatability
			
			/*
				MSS : 
			*/
			//GameState->
			
		
	
			//GameMode->ServerBotManager
			// should make it more proper ngl
			GameState->AirCraftBehavior = Playlist->AirCraftBehavior;
			GameState->OnRep_Aircraft(); // i dont think this does anything
		/*	FSlateBrush SigmaBus = GameState->AircraftPathTeamIndicatorBrush;
			
			Playlist->AircraftPathOffsetFromMapCenterMax
	
			GameState->AircraftPathTeamIndicatorBrush = SigmaBus;*/

			//GameMode->GameSession->MaxPlayers = Playlist->MaxPlayers;
			//GameMode->FortGameSession->MaxPlayers = Playlist->MaxPlayers;
			std::cout << "PlaylistPlayers: " << std::to_string(Playlist->MaxPlayers) << std::endl;

			// Loads the timer!
			ABuildingFoundation* BuildingFound2 = Utils::StaticFindObject<ABuildingFoundation>(L"/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.BP_Countdown_Child_Jerky_2");
			Utils::ShowFoundation((ABuildingFoundation*)BuildingFound2);
			
			if (Config::Event) {
				std::cout << "EVENT IS LOADED" << std::endl;

				TArray<AActor*> BuildingFoundations;
				UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ABuildingFoundation::StaticClass(), &BuildingFoundations);


				for (AActor*& BuildingFoundation : BuildingFoundations) {
					ABuildingFoundation* Foundation = (ABuildingFoundation*)BuildingFoundation;
					if (!Foundation) continue;

					if (BuildingFoundation->GetName().contains("Jerky") ||
						BuildingFoundation->GetName().contains("LF_Athena_POI_19x19")) {
						std::cout << BuildingFoundation->GetName() << std::endl;
						Utils::ShowFoundation((ABuildingFoundation*)BuildingFoundation);
					}
				}

				BuildingFoundations.Free();

				
			}
			else {
				// you can remove i just feel like its needed
				// load the event stage but not event stuff 
				ABuildingFoundation* BuildingFound = Utils::StaticFindObject<ABuildingFoundation>(L"/Game/Athena/Apollo/Maps/Apollo_POI_Foundations.Apollo_POI_Foundations.PersistentLevel.LF_Athena_POI_19x19_2");
				Utils::ShowFoundation((ABuildingFoundation*)BuildingFound);
			}
		}

		if (!GameState->MapInfo) 
			return false; // if map isnt loaded we dont want servers up or gs will not like it
	
		static bool IsListening = false;
		
		if (!IsListening) {
			IsListening = true;
			// every time i dont call this after mapinfo nothing works
			GameState->OnRep_CurrentPlaylistId();
			GameState->OnRep_CurrentPlaylistInfo();

			// Load Additional Actors/Objects
			if (Config::Event) {
				Starter = Utils::StaticLoadObject<UClass>("/CycloneJerky/Gameplay/BP_Jerky_Scripting.BP_Jerky_Scripting_C");
				JerkyLoader = UObject::FindObject<UObject>("BP_Jerky_Loader_C JerkyLoaderLevel.JerkyLoaderLevel.PersistentLevel.BP_Jerky_Loader_2");
			}

			// this isnt proper but ggs
			GameState->MapInfo->SupplyDropInfoList[0]->SupplyDropClass = Utils::StaticLoadObject<UClass>("/Game/Athena/SupplyDrops/AthenaSupplyDrop_Donut.AthenaSupplyDrop_Donut_C");

			//GameState->OnRep_cosme
			
			
			TArray<AActor*> VendingMatchine;
			// might be it else ill just fine the blueprint or smth

			
		
			UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), UObject::FindObject<UClass>("BlueprintGeneratedClass B_Athena_VendingMachine.B_Athena_VendingMachine_C"), &VendingMatchine);
			
			for (AActor* Test : VendingMatchine) {
				//ABuildingProp_VendingMachine* test = Utils::Cast<ABuildingProp_VendingMachine>(Test);
				//if (!test) continue;

				//test->AllowedItemRotationDamageTags.

				Test->K2_DestroyActor();
			}

			VendingMatchine.Free();

			TArray<AActor*> BuildingContainers;
			UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), ABuildingContainer::StaticClass(), &BuildingContainers);

			for (AActor*& Test : BuildingContainers)
			{
				ABuildingContainer* Container = (ABuildingContainer*)Test;
				// alr idrk how to do this
				if (Container)
				{
					if (Container->GetName().contains("Chest"))
					{
						// for testing here
						if (Container->PotentialRandomUpgrades.Num() > 0)
						{
							Container->SearchLootTierGroup = Container->PotentialRandomUpgrades[0].LootTierGroupIfApplied;
						
							//UFortKismetLibrary::New
							//Container->AlternateMeshes[0].MeshSets[0].SwapInSounds[0].Substitute;
							//Container->SearchingSoundCueLoop = 

							//Container->AlternateMeshes[0].MeshSets[0].
							/*UStaticMeshComponent* Sigma = (UStaticMeshComponent*)UGameplayStatics::SpawnObject(UStaticMeshComponent::StaticClass(), Container->Owner);
							Sigma->SetStaticMesh(Container->StaticMesh);
							Sigma->bIsActive = true;
							Sigma->bHiddenInGame = false;*/
							Container->StaticMeshComponent->SetStaticMesh(Container->AlternateMeshes[0].MeshSets[0].BaseMesh);
							Container->StaticMeshComponent->OnRep_StaticMesh(Container->StaticMesh);
							Container->StaticMesh = Container->AlternateMeshes[0].MeshSets[0].BaseMesh;

							//Sigma->OnRep_StaticMesh();
							
							//Container->StaticMeshComponent->SetStaticMesh(Container->StaticMesh);
						
							
							//Container->SearchingSoundCueLoop = Container->AlternateMeshes[0].MeshSets.;
							Container->SearchedMesh = Container->AlternateMeshes[0].MeshSets[0].SearchedMesh;
							Container->SearchSpeed = Container->AlternateMeshes[0].MeshSets[0].SearchSpeed;
							
						}
						//Container->PotentialRandomUpgrades
					}
				}

			}
			BuildingContainers.Free();
			// supposed to free it
			if (!Config::Event) {
				Vehicle::SpawnVehicle();


				GameMode->ServerBotManager = (UFortServerBotManagerAthena*)UGameplayStatics::SpawnObject(UFortServerBotManagerAthena::StaticClass(), GameMode);
				GameMode->ServerBotManager->CachedGameMode = GameMode;
				GameMode->ServerBotManager->CachedGameState = GameState;
				GameMode->ServerBotManager->CachedBotMutator = Utils::SpawnActor<AFortAthenaMutator_Bots>();
				GameMode->ServerBotManager->CachedBotMutator->CachedGameMode = GameMode;
				GameMode->ServerBotManager->CachedBotMutator->CachedGameState = GameState;
			}
			//GameMode->ServerBotManager->CachedBotMutator->CachedMMRSpawningInfo
			//GameMode->ServerBotManager->bots
			// gotta loot at this ngl
			// 
			// 
			
			//std::cout << "attemtping to spawn stuff in idk hahh " << std::endl;
			//
			//Utils::StaticLoadObject("")
			UFortPlaylistAthena* Playlist = GameState->CurrentPlaylistInfo.BasePlaylist;

			for (const auto& LoopyWoopy : Playlist->AdditionalLevels) {
				bool OutS = false;
				FString SmthValue;
				ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), LoopyWoopy, {}, {}, &OutS, SmthValue);
				FAdditionalLevelStreamed LevelsStreamed {};
				LevelsStreamed.bIsServerOnly = false;
				LevelsStreamed.LevelName = LoopyWoopy.ObjectID.AssetPathName;
				GameState->AdditionalPlaylistLevelsStreamed.Add(LevelsStreamed);
			}

			for (const auto& LoopyWoopy : Playlist->AdditionalLevelsServerOnly) {
				bool OutS = false;
				FString SmthValue;
				ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UWorld::GetWorld(), LoopyWoopy, {}, {}, &OutS, SmthValue);
				FAdditionalLevelStreamed LevelsStreamed{};
				LevelsStreamed.bIsServerOnly = true;
				LevelsStreamed.LevelName = LoopyWoopy.ObjectID.AssetPathName;
				GameState->AdditionalPlaylistLevelsStreamed.Add(LevelsStreamed);
			}
		
			GameState->OnRep_AdditionalPlaylistLevelsStreamed();
			GameState->OnFinishedStreamingAdditionalPlaylistLevel();

			
			/*for (auto& AmICooked : LootPackages->RowMap) {
				AmICooked.
			}*/

			/*
			"RowStruct": {
				"ObjectName": "ScriptStruct'FortLootTierData'",
				"ObjectPath": "/Script/FortniteGame"
				}
			*/

			/*
			"first": { second }
			*/

			//Tiered_Athena_FloorLoot_01_C
			// should be looped through since theres different tier groups

			TArray<AActor*> FloorLoot;
			static UClass* FloorLootClass = Utils::StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C");
			UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), FloorLootClass, &FloorLoot);
			//UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), UObject::FindObject<UClass>("BlueprintGeneratedClass Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C"), &FloorLoot);
			
			//
			// "ObjectName": "Class'BuildingContainer'",
			for (AActor* FloorActor : FloorLoot) {
				ABuildingContainer* BuildCon = (ABuildingContainer*)FloorActor;
				if (!BuildCon) continue;

				FloorLoot::LootTier(BuildCon->SearchLootTierGroup, BuildCon->K2_GetActorLocation());
			}

			FloorLoot.Free();

			TArray<AActor*> LobbyFloorLoot;
			static UClass* LobbuFloorLootClass = Utils::StaticLoadObject<UClass>("/Game/Athena/Environments/Blueprints/Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C");
			UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), LobbuFloorLootClass, &LobbyFloorLoot);

			for (AActor* FloorActor : LobbyFloorLoot) {
				ABuildingContainer* BuildCon = (ABuildingContainer*)FloorActor;
				if (!BuildCon) continue;

				FloorLoot::LootTier(BuildCon->SearchLootTierGroup, BuildCon->K2_GetActorLocation());
			}

			LobbyFloorLoot.Free();

			//
			//for (auto& Tiers : LootTierData->RowMap) {
			//	// each loop is a FortLootTierData
			//	FFortLootTierData* TierData = (FFortLootTierData*)Tiers.Second;
			//}

			// skunked bots but who cares
			
			//FortniteGame/Content/Athena/AI/MANG/Apollo_POI_Agency_MANG.umap
			//Utils::StaticLoadObject("/Game/Athena/AI/MANG/BP_MANG_Spawner.BP_MANG_Spawner")
			
			//

			FName NetDriverDef = UKismetStringLibrary::Conv_StringToName(FString(L"GameNetDriver"));

			UNetDriver* NetDriver = CreateNetDriver(UEngine::GetEngine(), UWorld::GetWorld(), NetDriverDef);
			NetDriver->NetDriverName = NetDriverDef;
			NetDriver->World = UWorld::GetWorld();

			FString Error;
			FURL url = FURL();
			url.Port = 7777; // real hosting use random ports! or just make it proper asf where you cant bypass

			InitListen(NetDriver, UWorld::GetWorld(), url, false, Error);
			SetWorld(NetDriver, UWorld::GetWorld());

			UWorld::GetWorld()->NetDriver = NetDriver;

			for (size_t i = 0; i < UWorld::GetWorld()->LevelCollections.Num(); i++) {
				UWorld::GetWorld()->LevelCollections[i].NetDriver = NetDriver;
			}

			SetConsoleTitleA(("Eon Hosting on port " + std::to_string(url.Port)).c_str());

			// func doesnt even have a xref so i cant get index
			ServerReplicateActors = decltype(ServerReplicateActors)(InSDKUtils::GetImageBase() + 0x1023F60);

			GameMode->bWorldIsReady = true;
			GameMode->WarmupRequiredPlayerCount = 1;
			Config::bWorldisReady = true;

			StartedMatch = true;
		}
	}

	// returning is stripped on chapter 2 >

	return UWorld::GetWorld()->NetDriver->ClientConnections.Num() > 0;
}

// this is replication but isnt stripped so doesnt own its own file
void GameMode::TickFlush(UNetDriver* NetDriver, float DeltaSeconds) {
	if (!NetDriver) return;

	if (NetDriver->ClientConnections.Num() > 0 && 
		NetDriver->ReplicationDriver && 
		!NetDriver->ClientConnections[0]->InternalAck)
	{
		ServerReplicateActors(NetDriver->ReplicationDriver/*, DeltaSeconds*/);
	}

	return OrginalTickFlush(NetDriver, DeltaSeconds);
}

APawn* GameMode::SpawnDefaultPawnFor(AGameMode* GameMode, AFortPlayerController* NewPlayer, AActor* SpawnLocation) {
	std::cout << "FORTNITE BALLS" << std::endl;
	if (NewPlayer && SpawnLocation) {
		std::cout << "default pawn" << std::endl;
		FTransform Transform = SpawnLocation->GetTransform();
		return GameMode->SpawnDefaultPawnAtTransform(NewPlayer, Transform);
	}

	return nullptr;
}

void GameMode::OnAircraftExitedDropZone(AFortGameModeAthena* GameMode, AFortAthenaAircraft* FortAthenaAircraft)
{
	if (Config::Event)
	{
		UFunction* StartEventFunc = JerkyLoader->Class->GetFunction("BP_Jerky_Loader_C", "startevent");

		float ToStart = 0.f;
		JerkyLoader->ProcessEvent(StartEventFunc, &ToStart);
	}

	// yk you have to return it every time ~ stuff breaks without it and could crash
	//if (!Config::Event) // alr marvelco
	//{
		return OriginalOnAircraftExitedDropZone(GameMode, FortAthenaAircraft);
	//}
}
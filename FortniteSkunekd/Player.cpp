#include "framework.h"
#include "Abilites.h"
#include "Player.h"
#include "Inventory.h"
#include "Miscs.h"
#include "util.h"
#include "FloorLoot.h"


void Player::ServerReadyToStartMatch(AFortPlayerControllerAthena* PC) {
	if (!PC) return;

	//PC->ClientReportTournamentPlacementPointsScored(10, 69);

	// crashed on death bc matchreport is actually null
	PC->MatchReport = (UAthenaPlayerMatchReport*)UGameplayStatics::SpawnObject(UAthenaPlayerMatchReport::StaticClass(), PC);

	std::cout << "ServerReadyToStartMatch" << std::endl;

	static bool FirstPersonJoined;
	if (!FirstPersonJoined) {
		FirstPersonJoined = true;
		AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
		AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
		GameState->DefaultBattleBus = Utils::StaticLoadObject<UAthenaBattleBusItemDefinition>("/Game/Athena/Items/Cosmetics/BattleBuses/BBID_DonutBus.BBID_DonutBus");
		for (UAthenaCosmeticItemDefinition*& ItemDef : GameMode->BattleBusCosmetics) {
			ItemDef = GameState->DefaultBattleBus;
		}

		// arena thing skunked
	    //ArenaScoringHUD_C
		//FortniteGame/Content/UI/Competitive/Arena/ArenaScoringHUD.uasset
		//FortniteGame/Content/UI/Competitive/FlagTile.uasset
		//UUserWidget* FRFR = (UUserWidget*)Utils::StaticLoadObject<UClass>("/Game/Content/UI/Competitive/FlagTile.FlagTile_C");
		//UClass* WidgetClass = Utils::StaticLoadObject<UClass>("/Game/Content/UI/Competitive/Arena/ArenaScoringHUD.ArenaScoringHUD_C");

		//UUserWidget* ArenaScoringHUD = (UUserWidget*)UGameplayStatics::SpawnObject(WidgetClass, PC);
		//ArenaScoringHUD->AddToViewport(9);*/
	
		//Utils::StaticLoadObject<UClass>("/Game/Content/UI/Competitive/Arena/ArenaScoringHUD.ArenaScoringHUD_C");
	
		//FRFR->AddToPlayerScreen
	
		GameState->EventTournamentRound = EEventTournamentRound::Arena;
		GameState->OnRep_EventTournamentRound();
	}

	return OrginalServerReadyToStartMatch(PC);
}

void Player::ServerLoadingScreenDropped(AFortPlayerControllerAthena* PC) {

	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

	UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PlayerState);
	PlayerState->OnRep_CharacterData();


	PlayerState->SquadId = PlayerState->TeamIndex - 2;
	PlayerState->OnRep_TeamIndex(0);
	PlayerState->OnRep_SquadId();
	PlayerState->OnRep_PlayerTeam();
	PlayerState->OnRep_PlayerTeamPrivate();

	FGameMemberInfo Member{ -1,-1,-1 };
	Member.MemberUniqueId = PC->PlayerState->UniqueId;
	Member.SquadId = PlayerState->SquadId;
	Member.TeamIndex = PlayerState->TeamIndex;

	((AFortGameStateAthena*)UWorld::GetWorld()->GameState)->GameMemberInfoArray.Members.Add(Member);
	((AFortGameStateAthena*)UWorld::GetWorld()->GameState)->GameMemberInfoArray.MarkItemDirty(Member);

	// gives the set
	Abilities::GiveAbilitySet(PC, "GAS_AthenaPlayer.GAS_AthenaPlayer");

	Inventory::InitInventory(PC);
	

	PC->XPComponent->bRegisteredWithQuestManager = true;
	PC->XPComponent->OnRep_bRegisteredWithQuestManager();

	return OrginalServerLoadingScreenDropped(PC);
}

void Player::ServerAcknowledgePossession(AFortPlayerControllerAthena* PC, AFortPlayerPawnAthena* Pawn) {
	PC->AcknowledgedPawn = Pawn;

	std::cout << "ServerAck" << std::endl;
	return OrginalServerAcknowledgePossession(PC, Pawn);
}

void Player::ServerExecuteInventoryItem(AFortPlayerControllerAthena* PC, FGuid& ItemGuid)
{
	if (!PC || !PC->MyFortPawn) return;

	if (PC->IsInAircraft()) return;

	for (FFortItemEntry& ItemEntry : PC->WorldInventory->Inventory.ReplicatedEntries)
	{
		if (ItemEntry.ItemGuid.Equals(ItemGuid)) {
			if (!ItemEntry.ItemDefinition)
				continue;

			UFortWeaponItemDefinition* WeaponDef = (UFortWeaponItemDefinition*)ItemEntry.ItemDefinition;

			if (ItemEntry.ItemDefinition->IsA(UFortGadgetItemDefinition::StaticClass()))
				WeaponDef = ((UFortGadgetItemDefinition*)ItemEntry.ItemDefinition)->GetWeaponItemDefinition();
			//if (ItemEntry.ItemDefinition->IsA(UFortWeaponItemDefinition::StaticClass()) 
				//|| ItemEntry.ItemDefinition->IsA(UFortSpyTechItemDefinition::StaticClass())) 
			//{
				PC->MyFortPawn->EquipWeaponDefinition(WeaponDef, ItemGuid);
			//}

			break;
		}
	}
}

void Player::ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Aircraft, const struct FRotator& ClientRotation) {
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Aircraft->GetOwner();

	((AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode)->RestartPlayer(PC);

	if (PC->MyFortPawn)
	{
		PC->ClientSetRotation(ClientRotation, true); // so it like looks straight when you jump not porper
		PC->MyFortPawn->BeginSkydiving(true);
		PC->MyFortPawn->SetHealth(100);
	}
}


void Player::ServerSetInAircraft(AFortPlayerStateAthena* PlayerState, bool bNewInAircraft)
{
	// Clear Inventory!
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)PlayerState->Owner;
	if (!PC) return;
	
	Inventory::InitInventory(PC);
	

	if (Config::Event)
	{
		AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

		// alr so the real paths only works correctly like 70% of the then its like in a weird place
		AActor* BattleBus = GameState->GetAircraft(0);
		auto Aircraft = GameState->GetAircraft(0);
		FVector Loc = FVector(62590, -75501, 13982); // is this even the proper one marvelco
		Aircraft->FlightInfo.FlightStartLocation = (FVector_NetQuantize100)Loc;
		Aircraft->FlightInfo.FlightSpeed = 1000;
		Aircraft->FlightInfo.TimeTillDropStart = 1;
		Aircraft->DropStartTime = UGameplayStatics::GetTimeSeconds(UWorld::GetWorld()) + 1;
		GameState->bAircraftIsLocked = false;
	}

	return OrginalServerSetInAircraft(PlayerState, bNewInAircraft);
}

// dont use this to kick a user AND only for the user
void Player::ServerReturnToMainMenu(AFortPlayerControllerAthena* PC) {
	PC->ClientReturnToMainMenu(L"Thanks For Playing Oryn");
}

void Player::ServerHandlePickup(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, float InFlyTime, FVector& InStartDirection, bool bPlayPickupSound)
{
	
	auto& HandlePickup = OriginalServerHandlePickup;
	std::cout << "PICKUP" << std::endl;
	if (!Pawn || !Pickup || Pickup->bPickedUp)
		return HandlePickup(Pawn, Pickup, InFlyTime, InStartDirection, bPlayPickupSound); // no man full boxed 200

	//sub_7FF77491FBB0

	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->GetController();
	if (!PC || PC->IsA(AFortAthenaAIBotController::StaticClass())) return HandlePickup(Pawn, Pickup, InFlyTime, InStartDirection, bPlayPickupSound);;
	if (PC->IsInAircraft()) return HandlePickup(Pawn, Pickup, InFlyTime, InStartDirection, bPlayPickupSound);

	// some checks so users cant do stuff
	// you also have to do Pawn since pc distance isnt the same as PC (0,0,0 or smth) ~ or my 1.11 replication doesnt change it
	if (Pawn->GetDistanceTo(Pickup) > 500) {
		((AFortPlayerStateAthena*)PC->PlayerState)->KickedFromSessionReason = EFortKickReason::WasBanned;
		ValidationKick(Utils::Cast<AFortGameSession>(UWorld::GetWorld()->AuthorityGameMode->GameSession), PC, nullptr, nullptr);
		//PC->ClientReturnToMainMenu(L"Far Pickup"); // if the cheat doesnt return
		return;
	}

	
	// Not really proper but ggs i cant be btoihred
	// swapping is like swappinginternal or smth

	if(!Inventory::GiveItem( // this layout looks bad but ggs
		PC,
		Pickup->PrimaryPickupItemEntry.ItemDefinition,
		Pickup->PrimaryPickupItemEntry.Count,
		Pickup // this is ONLY for swapping func bc skunekd asf
		))
		return; // if it fails it should really give the item like this

	Pickup->PickupLocationData.FlyTime = InFlyTime;
	Pickup->PickupLocationData.bPlayPickupSound = true;
	Pickup->PickupLocationData.PickupTarget = Pawn;
	Pickup->PickupLocationData.StartDirection = (FVector_NetQuantizeNormal)InStartDirection;
	Pickup->OnRep_PickupLocationData();
	Pickup->bPickedUp = true;
	Pickup->OnRep_bPickedUp();


	return HandlePickup(Pawn, Pickup, InFlyTime, InStartDirection, bPlayPickupSound);
}

void Player::ServerHandlePickupWithSwap(AFortPlayerPawn* Pawn, AFortPickup* Pickup, const struct FGuid& Swap, float InFlyTime, const struct FVector& InStartDirection, bool bPlayPickupSound)
{
	if (!Pawn || !Pickup || Pickup->bPickedUp) return;
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Pawn->GetOwner();
	if (!PC) return;

	// legit find the current item, drop it and add it back but in like replace
	for (int i = 0; i < PC->WorldInventory->Inventory.ItemInstances.Num(); i++) // only bc i cant get index without this
	{
		UFortWorldItem*& Item = PC->WorldInventory->Inventory.ItemInstances[i];
		if (Item->ItemEntry.ItemGuid.Equals(Swap)
			&& Item->CanBeDropped()) {
			// if it fails to swap them idrk
			Pickup->PickupLocationData.FlyTime = InFlyTime;
			Pickup->PickupLocationData.PickupTarget = Pawn;
			Pickup->PickupLocationData.bPlayPickupSound = true;
			Pickup->PickupLocationData.StartDirection = (FVector_NetQuantizeNormal)InStartDirection;
			Pickup->OnRep_PickupLocationData();
			Pickup->bPickedUp = true;
			Pickup->OnRep_bPickedUp();

			std::cout << Pickup->GetFullName() << std::endl;

			// if its dropable then we just drop it ig?
			Inventory::RemoveItem(PC, Item->ItemEntry.ItemGuid, Item->ItemEntry.Count, true);

			Inventory::GiveItem(
				PC,
				Pickup->PrimaryPickupItemEntry.ItemDefinition,
				Pickup->PrimaryPickupItemEntry.Count,
				Pickup // skunekd
			);

			//Pawn->ServerHandlePickup(Pickup, InFlyTime, InStartDirection, bPlayPickupSound);

			return OriginalServerHandlePickupWithSwap(Pawn, Pickup, Swap, InFlyTime, InStartDirection, bPlayPickupSound);
		}
	}

	return OriginalServerHandlePickupWithSwap(Pawn, Pickup, Swap, InFlyTime, InStartDirection, bPlayPickupSound);
}

void Player::ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PC, const struct FGuid& ItemGuid, int32 Count, bool bTrash)
{
	if (!(PC && !PC->IsInAircraft())) return; // cooked
	//AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
	//GameState->CurrentPlaylistInfo.BasePlaylist->InventoryItemsToGrant
	// some gamemodes you shouldnt be able to drop the item
	Inventory::RemoveItem(PC, ItemGuid, Count, true);

	//bTrash might be just destroy the item instead of dropping?
}

void Player::OnCapsuleBeginOverlap(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult)
{
	auto& ReturnFunc = OriginalOnCapsuleBeginOverlap;
	if (!Pawn) return;
	if (Pawn->Controller->IsA(AFortAthenaAIBotController::StaticClass())) return;

	if (OtherActor->IsA(AFortPickup::StaticClass()))
	{
		AFortPickup* Pickup = (AFortPickup*)OtherActor;

		if (Pickup->PawnWhoDroppedPickup == Pawn)
			return ReturnFunc(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); // 
		
		UFortItemDefinition* ItemDef = (UFortItemDefinition*)Pickup->PrimaryPickupItemEntry.ItemDefinition;

		// todo~ stacked items

		if (ItemDef) {
			if (ItemDef->IsA(UFortResourceItemDefinition::StaticClass())
				|| ItemDef->IsA(UFortAmmoItemDefinition::StaticClass())
				|| ItemDef->IsA(UFortTrapItemDefinition::StaticClass()))
			{
				
				Pawn->ServerHandlePickup(Pickup, 0.4, Pawn->K2_GetActorLocation() + Pawn->GetActorForwardVector(), true);
			}
		}
	}

	return ReturnFunc(Pawn, OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

//ServerAttemptInteract
//0x458 ~ 8B
void Player::ServerAttemptInteract(UFortControllerComponent_Interaction* ControllerComponent, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId)
{
	OriginalServerAttemptInteract(ControllerComponent, ReceivingActor, InteractComponent, InteractType, OptionalObjectData, InteractionBeingAttempted, RequestId);
	if (!ControllerComponent || !ReceivingActor) return;
	AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)ControllerComponent->GetOwner();
	
	// quest stuff could be in here bc you interact for the quests!

}

// found this while at this func it should spawn loot
//ABuildingContainer::SpawnLoot() called 
// might be a bool bc of returning 0 but ONLY return in this
char Player::ContainerSpawnLoot(ABuildingContainer* a1)
{
	std::cout << "SPAWN LOOT" << std::endl;
	if (a1->bAlreadySearched)
		return 0;
	// not auto!!
	a1->bAlreadySearched = true;
	a1->SearchBounceData.SearchAnimationCount++; // fix the chest not opening
	a1->BounceContainer();
	a1->OnRep_bAlreadySearched();

	std::string LootTierGroup = a1->SearchLootTierGroup.ToString();
	std::cout << "SEARCH " << LootTierGroup << std::endl;
	if (LootTierGroup == "Loot_Treasure")
		LootTierGroup = "Loot_AthenaTreasure";
	else if (LootTierGroup == "Loot_Ammo")
		LootTierGroup = "Loot_AthenaAmmoSmall";

	FloorLoot::LootTier(
		UKismetStringLibrary::Conv_StringToName(std::wstring(LootTierGroup.begin(), LootTierGroup.end()).c_str()),
		a1->K2_GetActorLocation()
	);
	// idrk the best way to find this
	// note for me later
	// theres Loot_AthenaTreasure but with like _Red white ...
	std::cout << "SEARCH " << LootTierGroup << std::endl;
	//if (a1->GetName().contains("Tiered_Chest"))
	//	FloorLoot::LootTier(
	//		UKismetStringLibrary::Conv_StringToName(FString(L"Loot_AthenaTreasure")), 
	//		a1->K2_GetActorLocation()
	//	);
	//// note for later
	////Loot_ApolloAmmoBox_Rare
	//// 
	//else if(a1->GetName().contains("Tiered_Short_Ammo"))
	//	FloorLoot::LootTier(
	//		UKismetStringLibrary::Conv_StringToName(FString(L"Loot_AthenaTreasure")),
	//		a1->K2_GetActorLocation()
	//	);
	//else
	//{
	//	// has a 100% chance of beign wrong
	//	FloorLoot::LootTier(a1->SearchLootTierGroup, a1->K2_GetActorLocation());
	//}
}

void Player::ClientOnPawnDied(AFortPlayerControllerAthena* PC, const struct FFortPlayerDeathReport& DeathReport)
{
	auto& OG = OriginalClientOnPawnDied;
	if (!PC) return OG(PC, DeathReport);
	//PC -> user who died
	//DeathReport.
	//^ killer yk

	// 
	AFortPlayerPawnAthena* EnemyPawn = (AFortPlayerPawnAthena*)DeathReport.KillerPawn;
	AFortPlayerStateAthena* EnemyState = (AFortPlayerStateAthena*)DeathReport.KillerPlayerState;
	AFortPlayerStateAthena* PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

	if (PC->WorldInventory) 
	{
		for (UFortWorldItem*& WorldItem : PC->WorldInventory->Inventory.ItemInstances)
		{
			if (WorldItem->CanBeDropped())
				//Inventory::RemoveItem(PC, WorldItem->ItemEntry.ItemGuid, -1, true);
				// i mean i might as well just spawn pickup bc it uses a diff enum for floorloot
				Inventory::SpawnPickup(
					WorldItem->ItemEntry.ItemDefinition,
					PC->MyFortPawn->K2_GetActorLocation(),
					WorldItem->ItemEntry.Count,
					WorldItem->ItemEntry.LoadedAmmo,
					EFortPickupSourceTypeFlag::Player,
					EFortPickupSpawnSource::PlayerElimination,
					PC->MyFortPawn
				);
		}
	}

	PlayerState->DeathInfo.DeathTags = DeathReport.Tags;
	PlayerState->DeathInfo.DeathLocation = PC->MyFortPawn ? PC->MyFortPawn->K2_GetActorLocation() : FVector();
	PlayerState->DeathInfo.bDBNO = PC->MyFortPawn ? PC->MyFortPawn->bWasDBNOOnDeath : false; // no pawn dead ? left
	PlayerState->DeathInfo.DeathCause = PlayerState->KickedFromSessionReason == EFortKickReason::WasBanned ? EDeathCause::RemovedFromGame : AFortPlayerStateAthena::ToDeathCause(DeathReport.Tags, PlayerState->DeathInfo.bDBNO);
	PlayerState->DeathInfo.FinisherOrDowner = EnemyPawn;
	if (PlayerState->DeathInfo.bDBNO) // downer is clearly knocked not dead
		PlayerState->DeathInfo.Downer = EnemyPawn;
	else
		Miscs::RemoveAlivePlayers(
			(AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode,
			PC,
			(EnemyState == PC->PlayerState ? nullptr : EnemyState), 
			EnemyPawn, 
			DeathReport.KillerWeapon ? DeathReport.KillerWeapon : nullptr, 
			PC->PlayerState ? PlayerState->DeathInfo.DeathCause : EDeathCause::Unspecified,
			0
		);
	//PlayerState->DeathInfo.Distance = EnemyPawn ? EnemyPawn->GetDistanceTo(PC->MyFortPawn) : 0;9
	PlayerState->DeathInfo.Distance = PC->MyFortPawn ? 
		(PlayerState->DeathInfo.DeathCause == EDeathCause::FallDamage ? 
			Utils::Cast<AFortPlayerPawnAthena>(PC->MyFortPawn)->LastFallDistance :
			(EnemyPawn ? EnemyPawn->GetDistanceTo(PC->MyFortPawn) : 0)) : 0;
	PlayerState->DeathInfo.bInitialized = true;
	PlayerState->OnRep_DeathInfo();

	if (EnemyPawn && EnemyPawn->Controller != PC)
	{
		AFortPlayerControllerAthena* EnControll = (AFortPlayerControllerAthena*)EnemyPawn->Controller;
		if (EnControll)
		{
			// you shouldn't get knocked players WHILE this shouldnt matter
			if (!PlayerState->DeathInfo.bDBNO)
			{
				EnControll->ClientReceiveKillNotification(EnemyState, (AFortPlayerStateAthena*)PC->PlayerState);
				EnemyState->TeamKillScore++;
				EnemyState->KillScore++;
				EnemyState->ClientReportTeamKill(EnemyState->TeamKillScore);
				EnemyState->ClientReportKill((AFortPlayerStateAthena*)PC->PlayerState);
				EnemyState->OnRep_Kills();
				EnemyState->OnRep_TeamKillScore();
			}
			
			

			//EnControll->MatchReport->MatchStats.Stats
			//// need to loop this and just hope for the best ngl
	
		}
	}

	if (PlayerState->IsSquadDead())
	{
		PlayerState->Place = GameState->PlayersLeft + 1; // skuinekd
		PlayerState->OnRep_Place();
		PC->MatchReport->TeamStats.Place = GameState->PlayersLeft;
		PC->MatchReport->TeamStats.TotalPlayers = GameState->TotalPlayers;
		PC->ClientSendTeamStatsForPlayer(PC->MatchReport->TeamStats);
		PC->ClientSendMatchStatsForPlayer(PC->MatchReport->MatchStats);
		PC->ClientSendEndBattleRoyaleMatchForPlayer(true, PC->MatchReport->EndOfMatchResults);
	}

	std::cout << "PLACEMENT " << std::to_string(PlayerState->Place) << std::endl;

	// imagine if only the enemystate was 1:! with 1 player plainyg
	// guessing placement is 0 for local testing with 1 player
	if ((EnemyState && EnemyState->Place == 1) || (PlayerState && PlayerState->Place == 1))
	{
		// sigma ngl
		if (EnemyState)
		{
			std::cout << "YOU WON!!!" << std::endl;
			AFortPlayerControllerAthena* EnemyPC = (AFortPlayerControllerAthena*)EnemyState->GetOwner();

			EnemyPC->ClientNotifyTeamWon(EnemyPC->MyFortPawn, DeathReport.KillerWeapon, PlayerState->DeathInfo.DeathCause);
			EnemyPC->ClientNotifyWon(EnemyPC->MyFortPawn, DeathReport.KillerWeapon, PlayerState->DeathInfo.DeathCause);
			EnemyPC->PlayWinEffects(EnemyPC->MyFortPawn, DeathReport.KillerWeapon, PlayerState->DeathInfo.DeathCause, false);
			EnemyPC->ClientSendEndBattleRoyaleMatchForPlayer(true, EnemyPC->MatchReport->EndOfMatchResults);

			EnemyPC->ClientSendTeamStatsForPlayer(EnemyPC->MatchReport->TeamStats);
			EnemyPC->ClientSendMatchStatsForPlayer(EnemyPC->MatchReport->MatchStats);

			GameState->WinningTeam = EnemyState->TeamIndex;
			GameState->OnRep_WinningTeam();
			GameState->WinningPlayerState = EnemyState;
			GameState->OnRep_WinningPlayerState();
		}
		else
		{
			// if this is called i'm skunked i have to remove this
			std::cout << "shopuldnt calll this" << std::endl;

			// not proper skul
			if (PC->MyFortPawn)
			{
				PC->ClientNotifyWon(PC->MyFortPawn, DeathReport.KillerWeapon, PlayerState->DeathInfo.DeathCause);
				PC->PlayWinEffects(PC->MyFortPawn, DeathReport.KillerWeapon, PlayerState->DeathInfo.DeathCause, false);
			}
			PC->ClientSendTeamStatsForPlayer(PC->MatchReport->TeamStats);
			PC->ClientSendMatchStatsForPlayer(PC->MatchReport->MatchStats);

			PC->ClientSendEndBattleRoyaleMatchForPlayer(true, PC->MatchReport->EndOfMatchResults);
		}
		
		if (EnemyPawn && EnemyPawn->Controller != PC)
		{
			std::cout << "send client lost to other team!" << std::endl;
			// PC -> who you killed sooooo should be fine
			PC->ClientNotifyTeamLost();
			PC->ClientNotifyLost(EnemyPawn, EEndOfMatchReason::AllEliminated);
			PC->PlayWinEffects(EnemyPawn, DeathReport.KillerWeapon, PlayerState->DeathInfo.DeathCause, false);
		}

		std::cout << "win win win " << std::endl;

	}

	return OG(PC, DeathReport);
}
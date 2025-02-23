#pragma once

// player aka playercontroller
class Player {
private:
	Player() = default;
public:
	static inline void (*OrginalServerReadyToStartMatch)(AFortPlayerControllerAthena*);
	static void ServerReadyToStartMatch(AFortPlayerControllerAthena* PlayerController);

	static inline void (*OrginalServerLoadingScreenDropped)(AFortPlayerControllerAthena*);
	static void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PlayerController);

	static inline void (*OrginalServerAcknowledgePossession)(AFortPlayerControllerAthena* PlayerController, AFortPlayerPawnAthena* Pawn);
	static void ServerAcknowledgePossession(AFortPlayerControllerAthena* PlayerController, AFortPlayerPawnAthena* Pawn);

	static void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PlayerController, FGuid& ItemGuid);
	static void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Aircraft, const struct FRotator& ClientRotation);

	static inline void(*OrginalServerSetInAircraft)(AFortPlayerStateAthena* PlayerState, bool bNewInAircraft);
	static void ServerSetInAircraft(AFortPlayerStateAthena* PlayerState, bool bNewInAircraft);
	static void ServerReturnToMainMenu(AFortPlayerControllerAthena* PlayerController);

	static inline void(*OriginalServerHandlePickup)(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, float InFlyTime, FVector& InStartDirection, bool bPlayPickupSound);
	static void ServerHandlePickup(AFortPlayerPawnAthena* Pawn, AFortPickup* Pickup, float InFlyTime, FVector& InStartDirection, bool bPlayPickupSound);
	static inline void(*OriginalServerHandlePickupWithSwap)(AFortPlayerPawn* Pawn, AFortPickup* Pickup, const struct FGuid& Swap, float InFlyTime, const struct FVector& InStartDirection, bool bPlayPickupSound);
	static void ServerHandlePickupWithSwap(AFortPlayerPawn* Pawn, AFortPickup* Pickup, const struct FGuid& Swap, float InFlyTime, const struct FVector& InStartDirection, bool bPlayPickupSound);

	static void ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PlayerController, const struct FGuid& ItemGuid, int32 Count, bool bTrash);

	// cooked asf
	static inline void(*OriginalOnCapsuleBeginOverlap)(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult);
	static void OnCapsuleBeginOverlap(AFortPlayerPawn* Pawn, UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult);

	//UFortControllerComponent_Interaction::ServerAttemptInteract(class AActor* ReceivingActor, class UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, class UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId)
	static inline void(*OriginalServerAttemptInteract)(UFortControllerComponent_Interaction* ControllerComponent, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId);
	static void ServerAttemptInteract(UFortControllerComponent_Interaction* ControllerComponent, AActor* ReceivingActor, UPrimitiveComponent* InteractComponent, ETInteractionType InteractType, UObject* OptionalObjectData, EInteractionBeingAttempted InteractionBeingAttempted, int32 RequestId);

	static char ContainerSpawnLoot(ABuildingContainer* a1);

	static inline void (*OriginalClientOnPawnDied)(AFortPlayerControllerAthena* PlayerController, const struct FFortPlayerDeathReport& DeathReport);
	static void ClientOnPawnDied(AFortPlayerControllerAthena* PlayerController, const struct FFortPlayerDeathReport& DeathReport);
};
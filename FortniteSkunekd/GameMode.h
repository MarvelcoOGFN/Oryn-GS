#pragma once

// dont include functions here!!! if you want to include do it in private

class GameMode {
	private: 
		GameMode() = default;
public:
	// inline is used on orginal hokos/non functions we use in the class (GameMode::Func
	static inline UNetDriver* (*CreateNetDriver)(void* Engine, void* InWorld, FName NetDriverDef);

	// tip using "&" in c++ basically is calling a static value in c# you can keep calling and you get the value you changed
	static inline bool (*InitListen)(
		UNetDriver* NetDriver,
		void* InNotify,
		FURL& ListenURL, 
		bool bReuseAddressAndPort,
		FString& Error
	);

	// these unreal stuff helps
	// things like UNetDriver::InitListen is giving 1 more arg in "IDA" i cant really explain but like yeah
	// ^^ https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Source/Runtime/Engine/Private/UnrealEngine.cpp#L10766
	// ^^ the args on createnetdriver is from the function above since CreateNetDriver just returns _Local
	//^^ https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Source/Runtime/Engine/Private/World.cpp#L4874
	// ^^ https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Plugins/Online/OnlineSubsystemUtils/Source/OnlineSubsystemUtils/Private/IpNetDriver.cpp#L193

	// You find setworld inside InitHost cant be bothered to find this in unreals src rn
	static inline void (*SetWorld)(UNetDriver* a1, UWorld* a2);

	// https://github.com/EpicGames/UnrealEngine/blob/0c544b150542b59fc87bdcf64caae09f4e3bc11c/Engine/Plugins/Runtime/ReplicationGraph/Source/Private/ReplicationGraph.cpp#L1112
	static inline int32 (*ServerReplicateActors)(UReplicationDriver* ReplicationDriver/*, float DeltaSeconds*/);
	
	// Jerk y off
	static inline UClass* Starter;
	static inline UObject* JerkyLoader;


	static bool ReadyToStartMatch(AFortGameModeAthena* GameMode);
	static inline bool (*OriginalRTSM)(void*);

	//https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Source/Runtime/Engine/Private/NetworkDriver.cpp#L388
	static inline void (*OrginalTickFlush)(UNetDriver* NetDriver, float DeltaSeconds);
	static void TickFlush(UNetDriver* NetDriver, float DeltaSeconds);

	//https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/GameFramework/AGameModeBase/SpawnDefaultPawnFor?application_version=5.4
	static APawn* SpawnDefaultPawnFor(AGameMode* GameMode, AFortPlayerController* NewPlayer, AActor* SpawnLocation);

	static inline void (*OriginalOnAircraftExitedDropZone)(AFortGameModeAthena* GameMode, AFortAthenaAircraft* FortAthenaAircraft);
	static void OnAircraftExitedDropZone(AFortGameModeAthena* GameMode, AFortAthenaAircraft* FortAthenaAircraft);
};
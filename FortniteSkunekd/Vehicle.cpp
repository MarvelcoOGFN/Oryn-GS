#include "framework.h"
#include "util.h"
#include "Vehicle.h"

void Vehicle::SpawnVehicle() {

	AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
	AFortGameStateAthena* GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
	static bool bHasSpawnedSharkChopper = false;

	TArray<AActor*> Spawners;
	UGameplayStatics::GetAllActorsOfClass(UWorld::GetWorld(), AFortAthenaVehicleSpawner::StaticClass(), &Spawners);

	for (AActor* ActorSp : Spawners) {
		AFortAthenaVehicleSpawner* Spawner = Utils::Cast<AFortAthenaVehicleSpawner>(ActorSp);
		if (!Spawner) continue; // failed to cast/invaild actor

		std::string Name = Spawner->GetName();

		std::cout << "SPAWNING VEHICLE " << Name << std::endl;
		
		if (!Config::AllVehicles) {
			// add vehicles you want to spawn
			if (!Name.starts_with("Apollo_Hoagie_Spawner") && !Name.starts_with("Athena_Meatball_L_Spawner")) continue;
		}

		AActor* Vehicle = Utils::SpawnActorFromClass<AActor>(Spawner->K2_GetActorLocation(), Spawner->K2_GetActorRotation(), Spawner->GetVehicleClass());

		if (!bHasSpawnedSharkChopper && Name.starts_with("Apollo_Hoagie_Spawner")) {
			bHasSpawnedSharkChopper = true;

			AActor* Shark_Chopper = Utils::SpawnActorFromClass<AActor>(FVector(113665, -91120, -3065), {}, Spawner->GetVehicleClass());

			Utils::HookVTableFunction(Shark_Chopper->VTable, 0xEE, ServerMove);
		}

		Utils::HookVTableFunction(Vehicle->VTable, 0xEE, ServerMove);
	}

	// supposed to free this
	Spawners.Free();
}

void Vehicle::ServerMove(AFortPhysicsPawn* Pawn, FReplicatedPhysicsPawnState InState) {
	UPrimitiveComponent* Mesh = (UPrimitiveComponent*)Pawn->RootComponent;

	// i want to look at this more since clearly
	if (Mesh) {
		InState.Rotation.X -= 2.5;
		InState.Rotation.Y /= 0.3;
		InState.Rotation.Z -= -2.0;
		InState.Rotation.W /= -1.2;

		FTransform Transform{};
		Transform.Translation = InState.Translation;
		Transform.Rotation = InState.Rotation;
		Transform.Scale3D = FVector{ 1, 1, 1 };

		Mesh->K2_SetWorldTransform(Transform, false, nullptr, true);
		Mesh->bComponentToWorldUpdated = true;
		Mesh->SetPhysicsLinearVelocity(InState.LinearVelocity, 0, FName());
		Mesh->SetPhysicsAngularVelocity(InState.AngularVelocity, 0, FName());
	}
}
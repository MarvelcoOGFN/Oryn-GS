#pragma once

class Vehicle {
private:
	Vehicle() = default;
public:
	static void SpawnVehicle();
	static void ServerMove(AFortPhysicsPawn* Pawn, FReplicatedPhysicsPawnState InState);
};
#pragma once


class FloorLoot {
private:
	FloorLoot() = default;
public:
	static FFortLootTierData* FindRandomTierGroup(std::vector<FFortLootTierData*> FloorArray);
	static FFortLootPackageData* FindRandomPackageGroup(std::vector<FFortLootPackageData*> FloorArray);
	static void SearchPackages(FFortLootPackageData* PackageData, FString PackageString, AActor* ggs);
	static void LootTier(FName SearchLootTierGroup, FVector Location);
};
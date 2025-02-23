#pragma once
// if theres include errors during build include framework above this
// only reason its "INLINE" as its not a class and utils may be included anywhere in the project

// eh doesnt really matter how i do this
// https://github.com/EpicGames/UnrealEngine/blob/0c544b150542b59fc87bdcf64caae09f4e3bc11c/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectGlobals.h#L552
// its really just tghius
//https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Source/Runtime/CoreUObject/Private/UObject/UObjectGlobals.cpp#L979
//just find a string in that function and boom
static void* (*OrginalStaticLoadObject)(UClass* Class, UObject* InOuter, const TCHAR* Name, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation, void*) = decltype(OrginalStaticLoadObject)(InSDKUtils::GetImageBase() + 0x2E1CD60);

//https://github.com/EpicGames/UnrealEngine/blob/4.20/Engine/Source/Runtime/CoreUObject/Private/UObject/UObjectGlobals.cpp#L299
// some in here any string works
static void* (*OrginalStaticFindObject)(UClass*, UObject* Package, const wchar_t* OrigInName, bool ExactClass) = decltype(OrginalStaticFindObject)(InSDKUtils::GetImageBase() + 0x2E1C4B0);
static char (*ValidationKick)(AGameSession* a1, AController* a2, __int64* a3, __int64* a4) = decltype(ValidationKick)(InSDKUtils::GetImageBase() + 0x20DFE60);
//AFortGameSession ~ ^^ isnt a pointer for last two but doesnt matter
//AFortGameSessionDedicated
//AFortGameSessionDedicatedAthena
namespace Utils {
	inline void InitConsole() {
		AllocConsole();
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		SetConsoleTitleA("EonGS | FORNITESKUNEKD 12.41");
	}

	// 
	inline void HookVTableFunction(void** vtable, int Index, PVOID Hook, void** OrginalHook = nullptr) {

		if (!Hook) return;

		if (!vtable || !vtable[Index]) {
			std::cout << "Couldn't find vtable or index in vtable!" << std::endl;
			return;
		}

		if (OrginalHook) {
			*OrginalHook = vtable[Index];
		}


		DWORD oldProtect;
		if (VirtualProtect(&vtable[Index], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect)) {
			vtable[Index] = Hook;
			VirtualProtect(&vtable[Index], sizeof(void*), oldProtect, &oldProtect);

		}
		else {
			std::cout << "FAILED TO VIRUAL PROTECT " << std::endl;
		}

	}

	// legit makes the hook null
	inline void NullifyFunction(LPVOID target) {
		MH_CreateHook(target, reinterpret_cast<LPVOID>(+[]() -> __int64 {
			return reinterpret_cast<__int64>(nullptr);
		}), nullptr);
	}


	static FQuat RotatorToQuat(FRotator Rotation) {
		FQuat Quat;
		const float DEG_TO_RAD = 3.14159f / 180.0f;
		const float DIVIDE_BY_2 = DEG_TO_RAD / 2.0f;

		float SP = sin(Rotation.Pitch * DIVIDE_BY_2);
		float CP = cos(Rotation.Pitch * DIVIDE_BY_2);
		float SY = sin(Rotation.Yaw * DIVIDE_BY_2);
		float CY = cos(Rotation.Yaw * DIVIDE_BY_2);
		float SR = sin(Rotation.Roll * DIVIDE_BY_2);
		float CR = cos(Rotation.Roll * DIVIDE_BY_2);

		Quat.X = CR * SP * SY - SR * CP * CY;
		Quat.Y = -CR * SP * CY - SR * CP * SY;
		Quat.Z = CR * CP * SY - SR * SP * CY;
		Quat.W = CR * CP * CY + SR * SP * SY;

		return Quat;
	}


	template<typename T>
	static inline T* SpawnActor(FVector Loc = { 0,0,0 }, FRotator Rot = { 0,0,0 }, AActor* Owner = nullptr)
	{
		FTransform Transform{};
		Transform.Scale3D = { 1,1,1 };
		Transform.Translation = Loc;
		Transform.Rotation = RotatorToQuat(Rot);

		AActor* NewActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), T::StaticClass(), Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner);
		return (T*)UGameplayStatics::FinishSpawningActor(NewActor, Transform);
	}

	// 
	template<typename T>
	static inline T* SpawnActorFromClass(FVector Loc = { 0,0,0 }, FRotator Rot = { 0,0,0 }, UClass* Class = nullptr, AActor* Owner = nullptr)
	{
		FTransform Transform{};
		Transform.Scale3D = { 1,1,1 };
		Transform.Translation = Loc;
		Transform.Rotation = RotatorToQuat(Rot);

		AActor* NewActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), Class, Transform, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn, Owner);
		return (T*)UGameplayStatics::FinishSpawningActor(NewActor, Transform);
	}

	template <typename T>
	static inline T* StaticFindObject(std::wstring ObjectName)
	{
		//std::wcout << ObjectName << std::endl;

		return (T*)OrginalStaticFindObject(T::StaticClass(), nullptr, ObjectName.c_str(), false);
	}

	// with load object we check if we can find the object first
	template<typename T = UObject>
	static inline T* StaticLoadObject(const std::string& Name)
	{
		auto ConvName = std::wstring(Name.begin(), Name.end());

		T* Object = StaticFindObject<T>(ConvName);

		if (!Object)
		{
			Object = (T*)OrginalStaticLoadObject(T::StaticClass(), nullptr, ConvName.c_str(), nullptr, 0, nullptr, false, nullptr);

			if (!Object) {
				std::cout << "Failed to find " + Name << std::endl;
			}
		}

		return Object;
	}

	template <typename T, typename U>
	static inline T* Cast(U* Object)
	{
		return (Object && Object->IsA(T::StaticClass())) ? static_cast<T*>(Object) : nullptr;
	}

	inline void ShowFoundation(ABuildingFoundation* BuildingFoundation) {
		if (!BuildingFoundation)
			return;

		BuildingFoundation->bServerStreamedInLevel = true;
		BuildingFoundation->DynamicFoundationType = EDynamicFoundationType::Static;
		BuildingFoundation->OnRep_ServerStreamedInLevel();

		BuildingFoundation->FoundationEnabledState = EDynamicFoundationEnabledState::Enabled;
		BuildingFoundation->DynamicFoundationRepData.EnabledState = EDynamicFoundationEnabledState::Enabled;
		BuildingFoundation->DynamicFoundationTransform = BuildingFoundation->GetTransform();
		BuildingFoundation->OnRep_DynamicFoundationRepData();
	}
}
// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "util.h"
// i will just redo the gs with my style with comments on functions and more yap? please dont skunk my code even more

#include "Abilites.h"
#include "GameMode.h"
#include "Player.h"
#include "Miscs.h"

#include "Gui.h"

// need to move this to diff files
void (*OrginalDispatchRequest)(UMcpProfileGroup& a1, _int64* a2, int a3);
void DispatchRequest(UMcpProfileGroup& a1, _int64* a2, int a3) {
    return OrginalDispatchRequest(a1, a2, 3); // no mcp use a3
}

int ReturnTrue() {
    return 1;
}

int ReturnFalse() {
    return 0;
}

// 1 is dedicated server its a enum in the unreal src
__int64 AActorGetNetMode(AActor* a1)
{
    return 1;
}

__int64 WorldGetNetMode(UWorld* a1)
{
    return 1;
}

//needed for player count (usaully people handle it but since we are only doing solo's now no need just return (we return 3 so spectors dont count)
__int64 PickTeam(__int64 a1, unsigned __int8 a2, __int64 a3)
{
    return 3;
}


void (*ProcessEventOG)(void*, void*, void*);
void ProcessEventHook(UObject* Object, UFunction* Function, void* Params)
{
    //std::cout << Object->GetName() << std::endl;
    //std::cout << Function->GetName() << std::endl;
    return ProcessEventOG(Object, Function, Params);
}

float GetMaxTickRate()
{
    return 30.f;
}


// 0 is shouldnt 1 is should, ONLY if we want to kick the player it should kick
//0x4155600
char ShouldKickFromSession(AFortGameSessionDedicatedAthena* a1, AFortPlayerControllerAthena* a2, AFortGameModeAthena* a3)
{
   
    std::cout << std::to_string((uint8)((AFortPlayerStateAthena*)a2->PlayerState)->KickedFromSessionReason) << std::endl;
    if (((AFortPlayerStateAthena*)a2->PlayerState)->KickedFromSessionReason == EFortKickReason::WasBanned)
    {
        std::cout << "FORCED KICKED" << std::endl;
        a2->ClientReturnToMainMenu(L"stop cheating sir");
        return 1;
    }
       
 /*   std::cout << ((UObject*)a1)->GetName() << std::endl;
    std::cout << ((UObject*)a2)->GetName() << std::endl;
    std::cout << ((UObject*)a3)->GetName() << std::endl;*/
    return 0; // required for startup
}


// marvelco like it or hate it we are using class's
DWORD WINAPI Main(LPVOID)
{
    Utils::InitConsole(); // creates windows terminal
    MH_Initialize();

    // Creates GUI disable during prod!
    CreateThread(0, 0, Gui::ImGuiThread, 0, 0, 0);

    // Hooking

    // ~ this is for me only from ida (i dont rebase bc that broke stuf for me) 
    //7FF772840000

    //TODO null functions like things for "change gamesessions"
    Utils::NullifyFunction(LPVOID(InSDKUtils::GetImageBase() + 0x1E23840)); // gamesession id change crash thing

    // any index's on vtables you have to divide by 8

    GameMode::CreateNetDriver = decltype(GameMode::CreateNetDriver)(InSDKUtils::GetImageBase() + 0x4573990);
    GameMode::InitListen = decltype(GameMode::InitListen)(LPVOID(InSDKUtils::GetImageBase() + 0xD44C40));
    GameMode::SetWorld = decltype(GameMode::SetWorld)(InSDKUtils::GetImageBase() + 0x42C2B20);
    Abilities::InternalTryActivateAbility = decltype(Abilities::InternalTryActivateAbility)(InSDKUtils::GetImageBase() + 0x6B33F0);
    Abilities::GiveAbilityAndActivateOnce = decltype(Abilities::GiveAbilityAndActivateOnce)(InSDKUtils::GetImageBase() + 0x6B19E0);
    Abilities::CombineStruct = decltype(Abilities::CombineStruct)(InSDKUtils::GetImageBase() + 0x6D6DD0);
    Miscs::RemoveAlivePlayers = decltype(Miscs::RemoveAlivePlayers)(InSDKUtils::GetImageBase() + 0x18ECBB0);

    //7FF7738CD740
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x108D740), DispatchRequest, (PVOID*)&OrginalDispatchRequest);
    //MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x1EAA500), ReturnTrue, nullptr); // nomcp ~ enable if no mcp

    //MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x20DFE60), ReturnFalse, nullptr); //validation
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x4155600), ShouldKickFromSession, nullptr);

    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x2D95E00), ReturnTrue, nullptr); // collect garbage
    // 
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + Offsets::ProcessEvent), ProcessEventHook, (PVOID*)&ProcessEventOG);

    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x18E6B20), PickTeam, nullptr); //return

    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x22A08C0), Player::OnCapsuleBeginOverlap, (PVOID*)&Player::OriginalOnCapsuleBeginOverlap);
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x1B46D00), Player::ContainerSpawnLoot, nullptr);
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x260C490), Miscs::OnReload, (PVOID*)&Miscs::OriginalOnReload);
    
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x2683F80), Miscs::OnDamageServer, (PVOID*)&Miscs::OriginalOnDamageServer);
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x29B5C80), Player::ClientOnPawnDied, (PVOID*)&Player::OriginalClientOnPawnDied);
    
    Utils::HookVTableFunction(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x20D, Player::ServerExecuteInventoryItem, nullptr);
    Utils::HookVTableFunction(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x10D, Player::ServerAcknowledgePossession, (PVOID*)&Player::OrginalServerAcknowledgePossession);
    Utils::HookVTableFunction(UFortControllerComponent_Aircraft::GetDefaultObj()->VTable, 0x89, Player::ServerAttemptAircraftJump, nullptr);
    Utils::HookVTableFunction(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x269, Player::ServerReadyToStartMatch, (PVOID*)&Player::OrginalServerReadyToStartMatch);
    Utils::HookVTableFunction(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x26B, Player::ServerLoadingScreenDropped, (PVOID*)&Player::OrginalServerLoadingScreenDropped);
    Utils::HookVTableFunction(AFortPlayerStateAthena::GetDefaultObj()->VTable, 0xFF, Player::ServerSetInAircraft, (PVOID*)&Player::OrginalServerSetInAircraft);
    Utils::HookVTableFunction(AFortPlayerPawnAthena::GetDefaultObj()->VTable, 0x1EA, Player::ServerHandlePickup, (PVOID*)&Player::OriginalServerHandlePickup);
    Utils::HookVTableFunction(AFortPlayerPawnAthena::GetDefaultObj()->VTable, 0x1E6, Player::ServerHandlePickupWithSwap, (PVOID*)&Player::OriginalServerHandlePickupWithSwap);
    Utils::HookVTableFunction(AFortPlayerPawnAthena::GetDefaultObj()->VTable, 0x1E8, Player::ServerHandlePickupWithSwap, (PVOID*)&Player::OriginalServerHandlePickupWithSwap); //ServerHandlePickupWithRequestedSwap
    Utils::HookVTableFunction(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x265, Player::ServerReturnToMainMenu);
    Utils::HookVTableFunction(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x21D, Player::ServerAttemptInventoryDrop);
    Utils::HookVTableFunction(UFortControllerComponent_Interaction::GetDefaultObj()->VTable, 0x8B, Player::ServerAttemptInteract, (PVOID*)&Player::OriginalServerAttemptInteract);
    //Utils::HookVTableFunction(AFortPlayerControllerAthena::GetDefaultObj()->VTable, 0x426, Player::ClientOnPawnDied, (PVOID*)&Player::OriginalClientOnPawnDied);
    //21D
    //0x1328
    //F50
    Utils::HookVTableFunction(AFortPlayerPawnAthena::GetDefaultObj()->VTable, 0x119, Miscs::NetMulticast_Athena_BatchedDamageCues, (void**)&Miscs::OldrNetMulticast_Athena_BatchedDamageCues);

    Utils::HookVTableFunction(UFortAbilitySystemComponentAthena::GetDefaultObj()->VTable, 0xFA, Abilities::InternalServerTryActivateAbility);
    Utils::HookVTableFunction(UFortAbilitySystemComponent::GetDefaultObj()->VTable, 0xFA, Abilities::InternalServerTryActivateAbility);
    Utils::HookVTableFunction(UAbilitySystemComponent::GetDefaultObj()->VTable, 0xFA, Abilities::InternalServerTryActivateAbility);

    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x3EB6780), AActorGetNetMode, nullptr);
    MH_CreateHook(LPVOID(InSDKUtils::GetImageBase() + 0x45C9D90), WorldGetNetMode, nullptr);
    MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x4576310), GetMaxTickRate, nullptr);

    MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x4640A30), GameMode::ReadyToStartMatch, (void**)&GameMode::OriginalRTSM);
    MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x42C3ED0), GameMode::TickFlush, (void**)&GameMode::OrginalTickFlush);
    MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x18F6250), GameMode::SpawnDefaultPawnFor, nullptr);
    MH_CreateHook((LPVOID)(InSDKUtils::GetImageBase() + 0x18E07D0), GameMode::OnAircraftExitedDropZone, (void**)&GameMode::OriginalOnAircraftExitedDropZone);
    //18E07D0 ~ 0x35A ~ 6B
    MH_EnableHook(MH_ALL_HOOKS); // enables all hooks

    //All Windows Closed
    *(bool*)(InSDKUtils::GetImageBase() + 0x804B659) = false; //GIsClient
    //STAT_UpdateLevelStreaming
    *(bool*)(InSDKUtils::GetImageBase() + 0x804B65A) = true; //GIsServer
    
    //void __fastcall sub_7FF77412CBB0(__int64 a1, __int64 a2, int a3, __int64 a4, __int64 a5, char a6, char a7)

    UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);
    UWorld::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0); // we need to remove the local player!!!

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Main, 0, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


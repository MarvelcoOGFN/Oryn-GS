#pragma once

#include <windows.h>
#include <cstdio>


// i dont recommend including things in framework and just do it in the files
// in here should be like minhook, sdk?!?! maybe memcury but i dont see a point with that
// and includes like windows, iostream and more
// addinbg /bigobj fixes build errors on debug since its like a large project but its just useful for debugging

// if you want "GUI" it slows down the gameserver on the vps since re-renders the whole ui on each frame and normally it uses like 200 frames so thats alot per second

// you dont need to use pragma for the minhook lib as we already included it in the project
#include "minhook/minhook.h"

// we included the sdk classes to the project to fix build errors
#include "SDK/SDK.hpp"
using namespace SDK;


#include "Config.h"
// Oryn Config, Some will be changed by default by the playlist
//namespace Config {
//	static inline bool bWorldisReady = false; // leave false it auto changes ~ not actually proper but eh
//	static inline bool AllVehicles = false;
//	static inline std::string PlaylistID = "Playlist_DefaultSolo";
//}
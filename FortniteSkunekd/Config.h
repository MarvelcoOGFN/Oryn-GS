#pragma once
#include <string>

class Config {
private:
	Config() = default;
public:
	// change the values in .cpp
	static bool bWorldisReady; // leave false it auto changes ~ not actually proper but eh
	static bool AllVehicles;
	static std::string PlaylistID;
	static bool Event;
};
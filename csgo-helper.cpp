#define CURL_STATICLIB
#include "simple-radar.h"
#include "pytelnet-client.h"

bool compareFunction(std::string a, std::string b) { return a < b; }

struct Commands {
	static int getSimpleRadar(CURL* curl, fs::path csgoPath, bool preferences[4]) { return installSimpleRadar(curl, csgoPath, preferences); }
	static void help() {
		std::vector<std::string> commands = {
			"radar - Install Simple Radar in your CS:GO directory",
			"help - Show the menu that you are currently looking at",
			"dumpconfig - Dump your entire cfg folder and your personal config into a .zip file",
			"loadconfig - Load cfg folder and personal config from a .zip file",
			"knifecfg - Download configs for getting knives (only offline w/ bots, or if\n           server has sv_cheats set to 1. Skins are impossible to get, only vanilla)",
			"gmcfg - Download configs for interesting gamemodes (bhop, surf, hns, etc..).\n        All scripts must be run by server host, otherwise would not work!",
			"telnet - Install a telnet client written on python"
		};
		std::cout << "List of possible commands: " << std::endl;
		std::sort(commands.begin(), commands.end(), compareFunction);
		for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); ++it)
			std::cout << *it << std::endl;
	}
	static void dumpConfig(fs::path csgoPath) { std::cout << "Not implemented." << std::endl; } // TODO note: ignore file moddefaults.txt
	static void loadConfig(fs::path csgoPath) { std::cout << "Not implemented." << std::endl; } // TODO
	static void knifeCfg(fs::path csgoPath) { std::cout << "Not implemented." << std::endl; } // TODO
	static void gamemodesCfg(fs::path csgoPath) { std::cout << "Not implemented." << std::endl; } // TODO
	static int telnetInstall(CURL* curl, fs::path installLoc) { return installTelnetClient(curl, installLoc / "csgo-telnet"); }
};

int main(int argc, const char* argv[]) {
	CURL* curl = curl_easy_init();
	fs::path csgoPath;
	if (argc > 1) csgoPath = argv[1];
	if (!isCsgoDir(&csgoPath)) {
		try {
			csgoPath = getGamePath();
		}
		catch (std::exception e) {
			if (argc <= 1) std::cout << "CS:GO Path was not found. Please specify: ";
			else std::cout << "Specified as an argument path is invalid. Please enter your CS:GO path: ";
			std::string _a;
			getline(std::cin, _a);
			csgoPath = _a;
			if (!isCsgoDir(&csgoPath)) {
				do {
					std::cout << "Invalid path. Please specify path to CS:GO root folder: ";
					getline(std::cin, _a);
					csgoPath = _a;
				} while (!isCsgoDir(&csgoPath));
			}
		}
	}
	std::cout << "Welcome to CS:GO Helper!" << std::endl;
	std::cout << "Type the command you want to use. You can view full list of commands by typing 'help'" << std::endl;
	std::string _inp;
	while (true) {
		std::cout << ">";
		getline(std::cin, _inp);
		if (_inp == "help") Commands::help();
		else if (_inp == "radar") {
			auto _prefs = getUserPreferences();
			bool prefs[4] = { _prefs[0], _prefs[1], _prefs[2], _prefs[3] };
			Commands::getSimpleRadar(curl, csgoPath, prefs);
		}
		else if (_inp == "exit") exit(0);
		else if (_inp == "dumpconfig") Commands::dumpConfig(csgoPath);
		else if (_inp == "loadconfig") Commands::loadConfig(csgoPath);
		else if (_inp == "gmcfg") Commands::gamemodesCfg(csgoPath);
		else if (_inp == "knifecfg") Commands::knifeCfg(csgoPath);
		else if (_inp == "telnet") {
			std::cout << std::endl << "Enter path where you want to install the telnet client: ";
			std::string _b;
			getline(std::cin, _b);
			if (!fs::exists(_b)) {
				do {
					std::cout << "Non-existent path. Please enter a valid folder: ";
				} while (!fs::exists(_b));
			}
			Commands::telnetInstall(curl, _b);
		}
		else std::cout << "Invalid command. Type 'help' for possible options." << std::endl;
	}
}
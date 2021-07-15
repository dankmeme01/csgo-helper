#define CURL_STATICLIB
#include "simple-radar.h"
#include "pytelnet-client.h"
#include "filedata.h"

bool compareFunction(std::string a, std::string b) { return a < b; }

struct Commands {
	static int getSimpleRadar(CURL* curl, fs::path csgoPath, bool preferences[4]) { return installSimpleRadar(curl, csgoPath, preferences); }
	static void help() {
		std::vector<std::string> commands = {
			"radar - Install Simple Radar in your CS:GO directory",
			"help - Show the menu that you are currently looking at",
			"dumpconfig - Dump your entire cfg folder and your personal config into a .tar archive",
			"loadconfig - Load cfg folder and personal config from a .tar archive",
			"knifecfg - Install (offline) configs for getting knives (only offline w/ bots, or if\n           server has sv_cheats set to 1. Skins are impossible to get, only vanilla)",
			"gmcfg - Install (offline) configs for interesting gamemodes (bhop, surf, hns, etc..).\n        All scripts must be run by server host, otherwise would not work!",
			"telnet - Download and install a telnet client written on python"
		};
		std::cout << "List of possible commands: " << std::endl;
		std::sort(commands.begin(), commands.end(), compareFunction);
		for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); ++it)
			std::cout << *it << std::endl;
	}
	static int dumpConfig(fs::path csgoPath, fs::path udPath, fs::path _where) {
		int res;
		fs::path path1 = csgoPath / "csgo" / "cfg";
		res = makeZip(path1, _where/"csgo-configs.tar", false);
		if (res) return res;
		std::cout << "Several accounts were found on your computer.\nPlease select which one to use for dumping config:" << std::endl << std::endl;
		auto players = getUsersLogged(udPath);
		int ind = 1;
		std::map<int, fs::path> map;
		for (const auto& [k, v] : players) {
			int current = ind++;
			std::cout << current << ". " << v << std::endl;
			map.insert(std::pair<int, fs::path>(current, k));
		}
		std::cout << ">";
		std::string b;
		getline(std::cin, b);
		if (map.size() > std::stoul(b.c_str()) || std::stoul(b.c_str()) < 1) {
			do {
				std::cout << "Invalid index." << std::endl << ">";
				getline(std::cin, b);
			} while (map.size() > std::stoul(b.c_str()) || std::stoul(b.c_str()) < 1);
		}
		fs::path pathToDump = map.at(std::stoul(b.c_str()));
		res = makeZip(pathToDump, _where / "csgo-configs.tar", true);
		return res;
	}
	static int loadConfig(fs::path csgoPath, fs::path udPath, fs::path _from, std::vector<std::string> *arguments) {
		bool force = \
			(std::find(arguments->begin(), arguments->end(), "-f") != arguments->end() || std::find(arguments->begin(), arguments->end(), "--force") != arguments->end());
		int res;
		fs::path tempTarget = fs::temp_directory_path() / "temp-csgohelper";
		if (fs::exists(tempTarget)) {
			try {
				fs::remove_all(tempTarget);
				res = _mkdir(tempTarget.string().c_str());
				if (res) { return 1; }
			}
			catch (std::exception e) {
				std::cerr << e.what() << std::endl;
				res = removeAllInDir(tempTarget);
				if (res) { return 1; }
			}
		}
		else {
			res = _mkdir(tempTarget.string().c_str());
			if (res) { return 1; }
		}

		res = extractZip(_from, tempTarget);
		if (res) return 3;
		if (!fs::exists(tempTarget / "local") || !fs::exists(tempTarget / "cfg")) { return 4; }

		try {
			fs::copy(tempTarget / "cfg", csgoPath / "csgo" / "cfg", fs::copy_options::recursive | (force ? fs::copy_options::overwrite_existing : fs::copy_options::skip_existing));
		} catch (fs::filesystem_error& e) { std::cerr << e.code() << e.what() << std::endl; return 2; }

		std::cout << "Several accounts were found on your computer.\nPlease select which one to use for writing config:" << std::endl << std::endl;
		auto players = getUsersLogged(udPath);
		int ind = 1;
		std::map<int, fs::path> map;
		for (const auto& [k, v] : players) {
			int current = ind++;
			std::cout << current << ". " << v << std::endl;
			map.insert(std::pair<int, fs::path>(current, k));
		}
		std::cout << ">";
		std::string b;
		getline(std::cin, b);
		if (map.size() > std::stoul(b.c_str()) || std::stoul(b.c_str()) < 1) {
			do {
				std::cout << "Invalid index." << std::endl << ">";
				getline(std::cin, b);
			} while (map.size() > std::stoul(b.c_str()) || std::stoul(b.c_str()) < 1);
		}
		fs::path pathToDump = map.at(std::stoul(b.c_str()));
		
		try {
			fs::copy(tempTarget / "local", pathToDump, fs::copy_options::recursive | (force ? fs::copy_options::overwrite_existing : fs::copy_options::skip_existing));
		} catch (fs::filesystem_error& e) { std::cerr << e.code() << ": " << e.what() << std::endl; return 2; }

		return 0;
	}
	static void knifeCfg(fs::path csgoPath, std::vector<std::string>* arguments) {
		if (std::find(arguments->begin(), arguments->end(), "-f") != arguments->end() || std::find(arguments->begin(), arguments->end(), "--force") != arguments->end()) {
			std::cout << "Creating a knives folder in your csgo/cfg folder..";
			if (fs::exists(csgoPath / "csgo" / "cfg" / "knives")) {
				std::cout << "WARN" << std::endl;
				std::cout << "Folder 'knives' already exists in your cfg folder. Program will overwrite files in it since the --force option is used." << std::endl;
			}
			else {
				auto res = _mkdir((csgoPath / "csgo" / "cfg" / "knives").string().c_str());
				if (res) {
					std::cerr << "Failed to create a folder in your cfg folder.\nPlease try running as administrator!" << std::endl;
					return;
				}
				std::cout << "OK" << std::endl;
			}
		}
		else {
			std::cout << "Creating a 'knives' folder in your csgo/cfg folder..";
			if (fs::exists(csgoPath / "csgo" / "cfg" / "knives")) {
				std::cout << "WARN" << std::endl;
				std::cerr << "Folder 'knives' already exists in your cfg folder. Please delete it manually at " << fs::absolute(csgoPath / "csgo" / "cfg" / "knives") << std::endl << \
					"You could also specify option -f fo ignore this and forcefully overwrite all the files in this folder." << std::endl;
				return;
			}
			else {
				auto res = _mkdir((csgoPath / "csgo" / "cfg" / "knives").string().c_str());
				if (res) {
					std::cerr << "[Errno " << res << "] Failed to create folder 'knives' in your cfg folder. Please try running as administrator!" << std::endl;
					return;
				}
				std::cout << "OK" << std::endl;
			}
		}
		std::cout << "Writing files..";
		try {
			for (auto const& [key, val] : filedataKnives) {
				std::ofstream fp;
				fp.open(csgoPath / "csgo" / "cfg" / "knives" / key, std::ios::out | std::ios::trunc);
				fp << val;
				fp.close();
			}
			std::cout << "OK" << std::endl;
		}
		catch (std::exception& e) {
			std::cout << "NOT OK" << std::endl;
			std::cerr << "Failed to write files: " << e.what() << std::endl;
			return;
		}
		std::cout << std::endl << "Successfully finished installing /cfg/knives/ folder for your CS:GO installation" << std::endl \
			<< "To try it out in game, type exec knives/ and it will suggest options, or go to the /knives/ folder in your cfg folder" << std::endl << \
			"Note: to be able to drop and pick them up, run exec knives/enable or ask your host if you are not one." << std::endl;
	}
	static void gamemodesCfg(fs::path csgoPath, std::vector<std::string> *arguments) {
		if (std::find(arguments->begin(), arguments->end(), "-f") != arguments->end() || std::find(arguments->begin(), arguments->end(), "--force") != arguments->end()) {
			std::cout << "Creating a gamemodes folder in your csgo/cfg folder..";
			if (fs::exists(csgoPath / "csgo" / "cfg" / "gamemodes")) {
				std::cout << "WARN" << std::endl;
				std::cout << "Folder 'gamemodes' already exists in your cfg folder. Program will overwrite files in it since the --force option is used." << std::endl;
			}
			else {
				auto res = _mkdir((csgoPath / "csgo" / "cfg" / "gamemodes").string().c_str());
				if (res) {
					std::cerr << "Failed to create a folder in your cfg folder.\nPlease try running as administrator!" << std::endl;
					return;
				}
				std::cout << "OK" << std::endl;
			}
		}
		else {
			std::cout << "Creating a gamemodes folder in your csgo/cfg folder..";
			if (fs::exists(csgoPath / "csgo" / "cfg" / "gamemodes")) {
				std::cout << "WARN" << std::endl;
				std::cerr << "Folder 'gamemodes' already exists in your cfg folder. Please delete it manually at " << fs::absolute(csgoPath / "csgo" / "cfg" / "gamemodes") << std::endl << \
					"You could also specify option -f fo ignore this and forcefully overwrite all the files in this folder." << std::endl;
				return;
			}
			else {
				auto res = _mkdir((csgoPath / "csgo" / "cfg" / "gamemodes").string().c_str());
				if (res) {
					std::cerr << "[Errno " << res << "] Failed to create folder 'gamemodes' in your cfg folder. Please try running as administrator!" << std::endl;
					return;
				}
				std::cout << "OK" << std::endl;
			}
		}
		std::cout << "Writing files..";
		try {
			for (auto const& [key, val] : filedataGamemodes) {
				std::ofstream fp;
				fp.open(csgoPath / "csgo" / "cfg" / "gamemodes" / key, std::ios::out | std::ios::trunc);
				fp << val;
				fp.close();
			}
			std::cout << "OK" << std::endl;
		} catch (std::exception& e) {
			std::cout << "NOT OK" << std::endl;
			std::cerr << "Failed to write files: " << e.what() << std::endl;
			return;
		}
		std::cout << std::endl << "Successfully finished installing /cfg/gamemodes/ folder for your CS:GO installation" << std::endl \
			<< "To try it out in game, type exec gamemodes/ and it will suggest options, or go to the /gamemodes/ folder in your cfg folder" << std::endl \
			<< "Note: Please do not rename the gamemodes folder, otherwise it might break some scripts, but should not be very problematic." << std::endl;
	}
	static int telnetInstall(CURL* curl, fs::path installLoc) { return installTelnetClient(curl, installLoc / "csgo-telnet"); }
};

int main(int argc, const char* argv[]) {
	CURL* curl = curl_easy_init();
	fs::path csgoPath;
	fs::path udPath;
	if (argc > 1) csgoPath = argv[1];
	if (argc > 2) udPath = argv[2];
	udPath /= "userdata";
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
	if (!isUDDir(&udPath)) {
		try {
			udPath = getUserDataPath();
		}
		catch (std::exception e) {
			if (argc <= 2) std::cout << "Steam path was not found. Please specify: ";
			else std::cout << "Specified as an argument path is invalid. Please enter your Steam path: ";
			std::string _a;
			getline(std::cin, _a);
			csgoPath = _a;
			if (!isUDDir(&udPath)) {
				do {
					std::cout << "Invalid path. Please specify path to Steam root folder: ";
					getline(std::cin, _a);
					csgoPath = _a;
				} while (!isUDDir(&udPath));
			}
		}
	}
	std::cout << "Welcome to CS:GO Helper!" << std::endl;
	std::cout << "Type the command you want to use. You can view full list of commands by typing 'help'" << std::endl;
	std::string _inp;
	std::string cmd;
	std::vector<std::string> args;
	while (true) {
		args = {};
		std::cout << ">";
		getline(std::cin, _inp);
		_inp = trim(_inp);
		if (_inp.size() == 0) continue;

		tokenize(_inp, " ", args);

		cmd = args.at(0);
		args.erase(args.begin());

		if (cmd == "help") Commands::help();
		else if (cmd == "radar") {
			std::cout << "By proceeding, you agree to install the addition called \"Simple Radar\" to your game." << \
				"If you do not know what it is, then you can find out by going to their official website: " << \
				"https://readtldr.gg/simpleradar" << std::endl << std::endl;
			std::cout << "Type 'yes' to proceed and get asked about how you want your radar to look, or type anything else to cancel the installation: ";
			std::string inp_;
			getline(std::cin, inp_);
			if (inp_ != "yes") {
				std::cout << "Aborting." << std::endl;
				continue;
			}
			auto _prefs = getUserPreferences();
			bool prefs[4] = { _prefs[0], _prefs[1], _prefs[2], _prefs[3] };
			Commands::getSimpleRadar(curl, csgoPath, prefs);
		}
		else if (cmd == "exit") exit(0);
		else if (cmd == "dumpconfig") {
			std::cout << std::endl << "Enter path where you want to dump your config files: ";
			std::string _b;
			getline(std::cin, _b);
			if (!fs::exists(_b) || !fs::is_directory(_b)) {
				do {
					std::cout << "Non-existent path. Please enter a valid folder: ";
					getline(std::cin, _b);
				} while (!fs::exists(_b) || !fs::is_directory(_b));
			}
			int res = Commands::dumpConfig(csgoPath, udPath, _b);
			if (res) std::cout << "Error in making an archive. You can see the error above, if it says \"Permission denied\" please try closing the game." << std::endl;
			else std::cout << "Successfully dumped your cfg folder and personal config into a csgo-configs.tar file.\nTo restore use \"loadconfig\" command, or figure out yourself how to do it." << std::endl;
		}
		else if (cmd == "loadconfig") {
			std::cout << std::endl << "Enter path to your dumped csgo-configs.tar: ";
			std::string _b;
			getline(std::cin, _b);
			if (!fs::exists(_b) || fs::path(_b).filename() != "csgo-configs.tar") {
				do {
					std::cout << "Incorrect path. Please enter a valid path to your csgo-config.tar (filename matters!): ";
					getline(std::cin, _b);
				} while (!fs::exists(_b) || fs::path(_b).filename() != "csgo-configs.tar");
			}
			int res = Commands::loadConfig(csgoPath, udPath, _b, &args);
			if (res == 3) std::cout << "There was an error in extracting files from the archive. Please check if your archive is corrupted!" << std::endl;
			else if (res == 2) std::cout << "There was an error in moving files. Try running as administrator or closing CS:GO or any file editors/explorers." << std::endl;
			else if (res == 1) std::cout << "There was an error in creating/deleting a folder. Please go to " << fs::temp_directory_path() << " and check if there is a folder named 'temp-csgohelper'. If there is one, delete it manually." << std::endl;
			else if (res == 4) std::cout << "Error: Invalid csgo-configs.tar. Archive should contain two folders: local and cfg" << std::endl;
			else if (res == 0) std::cout << "Successfully loaded your config from the archive." << std::endl << "Warning: if you did not use the --force option, the program did not overwrite any existing files!" << std::endl;
			else std::cout << "There was an unknown error in loading your config. This should never happen, and error code is " << res << std::endl;
		}
		else if (cmd == "gmcfg") Commands::gamemodesCfg(csgoPath, &args);
		else if (cmd == "knifecfg") Commands::knifeCfg(csgoPath, &args);
		else if (cmd == "telnet") {
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
#pragma once
// pytelnet-client.h - Header file for csgo-helper.cpp
// Needed for installing a python telnet client for CS:GO

#include "utils.h"
std::string URL = "https://github.com/dankmeme01/csgo-telnet/archive/refs/heads/master.zip";

void _tcCleanup() {
	std::vector<fs::path> paths = {
		fs::temp_directory_path() / "csgo-telnet.zip",
		fs::temp_directory_path() / "csgo-telnet"
	};

	for (std::vector<fs::path>::iterator it = paths.begin(); it != paths.end(); ++it) {
		if (!fs::exists(*it)) continue;
		if (fs::is_directory(*it)) {
			try { fs::remove_all(*it); }
			catch (std::exception&) {}
		}
		else {
			try { fs::remove(*it); }
			catch (std::exception&) {}
		}
	}
}

static int installTelnetClient(CURL* curl, fs::path whereInstall) {
	if (!fs::exists(whereInstall)) {
		try {
			int res = _mkdir(whereInstall.string().c_str());
			if (res) {
				std::cerr << "[Errno " << res << "] Error in creating a folder in " + whereInstall.parent_path().string() << std::endl;
				std::cerr << "Please run as administrator or install in a different folder." << std::endl;
				return 1;
			}
		} catch(std::exception e) {
			std::cerr << "Error in creating a folder: " << e.what() << std::endl;
			std::cerr << "Please run as administrator or install in a different folder." << std::endl;
			return 1;
		}
	}
	std::cout << std::endl << "Please read this before installing, so you don't get any unexpected results!" << std::endl << std::endl;
	std::cout << "You are about to install 'Python telnet client for CS:GO'. Python (from python.org) is required to use this package." << std::endl << \
		"This package is used for sending commands from python script to your in-game console." << std::endl << \
		"It can be used in basic way (creating a simple script for it) and binding a key to executing it," << std::endl << \
		"Or in a more advanced way, by creating custom .py scripts (only with python knowledge)." << std::endl << \
		"Advanced instructions on how to do both will be bundled with the package in the folder where you install it" << std::endl << std::endl;
	std::cout << "Write 'yes' if you agree to install this program, or anything else if you want to go back to csgo-utils: ";
	std::string _a;
	getline(std::cin, _a);
	if (_a != "yes") { std::cout << "Aborting." << std::endl; _tcCleanup(); return 0; }

	fs::path zipPath = fs::temp_directory_path() / "CSGO-Telnet.zip";

	std::cout << "Downloading CSGO-Telnet.zip from github..";
	CURLcode res = downloadFile(curl, URL, zipPath);
	if (res == CURLE_OK) std::cout << "OK" << std::endl;
	else {
		std::cout << "NOT OK" << std::endl;
		std::cout << "cURL Error: " << curl_easy_strerror(res) << std::endl;
		_tcCleanup();
		return (int)res;
	}

	std::cout << "Extracting the zip to the install folder..";
	int res2 = extractZip(zipPath, whereInstall);
	if (!res2) std::cout << "OK" << std::endl;
	else {
		std::cout << "NOT OK" << std::endl;
		std::cout << "Error in extracting package from .zip file. Please install tar on your computer." << std::endl;
		_tcCleanup();
		return res2;
	}

	std::cout << std::endl << "Finished installing telnet client for CS:GO.\nFor further help open the README.md in the folder where you installed it." << std::endl;
	return 0;
}
#pragma once
#include "utils.h"
std::string URL = "https://google.com"; // todo

static int installTelnetClient(fs::path whereInstall) {
	if (!fs::exists(whereInstall)) {
		try {
			int res = _mkdir(whereInstall.string().c_str());
			if (res) {
				std::cerr << "[Errno " << res << "] Error in creating a folder in " + whereInstall.parent_path().string() << std::endl;
				std::cerr << "Please run as administrator or install in a different folder." << std::endl;
				return 1;
			}
		} catch(std::exception e) {
			std::cerr << "Error in creating a folder in " + whereInstall.parent_path().string() << std::endl;
			std::cerr << "Please run as administrator or install in a different folder." << std::endl;
			return 1;
		}
	}
}
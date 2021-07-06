#pragma once
// simple-radar.h - Header file for csgo-helper.cpp
// Needed for installing Simple Radar into CS:GO

#include "utils.h"
#include <vector>
#include <iostream>
#include <ostream>
#include <array>
#ifdef WIN32
#include <direct.h>
#endif
#include <regex>
namespace fs = std::filesystem;

std::string getSimpleRadarLink(bool dark, bool callouts, bool buyzones, bool elevations) {
	std::string link = "https://files.readtldr.com/sr/";
	std::string ending = "/Simple%20Radar.zip";
	std::string opts = "Color" + (dark ? std::string("1") : std::string("0")) \
		+ "Pat" + (elevations ? std::string("1") : std::string("0")) \
		+ "Buy" + (buyzones ? std::string("1") : std::string("0")) \
		+ "Spec" + (callouts ? std::string("1") : std::string("0"));

	std::string URL = link + opts + ending;
	return URL;
}

void _srCleanup() {
	fs::path tempTarget = fs::temp_directory_path() / "Temp-SimpleRadar";
	fs::path fullpath = fs::temp_directory_path() / "SimpleRadar.zip";
	fs::path psScript = fs::temp_directory_path() / "TEMP_SCRIPT.ps1";
	if (fs::exists(tempTarget)) { try { fs::remove_all(tempTarget); } catch (std::exception&) { removeAllInDir(tempTarget); } }
	if (fs::exists(fullpath)) { try { fs::remove(fullpath); } catch (std::exception&) {} }
	if (fs::exists(psScript)) { try { fs::remove(psScript); } catch (std::exception&) {} }
}

static int installSR0() {
	int res;
	fs::path tempTarget = fs::temp_directory_path() / "Temp-SimpleRadar";
	if (fs::exists(tempTarget)) {
		try {
			fs::remove_all(tempTarget);
			res = _mkdir(tempTarget.string().c_str());
			if (res) {  return 1; }
		}
		catch (std::exception&) {
			// if we cannot remove temp folder, delete everything in it
			res = removeAllInDir(tempTarget);
			if (res) { return 1; }
		}
	}
	else {
		res = _mkdir(tempTarget.string().c_str());
		if (res) { return 1; }
	}
	return 0;
}

static CURLcode installSR1(CURL* curl, bool preferences[4]) {
	std::string url = getSimpleRadarLink(preferences[0], preferences[1], preferences[2], preferences[3]);
	fs::path fullpath = fs::temp_directory_path() / "SimpleRadar.zip";
	CURLcode result = downloadFile(curl, url, fullpath);
	return result;
}

static int installSR2(fs::path targetFolder, fs::path csgoPath) {
	try {
		int res = copyAllFrom((targetFolder / "Simple Radar" / "01 default"), csgoPath / "csgo" / "resource" / "overviews");
		if (res) return res;
		res = copyAllFrom((targetFolder / "Simple Radar" / "02 spectate"), csgoPath / "csgo" / "resource" / "overviews");
		if (res) return res;
		return 0;
	}
	catch (std::exception e) {
		return 2;
	}
}

static int installSimpleRadar(CURL* curl, fs::path csgoRootPath, bool preferences[4]) {
	int res1 = installSR0();
	if (res1) {
		std::cerr << "Unable to do actions in your Temp directory! Please try running as administrator, or reboot your computer!" << res1 << std::endl;
		_srCleanup();
		return res1;
	}

	std::cout << "Downloading simple radar..";
	CURLcode res2 = installSR1(curl, preferences);
	if (res2 == CURLE_OK) std::cout << "OK" << std::endl;
	else {
		std::cout << "NOT OK" << std::endl;
		std::cerr << "cURL error " << res2 << std::endl;
		_srCleanup();
		return res2;
	}

	std::cout << "Extracting files from .zip file to your temp folder..";
	int res3 = extractZip(fs::temp_directory_path() / "SimpleRadar.zip", fs::temp_directory_path() / "Temp-SimpleRadar");
	if (!res3) std::cout << "OK" << std::endl << "Extracted contents of SimpleRadar.zip, beginning to transfer to CS:GO folder..";
	else {
		std::cout << "NOT OK" << std::endl;
		std::cerr << "Failed extracting .zip file with Simple Radar. Try running as administrator or installing tar on your computer & adding it to PATH." << std::endl;
		_srCleanup();
		return res3;
	}

	int res4 = installSR2(fs::temp_directory_path() / "Temp-SimpleRadar", csgoRootPath);
	if (res4) {
		std::cout << "NOT OK" << std::endl;
		std::cerr << "There was an error in copying files, aborting. Please run this file as administrator!" << std::endl;
		_srCleanup();
		return res4;
	}
	std::cout << "OK" << std::endl;
	
	std::cout << "Successfully installed Simple Radar. Restart your game and you will see it in action. GLHF!" << std::endl;

	// Cleanup

	_srCleanup();
	return 0;
}

std::array<bool,4> getUserPreferences() {
	std::string opt1, opt2, opt3, opt4;
	std::cout << "Dark or light mode? (enter d or l): ";
	getline(std::cin, opt1);

	std::cout << "With or without callouts? (enter w or n): ";
	getline(std::cin, opt2);

	std::cout << "With or without buyzones? (enter w or n): ";
	getline(std::cin, opt3);

	std::cout << "With or without elevations? (enter w or n): ";
	getline(std::cin, opt4);
	std::cout << std::endl;

	bool op1, op2, op3, op4;

	op1 = parseOpt(opt1.at(0), 'd', 'l', true);
	op2 = parseOpt(opt2.at(0), 'w', 'n', false);
	op3 = parseOpt(opt3.at(0), 'w', 'n', true);
	op4 = parseOpt(opt4.at(0), 'w', 'n', true);
	std::array<bool, 4> arr = { op1,op2,op3,op4 };
	return arr;
}
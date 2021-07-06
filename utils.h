#pragma once
#include "curl/curl.h"
#include <fstream>
#include <TlHelp32.h>
#include <Psapi.h>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

#ifdef WIN32
std::string SCRIPT = "function extractzip{param([string]$zipfilename, [string] $destination)\n" \
"    if(test-path($zipfilename)){\n" \
"		$shellApplication = new-object -com shell.application\n" \
"		$zipPackage = $shellApplication.NameSpace($zipfilename)\n" \
"		$destinationFolder = $shellApplication.NameSpace($destination)\n" \
"		$destinationFolder.CopyHere($zipPackage.Items())\n" \
"        Remove-Variable shellApplication\n" \
"        Remove-Variable zipPackage\n" \
"        Remove-Variable destinationFolder}}\n";
#endif

#ifdef WIN32
fs::path getSteamProcess() {
	fs::path retVal;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (_wcsicmp(entry.szExeFile, L"steam.exe") == 0) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				TCHAR filename[MAX_PATH];
				bool res = GetModuleFileNameEx(hProcess, NULL, filename, MAX_PATH);
				if (res) retVal = fs::path(filename);
				else throw(std::exception("Steam process was not found"));
				CloseHandle(hProcess);
			}
		}
	}
	CloseHandle(snapshot);
	return retVal;
}
#endif

fs::path getGamePath() {
	fs::path csgoSteamLibPath;
#ifdef WIN32
	try { csgoSteamLibPath = getSteamProcess().parent_path() / "steamapps" / "common" / "Counter-Strike Global Offensive"; }
	catch (std::exception e) {}
#endif
	if (fs::exists(csgoSteamLibPath) == 0) {
		std::vector<std::string> possiblePaths = { "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Counter-Strike Global Offensive",
									"C:\\Program Files\\Steam\\steamapps\\common\\Counter-Strike Global Offensive",
									"E:\\Steam\\steamapps\\common\\Counter-Strike Global Offensive",
									"D:\\Steam\\steamapps\\common\\Counter-Strike Global Offensive",
									"E:\\SteamLibrary\\steamapps\\common\\Counter-Strike Global Offensive",
									"D:\\SteamLibrary\\steamapps\\common\\Counter-Strike Global Offensive",
									"F:\\Steam\\steamapps\\common\\Counter-Strike Global Offensive",
									"G:\\Steam\\steamapps\\common\\Counter-Strike Global Offensive",
									"F:\\SteamLibrary\\steamapps\\common\\Counter-Strike Global Offensive",
									"G:\\SteamLibrary\\steamapps\\common\\Counter-Strike Global Offensive",
									"~/.steam/steam/SteamApps/common/Counter-Strike Global Offensive",
									"~/.local/share/steam/steamapps/common/Counter-Strike Global Offensive",
									"/usr/bin/steam/steamapps/common/Counter-Strike Global Offensive",
									"/home/usr/steam/steamapps/common/Counter-Strike Global Offensive" };
		for (std::vector<std::string>::iterator it = possiblePaths.begin(); it != possiblePaths.end(); ++it) {
			fs::path posPath = *it;
			if (fs::exists(posPath)) { csgoSteamLibPath = posPath; break; }
		}
	}

	if (fs::exists(csgoSteamLibPath)) return csgoSteamLibPath;
	else throw(ERROR_FILE_NOT_FOUND);
}

bool parseOpt(char current, char ifTrue, char ifFalse, bool _default) {
	if (current == ifTrue) return 1;
	else if (current == ifFalse) return 0;
	else return _default;
}

#ifdef WIN32
static int extractZipPowerShell(const fs::path& filepath, const fs::path& output) {
	fs::path tempScriptLoc = fs::temp_directory_path() / "TEMP_SCRIPT.ps1";
	if (fs::exists(tempScriptLoc)) fs::remove(tempScriptLoc);
	std::ofstream File(tempScriptLoc.string().c_str());
	File << SCRIPT << "extractzip \"" << filepath.string() << "\" \"" << output.string() << "\"";
	File.close();
	std::string startline = "powershell -executionPolicy bypass -file ";
	std::string fullLine = startline + "\"" + tempScriptLoc.string() + "\"";
	int returned = system(fullLine.c_str());
	return returned;
}
#endif

static int extractZipTar(const fs::path& filepath, const fs::path& output) {
	std::string startline = "tar -xf \"";
	std::string fullLine = startline + filepath.string() + "\" -C \"" + output.string() + "\"";
	int returned = system(fullLine.c_str());
	return returned;
}

static int removeAllInDir(const fs::path& folder) {
	int removed = 0;
	int total = 0;
	bool _throw = false;
	std::exception tothrow;
	for (const auto& entry : fs::directory_iterator(folder)) {
		++total;
		try {
			if (fs::is_directory(entry)) removeAllInDir(entry);
			else fs::remove(entry);
			++removed;
		}
		catch (std::exception& e) { tothrow = e; _throw = true; }
	}
	if (_throw) throw(tothrow);
	if (removed < total) return 1;
	return 0;
}

static int copyAllFrom(const fs::path& src, const fs::path& target) {
	int copied = 0;
	int total = 0;
	std::exception tothrow;
	bool _throw = false;
	for (const auto& entry : fs::directory_iterator(src)) {
		++total;
		try {
			fs::copy(entry, target, fs::copy_options::overwrite_existing);
			++copied;
		}
		catch (std::exception& e) { tothrow = e; _throw = true; }
	}
	if (_throw) throw(tothrow);
	if (copied < total) return 1;
	return 0;
}

size_t _writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

bool isCsgoDir(fs::path* targetPath) {
	if (!fs::exists(*targetPath)) return false;
	std::vector<fs::path> neededPaths = {
		*targetPath / "csgo",
		*targetPath / "csgo" / "resource",
		*targetPath / "csgo" / "resource" / "overviews"
	};
	for (std::vector<fs::path>::iterator it = neededPaths.begin(); it != neededPaths.end(); ++it) {
		fs::path posPath = *it;
		if (!fs::exists(posPath)) { return false; }
	}
	return true;
}
#pragma once
#pragma warning(push, 0)
#include "curl/curl.h"
#pragma warning(pop)
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <vdf_parser.hpp>
#include <ostream>
namespace fs = std::filesystem;
const std::string WHITESPACE = " \n\r\t\f\v";

#ifdef WIN32
#include <direct.h>
#include <TlHelp32.h>
#include <Psapi.h>

std::string SCRIPT = "function extractzip{param([string]$zipfilename, [string] $destination)\n" \
"    if(test-path($zipfilename)){\n" \
"		$shellApplication = new-object -com shell.application\n" \
"		$zipPackage = $shellApplication.NameSpace($zipfilename)\n" \
"		$destinationFolder = $shellApplication.NameSpace($destination)\n" \
"		$destinationFolder.CopyHere($zipPackage.Items())\n" \
"        Remove-Variable shellApplication\n" \
"        Remove-Variable zipPackage\n" \
"        Remove-Variable destinationFolder}}\n";

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

static int extractZipPowerShell(const fs::path& filepath, const fs::path& output) {
	fs::path tempScriptLoc = fs::temp_directory_path() / "TEMP_SCRIPT.ps1";
	if (fs::exists(tempScriptLoc)) fs::remove(tempScriptLoc);
	std::ofstream File(tempScriptLoc.string().c_str());
	File << SCRIPT << "extractzip \"" << filepath.string() << "\" \"" << output.string() << "\"";
	File.close();
	std::string startline = "start powershell -executionPolicy bypass -file ";
	std::string fullLine = startline + "\"" + tempScriptLoc.string() + "\"";
	int returned = system(fullLine.c_str());
	return returned;
}
#endif

std::string folderize(std::string path);

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
fs::path getUserDataPath() {
	fs::path csgoSteamLibPath;
#ifdef WIN32
	try { csgoSteamLibPath = getSteamProcess().parent_path() / "userdata"; }
	catch (std::exception e) {}
#endif
	if (fs::exists(csgoSteamLibPath) == 0) {
		std::vector<std::string> possiblePaths = { "C:\\Program Files (x86)\\Steam\\steamapps\\userdata",
									"C:\\Program Files\\Steam\\steamapps\\userdata",
									"E:\\Steam\\steamapps\\userdata",
									"D:\\Steam\\steamapps\\userdata",
									"E:\\SteamLibrary\\steamapps\\userdata",
									"D:\\SteamLibrary\\steamapps\\userdata",
									"F:\\Steam\\steamapps\\userdata",
									"G:\\Steam\\steamapps\\userdata",
									"F:\\SteamLibrary\\steamapps\\userdata",
									"G:\\SteamLibrary\\steamapps\\userdata",
									"~/.steam/steam/SteamApps/userdata",
									"~/.local/share/steam/steamapps/userdata",
									"/usr/bin/steam/steamapps/userdata",
									"/home/usr/steam/steamapps/userdata" };
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

bool hasTar() {
	return !system("start tar --version");
}
static int extractZipTar(const fs::path& filepath, const fs::path& output) {
	std::string startline = "tar -xf \"";
	std::string fullLine = startline + filepath.string() + "\" -C \"" + output.string() + "\"";
	int returned = system(fullLine.c_str());
	return returned;
}
static int extractZip(const fs::path& filepath, const fs::path& output) {
	int res;
	if (hasTar()) {
		res = extractZipTar(filepath, output);
	} else {
#ifdef WIN32
		res = extractZipPowerShell(filepath, output);
#else
		res = 1
#endif
	}
	return res;
}

std::string longestPath(const std::vector<std::string>& dirs, char separator) {
	// Note: must be sorted in ascending order by length! Spent half an hour there.
	std::vector<std::string>::const_iterator vsi = dirs.begin();
	int maxCharactersCommon = vsi->length();
	std::string compareString = *vsi;
	for (vsi = dirs.begin() + 1; vsi != dirs.end(); vsi++) {
		std::pair<std::string::const_iterator, std::string::const_iterator> p =
			std::mismatch(compareString.begin(), compareString.end(), vsi->begin());
		if ((p.first - compareString.begin()) < maxCharactersCommon)
			maxCharactersCommon = p.first - compareString.begin();
	}
	std::string::size_type found = compareString.rfind(separator, maxCharactersCommon);
	return compareString.substr(0, found);
}
static int makeZip(const fs::path& folder, const fs::path& outputName, bool appending) {
	if (!hasTar()) return 1;
#ifdef WIN32
	char sep = '\\';
#else
	char sep = '/';
#endif
	std::string opt;
	if (appending) opt = "-r";
	else opt = "-c";
	std::string startline = "tar " + opt + " -f \"" + outputName.string() + "\"";
	auto pp = folder.parent_path().string();
	if(pp.back() == sep) pp.pop_back();
	std::string fullLine3 = startline + " -C \"" + folder.parent_path().string() + "\" \"" + folder.filename().string() + "\"";
	int returned = system(fullLine3.c_str());
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
static int copyAllFrom(const fs::path& src, const fs::path& target, bool overwrite) {
	int copied = 0;
	int total = 0;
	bool _throw = false;
	for (const auto& entry : fs::directory_iterator(src)) {
		if (entry.path().parent_path() != src) continue;
		++total;
		try {
			fs::copy(entry, target, (overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::skip_existing) | fs::copy_options::recursive);
			++copied;
		}
		catch (std::exception& e) { std::cout << e.what() << std::endl; _throw = true; }
	}
	if (_throw) return 1;
	if (copied < total) return 1;
	return 0;
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
bool isUDDir(fs::path* targetPath) {
	if (!fs::exists(*targetPath)) return false;
	if (targetPath->filename() != "userdata") return false;
	return true;
}

size_t _writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
CURLcode downloadFile(CURL* curl, std::string url, fs::path savepath) {
	FILE* fp;
	fopen_s(&fp, savepath.string().c_str(), "wb");
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _writeData);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	if (fp) fclose(fp);
	return res;
}

std::string ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}
std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
std::string trim(const std::string& s) {
	return rtrim(ltrim(s));
}
void tokenize(std::string& str, const char *delim, std::vector<std::string>& out) {
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
	{
		end = str.find(delim, start);
		out.push_back(str.substr(start, end - start));
	}
}

std::string folderize(std::string path) {
#ifdef WIN32
	char sep = '\\';
#else
	char sep = '/';
#endif
	if (fs::is_directory(path)) {
		if (path.back() != sep) path += sep;
	}
	return path;
}

std::map<fs::path, std::string> getUsersLogged(fs::path userdataPath) {
	std::vector<fs::path> paths;
	for (const auto& entry : fs::directory_iterator(userdataPath)) {
		fs::path csgoCfg = entry.path() / "730" / "local";
		if (!fs::exists(csgoCfg) || !fs::exists(entry.path() / "config" / "localconfig.vdf")) continue;
		paths.push_back(entry.path());
	}
	std::map<fs::path, std::string> map;
	for (const fs::path path : paths) {
		std::ifstream file(path / "config" / "localconfig.vdf");
		auto root = tyti::vdf::read(file);
		auto* friendObj = root.childs["friends"].get();
		std::string& name = friendObj->attribs["PersonaName"];
		map.insert(std::pair<fs::path, std::string>(path / "730" / "local", name));
	}
	return map;
}
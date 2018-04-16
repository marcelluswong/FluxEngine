#include "FluxEngine.h"
#include "Paths.h"


const std::string Paths::LogsFolder =			"Saved\\Logs";
const std::string Paths::ProfilingFolder =		"Saved\\Profiling";
const std::string Paths::ScreenshotFolder =		"Saved\\Screenshots";
const std::string Paths::GameIniFile =			"Saved\\Config\\Game.ini";
const std::string Paths::EngineIniFile =		"Saved\\Config\\Engine.ini";
const std::string Paths::ResourcesFolder =		".\\Resources";
const std::string Paths::PakFilesFolder =		".";

bool Paths::IsSlash(const char c)
{
	if (c == '\\')
		return true;
	return c == '/';
}

std::string Paths::GetFileName(const std::string& filePath)
{
	auto it = std::find_if(filePath.rbegin(), filePath.rend(), [](const char c)
	{
		return IsSlash(c);
	});
	if (it == filePath.rend())
		return filePath;

	return filePath.substr(it.base() - filePath.begin());
}

std::string Paths::GetFileNameWithoutExtension(const std::string& filePath)
{
	std::string fileName = GetFileName(filePath);
	size_t dotPos = fileName.find('.');
	if (dotPos == std::string::npos)
		return fileName;
	return fileName.substr(0, dotPos);
}

std::string Paths::GetFileExtenstion(const std::string& filePath)
{
	size_t dotPos = filePath.rfind('.');
	if (dotPos == std::string::npos)
		return filePath;
	return filePath.substr(dotPos + 1);
}

std::string Paths::GetDirectoryPath(const std::string& filePath)
{
	auto it = std::find_if(filePath.rbegin(), filePath.rend(), [](const char c)
	{
		return IsSlash(c);
	});
	if (it == filePath.rend())
	{
		if (filePath.rfind('.') == std::string::npos)
			return "/";
		return filePath;
	}

	return filePath.substr(0, it.base() - filePath.begin());
}

std::string Paths::Normalize(const std::string& filePath)
{
	std::string output = std::string(filePath.begin(), filePath.end());
	NormalizeInline(output);
	return output;
}

void Paths::NormalizeInline(std::string& filePath)
{
	for (char& c : filePath)
	{
		if (c == '\\')
			c = '/';
		c = (char)tolower(c);
	}
	if (filePath.find("./") == 0)
		filePath = std::string(filePath.begin() + 2, filePath.end());
}

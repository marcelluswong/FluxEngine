#pragma once
#include "Async\Mutex.h"

enum class LogType
{
	Info,
	Warning,
	Error,
};

class File;

class Console
{
public:
	Console();
	~Console();

	void FlushThreadedMessages();

	static bool LogFmodResult(FMOD_RESULT result);
	static bool LogHRESULT(const std::string &source, HRESULT hr);
	static bool LogHRESULT(char* source, HRESULT hr);
	static void Log(const std::string &message, LogType type = LogType::Info);
	static void LogWarning(const std::string &message);
	static void LogError(const std::string &message);
	static void LogFormat(LogType type, const char* format, ...);
	static void LogFormat(LogType type, const std::string& format, ...);

private:
	void InitializeConsoleWindow();
	static std::string GetTime();

	char* m_ConvertBuffer;
	const size_t m_ConvertBufferSize = 2048;

	struct QueuedMessage
	{
		QueuedMessage(const std::string& message, const LogType type) :
			Message(message), Type(type)
		{}
		std::string Message;
		LogType Type;
	};
	std::queue<QueuedMessage> m_MessageQueue;
	Mutex m_QueueMutex;

	File* m_pFileLog;
	HANDLE m_ConsoleHandle;
};

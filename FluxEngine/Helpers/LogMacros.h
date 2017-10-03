#pragma once

#define FLUX_LOG_FMOD(fmod) \
Console::LogFmodResult(fmod)

#define FLUX_LOG_HR(description, hr) \
Console::LogHRESULT(description, hr)

#define FLUX_LOG(type, description, ...) \
Console::LogFormat(LogType::type, description, __VA_ARGS__);

#ifdef _DEBUG

#define HR(command)\
{HRESULT r = command;\
stringstream stream;\
stream << typeid(*this).name() << "::" << __func__ << "()" << endl;\
stream << "Line: " << __LINE__  << endl;\
stream << "Action: " << #command ;\
Console::LogHRESULT(stream.str(), r);}

#else

#define HR(command) UNREFERENCED_PARAMETER(command);

#endif
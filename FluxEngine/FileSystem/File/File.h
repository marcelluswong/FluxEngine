#pragma once
#include "IO\IOStream.h"

class IMountPoint;

enum class FileMode
{
	Read,
	Write,
	ReadWrite,
};

enum class ContentType
{
	Text,
	Binary,
};

class File : public IOStream
{
public:
	File(const std::string& fileName) :
		m_FileName(fileName)
	{
		m_Source = fileName;
	}
	virtual ~File() {}

	virtual bool Open(const FileMode mode) = 0;
	bool virtual Flush() = 0;
	virtual bool Close() = 0;
	virtual bool IsOpen() const = 0;

	File& operator<<(const std::string& text);
	File& operator>>(std::string& text);

	std::string GetDirectoryPath() const;
	std::string GetFileName() const;

protected:
	std::string m_FileName;
};
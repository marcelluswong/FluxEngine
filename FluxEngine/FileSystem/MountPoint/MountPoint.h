#pragma once

class IFile;
class PhysicalFile;
class PakFile;

class IMountPoint
{
public:

	IMountPoint(const std::string& physicalPath, const int order = -1);

	virtual ~IMountPoint() {}

	virtual bool OnMount() = 0;
	virtual std::unique_ptr<IFile> GetFile(const std::string& filePath) = 0;
	virtual bool HasFile(const std::string& filePath) const = 0;

	const std::string& GetPhysicalPath() const { return m_PhysicalPath; }
	int GetOrder() const { return m_Order; }

protected:
	std::string m_PhysicalPath;
	int m_Order = -1;
};
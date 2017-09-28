#pragma once
#include <unordered_map>
#include "../Debugging/Profiler.h"

class Graphics;

class Loader
{
public:
	Loader(void){}
	virtual ~Loader(void){}

	virtual const type_info& GetType() const = 0;
	virtual void Unload() = 0;
	virtual void SetDevice(Graphics* pGraphics) { m_pGraphics = pGraphics; }

protected:
	Graphics* m_pGraphics = nullptr;

private:
	Loader( const Loader &obj);
	Loader& operator=( const Loader& obj );
};

template <class T>
class ResourceLoader : public Loader
{
public:
	ResourceLoader()	{ ++m_loaderReferences; }

	~ResourceLoader() {}

	ResourceLoader(const ResourceLoader &obj) = delete;
	ResourceLoader& operator=(const ResourceLoader& obj) = delete;

	virtual const type_info& GetType() const { return typeid(T); }

	T* GetContent(const string& assetFile)
	{
		for(pair<string, T*> kvp:m_contentReferences)
		{
			if(kvp.first.compare(assetFile)==0)
				return kvp.second;
		}

		//Does File Exists?
		struct _stat buff;
		int result = _stat(assetFile.c_str(), &buff);
		if(result != 0)
		{
			stringstream ss;
			ss << "ResourceManager> File not found!\nPath: ";
			ss << assetFile;
			FLUX_LOG(ERROR, ss.str());
		}

		AUTOPROFILE_DESC(LoadContent, assetFile);

		T* content = LoadContent(assetFile);
		if(content!=nullptr)m_contentReferences.insert(pair<string,T*>(assetFile, content));
		return content;
	}

	T* GetContent_Reload(const string& assetFile)
	{
		for (auto it = m_contentReferences.begin(); it != m_contentReferences.end(); ++it)
		{
			if (it->first.compare(assetFile) == 0)
			{
				SafeDelete(it->second);
				m_contentReferences.erase(it);
				break;
			}
		}
		return GetContent(assetFile);
	}

	T* GetContent_Unmanaged(const string& assetFile)
	{
		AUTOPROFILE_DESC(LoadContent, assetFile);

		T* content = LoadContent(assetFile);
		FLUX_LOG_INFOFormat(LogType::WARNING, "ResourceManager > Asset '%s' loaded unmanaged.", assetFile.c_str());
		return content;
	}

	virtual void Unload()
	{
		--m_loaderReferences;

		if(m_loaderReferences<=0)
		{
			for(pair<string,T*> kvp:m_contentReferences)
			{
				Destroy(kvp.second);
			}

			m_contentReferences.clear();
		}
	}

protected:
	virtual T* LoadContent(const string& assetFile) = 0;
	virtual void Destroy(T* objToDestroy) = 0;

private:
	static unordered_map<string, T*> m_contentReferences;
	static int m_loaderReferences;
};

template<class T>
unordered_map<string, T*> ResourceLoader<T>::m_contentReferences = unordered_map<string, T*>();

template<class T>
int ResourceLoader<T>::m_loaderReferences = 0;


#include "stdafx.h"
#include "Material.h"

#include "External/TinyXml/tinyxml2.h"

#include "Core/Graphics.h"
#include "Core/Shader.h"
#include "Core/ShaderVariation.h"
#include "Core/Texture.h"

namespace XML = tinyxml2;

Material::~Material()
{
}

bool Material::Load(const std::string& filePath)
{
	AUTOPROFILE_DESC(Material_Load, Paths::GetFileName(filePath));

	unique_ptr<IFile> pFile = FileSystem::GetFile(filePath);
	if (pFile == nullptr)
		return false;
	if (!pFile->Open(FileMode::Read, ContentType::Text))
		return false;
	vector<char> buffer;
	if (!pFile->ReadAllBytes(buffer))
		return false;

	XML::XMLDocument document;
	if (document.Parse(buffer.data(), buffer.size()) != XML::XML_SUCCESS)
	{
		FLUX_LOG(ERROR, "[Material::Load] > %s", document.ErrorStr());
		return false;
	}

	XML::XMLElement* pRootNode = document.FirstChildElement();
	m_Name = pRootNode->Attribute("name");

	//Load the shader data
	XML::XMLElement* pShaders = pRootNode->FirstChildElement("Shaders");
	if (pShaders == nullptr)
		return false;

	XML::XMLElement* pShader = pShaders->FirstChildElement();
	while (pShader != nullptr)
	{
		string shaderType = pShader->Attribute("type");
		ShaderType type = ShaderType::NONE;
		if (shaderType == "Vertex")
			type = ShaderType::VertexShader;
		else if (shaderType == "Pixel")
			type = ShaderType::PixelShader;
		else if (shaderType == "Geometry")
			type = ShaderType::GeometryShader;
		else if (shaderType == "Compute")
			type = ShaderType::ComputeShader;
		else
			return false;

		checkf(m_ShaderVariations[(unsigned int)type] == nullptr, "Shader for slot already defined");

		string source = pShader->Attribute("source");
		m_Shaders[source] = m_pGraphics->GetShader(source);

		const char* pAttribute = pShader->Attribute("defines");
		string defines = "";
		if (pAttribute)
			defines = pAttribute;

		m_ShaderVariations[(unsigned int)type] = m_Shaders[source]->GetVariation(type, defines);
		pShader = pShader->NextSiblingElement();
	}


	//Load the Parameter data
	XML::XMLElement* pParameters = pRootNode->FirstChildElement("Parameters");
	if (pParameters != nullptr)
	{
		XML::XMLElement* pParameter = pParameters->FirstChildElement();
		while (pParameter != nullptr)
		{
			string parameterType = pParameter->Value();
			if (parameterType == "Texture")
			{
				string slot = pParameter->Attribute("slot");
				TextureSlot slotType = TextureSlot::MAX;
				if (slot == "Diffuse")
					slotType = TextureSlot::Diffuse;
				else if (slot == "Normal")
					slotType = TextureSlot::Normal;
				else if (stoi(slot) < (int)TextureSlot::MAX)
					slotType = (TextureSlot)stoi(slot);
				else
					return false;

				unique_ptr<Texture> pTexture = make_unique<Texture>(m_pGraphics);
				pTexture->Load(pParameter->Attribute("value"));
				m_TextureCache.push_back(std::move(pTexture));
				m_Textures.push_back(pair<TextureSlot, Texture*>(slotType, m_TextureCache[m_TextureCache.size() - 1].get()));
			}
			else if (parameterType == "Value")
			{
				string name = pParameter->Attribute("name");
				string value = pParameter->Attribute("value");
				ParseValue(name, value);
			}

			pParameter = pParameter->NextSiblingElement();
		}
	}

	//Load the properties
	XML::XMLElement* pProperties = pRootNode->FirstChildElement("Properties");
	if (pProperties != nullptr)
	{
		XML::XMLElement* pProperty = pProperties->FirstChildElement();
		while (pProperty != nullptr)
		{
			string propertyType = pProperty->Value();
			if (propertyType == "CullMode")
			{
				string value = pProperty->Attribute("value");
				if (value == "Back")
					m_CullMode = CullMode::BACK;
				else if (value == "Front")
					m_CullMode = CullMode::FRONT;
				else
					m_CullMode = CullMode::NONE;
			}
			else if (propertyType == "Blending")
			{
				m_Blending = pProperty->BoolAttribute("value");
			}
			else
				return false;

			pProperty = pProperty->NextSiblingElement();
		}
	}

	return true;
}

ShaderVariation* Material::GetShader(const ShaderType type) const
{
	return m_ShaderVariations[(unsigned int)type];
}

void Material::ParseValue(const std::string name, const std::string valueString)
{
	stringstream stream(valueString);
	string stringValue;
	vector<string> values;
	while (getline(stream, stringValue, ' '))
	{
		values.push_back(stringValue);
	}
	bool isInt = values[0].find('.') == string::npos;
	if (isInt)
	{
		m_ParameterBuffer.resize(m_ParameterBuffer.size() + values.size() * sizeof(int));
		for (size_t i = 0; i < values.size(); ++i)
		{
			int v = stoi(values[i]);
			memcpy(&m_ParameterBuffer[0] + m_BufferOffset, &v, sizeof(int));
			m_BufferOffset += sizeof(int);
		}
	}
	else
	{
		m_ParameterBuffer.resize(m_ParameterBuffer.size() + values.size() * sizeof(float));
		for (size_t i = 0; i < values.size(); ++i)
		{
			float v = stof(values[i]);
			memcpy(&m_ParameterBuffer[0] + m_BufferOffset, &v, sizeof(float));
			m_BufferOffset += sizeof(float);
		}
	}
	m_Parameters.push_back(pair<string, unsigned int>(name, (unsigned int)(m_ParameterBuffer.size() - values.size() * sizeof(float))));
}
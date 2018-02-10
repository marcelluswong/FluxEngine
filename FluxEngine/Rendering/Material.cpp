#include "FluxEngine.h"
#include "Material.h"

#include "External/TinyXml/tinyxml2.h"

#include "Core/Graphics.h"
#include "Core/Shader.h"
#include "Core/ShaderVariation.h"
#include "Core/Texture.h"

namespace XML = tinyxml2;

Material::Material(Context* pContext) :
	Resource(pContext)
{
	m_pGraphics = pContext->GetSubsystem<Graphics>();
}

Material::~Material()
{
}

bool Material::Load(const std::string& filePath)
{
	AUTOPROFILE_DESC(Material_Load, Paths::GetFileName(filePath));

	std::unique_ptr<IFile> pFile = FileSystem::GetFile(filePath);
	if (pFile == nullptr)
		return false;
	if (!pFile->Open(FileMode::Read, ContentType::Text))
		return false;
	std::vector<char> buffer;
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
	{
		FLUX_LOG(ERROR, "[Material::Load()] > '%s' : does not have a 'Shaders' section. This is required", m_Name.c_str());
		return false;
	}

	XML::XMLElement* pShader = pShaders->FirstChildElement();
	while (pShader != nullptr)
	{
		std::string shaderType = pShader->Attribute("type");
		ShaderType type;
		if (shaderType == "Vertex")
			type = ShaderType::VertexShader;
		else if (shaderType == "Pixel")
			type = ShaderType::PixelShader;
		else if (shaderType == "Geometry")
			type = ShaderType::GeometryShader;
		else if (shaderType == "Compute")
			type = ShaderType::ComputeShader;
		else
		{
			FLUX_LOG(ERROR, "[Material::Load()] > %s : Shader type '%s' is invalid", m_Name.c_str(), shaderType.c_str());
			return false;
		}

		checkf(m_ShaderVariations[(unsigned int)type] == nullptr, "[Material::Load] >Shader for slot already defined");

		const char* pAttribute = pShader->Attribute("defines");
		std::string defines = "";
		if (pAttribute)
			defines = pAttribute;
		std::string source = pShader->Attribute("source");
		m_ShaderVariations[(unsigned int)type] = m_pGraphics->GetShader(source, type, defines);
		pShader = pShader->NextSiblingElement();
	}


	//Load the Parameter data
	XML::XMLElement* pParameters = pRootNode->FirstChildElement("Parameters");
	if (pParameters != nullptr)
	{
		XML::XMLElement* pParameter = pParameters->FirstChildElement();
		while (pParameter != nullptr)
		{
			std::string parameterType = pParameter->Value();
			if (parameterType == "Texture")
			{
				std::string slot = pParameter->Attribute("slot");
				TextureSlot slotType = TextureSlot::MAX;
				if (slot == "Diffuse")
					slotType = TextureSlot::Diffuse;
				else if (slot == "Normal")
					slotType = TextureSlot::Normal;
				else
				{
					FLUX_LOG(ERROR, "[Material::Load()] > %s : Slot with name '%s' is not valid", m_Name.c_str(), slot.c_str());
					return false;
				}

				Texture* pTexture = GetSubsystem<ResourceManager>()->Load<Texture>(pParameter->Attribute("value"));
				m_Textures.push_back(std::pair<TextureSlot, Texture*>(slotType, pTexture));
			}
			else if (parameterType == "Value")
			{
				std::string name = pParameter->Attribute("name");
				std::string value = pParameter->Attribute("value");
				ParseValue(name, value);
			}
			else
			{
				FLUX_LOG(WARNING, "[Material::Load()] > %s : Parameter with name '%' is not valid, skipping", m_Name.c_str(), parameterType.c_str());
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
			std::string propertyType = pProperty->Value();
			if (propertyType == "CullMode")
			{
				std::string value = pProperty->Attribute("value");
				if (value == "Back")
					m_CullMode = CullMode::BACK;
				else if (value == "Front")
					m_CullMode = CullMode::FRONT;
				else if (value == "None")
					m_CullMode = CullMode::NONE;
				else
					FLUX_LOG(WARNING, "[Material::Load()] > %s : Cull mode '%s' is not valid, falling back to default", m_Name.c_str(), value.c_str());
			}
			else if (propertyType == "BlendMode")
			{
				std::string value = pProperty->Attribute("value");
				if (value == "Replace")
					m_BlendMode = BlendMode::REPLACE;
				else if (value == "Alpha")
					m_BlendMode = BlendMode::ALPHA;
				else if (value == "Add")
					m_BlendMode = BlendMode::ADD;
				else if (value == "AddAlpha")
					m_BlendMode = BlendMode::ADDALPHA;
				else
					FLUX_LOG(WARNING, "[Material::Load()] > %s : Blend mode '%s' is not valid, falling back to default", m_Name.c_str(), value.c_str());
			}
			else
				FLUX_LOG(WARNING, "[Material::Load()] > %s : Property with name '%s' is not valid, skipping", m_Name.c_str(), propertyType.c_str());

			pProperty = pProperty->NextSiblingElement();
		}
	}

	return true;
}

ShaderVariation* Material::GetShader(const ShaderType type) const
{
	return m_ShaderVariations[(unsigned int)type];
}

void Material::SetTexture(const TextureSlot slot, Texture* pTexture)
{
	for (auto& p : m_Textures)
	{
		if (p.first == slot)
		{
			p.second = pTexture;
			return;
		}
	}
	m_Textures.push_back(std::pair<TextureSlot, Texture*>(slot, pTexture));
}

//#todo: This is really hacky and unsafe, should find a better way to store arbitrary shader values
void Material::ParseValue(const std::string& name, const std::string& valueString)
{
	AUTOPROFILE_DESC(Material_ParseValue, name);

	std::stringstream stream(valueString);
	std::string stringValue;
	std::vector<std::string> values;
	while (std::getline(stream, stringValue, ' '))
	{
		values.push_back(stringValue);
	}
	bool isInt = values[0].find('.') == std::string::npos;
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
	m_Parameters.push_back(std::pair<std::string, unsigned int>(name, (unsigned int)(m_ParameterBuffer.size() - values.size() * sizeof(float))));
}
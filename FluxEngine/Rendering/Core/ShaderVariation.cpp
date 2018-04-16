#include "FluxEngine.h"
#include "ShaderVariation.h"
#include "Shader.h"
#include "Graphics.h"
#include "ConstantBuffer.h"

ShaderVariation::ShaderVariation(Context* pContext, Shader* pShader, const ShaderType type) :
	Object(pContext),
	m_pParentShader(pShader),
	m_ShaderType(type)
{

}

ShaderVariation::~ShaderVariation()
{
	Release();
}


void ShaderVariation::Release()
{
	SafeRelease(m_pShaderObject);
}

void ShaderVariation::AddDefine(const std::string& define)
{
	m_Defines.push_back(define);
}

void ShaderVariation::SetDefines(const std::string& defines)
{
	std::stringstream stream(defines);
	std::string define;
	while (std::getline(stream, define, ','))
	{
		m_Defines.push_back(define);
	}
}
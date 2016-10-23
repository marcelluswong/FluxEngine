#include "stdafx.h"
#include "Material.h"
#include "../Components/MeshComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/CameraComponent.h"

Material::~Material()
{
}

void Material::Initialize(GameContext* pGameContext)
{
	if (m_IsInitialized)
	{
		DebugLog::LogFormat(LogType::WARNING, L"Material using effect: %s and technique: %s is already initialized. Skipping initialization.", m_MaterialDesc.EffectName.c_str(), m_MaterialDesc.TechniqueName.c_str());
		return;
	}

	m_MaterialDesc.Validate();

	m_pGameContext = pGameContext;
	LoadEffect();
	CreateInputLayout();
	LoadShaderVariables();
	m_IsInitialized = true;
}

void Material::LoadEffect()
{
	m_pEffect = ResourceManager::Load<ID3DX11Effect>(m_MaterialDesc.EffectName);

	if (m_MaterialDesc.TechniqueName.length() > 0)
	{
		string techName(m_MaterialDesc.TechniqueName.begin(), m_MaterialDesc.TechniqueName.end());
		m_pTechnique = m_pEffect->GetTechniqueByName(techName.c_str());
	}
	else
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	//Link the variables
	if(m_MaterialDesc.HasWorldMatrix)
		BIND_AND_CHECK_SEMANTIC(m_pWorldMatrixVariable, "World", AsMatrix);
	if (m_MaterialDesc.HasViewMatrix)
		BIND_AND_CHECK_SEMANTIC(m_pViewMatrixVariable, "View", AsMatrix);
	if (m_MaterialDesc.HasViewInverseMatrix)
		BIND_AND_CHECK_SEMANTIC(m_pViewInverseMatrixVariable, "ViewInverse", AsMatrix);
	if (m_MaterialDesc.HasWvpMatrix)
		BIND_AND_CHECK_SEMANTIC(m_pWvpMatrixVariable, "WorldViewProjection", AsMatrix);
}

void Material::Update(MeshComponent* pMeshComponent)
{
	if(!m_IsInitialized)
	{
		DebugLog::LogFormat(LogType::ERROR, L"Material using effect: %s and technique: %s is not initialized.", m_MaterialDesc.EffectName.c_str(), m_MaterialDesc.TechniqueName.c_str());
		return;
	}
	auto world = XMMatrixIdentity();
	if(pMeshComponent)
		world = XMLoadFloat4x4(&pMeshComponent->GetTransform()->GetWorldMatrix());
	auto view = XMLoadFloat4x4(&m_pGameContext->Scene->CurrentCamera->GetView());
	auto projection = XMLoadFloat4x4(&m_pGameContext->Scene->CurrentCamera->GetProjection());

	if (m_pWorldMatrixVariable)
		m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<float*>(&world));
	if (m_pViewMatrixVariable)
		m_pViewMatrixVariable->SetMatrix(reinterpret_cast<float*>(&view));
	if (m_pViewInverseMatrixVariable)
	{
		auto viewInv = XMLoadFloat4x4(&m_pGameContext->Scene->CurrentCamera->GetViewInverse());
		m_pViewInverseMatrixVariable->SetMatrix(reinterpret_cast<float*>(&viewInv));
	}
	if (m_pWvpMatrixVariable)
	{
		auto wvp = world * view * projection;
		m_pWvpMatrixVariable->SetMatrix(reinterpret_cast<float*>(&wvp));
	}

	UpdateShaderVariables(pMeshComponent);
}

void Material::CreateInputLayout()
{
	D3DX11_PASS_SHADER_DESC passShaderDesc;
	m_pTechnique->GetPassByIndex(0)->GetVertexShaderDesc(&passShaderDesc);
	D3DX11_EFFECT_SHADER_DESC effectShaderDesc;
	passShaderDesc.pShaderVariable->GetShaderDesc(passShaderDesc.ShaderIndex, &effectShaderDesc);
	UINT numInputElements = effectShaderDesc.NumInputSignatureEntries;

	UINT currentOffset = 0;
	D3D11_SIGNATURE_PARAMETER_DESC pDesc;

	for (UINT i = 0; i < numInputElements; i++)
	{
		passShaderDesc.pShaderVariable->GetInputSignatureElementDesc(passShaderDesc.ShaderIndex, i, &pDesc);

		ILElement ilElement;

		//MASK LAYOUT
		//0000 0001 => 4 bytes	(Active Bits = 1)
		//0000 0011 => 8 bytes	(Active Bits = 2)
		//0000 0111 => 12 bytes	(Active Bits = 3)
		//0000 1111 => 16 bytes	(Active Bits = 4)
		//OFFSET = 'Active Bits' * 4 [Active Bits = log2(number) + 1]
		ilElement.Size = static_cast<UINT>(floor(log(pDesc.Mask) / log(2)) + 1) * 4;
		switch(pDesc.ComponentType)
		{
		case D3D_REGISTER_COMPONENT_UINT32:
			if (pDesc.Mask == 1) ilElement.Format = DXGI_FORMAT_R32_UINT;
			else if (pDesc.Mask == 3) ilElement.Format = DXGI_FORMAT_R32G32_UINT;
			else if (pDesc.Mask == 7) ilElement.Format = DXGI_FORMAT_R32G32B32_UINT;
			else ilElement.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		case D3D_REGISTER_COMPONENT_SINT32:
			if (pDesc.Mask == 1) ilElement.Format = DXGI_FORMAT_R32_SINT;
			else if (pDesc.Mask == 3) ilElement.Format = DXGI_FORMAT_R32G32_SINT;
			else if (pDesc.Mask == 7) ilElement.Format = DXGI_FORMAT_R32G32B32_SINT;
			else ilElement.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		case D3D_REGISTER_COMPONENT_FLOAT32:
			if (pDesc.Mask == 1) ilElement.Format = DXGI_FORMAT_R32_FLOAT;
			else if (pDesc.Mask == 3) ilElement.Format = DXGI_FORMAT_R32G32_FLOAT;
			else if (pDesc.Mask == 7) ilElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			else ilElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		default:
			break;
		}

		D3D11_INPUT_ELEMENT_DESC desc;
		desc.Format = ilElement.Format;
		desc.SemanticIndex = pDesc.SemanticIndex;
		desc.SemanticName = pDesc.SemanticName;
		desc.InstanceDataStepRate = 0;
		desc.InputSlot = 0;
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc.AlignedByteOffset = currentOffset;

		m_InputLayoutDesc.InputLayoutDesc.push_back(desc);
		currentOffset += ilElement.Size;
	}

	m_InputLayoutDesc.VertexStride = currentOffset;

	D3DX11_PASS_DESC PassDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	HR(m_pGameContext->Engine->D3Device->CreateInputLayout(m_InputLayoutDesc.InputLayoutDesc.data(), m_InputLayoutDesc.InputLayoutDesc.size(), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, m_pInputLayout.GetAddressOf()));
}
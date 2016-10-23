#pragma once
#include "..\Material.h"

class Texture;
namespace FlexHelper
{
	struct FlexData;
	struct FlexMeshInstance;
}

class FlexSoftbodyMaterial : public Material
{
public:
	FlexSoftbodyMaterial(FlexHelper::FlexData* pFlexData, FlexHelper::FlexMeshInstance* pMeshInstance);
	~FlexSoftbodyMaterial();
	void SetTexture(Texture* pTexture) { m_pTexture = pTexture; }

protected:
	void LoadShaderVariables();
	void UpdateShaderVariables(MeshComponent* pMeshComponent);
	void CreateInputLayout();

private:
	FlexHelper::FlexMeshInstance* m_pMeshInstance;
	FlexHelper::FlexData* m_pFlexData;

	static ID3DX11EffectVectorVariable* m_pLightDirectionVar;
	static ID3DX11EffectVectorVariable* m_pColorVar;
	static ID3DX11EffectMatrixVariable* m_pViewProjVar;
	static ID3DX11EffectMatrixVariable* m_pViewInverseVar;
	static ID3DX11EffectVectorVariable* m_pRigidRestPosesVar;
	static ID3DX11EffectVectorVariable* m_pRigidTranslationsVar;
	static ID3DX11EffectVectorVariable* m_pRigidRotationsVar;
	static ID3DX11EffectScalarVariable* m_pUseDiffuseTextureVar;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseTextureVar;


	XMFLOAT4 m_Color;
	Texture* m_pTexture = nullptr;
};


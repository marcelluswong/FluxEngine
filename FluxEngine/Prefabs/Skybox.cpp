#include "stdafx.h"
#include "Skybox.h"
#include "../Components/MeshComponent.h"
#include "../Materials/SkyboxMaterial.h"

Skybox::Skybox()
{
}


Skybox::~Skybox()
{
}

void Skybox::Initialize()
{
	MeshComponent* pMesh = new MeshComponent(ResourceManager::Load<MeshFilter>(L"./Resources/Meshes/Cube.flux"));
	AddComponent(pMesh);
	m_pMaterial = make_unique<SkyboxMaterial>();
	m_pMaterial->Initialize(m_pGameContext);
	SetTexture(ResourceManager::Load<Texture>(L"./Resources/Textures/MinecraftSkybox.dds"));
	pMesh->SetMaterial(m_pMaterial.get());
}

void Skybox::Update()
{
}

void Skybox::Render()
{
}

void Skybox::SetTexture(Texture* pTexture)
{
	m_pMaterial->SetTexture(pTexture);
}

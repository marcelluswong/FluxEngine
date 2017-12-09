//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ParticleEmitter.h"
#include "Particle.h"
#include "SceneGraph/Transform.h"
#include "Rendering/Camera/Camera.h"
#include "Rendering/Core/VertexBuffer.h"
#include "Rendering/Core/GraphicsDefines.h"
#include "Rendering/Core/Graphics.h"
#include "Scenegraph/Scene.h"
#include "Rendering/Geometry.h"
#include "Scenegraph/SceneNode.h"
#include "Rendering/Material.h"
#include "../Core/Texture.h"

ParticleEmitter::ParticleEmitter(Graphics* pGraphics, ParticleSystem* pSystem) : 
	m_pParticleSystem(pSystem),
	m_pGraphics(pGraphics)
{
	m_Particles.resize(m_pParticleSystem->MaxParticles);
	for (int i = 0; i < m_pParticleSystem->MaxParticles; i++)
		m_Particles[i] = new Particle(m_pParticleSystem);
	m_BufferSize = m_pParticleSystem->MaxParticles;

	m_pGeometry = make_unique<Geometry>();
	m_pGeometry->SetVertexBuffer(m_pVertexBuffer.get());
	m_Batches.resize(1);
	m_Batches[0].pGeometry = m_pGeometry.get();
	m_pMaterial = make_unique<Material>(m_pGraphics);
	m_pMaterial->Load("Resources/Materials/Particles.xml");
	m_pMaterial->SetDepthTestMode(CompareMode::ALWAYS);
	m_Batches[0].pMaterial = m_pMaterial.get();
}

ParticleEmitter::~ParticleEmitter()
{
	for (size_t i = 0; i < m_Particles.size(); i++)
		delete m_Particles[i];
	m_Particles.clear();
}

void ParticleEmitter::SetSystem(ParticleSystem* pSettings)
{
	m_pParticleSystem = pSettings;
	if (m_pParticleSystem->ImagePath == "")
		m_pParticleSystem->ImagePath = ERROR_TEXTURE;
	CreateVertexBuffer();
	m_pTexture.reset();
	m_pTexture = make_unique<Texture>(m_pGraphics);
	if (!m_pTexture->Load(pSettings->ImagePath))
	{
		FLUX_LOG(ERROR, "[ParticleEmitter::SetSystem] > Failed to load texture '%s'", pSettings->ImagePath.c_str());
		return;
	}
	m_pMaterial->SetTexture(TextureSlot::Diffuse, m_pTexture.get());
	m_BurstIterator = m_pParticleSystem->Bursts.begin();
	Reset();
}

void ParticleEmitter::Reset()
{
	m_Timer = 0.0f;
	m_pParticleSystem->PlayOnAwake ? m_Playing = true : m_Playing = false;
	for (Particle* p : m_Particles)
		p->Reset();
}

void ParticleEmitter::OnSceneSet(Scene* pScene)
{
	Drawable::OnSceneSet(pScene);

	if (m_pParticleSystem->ImagePath == "")
		m_pParticleSystem->ImagePath = ERROR_TEXTURE;
	CreateVertexBuffer();
	m_BurstIterator = m_pParticleSystem->Bursts.begin();
	if (m_pParticleSystem->PlayOnAwake)
		Play();
}

void ParticleEmitter::OnNodeSet(SceneNode* pNode)
{
	Drawable::OnNodeSet(pNode);
	m_Batches[0].pModelMatrix = &pNode->GetTransform()->GetWorldMatrix();
}

void ParticleEmitter::CreateVertexBuffer()
{
	m_pVertexBuffer.reset();

	vector<VertexElement> elementDesc = {
		VertexElement(VertexElementType::VECTOR3, VertexElementSemantic::POSITION, 0, false),
		VertexElement(VertexElementType::VECTOR4, VertexElementSemantic::COLOR, 0, false),
		VertexElement(VertexElementType::FLOAT, VertexElementSemantic::TEXCOORD, 0, false),
		VertexElement(VertexElementType::FLOAT, VertexElementSemantic::TEXCOORD, 1, false),
	};

	m_pVertexBuffer = make_unique<VertexBuffer>(m_pGraphics);
	m_pGeometry->SetVertexBuffer(m_pVertexBuffer.get());
	m_pVertexBuffer->Create(m_BufferSize, elementDesc, true);
}

void ParticleEmitter::SortParticles()
{
	switch (m_pParticleSystem->SortingMode)
	{
	case ParticleSortingMode::FrontToBack:
		sort(m_Particles.begin(), m_Particles.begin() + m_ParticleCount, [this](Particle* a, Particle* b)
		{
			float d1 = Vector3::DistanceSquared(a->GetVertexInfo().Position, m_pScene->GetCamera()->GetTransform()->GetWorldPosition());
			float d2 = Vector3::DistanceSquared(b->GetVertexInfo().Position, m_pScene->GetCamera()->GetTransform()->GetWorldPosition());
			return d1 > d2;
		});
		break;
	case ParticleSortingMode::BackToFront:
		sort(m_Particles.begin(), m_Particles.begin() + m_ParticleCount, [this](Particle* a, Particle* b)
		{
			float d1 = Vector3::DistanceSquared(a->GetVertexInfo().Position, m_pScene->GetCamera()->GetTransform()->GetWorldPosition());
			float d2 = Vector3::DistanceSquared(b->GetVertexInfo().Position, m_pScene->GetCamera()->GetTransform()->GetWorldPosition());
			return d1 < d2;
		});
	case ParticleSortingMode::OldestFirst:
		sort(m_Particles.begin(), m_Particles.begin() + m_ParticleCount, [](Particle* a, Particle* b)
		{
			float lifeTimerA = a->GetLifeTimer();
			float lifeTimerB = b->GetLifeTimer();
			return lifeTimerA < lifeTimerB;
		});
		break;
	case ParticleSortingMode::YoungestFirst:
		sort(m_Particles.begin(), m_Particles.begin() + m_ParticleCount, [](Particle* a, Particle* b)
		{
			float lifeTimerA = a->GetLifeTimer();
			float lifeTimerB = b->GetLifeTimer();
			return lifeTimerA > lifeTimerB;
		});
		break;
	default: 
		break;
	}
}

void ParticleEmitter::Update()
{
	if (m_Playing == false)
		return;

	m_Timer += GameTimer::DeltaTime();
	if (m_Timer >= m_pParticleSystem->Duration && m_pParticleSystem->Loop)
	{
		m_Timer = 0;
		m_BurstIterator = m_pParticleSystem->Bursts.begin();
	}

	float emissionTime = 1.0f / m_pParticleSystem->Emission;
	m_ParticleSpawnTimer += GameTimer::DeltaTime();

	SortParticles();

	m_ParticleCount = 0;

	ParticleVertex* pBuffer = (ParticleVertex*)m_pVertexBuffer->Map(true);

	if(m_pParticleSystem->MaxParticles > (int)m_Particles.size())
	{
		int startIdx = (int)m_Particles.size();
		m_Particles.resize(m_pParticleSystem->MaxParticles);
		for (int i = startIdx; i < m_pParticleSystem->MaxParticles; i++)
			m_Particles[i] = new Particle(m_pParticleSystem);
	}

	int burstParticles = 0;
	for (int i = 0; i < m_pParticleSystem->MaxParticles; i++)
	{
		Particle* p = m_Particles[i];
		//Update if active
		if (p->IsActive())
		{
			p->Update();
			pBuffer[m_ParticleCount] = p->GetVertexInfo();
			++m_ParticleCount;
		}
		//Spawn particle on burst tick
		else if(m_BurstIterator != m_pParticleSystem->Bursts.end() && m_Timer > m_BurstIterator->first && burstParticles < m_BurstIterator->second)
		{
			p->Init();
			pBuffer[m_ParticleCount] = p->GetVertexInfo();
			++m_ParticleCount;
			++burstParticles;
		}

		//Spawn particle on emission tick
		else if (m_ParticleSpawnTimer >= emissionTime && m_Timer < m_pParticleSystem->Duration)
		{
			p->Init();
			pBuffer[m_ParticleCount] = p->GetVertexInfo();
			++m_ParticleCount;
			m_ParticleSpawnTimer -= emissionTime;
		}
	}
	if (burstParticles > 0)
		++m_BurstIterator;

	m_pVertexBuffer->Unmap();

	if (m_pParticleSystem->MaxParticles > m_BufferSize)
	{
		m_BufferSize = m_pParticleSystem->MaxParticles + 500;
		FLUX_LOG(WARNING, "ParticleEmitter::Render() > VertexBuffer too small! Increasing size...");
		CreateVertexBuffer();
	}
	m_pGeometry->SetDrawRange(PrimitiveType::POINTLIST, 0, m_ParticleCount);
}
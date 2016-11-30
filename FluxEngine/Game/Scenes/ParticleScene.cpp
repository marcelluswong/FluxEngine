#include "stdafx.h"
#include "ParticleScene.h"
#include "../Scenegraph/GameObject.h"
#include "../Components/MeshComponent.h"
#include "../Prefabs/Skybox.h"

#include "../Materials/Forward/DefaultMaterial.h"
#include "../Components/TransformComponent.h"
#include <ctime>
#include "../Game/GameManager.h"
#include "../Physics/Flex/FlexSoftbody.h"
#include "../Managers/SoundManager.h"
#include "../Physics/Flex/FlexMousePicker.h"
#include "../Physics/Flex/FlexDebugRenderer.h"
#include "../Materials/Deferred/BasicMaterial_Deferred.h"
#include "../Components/AudioSource.h"
#include "../Graphics/Texture.h"
#include "../Graphics/RenderTarget.h"
#include "../Physics/Flex/FlexSystem.h"
#include "../Physics/Flex/FlexTriangleMeshCollider.h"
#include "../Graphics/MeshFilter.h"
#include "../Managers/MaterialManager.h"
#include "../Physics/Flex/FluidRenderer.h"
#include "../Components/ParticleEmitterComponent.h"
#include "../UI/ImgUI/imgui.h"

class FlexTriangleMeshCollider;

ParticleScene::ParticleScene()
{}

ParticleScene::~ParticleScene()
{
	//delete m_pSystem;
	//flexShutdown();
}

void ParticleScene::Initialize()
{
	//Create the ground plane
	GameObject* pGroundPlane = new GameObject();
	MeshComponent* pMeshComponent = new MeshComponent(L"./Resources/Meshes/unit_plane.flux");

	m_pGroundMaterial = make_unique<DefaultMaterial>();
	m_pGroundMaterial->Initialize(m_pGameContext);
	m_pGroundMaterial->SetColor(Vector4(0.0f, 0.7f, 1.0f, 1.0f));
	pMeshComponent->SetMaterial(m_pGroundMaterial.get());
	pGroundPlane->AddComponent(pMeshComponent);
	pGroundPlane->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	pGroundPlane->GetTransform()->SetScale(200.0f, 200.0f, 200.0f);
	AddChild(pGroundPlane);

	//Add a skybox
	Skybox* pSky = new Skybox();
	AddChild(pSky);

	ParticleSystem* pSettings = ResourceManager::Load<ParticleSystem>(L"./Resources/Particle.json");

	GameObject* pObj = new GameObject();
	m_pEmitter = new ParticleEmitterComponent(pSettings);
	pObj->AddComponent(m_pEmitter);
	AddChild(pObj);

	/*flexInit(FLEX_VERSION);

	m_pSystem = new FlexSystem();
	m_pSystem->SetDefaultParams();
	m_pSystem->CreateGroundPlane();

	vector<Vector4> particles;
	vector<Vector3> velocities;
	vector<int> indices;
	vector<int> phases;

	m_pSystem->Params.mRadius = 0.7f;
	m_pSystem->Params.mDynamicFriction = 0.35f;
	m_pSystem->Params.mNumIterations = 4;
	m_pSystem->Params.mCollisionDistance = m_pSystem->Params.mRadius;
	//m_pSystem->Params.mFluid = true;
	//m_pSystem->Params.mFluidRestDistance = 0.8f;
	
	int i = 0;
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			for (int z = 0; z < 10; z++)
			{
				m_pSystem->Positions.push_back(Vector4((float)x, (float)y + 10.0f, (float)z, 1.0f));
				m_pSystem->Velocities.push_back(Vector3());

				m_pSystem->Phases.push_back(flexMakePhase(rand() % 3, eFlexPhaseSelfCollide));
				++i;
			}
		}
	}
	m_pSystem->AdjustParams();
	m_pSystem->InitializeSolver();
	m_pSystem->UploadFlexData();

	m_pFluidRenderer = new FluidRenderer(m_pSystem);
	AddChild(m_pFluidRenderer);*/
}

void ParticleScene::Update()
{
	//m_pSystem->UpdateSolver(1.0f / 60.0f);
	//m_pSystem->FetchData();
}

void ParticleScene::LateUpdate()
{
	//Update data
	//m_pSystem->UpdateData();
}

void ParticleScene::Render()
{

}
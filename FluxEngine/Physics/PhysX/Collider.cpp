#include "FluxEngine.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "PhysicsSystem.h"
#include "Scenegraph\SceneNode.h"
#include "Rendering\Model.h"
#include "PhysicsMesh.h"

Collider::Collider(Context* pContext, PhysicsSystem* pPhysicsSystem, PxMaterial* pMaterial, physx::PxShapeFlags shapeFlags) :
	Component(pContext),
	m_pPhysicsSystem(pPhysicsSystem),
	m_ShapeFlags(shapeFlags)
{
	m_pMaterial = pMaterial ? pMaterial : m_pPhysicsSystem->GetDefaultMaterial();
}

Collider::~Collider()
{
	SafeDelete(m_pGeometry);
}

void Collider::CreateShape()
{
	CreateGeometry();
	m_pShape = m_pPhysicsSystem->GetPhysics()->createShape(*m_pGeometry, *m_pMaterial, true, m_ShapeFlags);
	m_pShape->userData = this;
	SetCollisionGroup(m_CollisionGroup, m_ListenForCollisionGroups);
}

void Collider::RemoveShape()
{
	if (m_pRigidbody == nullptr || m_pShape == nullptr)
		return;

	m_pRigidbody->GetBody()->detachShape(*m_pShape);
}

void Collider::SetTrigger(bool isTrigger)
{
	if (m_pShape == nullptr)
	{
		FLUX_LOG(WARNING, "[Collider::SetTrigger] Collider does not have a shape");
		return;
	}
	m_pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
}

void Collider::SetMaterial(PxMaterial* pMaterial)
{
	if (m_pShape)
	{
		m_pMaterial = pMaterial;
		m_pShape->setMaterials(&pMaterial, 1);
	}
}

void Collider::SetCollisionGroup(const CollisionGroup group, const CollisionGroup listenerForGroup)
{
	m_CollisionGroup = group;
	m_ListenForCollisionGroups = listenerForGroup;
	if (m_pShape)
	{
		PxFilterData filter;
		filter.setToDefault();
		filter.word0 = (unsigned int)group;
		filter.word1 = (unsigned int)(listenerForGroup | group);
		m_pShape->setSimulationFilterData(filter);
	}
}

void Collider::SetCollisionGroup(const CollisionGroup group)
{
	SetCollisionGroup(group, group);
}

void Collider::OnNodeSet(SceneNode* pNode)
{
	Component::OnNodeSet(pNode);

	m_pRigidbody = GetComponent<Rigidbody>();
	if (m_pRigidbody == nullptr)
	{
		m_pRigidbody = new Rigidbody(m_pContext, m_pPhysicsSystem);
		pNode->AddComponent(m_pRigidbody);
	}
	CreateShape();
	if (m_pShape)
		m_pRigidbody->GetBody()->attachShape(*m_pShape);
}

void Collider::OnNodeRemoved()
{
	RemoveShape();
}

SphereCollider::SphereCollider(Context* pContext, 
	PhysicsSystem* pSystem,
	float radius,
	PxMaterial* pMaterial /*= nullptr*/,
	physx::PxShapeFlags shapeFlags /*= physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION*/) :
	Collider(pContext, pSystem, pMaterial, shapeFlags),
	m_Radius(radius)
{}

SphereCollider::~SphereCollider()
{
	SafeDelete(m_pGeometry);
}

void SphereCollider::CreateGeometry()
{
	m_pGeometry = new PxSphereGeometry(m_Radius);
}

//BOX
BoxCollider::BoxCollider(Context* pContext, PhysicsSystem* pSystem, const Vector3& extents, PxMaterial* pMaterial /*= nullptr*/, physx::PxShapeFlags shapeFlags /*= physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION*/) :
	Collider(pContext, pSystem, pMaterial, shapeFlags),
	m_Extents(extents)
{}

BoxCollider::BoxCollider(Context* pContext, PhysicsSystem* pSystem, const BoundingBox& boundingBox, PxMaterial* pMaterial /*= nullptr*/, physx::PxShapeFlags shapeFlags /*= physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION*/):
	Collider(pContext, pSystem, pMaterial, shapeFlags),
	m_Extents(boundingBox.Extents.x, boundingBox.Extents.y, boundingBox.Extents.z)
{}

void BoxCollider::CreateGeometry()
{
	m_pGeometry = new PxBoxGeometry(m_Extents.x, m_Extents.y, m_Extents.z);
}

//PLANE
PlaneCollider::PlaneCollider(Context* pContext, PhysicsSystem* pSystem, PxMaterial* pMaterial /*= nullptr*/, physx::PxShapeFlags shapeFlags /*= physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION*/) :
	Collider(pContext, pSystem, pMaterial, shapeFlags)
{}

void PlaneCollider::CreateGeometry()
{
	m_pGeometry = new PxPlaneGeometry();
}

//CAPSULE
CapsuleCollider::CapsuleCollider(Context* pContext, PhysicsSystem* pSystem, const float radius, const float height, PxMaterial* pMaterial /*= nullptr*/, physx::PxShapeFlags shapeFlags /*= physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION*/) :
	Collider(pContext, pSystem, pMaterial, shapeFlags),
	m_Height(height / 2), m_Radius(radius)
{}

void CapsuleCollider::CreateGeometry()
{
	m_pGeometry = new PxCapsuleGeometry(m_Radius, m_Height);
}

//MESH
MeshCollider::MeshCollider(Context* pContext, PhysicsSystem* pSystem, PhysicsMesh* pMesh, PxMaterial* pMaterial /*= nullptr*/, physx::PxShapeFlags shapeFlags /*= physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION*/) :
	Collider(pContext, pSystem, pMaterial, shapeFlags),
	m_pMesh(pMesh)
{}

MeshCollider::MeshCollider(Context* pContext, PhysicsSystem* pSystem, const std::string& filePath, PxMaterial* pMaterial /*= nullptr*/, physx::PxShapeFlags shapeFlags /*= physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eVISUALIZATION*/) :
	Collider(pContext, pSystem, pMaterial, shapeFlags)
{
	m_pMesh = ResourceManager::Instance().Load<PhysicsMesh>(filePath, pSystem);
}

void MeshCollider::CreateGeometry()
{
	if (m_pMesh == nullptr)
	{
		FLUX_LOG(ERROR, "[MeshCollider::MeshCollider] > Given PhysicsMesh is null");
		return;
	}
	switch (m_pMesh->GetMeshType())
	{
	case PhysicsMesh::Type::ConvexMesh:
		m_pGeometry = new PxConvexMeshGeometry((PxConvexMesh*)m_pMesh->GetMesh());
		break;
	case PhysicsMesh::Type::TriangleMesh:
		m_pGeometry = new PxTriangleMeshGeometry((PxTriangleMesh*)m_pMesh->GetMesh());
		break;
	case PhysicsMesh::Type::None:
	default:
		FLUX_LOG(ERROR, "[MeshCollider::MeshCollider] > Given PhysicsMesh has an invalid type");
		return;
	}
	return;
}
#include "FluxEngine.h"
#include "Model.h"
#include "Scenegraph\Scene.h"
#include "Mesh.h"
#include "Scenegraph/SceneNode.h"
#include "SceneGraph/Transform.h"

Model::Model(Context* pContext):
	Drawable(pContext)
{

}

Model::~Model()
{
}

void Model::OnSceneSet(Scene* pScene)
{
	Drawable::OnSceneSet(pScene);
}

void Model::OnMarkedDirty(const Transform* pTransform)
{
	for (Batch& batch : m_Batches)
		batch.pModelMatrix = &pTransform->GetWorldMatrix();
}

void Model::SetMesh(Mesh* pMesh)
{
	if (m_pNode == nullptr)
	{
		FLUX_LOG(Warning, "[Model::SetMesh] Cannot set mesh when component is not attached to a node");
		return;
	}

	int geometries = pMesh->GetGeometryCount();
	m_Batches.resize(geometries);
	for (int i = 0; i < geometries; ++i)
	{
		m_Batches[i].pGeometry = pMesh->GetGeometry(i);
		m_Batches[i].pModelMatrix = &m_pNode->GetTransform()->GetWorldMatrix();
		m_Batches[i].NumSkinMatrices = 1;
	}
	m_BoundingBox = pMesh->GetBoundingBox();

	m_pMesh = pMesh;
}

void Model::SetMaterial(Material* pMaterial)
{
	for (Batch& batch : m_Batches)
	{
		batch.pMaterial = pMaterial;
	}
}

void Model::SetMaterial(int index, Material* pMaterial)
{
	if (index >= (int)m_Batches.size())
	{
		FLUX_LOG(Error, "[Model::SetMaterial] > Index out of range! Is '%i' but model only has '%i' batches", index, m_Batches.size());
		return;
	}
	m_Batches[index].pMaterial = pMaterial;
}
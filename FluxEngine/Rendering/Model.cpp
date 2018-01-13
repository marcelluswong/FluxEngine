#include "FluxEngine.h"
#include "Model.h"
#include "Scenegraph\Scene.h"
#include "Mesh.h"
#include "Scenegraph/SceneNode.h"
#include "SceneGraph/Transform.h"

Model::Model()
{

}

Model::~Model()
{
}

void Model::OnSceneSet(Scene* pScene)
{
	Drawable::OnSceneSet(pScene);
}

void Model::OnNodeSet(SceneNode* pNode)
{	
	Drawable::OnNodeSet(pNode);

	for (Batch& batch : m_Batches)
	{
		batch.pModelMatrix = &m_pNode->GetTransform()->GetWorldMatrix();
	}
}

void Model::SetMesh(Mesh* pMesh)
{
	if (pMesh)
	{
		int geometries = pMesh->GetGeometryCount();
		m_Batches.resize(geometries);
		for (int i = 0; i < geometries; ++i)
			m_Batches[i].pGeometry = pMesh->GetGeometry(i);
		m_BoundingBox = pMesh->GetBoundingBox();
	}
	else
	{
		m_BoundingBox = BoundingBox();
	}
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
		FLUX_LOG(ERROR, "[Model::SetMaterial] > Index out of range! Is '%i' but model only has '%i' batches", index, m_Batches.size());
		return;
	}
	m_Batches[index].pMaterial = pMaterial;
}

DirectX::BoundingBox Model::GetWorldBoundingBox() const
{
	BoundingBox worldBoundingBox;
	m_BoundingBox.Transform(worldBoundingBox, m_pNode->GetTransform()->GetWorldMatrix());
	return worldBoundingBox;
}

void Model::Update()
{
}

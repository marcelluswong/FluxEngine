#include "stdafx.h"
#include "Component.h"
#include "SceneNode.h"

Component::Component()
{

}

Component::~Component()
{

}

void Component::OnSceneSet(Scene* pScene)
{
	if (m_pScene)
	{
		FLUX_LOG(ERROR, "[Component::OnSceneSet] > Component already has a scene assigned!");
		return;
	}
	m_pScene = pScene;
}

void Component::OnNodeSet(SceneNode* pNode)
{
	if (m_pNode)
	{
		FLUX_LOG(ERROR, "[Component::OnNodeSet] > Component already has a node assigned!");
		return;
	}
	m_pNode = pNode;
}

void Component::OnNodeRemoved()
{
	m_pNode = nullptr;
}

void Component::OnSceneRemoved()
{
	m_pScene = nullptr;
}

Transform* Component::GetTransform()
{
	checkf(m_pNode, "[Component::GetTransform()] > Component is not attached to a SceneNode");
	return m_pNode->GetTransform();
}

Component* Component::GetComponent(StringHash type)
{
	checkf(m_pNode, "Component is not attached to a SceneNode");
	return m_pNode->GetComponent(type);
}

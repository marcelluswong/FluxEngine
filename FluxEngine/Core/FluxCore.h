#pragma once

class Graphics;
class ImmediateUI;
class InputEngine;
class FreeCamera;
class Scene;
class SceneNode;
class PhysicsSystem;
class Window;
class DebugRenderer;

class FluxCore
{
public:
	FluxCore();
	virtual ~FluxCore();

	DELETE_COPY(FluxCore)

	int Run(HINSTANCE hInstance);
	void GameLoop();
	void RenderUI();
	void InitGame();

private:
	void OnPause(bool isActive);

	//Window variables
	HINSTANCE m_hInstance = nullptr;

	std::unique_ptr<Scene> m_pScene;
	FreeCamera* m_pCamera = nullptr;
	SceneNode* m_pNode = nullptr;

	bool m_DebugPhysics = false;

	//Systems
	Window* m_pWindow = nullptr;
	Graphics* m_pGraphics = nullptr;
	ImmediateUI* m_pImmediateUI = nullptr;
	InputEngine* m_pInput = nullptr;
	PhysicsSystem* m_pPhysics = nullptr;
	DebugRenderer* m_pDebugRenderer = nullptr;

	map<StringHash, unique_ptr<Subsystem>> m_Systems;
	template<typename T>
	T* GetSubsystem()
	{
		auto pIt = m_Systems.find(T::GetTypeStatic());
		if (pIt == m_Systems.end())
			return nullptr;
		return (T*)pIt->second.get();
	}

	template<typename T>
	T* RegisterSubsystem(unique_ptr<T> pSystem)
	{
		StringHash type = pSystem->GetType();
		m_Systems[type] = std::move(pSystem);
		return (T*)m_Systems[type].get();
	}
};

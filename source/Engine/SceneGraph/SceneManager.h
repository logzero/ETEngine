#pragma once

#include <string>
#include <vector>

#include <EtCore/Helper/Singleton.h>
#include <EtCore/UpdateCycle/Tickable.h>

#include <Engine/Base/TickOrder.h>


//Forward Declaration
class AbstractScene;
class AbstractFramework;

class SceneManager : public Singleton<SceneManager>, public I_Tickable
{
public:
	~SceneManager();

	void AddGameScene(AbstractScene* scene);
	void RemoveGameScene(AbstractScene* scene);
	void SetActiveGameScene(std::string sceneName);
	void NextScene();
	void PreviousScene();
	AbstractScene* GetActiveScene() const { return m_ActiveScene; }
	
private:
	friend class AbstractFramework;
	friend class Singleton<SceneManager>;

	SceneManager() : I_Tickable(static_cast<uint32>(E_TickOrder::TICK_SceneManager)) {}

	void Update();

	std::vector<AbstractScene*> m_pSceneVec;
	bool m_IsInitialized = false;
	AbstractScene* m_ActiveScene = nullptr
		, *m_NewActiveScene = nullptr;
};


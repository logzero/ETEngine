#include "stdafx.h"
#include "AbstractScene.h"

#include "Entity.h"

#include <Engine/Base/Time.h>
#include <Engine/Components/CameraComponent.h>
#include <Engine/Components/LightComponent.h>
#include <Engine/Prefabs/FreeCamera.h>
#include <Engine/Prefabs/Skybox.h>
#include <Engine/Prefabs/FreeCamera.h>
#include <Engine/Framebuffers/Gbuffer.h>
#include <Engine/Framebuffers/PostProcessingRenderer.h>
#include <Engine/GraphicsHelper/TextRenderer.h>
#include <Engine/GraphicsHelper/RenderPipeline.h>
#include <Engine/Physics/PhysicsWorld.h>


#define CONTEXT Context::GetInstance()

AbstractScene::AbstractScene(std::string name) 
	: m_Name(name)
	, m_IsInitialized(false)
{
}

AbstractScene::~AbstractScene()
{
	for (Entity* pEntity : m_pEntityVec)
	{
		SafeDelete(pEntity);
	}
	m_pEntityVec.clear();
	if (m_pSkybox)SafeDelete(m_pSkybox);

	SafeDelete(m_pPhysicsWorld);
	SafeDelete(m_SceneContext);
}

void AbstractScene::AddEntity(Entity* pEntity)
{
	pEntity->m_pParentScene = this;
	pEntity->RootInitialize();
	m_pEntityVec.push_back(pEntity);
}

void AbstractScene::RemoveEntity(Entity* pEntity, bool deleteEntity)
{
	auto it = find(m_pEntityVec.begin(), m_pEntityVec.end(), pEntity);
	m_pEntityVec.erase(it);
	if (deleteEntity)
	{
		delete pEntity;
		pEntity = nullptr;
	}
	else pEntity->m_pParentScene = nullptr;
}

void AbstractScene::RootInitialize()
{
	if (m_IsInitialized)return;

	//Create SceneContext
	FreeCamera* freeCam = new FreeCamera();
	freeCam->GetTransform()->SetPosition(0, -1, -3.5);
	freeCam->GetTransform()->RotateEuler(etm::radians(20.f), 0, 0);
	AddEntity(freeCam);

	m_SceneContext = new SceneContext();
	m_SceneContext->camera = freeCam->GetComponent<CameraComponent>();
	m_SceneContext->time = new Time();
	m_SceneContext->scene = this;

	CONTEXT->SetContext(m_SceneContext);

	m_PostProcessingSettings = PostProcessingSettings();

	m_pPhysicsWorld = new PhysicsWorld();
	m_pPhysicsWorld->Initialize();

	Initialize();

	for (Entity* pEntity : m_pEntityVec)
	{
		pEntity->RootInitialize();
	}

	m_IsInitialized = true;

	m_SceneContext->time->Start();
}

void AbstractScene::RootUpdate()
{
	m_SceneContext->time->Update();

	PERFORMANCE->StartFrameTimer();

	m_SceneContext->camera->Update();

	Update();
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_UP))
	{
		float exposure = m_PostProcessingSettings.exposure;
		float newExp = exposure * 4.f;
		exposure += (newExp - exposure)*TIME->DeltaTime();
		LOG("Exposure: " + std::to_string(exposure));
		m_PostProcessingSettings.exposure = exposure;
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_DOWN))
	{
		float exposure = m_PostProcessingSettings.exposure;
		float newExp = exposure * 4.f;
		exposure -= (newExp - exposure)*TIME->DeltaTime();
		LOG("Exposure: " + std::to_string(exposure));
		m_PostProcessingSettings.exposure = exposure;
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_LEFT) && m_UseSkyBox)
	{
		float r = std::min(std::max(m_pSkybox->GetRoughness() -TIME->DeltaTime(), 0.f), 1.f);
		LOG("Roughness: " + std::to_string(r));
		m_pSkybox->SetRoughness(r);
	}
	if (INPUT->IsKeyboardKeyDown(SDL_SCANCODE_RIGHT) && m_UseSkyBox)
	{
		float r = std::min(std::max(m_pSkybox->GetRoughness() + TIME->DeltaTime(), 0.f), 1.f);
		LOG("Roughness: " + std::to_string(r));
		m_pSkybox->SetRoughness(r);
	}

	for (Entity* pEntity : m_pEntityVec)
	{
		pEntity->RootUpdate();
	}
	if (m_UseSkyBox)
	{
		m_pSkybox->RootUpdate();
	}

	m_pPhysicsWorld->Update();
}

void AbstractScene::RootOnActivated()
{
	RootInitialize();
	CONTEXT->SetContext(m_SceneContext);
	OnActivated();
}
void AbstractScene::RootOnDeactivated()
{
	OnDeactivated();
}

void AbstractScene::SetActiveCamera(CameraComponent* pCamera)
{
	m_SceneContext->camera = pCamera;
}

std::vector<LightComponent*> AbstractScene::GetLights()
{
	std::vector<LightComponent*> ret;
	for (auto *pEntity : m_pEntityVec)
	{
		auto entityLights = pEntity->GetComponents<LightComponent>();
		ret.insert(ret.end(), entityLights.begin(), entityLights.end());
	}
	return ret;
}

const PostProcessingSettings& AbstractScene::GetPostProcessingSettings() const
{
	//Any settings blending should be done here
	return m_PostProcessingSettings;
}

void AbstractScene::SetSkybox(std::string assetFile)
{
	m_UseSkyBox = true;
	SafeDelete(m_pSkybox);
	m_pSkybox = new Skybox(assetFile);
	m_pSkybox->RootInitialize();
	m_pSkybox->SetRoughness(0.15f);
}

HDRMap* AbstractScene::GetEnvironmentMap()
{
	if (m_UseSkyBox)
	{
		return m_pSkybox->GetHDRMap();
	}
	return nullptr;
}
#pragma once
#include <EtCore/Content/AssetPointer.h>

#include <EtFramework/ECS/EcsController.h>
#include <EtFramework/SceneGraph/ComponentDescriptor.h>


namespace et {
namespace fw {


//---------------------------------
// SpriteComponent
//
// Component that causes a sprite to be drawn in the scene
//
class SpriteComponent final 
{
	// definitions
	//-------------
	ECS_DECLARE_COMPONENT

	friend class SpriteComponent;

	// init deinit
	//-------------
public:
	static void OnComponentAdded(EcsController& controller, SpriteComponent& component, T_EntityId const entity);
	static void OnComponentRemoved(EcsController& controller, SpriteComponent& component, T_EntityId const entity);

	// construct destruct
	//--------------------
	SpriteComponent(core::HashString const textureAsset, vec2 const& pivot = vec2(), vec4 const& color = vec4(1.f));
	~SpriteComponent() = default;

	// accessors
	//-----------
	vec2 GetPivot() const { return m_Pivot; }
	vec4 const& GetColor() const { return m_Color; }

	// modifiers
	//-----------
	void SetPivot(vec2 const& pivot);
	void SetColor(vec4 const& color);
	void SetTexture(core::HashString const textureAsset);

	// Data
	///////

private:
	core::T_SlotId m_Id = core::INVALID_SLOT_ID;
	core::HashString m_TextureAssetId;
	vec2 m_Pivot;
	vec4 m_Color;
};

//---------------------------------
// SpriteComponentDesc
//
// Descriptor for serialization and deserialization of sprite components
//
class SpriteComponentDesc final : public ComponentDescriptor<SpriteComponent>
{
	// definitions
	//-------------
	RTTR_ENABLE(ComponentDescriptor<SpriteComponent>)

	// construct destruct
	//--------------------
public:
	SpriteComponentDesc() : ComponentDescriptor<SpriteComponent>() {}
	~SpriteComponentDesc() = default;

	// ComponentDescriptor interface
	//-------------------------------
	SpriteComponent* MakeData() override;

	// Data
	///////

	core::HashString textureAsset;
	vec2 pivot;
	vec4 color;
};


} // namespace fw
} // namespace et

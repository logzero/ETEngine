#pragma once
#include <rttr/type>

#include <EtFramework/ECS/RawComponentPointer.h>
#include <EtFramework/ECS/EntityFwd.h>


namespace fw {


class EcsController;


//-----------------------
// I_ComponentDescriptor
//
// Abstract class for generating components from data
//
class I_ComponentDescriptor
{
	// definitions
	//-------------
	RTTR_ENABLE()

	// construct destruct
	//--------------------
public:
	virtual ~I_ComponentDescriptor() = default;

	// interface
	//-----------
	virtual T_CompTypeIdx GetType() const = 0;
	virtual RawComponentPtr MakeRawData() = 0; // has ownership
	virtual bool CallScenePostLoad() const = 0;
	virtual void OnScenePostLoadRoot(EcsController& ecs, T_EntityId const id, void* const componentData) = 0;
};

//---------------------
// ComponentDescriptor
//
// Class to create a specific component descriptor
//
template<class TComponentType, bool TCallPostLoad = false>
class ComponentDescriptor : public I_ComponentDescriptor
{
	// definitions
	//-------------
	RTTR_ENABLE(I_ComponentDescriptor)

	// construct destruct
	//--------------------
public:
	ComponentDescriptor() : I_ComponentDescriptor() {}
	virtual ~ComponentDescriptor() = default;

	// I_ComponentDescriptor interface
	//---------------------------------
	T_CompTypeIdx GetType() const override { return TComponentType::GetTypeIndex(); }
	RawComponentPtr MakeRawData() override { return RawComponentPtr(GetType(), reinterpret_cast<void*>(MakeData())); }
	bool CallScenePostLoad() const override { return TCallPostLoad; }
	void OnScenePostLoadRoot(EcsController& ecs, T_EntityId const id, void* const componentData) 
	{ 
		OnScenePostLoad(ecs, id, *static_cast<TComponentType*>(componentData)); 
	}

	// interface
	//-----------
	virtual TComponentType* MakeData() = 0;
	virtual void OnScenePostLoad(EcsController&, T_EntityId const, TComponentType&) {}
};


} // namespace fw
#pragma once
#include <rttr/type.h>


namespace et {
namespace fw {


// definitions
//*************
typedef uint16 T_CompTypeIdx; 
static constexpr T_CompTypeIdx INVALID_COMP_TYPE_IDX = std::numeric_limits<T_CompTypeIdx>::max();


//--------------------
// ComponentRegistry
//
// Contains a static enumeration of component info
//
class ComponentRegistry
{
	// definitions
	//-------------
	typedef void (*T_CompCopyAssign)(void const* const, void* const); // source, target
	typedef void (*T_CompDestructor)(void const* const);

	struct ComponentTypeInfo
	{
	public:
		ComponentTypeInfo(rttr::type const& t, size_t const size) : type(t), data_size(size) {}

		rttr::type type;
		size_t data_size;
		T_CompCopyAssign copyAssign = nullptr;
		T_CompDestructor destructor = nullptr;
		T_CompDestructor fullDestructor = nullptr;
	};

public:
	static constexpr T_CompTypeIdx s_InvalidTypeIdx = std::numeric_limits<T_CompTypeIdx>::max();

	// static access
	//---------------
	static ComponentRegistry& Instance();

	// construct destruct
	//--------------------
private:
	ComponentRegistry() = default;
public:
	ComponentRegistry(ComponentRegistry const&) = delete;
	void operator=(ComponentRegistry const&) = delete;

	// functionality
	//---------------
	template<typename TComponentType>
	T_CompTypeIdx Register(); // returns the type index

	// accessors
	//-----------
	size_t GetCount() const { return m_ComponentTypes.size(); }

	size_t GetSize(T_CompTypeIdx const idx) const;
	rttr::type const& GetType(T_CompTypeIdx const idx) const;
	T_CompCopyAssign GetCopyAssign(T_CompTypeIdx const idx) const;
	T_CompDestructor GetDestructor(T_CompTypeIdx const idx) const;
	T_CompDestructor GetFullDestructor(T_CompTypeIdx const idx) const;

	T_CompTypeIdx GetTypeIdx(rttr::type const& type) const;

	// Data
	///////

private:
	std::vector<ComponentTypeInfo> m_ComponentTypes;
};


// Component type list
typedef std::vector<T_CompTypeIdx> T_CompTypeList;

template<typename... Args>
T_CompTypeList GenCompTypeList();


} // namespace fw
} // namespace et


// macros
//********

//-----------------------
// ECS_DECLARE_COMPONENT
//
// Declare this in a components header to add type info
//
#define ECS_DECLARE_COMPONENT \
private: \
static et::fw::T_CompTypeIdx const s_TypeIndex; \
public: \
static et::fw::T_CompTypeIdx GetTypeIndex() { return s_TypeIndex; } \
private:

//------------------------
// ECS_REGISTER_COMPONENT
//
// Declare this in a components source file after registering it with RTTR
//
#define ECS_REGISTER_COMPONENT(TComponentType) \
et::fw::T_CompTypeIdx const TComponentType::s_TypeIndex = et::fw::ComponentRegistry::Instance().Register<TComponentType>();


#include "ComponentRegistry.inl"

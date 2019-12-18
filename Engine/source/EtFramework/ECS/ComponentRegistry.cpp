#include "stdafx.h"
#include "ComponentRegistry.h"


namespace framework {


//====================
// Component Registry
//====================


//-----------------------------
// ComponentRegistry::Instance
//
// Global singleton access
//
ComponentRegistry& ComponentRegistry::Instance()
{
	static ComponentRegistry instance; 
	return instance;
}

//----------------------------
// ComponentRegistry::GetSize
//
// Size in bytes required to store a component
//
size_t ComponentRegistry::GetSize(T_CompTypeIdx const idx) const
{
	ET_ASSERT(static_cast<size_t>(idx) < m_ComponentTypes.size());

	return m_ComponentTypes[idx].data_size;
}

//----------------------------
// ComponentRegistry::GetType
//
// Any other type info related to a component
//
rttr::type const& ComponentRegistry::GetType(T_CompTypeIdx const idx) const
{
	ET_ASSERT(static_cast<size_t>(idx) < m_ComponentTypes.size());

	return m_ComponentTypes[idx].type;
}

//-------------------------------
// ComponentRegistry::GetTypeIdx
//
T_CompTypeIdx ComponentRegistry::GetTypeIdx(rttr::type const& type) const
{
	auto const foundTiIt = std::find_if(m_ComponentTypes.cbegin(), m_ComponentTypes.cend(), [type](ComponentTypeInfo const& info)
		{
			return info.type == type;
		});

	if (foundTiIt == m_ComponentTypes.cend())
	{
		return s_InvalidTypeIdx;
	}

	return static_cast<T_CompTypeIdx>(foundTiIt - m_ComponentTypes.cbegin());
}


} // namespace framework

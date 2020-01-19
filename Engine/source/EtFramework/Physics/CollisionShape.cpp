#include "stdafx.h"
#include "CollisionShape.h"

#include <btBulletDynamicsCommon.h>

#include <EtFramework/Physics/BulletETM.h>
#include <EtFramework/Physics/PhysicsWorld.h>
#include <EtFramework/Physics/PhysicsManager.h>


namespace fw {


//=================
// Collision Shape
//=================


// reflection
//------------

RTTR_REGISTRATION
{
	BEGIN_REGISTER_POLYMORPHIC_CLASS(BoxShape, "box collider shape")
		.property("half extents", &BoxShape::m_HalfExtents)
	END_REGISTER_POLYMORPHIC_CLASS(BoxShape, CollisionShape);

	BEGIN_REGISTER_POLYMORPHIC_CLASS(SphereShape, "sphere collider shape")
		.property("radius", &SphereShape::m_Radius)
	END_REGISTER_POLYMORPHIC_CLASS(SphereShape, CollisionShape);
}

DEFINE_FORCED_LINKING(fw::BoxShape) // force the linker to include this unit


//------------------------------------
// BoxShape::MakeBulletCollisionShape
//
btCollisionShape* BoxShape::MakeBulletCollisionShape() const
{
	return PhysicsManager::GetInstance()->CreateBoxShape(m_HalfExtents);
}

//---------------------------------------
// SphereShape::MakeBulletCollisionShape
//
btCollisionShape* SphereShape::MakeBulletCollisionShape() const
{
	return PhysicsManager::GetInstance()->CreateSphereShape(m_Radius);
}


} // namespace fw

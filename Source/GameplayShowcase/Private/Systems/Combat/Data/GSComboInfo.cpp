// Copyright (c) 2025 Dawid Szoldra. All rights reserved.


#include "Systems/Combat/Data/GSComboInfo.h"

void FLineTrace::MakeShape()
{
	// Do nothing, its default
}

void FSphereTrace::MakeShape()
{
	Shape.SetSphere(Radius);
}

void FCapsuleTrace::MakeShape()
{
	Shape.SetCapsule(Radius, HalfHeight);
}

void FBoxTrace::MakeShape()
{
	Shape.SetBox(HalfExtent);
}

FCollisionShape UGSComboInfo::GetTraceShape()
{
	if (!bShapeSet)
	{
		bShapeSet = true;
	}
	
	if (FShapeInfoBase* ShapePtr = TraceShapeInfo.GetMutablePtr<FShapeInfoBase>())
	{
		ShapePtr->MakeShape();
		return ShapePtr->Shape;
	}
	// Default shape
	return FCollisionShape::MakeSphere(10.f);	
	
}

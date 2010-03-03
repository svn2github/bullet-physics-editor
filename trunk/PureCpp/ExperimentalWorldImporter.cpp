/*
 *  ExperimentalWorldImporter.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 3/3/10.
 *  For license see LICENSE.TXT
 *
 */

#include "ExperimentalWorldImporter.h"

ExperimentalWorldImporter::ExperimentalWorldImporter(btDynamicsWorld *world) : btBulletWorldImporter(world)
{
	m_fixedBody = new btRigidBody(0,0,0);
}

ExperimentalWorldImporter::~ExperimentalWorldImporter()
{
	delete m_fixedBody;
}

btRigidBody*  ExperimentalWorldImporter::createRigidBody(bool isDynamic, 
														 btScalar mass, 
														 const btTransform& startTransform,
														 btCollisionShape* shape,
														 const char* bodyName)
{
	if (bodyName)
	{
		bodiesNames.push_back(bodyName);
	}
	else
	{
		bodiesNames.push_back("UnnamedBody");
	}
	
	return btBulletWorldImporter::createRigidBody(isDynamic, mass, startTransform, shape, bodyName);
}

btGeneric6DofConstraint* ExperimentalWorldImporter::createGeneric6DofConstraint(btRigidBody& rbB, const btTransform& frameInB, bool useLinearReferenceFrameB)
{
	btTransform frameInA;
	frameInA = rbB.getCenterOfMassTransform() * frameInB;
	return btBulletWorldImporter::createGeneric6DofConstraint(*m_fixedBody,rbB, frameInA,frameInB,useLinearReferenceFrameB);
}

btSliderConstraint* ExperimentalWorldImporter::createSliderConstraint(btRigidBody& rbB, const btTransform& frameInB, bool useLinearReferenceFrameA)
{
	btTransform frameInA;
	frameInA = rbB.getCenterOfMassTransform() * frameInB;
	return btBulletWorldImporter::createSliderConstraint(*m_fixedBody,rbB,frameInA,frameInB,useLinearReferenceFrameA);
}

btConeTwistConstraint* ExperimentalWorldImporter::createConeTwistConstraint(btRigidBody& rbA,const btTransform& rbAFrame)
{
	return btBulletWorldImporter::createConeTwistConstraint(rbA, *m_fixedBody,rbAFrame,rbAFrame);
}

btHingeConstraint* ExperimentalWorldImporter::createHingeConstraint(btRigidBody& rbA,const btTransform& rbAFrame, bool useReferenceFrameA)
{
	///not providing rigidbody B means implicitly using worldspace for body B
	btTransform rbBFrame;
	rbBFrame = rbAFrame;
	rbBFrame.getOrigin() = rbA.getCenterOfMassTransform()(rbAFrame.getOrigin());
	return btBulletWorldImporter::createHingeConstraint(rbA,*m_fixedBody,rbAFrame,rbBFrame,useReferenceFrameA);
}

btPoint2PointConstraint* ExperimentalWorldImporter::createPoint2PointConstraint(btRigidBody& rbA,const btVector3& pivotInA)
{
	btVector3 pivotInB(rbA.getCenterOfMassTransform()(pivotInA));
	return btBulletWorldImporter::createPoint2PointConstraint(rbA,*m_fixedBody,pivotInA,pivotInB);
}

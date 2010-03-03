/*
 *  ExperimentalWorldImporter.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 3/3/10.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#ifdef WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#endif

#include <vector>
#include <string>
using namespace std;
#include "Enums.h"

#include "btBulletFile.h"
#include "Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"
#include "BulletCollision/BroadphaseCollision/btDispatcher.h"
#include "BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btDefaultMotionState.h"
#include "GL_ShapeDrawerClone.h"
#include "LinearMath/btSerializer.h"
using namespace bParse;

class ExperimentalWorldImporter : public btBulletWorldImporter
{
private:
	btRigidBody* m_fixedBody;
public:
	vector<string> bodiesNames;
	
	ExperimentalWorldImporter(btDynamicsWorld *world);
	virtual ~ExperimentalWorldImporter();
	
	virtual btRigidBody*  createRigidBody(bool isDynamic, 
										  btScalar mass, 
										  const btTransform& startTransform,
										  btCollisionShape* shape,
										  const char* bodyName);
	
	virtual btGeneric6DofConstraint* createGeneric6DofConstraint(btRigidBody& rbB, const btTransform& frameInB, bool useLinearReferenceFrameB);
	virtual btSliderConstraint* createSliderConstraint(btRigidBody& rbB, const btTransform& frameInB, bool useLinearReferenceFrameA);
	virtual btConeTwistConstraint* createConeTwistConstraint(btRigidBody& rbA,const btTransform& rbAFrame);
	virtual btHingeConstraint* createHingeConstraint(btRigidBody& rbA,const btTransform& rbAFrame, bool useReferenceFrameA);
	virtual btPoint2PointConstraint* createPoint2PointConstraint(btRigidBody& rbA,const btVector3& pivotInA);	
};

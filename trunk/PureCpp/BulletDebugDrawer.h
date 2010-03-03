/*
 *  BulletDebugDrawer.h
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

class BulletDebugDrawer : public btIDebugDraw
{
private:
	DebugDrawModes debugDrawMode;
public:
	BulletDebugDrawer();
	
	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
	
	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
	
	virtual void	reportErrorWarning(const char* warningString);
	
	virtual void	draw3dText(const btVector3& location,const char* textString);
	
	virtual void	setDebugMode(int debugMode);
	
	virtual int		getDebugMode() const;
};

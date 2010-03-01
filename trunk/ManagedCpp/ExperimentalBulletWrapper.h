/*
 *  ExperimentalBulletWrapper.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 04/02/10.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <string>
using namespace std;
#include "../PureCpp/Enums.h"
#include "OpenGLManipulatingModel.h"

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
#include "../PureCpp/GL_ShapeDrawerClone.h"
#include "LinearMath/btSerializer.h"
using namespace bParse;

class BulletDebugDrawer : public btIDebugDraw
{
private:
	DebugDrawModes debugDrawMode;
public:
	BulletDebugDrawer() 
	{
		debugDrawMode = DBG_MAX_DEBUG_DRAW_MODE;
	}
	
	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
	{
		glColor3f(color.x(), color.y(), color.z());
		glBegin(GL_LINES);
		glVertex3f(from.x(), from.y(), from.z());
		glVertex3f(to.x(), to.y(), to.z());
		glEnd();
	}
	
	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
	{
		// ignored
	}
	
	virtual void	reportErrorWarning(const char* warningString) 
	{
		//NSLog(@"Warning from Bullet: %s", warningString);
	}
	
	virtual void	draw3dText(const btVector3& location,const char* textString) 
	{
		// ignored
	}
	
	virtual void	setDebugMode(int debugMode)
	{
		debugDrawMode = (DebugDrawModes)debugMode;
	}
	
	virtual int		getDebugMode() const
	{
		return debugDrawMode;
	}
};

class ExperimentalWorldImporter : public btBulletWorldImporter
{
public:
	vector<string> bodiesNames;
	
	ExperimentalWorldImporter(btDynamicsWorld *world) : btBulletWorldImporter(world)
	{
		
	}
	
	virtual btRigidBody*  createRigidBody(bool isDynamic, 
										  btScalar mass, 
										  const btTransform& startTransform,
										  btCollisionShape* shape,
										  const char* bodyName)
	{
		//NSLog(@"Created Rigid Body: %s", bodyName);
		if (bodyName)
		{
			bodiesNames.push_back(bodyName);
		} else
		{
			bodiesNames.push_back("UnnamedBody");
		}
		return btBulletWorldImporter::createRigidBody(isDynamic, mass, startTransform, shape, bodyName);
	}
	
};

class BulletWrapperHelper
{
public:
	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btDbvtBroadphase *broadphase;
	btSequentialImpulseConstraintSolver *solver;
	btDynamicsWorld *dynamicsWorld;
	ExperimentalWorldImporter *worldImporter;
	GL_ShapeDrawer *shapeDrawer;
	BulletDebugDrawer *debugDrawer; 
	vector<CocoaBool> *selection;

	BulletWrapperHelper()
	{
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		broadphase = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
		dynamicsWorld->setGravity(btVector3(0, -10, 0));	
		worldImporter = new ExperimentalWorldImporter(dynamicsWorld);
		shapeDrawer = new GL_ShapeDrawer();
		debugDrawer = new BulletDebugDrawer();
		dynamicsWorld->setDebugDrawer(debugDrawer);
		selection = new vector<CocoaBool>();
	}

	~BulletWrapperHelper()
	{
		delete collisionConfiguration;
		delete dispatcher;
		delete broadphase;
		delete solver;
		delete dynamicsWorld;
		delete worldImporter;
		delete shapeDrawer;
		delete debugDrawer;
		delete selection;
	}

	bool LoadFile(const char *fileName)
	{
		if (worldImporter->loadFile(fileName))
		{
			for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
			{
				selection->push_back(NO);
			}
			return true;
		}
		return false;
	}

	void DrawSolid()
	{
		for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
			this->Draw(i, NO);		
	}

	void DrawWireframe()
	{
		glDisable(GL_LIGHTING);
		dynamicsWorld->debugDrawWorld();
	}

	void Draw(uint index, bool selected)
	{
		btScalar m[16];
		
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		btRigidBody *body = btRigidBody::upcast(colObj);
		
		if (body && body->getMotionState())
		{
			btDefaultMotionState *myMotionState = (btDefaultMotionState *)body->getMotionState();
			myMotionState->m_graphicsWorldTrans.getOpenGLMatrix(m);
		}
		else
		{
			colObj->getWorldTransform().getOpenGLMatrix(m);
		}
		btVector3 wireColor(1.0f, 1.0f, 0.5f); //wants deactivation
		if (index & 1) 
			wireColor = btVector3(0.0f, 0.0f, 1.0f);
		///color differently for active, sleeping, wantsdeactivation states
		if (colObj->getActivationState() == 1) //active
		{
			if (index & 1)
				wireColor += btVector3(1.0f, 0.0f, 0.0f);
			else
				wireColor += btVector3(0.5f, 0.0f, 0.0f);
		}
		if (colObj->getActivationState() == 2) //ISLAND_SLEEPING
		{
			if (index & 1)
				wireColor += btVector3(0.0f, 1.0f, 0.0f);
			else
				wireColor += btVector3(0.0f, 0.5f, 0.0f);
		}
		
		btVector3 aabbMin,aabbMax;
		dynamicsWorld->getBroadphase()->getBroadphaseAabb(aabbMin, aabbMax);
		
		aabbMin -= btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
		aabbMax += btVector3(BT_LARGE_FLOAT, BT_LARGE_FLOAT, BT_LARGE_FLOAT);
		
		shapeDrawer->enableTexture(true);
		
		if (selected)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
			shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, 0, aabbMin, aabbMax);
			glDisable(GL_POLYGON_OFFSET_FILL);
			wireColor = btVector3(0.5f, 0.5f, 0.5f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, 0, aabbMin, aabbMax);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			shapeDrawer->drawOpenGL(m, colObj->getCollisionShape(), wireColor, 0, aabbMin, aabbMax);
		}
	}

	Vector3D GetPosition(uint index)
	{
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		btVector3 origin = colObj->getWorldTransform().getOrigin();
		return Vector3D(origin.x(), origin.y(), origin.z());
	}

	void SetPosition(Vector3D position, uint index)
	{
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		colObj->getWorldTransform().setOrigin(btVector3(position.x, position.y, position.z));
		colObj->activate(true); // added activation mentioned in issue 6
	}

	Quaternion GetRotation(uint index)
	{
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		btQuaternion rotation = colObj->getWorldTransform().getRotation();
		return Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
	}

	void SetRotation(Quaternion rotation, uint index)
	{
		btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
		colObj->getWorldTransform().setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
		colObj->activate(true); // added activation mentioned in issue 6
	}

	string GetName(uint index)
	{
		return worldImporter->bodiesNames.at(index);
	}

	void Save(const char *fileName)
	{
		//create a large enough buffer. There is no method to pre-calculate the buffer size yet.
		int maxSerializeBufferSize = 1024 * 1024 * 5;
	
		btDefaultSerializer *serializer = new btDefaultSerializer(maxSerializeBufferSize);
		dynamicsWorld->serialize(serializer);
	
		FILE *file = fopen(fileName, "wb");
		fwrite(serializer->getBufferPointer(), serializer->getCurrentBufferSize(), 1, file);
		fclose(file);
	
		delete serializer;
	}
};

#pragma managed

namespace ManagedCpp
{
	public ref class ExperimentalBulletWrapper : OpenGLManipulatingModelItem
	{
	private:
		BulletWrapperHelper *wrapper;
	public:
		ExperimentalBulletWrapper();
		void StepSimulation(btScalar timeStep);
		void Load(String ^fileName);
		void Save(String ^fileName);

		virtual property uint Count { uint get(); }
		virtual	CocoaBool IsSelected(uint index);
		virtual	void SetSelected(CocoaBool selected, uint index);
		virtual void Draw(ViewMode mode);
		virtual	void Draw(uint index, CocoaBool forSelection, ViewMode mode);
		virtual	void CloneSelected();
		virtual	void RemoveSelected();
		
		virtual	void WillSelect();
		virtual	void DidSelect();
		virtual String^ GetName(uint index);

		virtual Vector3D GetPosition(uint index);
		virtual Quaternion GetRotation(uint index);
		virtual Vector3D GetScale(uint index);
		virtual void SetPosition(Vector3D position, uint index);
		virtual void SetRotation(Quaternion rotation, uint index);
		virtual void SetScale(Vector3D scale, uint index);
		virtual void MoveBy(Vector3D offset, uint index);
		virtual void RotateBy(Quaternion offset, uint index);
		virtual void ScaleBy(Vector3D offset, uint index);
	};
}
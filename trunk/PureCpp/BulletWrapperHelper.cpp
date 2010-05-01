/*
 *  BulletWrapperHelper.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 3/3/10.
 *  For license see LICENSE.TXT
 *
 */

#include "BulletWrapperHelper.h"

BulletWrapperHelper::BulletWrapperHelper()
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
	selection = new vector<bool>();
}

BulletWrapperHelper::~BulletWrapperHelper()
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

bool BulletWrapperHelper::Load(const char *fileName)
{
	if (worldImporter->loadFile(fileName))
	{
		for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
		{
			selection->push_back(true);
		}
		return true;
	}
	return false;
}

void BulletWrapperHelper::Save(const char *fileName)
{
	//create a large enough buffer. There is no method to pre-calculate the buffer size yet.
	int maxSerializeBufferSize = 1024 * 1024 * 5;
	
	btDefaultSerializer *serializer = new btDefaultSerializer(maxSerializeBufferSize);
	btCollisionObjectArray& collisionObjects = dynamicsWorld->getCollisionObjectArray();
	
	for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
		serializer->registerNameForPointer(collisionObjects[i], worldImporter->bodiesNames[i].c_str());
		
	dynamicsWorld->serialize(serializer);
	
	FILE *file = fopen(fileName, "wb");
	fwrite(serializer->getBufferPointer(), serializer->getCurrentBufferSize(), 1, file);
	fclose(file);
	
	delete serializer;
}

void BulletWrapperHelper::DrawSolid()
{
	for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
		this->Draw(i, false);		
}

void BulletWrapperHelper::DrawWireframe()
{
	glDisable(GL_LIGHTING);
	dynamicsWorld->debugDrawWorld();
}

void BulletWrapperHelper::Draw(uint index, bool selected)
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

Vector3D BulletWrapperHelper::GetPosition(uint index)
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	btVector3 origin = colObj->getWorldTransform().getOrigin();
	return Vector3D(origin.x(), origin.y(), origin.z());
}

void BulletWrapperHelper::SetPosition(Vector3D position, uint index)
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	colObj->getWorldTransform().setOrigin(btVector3(position.x, position.y, position.z));
	colObj->activate(true); // added activation mentioned in issue 6
}

Quaternion BulletWrapperHelper::GetRotation(uint index)
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	btQuaternion rotation = colObj->getWorldTransform().getRotation();
	return Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
}

void BulletWrapperHelper::SetRotation(Quaternion rotation, uint index)
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	colObj->getWorldTransform().setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	colObj->activate(true); // added activation mentioned in issue 6
}

string BulletWrapperHelper::GetShapeType(uint index)
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	return colObj->getCollisionShape()->getName();
}

float BulletWrapperHelper::GetMass(uint index)
{
	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	btRigidBody* body = btRigidBody::upcast(colObj);
	if (body)
	{
		return body->getInvMass() ? 1./body->getInvMass() : 0.f;
	}
	return 0.f;
}

void BulletWrapperHelper::SetMass(float mass, uint index)
{
/*	btCollisionObject *colObj = dynamicsWorld->getCollisionObjectArray()[index];
	if (mass)
	{
			colObj->getCollisionShape()->setM>getMass() ? 1./colObj->getCollisionShape()->getMass() : 0.f;
	} else
	{
	}
 */
}


string BulletWrapperHelper::GetName(uint index)
{
	return worldImporter->bodiesNames.at(index);
}

void BulletWrapperHelper::SetName(string name, uint index)
{
	worldImporter->bodiesNames.at(index) = name;
}

void BulletWrapperHelper::RemoveSelected()
{
	for (int i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++)
	{
		if (selection->at(i))
		{
			btCollisionObject* colObj = dynamicsWorld->getCollisionObjectArray()[i];
			//also remove the constraints if any
			btRigidBody* body = btRigidBody::upcast(colObj);
			if (body)
			{
				while (body->getNumConstraintRefs())
				{
					btTypedConstraint* constraint = body->getConstraintRef(0);
					dynamicsWorld->removeConstraint(constraint);
					delete constraint;
				}
			}

			dynamicsWorld->removeCollisionObject(colObj);
			selection->erase(selection->begin() + i);
			
			delete colObj;
			i--;
		}
	}
}

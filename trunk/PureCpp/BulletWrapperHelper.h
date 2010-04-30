/*
 *  BulletWrapperHelper.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 3/3/10.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "BulletDebugDrawer.h"
#include "ExperimentalWorldImporter.h"
#include "MathCore/MathDeclaration.h"

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
	vector<bool> *selection;

	BulletWrapperHelper();
	~BulletWrapperHelper();
	
	bool Load(const char *fileName);
	void Save(const char *fileName);

	void DrawSolid();
	void DrawWireframe();
	void Draw(uint index, bool selected);
	
	Vector3D GetPosition(uint index);
	void SetPosition(Vector3D position, uint index);
	Quaternion GetRotation(uint index);
	void SetRotation(Quaternion rotation, uint index);
	
	string GetShapeType(uint index);

	float GetMass(uint index);
	void SetMass(float value, uint index);


	string GetName(uint index);
	void SetName(string name, uint index);
	
	void RemoveSelected();
};
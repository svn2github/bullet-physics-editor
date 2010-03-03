/*
 *  BulletDebugDrawer.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 3/3/10.
 *  For license see LICENSE.TXT
 *
 */

#include "BulletDebugDrawer.h"

BulletDebugDrawer::BulletDebugDrawer() 
{
	debugDrawMode = DBG_MAX_DEBUG_DRAW_MODE;
}

void BulletDebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
	glColor3f(color.x(), color.y(), color.z());
	glBegin(GL_LINES);
	glVertex3f(from.x(), from.y(), from.z());
	glVertex3f(to.x(), to.y(), to.z());
	glEnd();
}

void BulletDebugDrawer::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
	// ignored
}

void BulletDebugDrawer::reportErrorWarning(const char* warningString) 
{
	// ignored
}

void BulletDebugDrawer::draw3dText(const btVector3& location,const char* textString) 
{
	// ignored
}

void BulletDebugDrawer::setDebugMode(int debugMode)
{
	debugDrawMode = (DebugDrawModes)debugMode;
}

int BulletDebugDrawer::getDebugMode() const
{
	return debugDrawMode;
}


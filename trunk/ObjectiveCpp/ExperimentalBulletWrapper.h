//
//  ExperimentalBulletWrapper.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/31/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <vector>
using namespace std;
#import "OpenGLManipulatingModel.h"

#import "btBulletFile.h"
#import "btBulletWorldImporter.h"
#import "btDynamicsWorld.h"
#import "btDiscreteDynamicsWorld.h"
#import "btDispatcher.h"
#import "btDefaultCollisionConfiguration.h"
#import "btDbvtBroadphase.h"
#import "btSequentialImpulseConstraintSolver.h"
#import "btIDebugDraw.h"
#import "btDefaultMotionState.h"
#import "GL_ShapeDrawerClone.h"
#import "btSerializer.h"
using namespace bParse;

class Transform
{
public:
	Vector3D position;
	Quaternion rotation;
	
	Transform(const btVector3& position, const btQuaternion& rotation)
	{
		this->position = Vector3D(position.x(), position.y(), position.z());
		this->rotation = Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
	}
	
	Matrix4x4 ToMatrix()
	{
		Matrix4x4 t, r;
		t.Translate(position);
		rotation.ToMatrix(r);
		return t * r;
	}
	
	btVector3 ToBulletVector3()
	{
		return btVector3(position.x, position.y, position.z);
	}
	
	btQuaternion ToBulletQuaternion()
	{
		return btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
	}
};

class BulletDebugDraw : public btIDebugDraw
{
private:
	DebugDrawModes debugDrawMode;
public:
	BulletDebugDraw() 
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
		NSLog(@"Warning from Bullet: %s", warningString);
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

@interface ExperimentalBulletWrapper : NSObject <OpenGLManipulatingModelItem>
{
	btDefaultCollisionConfiguration *collisionConfiguration;
	btCollisionDispatcher *dispatcher;
	btDbvtBroadphase *broadphase;
	btSequentialImpulseConstraintSolver *solver;
	btDynamicsWorld *dynamicsWorld;
	btBulletWorldImporter *worldImporter;
	GL_ShapeDrawer *shapeDrawer;
	BulletDebugDraw *debugDrawer; 
	vector<BOOL> *selection;
	vector<Transform> *transforms;	
}

- (id)initWithFileName:(NSString *)fileName;
- (void)stepSimulationWithTimeInterval:(NSTimeInterval)timeInterval;
- (void)saveWithFileName:(NSString *)fileName;

@end

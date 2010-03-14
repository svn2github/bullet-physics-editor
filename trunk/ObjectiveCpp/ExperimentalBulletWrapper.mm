//
//  ExperimentalBulletWrapper.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/31/10.
//  For license see LICENSE.TXT
//

#import "ExperimentalBulletWrapper.h"

@implementation ExperimentalBulletWrapper

- (id)initWithFileName:(NSString *)fileName
{
	self = [super init];
	if (self)
	{
		wrapper = new BulletWrapperHelper();
		if (wrapper->Load([fileName cStringUsingEncoding:NSASCIIStringEncoding]))
			return self;

		// loading failed
		[self dealloc];
		return nil;		
	}
	return self;
}

- (void)dealloc
{
	delete wrapper;
	[super dealloc];	
}

- (void)stepSimulationWithTimeInterval:(NSTimeInterval)timeInterval
{
	wrapper->dynamicsWorld->stepSimulation(timeInterval);
}

- (void)saveWithFileName:(NSString *)fileName
{
	wrapper->Save([fileName cStringUsingEncoding:NSASCIIStringEncoding]);
}

#pragma mark OpenGLManipulatingModel implementation

- (void)drawWithMode:(enum ViewMode)mode
{
	switch (mode) 
	{
		case ViewModeSolid:
			wrapper->DrawSolid();
			break;
		case ViewModeWireframe:
			wrapper->DrawWireframe();
			break;
		default:
			break;
	}
}

- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection withMode:(enum ViewMode)mode
{
	wrapper->Draw(index, !forSelection && [self isSelectedAtIndex:index]);
}

- (uint)count
{ 
	return (uint)wrapper->dynamicsWorld->getNumCollisionObjects();
}

- (void)removeSelected 
{
	wrapper->RemoveSelected();
}

- (void)cloneSelected
{
	NSLog(@"cloneSelected is not supported");
}

- (void)setSelected:(BOOL)selected atIndex:(uint)index 
{
	wrapper->selection->at(index) = selected;
}

- (BOOL)isSelectedAtIndex:(uint)index
{ 
	return wrapper->selection->at(index);
}

#pragma mark OpenGLManipulatingModelItem implementation

- (Vector3D)positionAtIndex:(uint)index
{
	return wrapper->GetPosition(index);
}

- (Quaternion)rotationAtIndex:(uint)index
{
	return wrapper->GetRotation(index);
}

- (Vector3D)scaleAtIndex:(uint)index
{
	return Vector3D(1, 1, 1); // ignored
}

- (void)setPosition:(Vector3D)position atIndex:(uint)index
{
	wrapper->SetPosition(position, index);
}

- (void)setRotation:(Quaternion)rotation atIndex:(uint)index
{
	wrapper->SetRotation(rotation, index);
}

- (void)setScale:(Vector3D)scale atIndex:(uint)index
{
	// ignored
}

- (void)moveByOffset:(Vector3D)offset atIndex:(uint)index
{
	Vector3D position = [self positionAtIndex:index];
	position += offset;
	[self setPosition:position atIndex:index];
}

- (void)rotateByOffset:(Quaternion)offset atIndex:(uint)index
{
	Quaternion rotation = [self rotationAtIndex:index];
	rotation = offset * rotation;
	[self setRotation:rotation atIndex:index];
}

- (void)scaleByOffset:(Vector3D)offset atIndex:(uint)index
{
	// ignored
}

- (NSString *)nameAtIndex:(uint)index
{
	string name = wrapper->GetName(index);
	return [NSString stringWithCString:name.c_str() encoding:NSASCIIStringEncoding];
}

- (void)setName:(NSString *)name atIndex:(uint)index
{
	string newName = [name cStringUsingEncoding:NSASCIIStringEncoding];
	wrapper->SetName(newName, index);
}

@end

//
//  ExperimentalBulletWrapper.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/31/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "OpenGLManipulatingModel.h"
#import "BulletDebugDrawer.h"
#import "ExperimentalWorldImporter.h"
#import "BulletWrapperHelper.h"

@interface ExperimentalBulletWrapper : NSObject <OpenGLManipulatingModelItem>
{
	BulletWrapperHelper *wrapper;
}

- (id)initWithFileName:(NSString *)fileName;
- (void)stepSimulationWithTimeInterval:(NSTimeInterval)timeInterval;
- (void)saveWithFileName:(NSString *)fileName;

@end

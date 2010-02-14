//
//  MyDocument.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import "OpenGLSceneView.h"
#import "PropertyReflector.h"
#import "OpenGLManipulatingController.h"
#import "ExperimentalBulletWrapper.h"

@interface MyDocument : NSDocument <OpenGLSceneViewDelegate, OpenGLTransforming>
{
@public // public for unit tests
	BOOL manipulationFinished;
	id<OpenGLManipulating> manipulated;
	IBOutlet OpenGLSceneView *viewLeft;
	IBOutlet OpenGLSceneView *viewTop;
	IBOutlet OpenGLSceneView *viewFront;
	IBOutlet OpenGLSceneView *viewPerspective;
	IBOutlet NSPopUpButton *viewModePopUp;
	
	NSMutableArray *views;	
	IBOutlet NSSplitView *topSplit;
	IBOutlet NSSplitView *bottomSplit;
	IBOutlet NSSplitView *mainSplit;
	BOOL oneView;
	
	PropertyReflector *propertyReflector;
	IBOutlet NSTableView *propertyView;
	
	ExperimentalBulletWrapper *bulletWrapper;
	BOOL simulationRunning;
	OpenGLManipulatingController *bulletController;
}

@property (readwrite, assign) id<OpenGLManipulating> manipulated;

- (IBAction)changeManipulator:(id)sender;
- (IBAction)changeViewMode:(id)sender;
- (IBAction)selectAll:(id)sender;
- (IBAction)invertSelection:(id)sender;
- (void)toggleOneViewFourView:(id)sender;
- (IBAction)play:(id)sender;
- (IBAction)pause:(id)sender;

@end

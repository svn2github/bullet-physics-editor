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

#ifdef MAC_OS_X_VERSION_10_6
@interface MyDocument : NSDocument <OpenGLSceneViewDelegate, OpenGLTransforming, NSTableViewDataSource, NSTableViewDelegate>
#else
@interface MyDocument : NSDocument <OpenGLSceneViewDelegate, OpenGLTransforming>
#endif
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
	OpenGLSceneView *oneView;
	
	PropertyReflector *propertyReflector;
	IBOutlet NSTableView *propertyView;
	
	IBOutlet NSTableView *objectView;
	
	ExperimentalBulletWrapper *bulletWrapper;
	BOOL simulationRunning;
	OpenGLManipulatingController *bulletController;
}

@property (readwrite, assign) id<OpenGLManipulating> manipulated;

- (void)setNeedsDisplayExceptView:(OpenGLSceneView *)view;
- (void)setNeedsDisplayOnAllViews;
- (void)syncObjectView;
- (IBAction)changeManipulator:(id)sender;
- (IBAction)changeViewMode:(id)sender;
- (IBAction)cloneSelected:(id)sender;
- (IBAction)deleteSelected:(id)sender;
- (IBAction)selectAll:(id)sender;
- (IBAction)invertSelection:(id)sender;
- (void)toggleOneViewFourView:(id)sender;
- (IBAction)play:(id)sender;

@end

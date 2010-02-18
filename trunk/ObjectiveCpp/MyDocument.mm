//
//  MyDocument.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#import "MyDocument.h"

@implementation MyDocument

- (id)init
{
    self = [super init];
    if (self) 
	{
        // Add your subclass-specific initialization here.
        // If an error occurs here, send a [self release] message and return nil.
		bulletController = [[OpenGLManipulatingController alloc] init];
		[bulletController addTransformationObserver:self];
		
		manipulationFinished = YES;
		
		views = [[NSMutableArray alloc] init];
		oneView = NO;
		bulletWrapper = nil;
		[NSTimer scheduledTimerWithTimeInterval:1.0 / 60.0
										 target:self
									   selector:@selector(timerProc:)
									   userInfo:nil
										repeats:YES];
		simulationRunning = NO;
    }
    return self;
}
						   
- (void)timerProc:(NSTimer *)theTimer
{
	if (bulletWrapper && simulationRunning)
	{
		[bulletWrapper stepSimulationWithTimeInterval:[theTimer timeInterval]];
		for (OpenGLSceneView *view in views)
			[view setNeedsDisplay:YES];
	}
}

- (void)dealloc
{
	[bulletController removeTransformationObserver:self];
	[bulletController release];
	[views release];
	[propertyReflector release];
	[bulletWrapper release];
	[super dealloc];
}

- (void)awakeFromNib
{
	// maximizing window on start
	[[viewPerspective window] performZoom:self];
	
	[viewModePopUp selectItemWithTag:0];
	
	[views addObject:viewTop];
	[views addObject:viewLeft];
	[views addObject:viewFront];
	[views addObject:viewPerspective];
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setManipulated:manipulated]; 
		[view setDisplayed:nil];
		[view setDelegate:self];
	};
	
	[viewTop setCameraMode:CameraModeTop];
	[viewLeft setCameraMode:CameraModeLeft];
	[viewFront setCameraMode:CameraModeFront];
	[viewPerspective setCameraMode:CameraModePerspective];
	
	propertyReflector = [[PropertyReflector alloc] initWithTableView:propertyView];
	[propertyReflector setReflectedObject:self];
	
	[objectView setDataSource:self];
}

- (id<OpenGLManipulating>)manipulated
{
	return manipulated;
}

- (void)setManipulated:(id<OpenGLManipulating>)value
{
	manipulated = value;
	
	for (OpenGLSceneView *view in views)
	{ 
		[view setManipulated:value];
		[view setNeedsDisplay:YES];
	}
}

- (float)positionX
{
	return [manipulated positionX];
}

- (float)positionY
{
	return [manipulated positionY];
}

- (float)positionZ
{
	return [manipulated positionZ];
}

- (float)rotationX
{
	return [manipulated rotationX];
}

- (float)rotationY
{
	return [manipulated rotationY];
}

- (float)rotationZ
{
	return [manipulated rotationZ];
}

- (float)scaleX
{
	return [manipulated scaleX];
}

- (float)scaleY
{
	return [manipulated scaleY];
}

- (float)scaleZ
{
	return [manipulated scaleZ];
}

- (void)setPositionX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionX:value];
	[self manipulationEndedInView:nil];
}

- (void)setPositionY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionY:value];
	[self manipulationEndedInView:nil];
}

- (void)setPositionZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setPositionZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationX:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationY:value];
	[self manipulationEndedInView:nil];
}

- (void)setRotationZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setRotationZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleX:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleX:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleY:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleY:value];
	[self manipulationEndedInView:nil];
}

- (void)setScaleZ:(float)value
{
	[self manipulationStartedInView:nil];
	[manipulated setScaleZ:value];
	[self manipulationEndedInView:nil];
}

- (void)setNilValueForKey:(NSString *)key
{
	[self setValue:[NSNumber numberWithFloat:0.0f] forKey:key];
}

- (void)observeValueForKeyPath:(NSString *)keyPath 
					  ofObject:(id)object 
						change:(NSDictionary *)change
					   context:(void *)context
{
	// Is it right way to do it? I really don't know, but it works.
	[self willChangeValueForKey:keyPath];
	[self didChangeValueForKey:keyPath];
}

- (void)manipulationStartedInView:(OpenGLSceneView *)view
{
	NSLog(@"manipulationStartedInView:");
	manipulationFinished = NO;
}

- (void)manipulationEndedInView:(OpenGLSceneView *)view
{
	NSLog(@"manipulationEndedInView:");	
	manipulationFinished = YES;
	
	for (OpenGLSceneView *v in views)
	{ 
		if (v != view)
			[v setNeedsDisplay:YES]; 
	}
}

- (void)selectionChangedInView:(OpenGLSceneView *)view
{
	NSLog(@"selectionChangedInView:");
	
	for (OpenGLSceneView *v in views)
	{ 
		if (v != view)
			[v setNeedsDisplay:YES]; 
	}
}

- (IBAction)changeViewMode:(id)sender
{
	ViewMode mode = (ViewMode)[[viewModePopUp selectedItem] tag];
	for (OpenGLSceneView *view in views)
	{ 
		[view setViewMode:mode]; 
	}
}

- (IBAction)changeManipulator:(id)sender
{
	ManipulatorType newManipulator = (ManipulatorType)[sender tag];
	for (OpenGLSceneView *view in views)
	{ 
		[view setCurrentManipulator:newManipulator]; 
	}
}

- (IBAction)selectAll:(id)sender
{
	[[self manipulated] changeSelection:YES];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (IBAction)invertSelection:(id)sender
{
	[[self manipulated] invertSelection];
	for (OpenGLSceneView *view in views)
	{ 
		[view setNeedsDisplay:YES]; 
	}
}

- (IBAction)play:(id)sender
{
	simulationRunning = YES;
}

- (IBAction)pause:(id)sender
{
	simulationRunning = NO;
}

#pragma mark TableViewDataSource

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn 
			row:(NSInteger)rowIndex
{
	return [bulletWrapper nameAtIndex:(uint)rowIndex];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return [bulletWrapper count];
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"MyDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *)aController
{
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

#pragma mark Archivation

- (BOOL)readFromBullet:(NSString *)fileName
{
	bulletWrapper = [[ExperimentalBulletWrapper alloc] initWithFileName:fileName];
	if (bulletWrapper)
	{
		// this is highly experimental, because bulletWrapper doesn't implement all necessary
		// functions, so when doing some operations, it can crash whole editor
		[bulletController setModel:bulletWrapper];
		[bulletController updateSelection];
		[self setManipulated:bulletController];
		return YES;
	}
	return NO;
}

- (void)writeToBullet:(NSString *)fileName
{
	if (bulletWrapper)
	{
		[bulletWrapper saveWithFileName:fileName];
	}
}

// these read/write methods are deprecated, I need to use newer methods

- (BOOL)readFromFile:(NSString *)fileName ofType:(NSString *)typeName
{
	NSLog(@"readFromFile:%@ typeName:%@", fileName, typeName);
	if ([typeName isEqual:@"bullet"])
	{
		return [self readFromBullet:fileName];
	}
	return NO;
}

- (BOOL)writeToFile:(NSString *)fileName ofType:(NSString *)typeName
{
	NSLog(@"writeToFile:%@ typeName:%@", fileName, typeName);
	if ([typeName isEqual:@"bullet"])
	{
		[self writeToBullet:fileName];
		return YES;
	}
	return NO;
}

#pragma mark Splitter sync

- (CGFloat)splitView:(NSSplitView *)splitView 
constrainSplitPosition:(CGFloat)proposedPosition 
		 ofSubviewAt:(NSInteger)dividerIndex
{
	if (oneView)
		return 0.0f;
	
	return proposedPosition;
}

// fix for issue four-views works independetly on Mac version
- (void)splitViewDidResizeSubviews:(NSNotification *)notification
{
	if (oneView)
		return;
	
	NSSplitView *splitView = (NSSplitView *)[notification object];
	if (splitView == mainSplit)
		return;
	
	NSView *topSubview0 = (NSView *)[[topSplit subviews] objectAtIndex:0];
	NSView *topSubview1 = (NSView *)[[topSplit subviews] objectAtIndex:1];
	
	NSView *bottomSubview0 = (NSView *)[[bottomSplit subviews] objectAtIndex:0];
	NSView *bottomSubview1 = (NSView *)[[bottomSplit subviews] objectAtIndex:1];
	
	// we are interested only in width change
	if (fabsf([bottomSubview0 frame].size.width - [topSubview0 frame].size.width) >= 1.0f)
	{
		if (splitView == topSplit)
		{
			NSLog(@"topSplit");
			[bottomSubview0 setFrame:[topSubview0 frame]];
			[bottomSubview1 setFrame:[topSubview1 frame]];
		}
		else
		{
			NSLog(@"bottomSplit");
			[topSubview0 setFrame:[bottomSubview0 frame]];
			[topSubview1 setFrame:[bottomSubview1 frame]];
		}
	}
}

- (void)collapseSplitView:(NSSplitView *)splitView
{
	[[[splitView subviews] objectAtIndex:0] setFrame:NSZeroRect];
	if ([splitView isVertical])
	{
		NSRect frame = [[[splitView subviews] objectAtIndex:1] frame];
		if (frame.size.width < 1.0f)
		{
			frame.size.width = 2.0f;
			[[[splitView subviews] objectAtIndex:1] setFrame:frame];
		}
	}
	else 
	{
		NSRect frame = [[[splitView subviews] objectAtIndex:1] frame];
		if (frame.size.height < 1.0f)
		{
			frame.size.height = 2.0f;
			[[[splitView subviews] objectAtIndex:1] setFrame:frame];
		}
	}
}

- (void)toggleOneViewFourView:(id)sender
{
	NSLog(@"toggleOneViewFourView");
	
	if (oneView)
	{
		[viewPerspective setCameraMode:CameraModePerspective];
		NSRect frame = [viewPerspective frame];
		[[[mainSplit subviews] objectAtIndex:0] setFrame:frame];
		oneView = NO;
		return;
	}
	
	NSWindow *window = [viewPerspective window];
	NSPoint point = [window convertScreenToBase:[NSEvent mouseLocation]];
	
	NSView *hittedView = [[[window contentView] superview] hitTest:point];
	
	for (OpenGLSceneView *view in views)
	{
		if (view == hittedView)
		{
			oneView = YES;
			
			[self collapseSplitView:topSplit];
			[self collapseSplitView:bottomSplit];
			[self collapseSplitView:mainSplit];
			
			[viewPerspective setCameraMode:[view cameraMode]];
			return;
		}
	}
	
	NSLog(@"No view is under mouse");
}

@end

//
//  MDNotificationCenter.m
//  iOS Restore
//
//  Created by John Heaton on 4/15/11.
//  Copyright 2011 GJB Software. All rights reserved.
//

#import "MDNotificationCenter.h"
#import "MDDeviceManager.h"

NSString *MDNotificationDeviceAttached = @"MDDeviceAttached";
NSString *MDNotificationDeviceDetached = @"MDDeviceDetached";

NSString *MDNotificationDeviceTypeKey = @"MDDeviceType";

AMDeviceSubscriptionRef subscription = NULL;

/* Callbacks */

void device_notification(AMDeviceNotificationRef notification) {
    AMDeviceRef device = notification->device;
    AMStatus status;
    
    switch(notification->message) {
        case kAMDeviceNotificationMessageConnected: {
            status = AMDeviceConnect(device);
            
            if(status == kAMStatusSuccess) {
                Boolean isPaired = AMDeviceIsPaired(device);
                
                if(!isPaired) {
                    status = AMDevicePair(device);
                    
                    if(status != kAMStatusSuccess) {
                        [[MDNotificationCenter sharedInstance] normalDeviceConnectionError];
                        return;
                    }
                }
                
                status = AMDeviceValidatePairing(device);
                
                if(status != kAMStatusSuccess) {
                    [[MDNotificationCenter sharedInstance] normalDeviceConnectionError];
                    return;
                }
                
                status = AMDeviceStartSession(device);
                
                if(status != kAMStatusSuccess) {
                    [[MDNotificationCenter sharedInstance] normalDeviceConnectionError];
                    return;
                }
                
                [[MDNotificationCenter sharedInstance] normalDeviceAttached:device];
            } else {
                AMRestoreModeDeviceRef restoreDevice = AMRestoreModeDeviceCreate(0, AMDeviceGetConnectionID(device), 0);
                
                [[MDNotificationCenter sharedInstance] restoreDeviceAttached:restoreDevice];
            }
        } break;
        case kAMDeviceNotificationMessageDisconnected: {
            switch([[MDDeviceManager sharedInstance] currentDeviceMode]) {
                case kAMDeviceNormalMode:
                    [[MDNotificationCenter sharedInstance] normalDeviceDetached:device];
                case kAMDeviceRestoreMode:
                    [[MDNotificationCenter sharedInstance] restoreDeviceDetached:(AMRestoreModeDeviceRef)device];
            }
            
        } break;
        default:
            break;
    }
}

void recovery_connected(AMRecoveryModeDeviceRef device) {
    [[MDNotificationCenter sharedInstance] recoveryDeviceAttached:device];
}

void recovery_disconnected(AMRecoveryModeDeviceRef device) {
    [[MDNotificationCenter sharedInstance] recoveryDeviceDetached:device];
}

void dfu_connected(AMDFUModeDeviceRef device) {
    [[MDNotificationCenter sharedInstance] dfuDeviceAttached:device];
}

void dfu_disconnected(AMDFUModeDeviceRef device) {
    [[MDNotificationCenter sharedInstance] dfuDeviceDetached:device];
}

/* --------- */


@implementation MDNotificationCenter

static MDNotificationCenter *sharedMDNotificationCenter = nil;

+ (MDNotificationCenter *)sharedInstance {
    @synchronized(self) {
        if (!sharedMDNotificationCenter) {
            sharedMDNotificationCenter = [[self alloc] init];
            
            AMDeviceNotificationSubscribe(device_notification, 0, 0, 0, &subscription);
            AMRestoreRegisterForDeviceNotifications(dfu_connected, recovery_connected, dfu_disconnected, recovery_disconnected, 0, NULL);
        }
    }
    
	return sharedMDNotificationCenter;
}

- (id)init {
    self = [super init];
    if (self) {
        _listeners = [[NSMutableSet alloc] init];
    }
    
    return self;
}

- (void)addListener:(id<MDListener>)listener {
    if(listener != nil && ![_listeners containsObject:listener]) {
        [_listeners addObject:listener];
        
        if([[MDDeviceManager sharedInstance] deviceIsPluggedIn]) {
            if([[MDDeviceManager sharedInstance] currentNormalDevice] != NULL 
               && [listener respondsToSelector:@selector(normalDeviceAttached:)]) 
                [listener normalDeviceAttached:[[MDDeviceManager sharedInstance] currentNormalDevice]];
            
            if([[MDDeviceManager sharedInstance] currentRecoveryDevice] != NULL 
               && [listener respondsToSelector:@selector(recoveryDeviceAttached:)]) 
                [listener recoveryDeviceAttached:[[MDDeviceManager sharedInstance] currentRecoveryDevice]];
            
            if([[MDDeviceManager sharedInstance] currentDFUDevice] != NULL 
               && [listener respondsToSelector:@selector(dfuDeviceAttached:)]) 
                [listener dfuDeviceAttached:[[MDDeviceManager sharedInstance] currentDFUDevice]];
            
            if([[MDDeviceManager sharedInstance] currentRestoreDevice] != NULL 
               && [listener respondsToSelector:@selector(restoreDeviceAttached:)]) 
                [listener restoreDeviceAttached:[[MDDeviceManager sharedInstance] currentRestoreDevice]];
        }
    }
}

- (void)removeListener:(id<MDListener>)listener {
    if(listener != nil && [_listeners containsObject:listener]) 
        [_listeners removeObject:listener];
}

- (void)clearAllListeners {
    [_listeners removeAllObjects];
}

- (NSUInteger)retainCount {
    return NSUIntegerMax;
}

- (oneway void)release {}

- (id)retain {
    return sharedMDNotificationCenter;
}

- (id)autorelease {
    return sharedMDNotificationCenter;
}

- (void)sendMessageToListeners:(SEL)message withDevice:(void *)object {
    [[MDDeviceManager sharedInstance] performSelector:message withObject:(id)object];
    
    NSEnumerator *listenersEnumerator = [_listeners objectEnumerator];
    
    id listener = nil;
    while((listener = [listenersEnumerator nextObject]) != nil) {
        if([listener respondsToSelector:message])
            [listener performSelector:message withObject:(id)object];
    }
}

// MDListener methods

- (void)normalDeviceAttached:(AMDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceAttached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceNormalMode] forKey:MDNotificationDeviceTypeKey]];
}

- (void)normalDeviceDetached:(AMDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceDetached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceNormalMode] forKey:MDNotificationDeviceTypeKey]];
}

- (void)normalDeviceConnectionError {
    [self sendMessageToListeners:_cmd withDevice:nil];
}

- (void)restoreDeviceAttached:(AMRestoreModeDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceAttached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceRestoreMode] forKey:MDNotificationDeviceTypeKey]];
}

- (void)restoreDeviceDetached:(AMRestoreModeDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceDetached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceRestoreMode] forKey:MDNotificationDeviceTypeKey]];
}

- (void)recoveryDeviceAttached:(AMRecoveryModeDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceAttached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceRecoveryMode] forKey:MDNotificationDeviceTypeKey]];
}

- (void)recoveryDeviceDetached:(AMRecoveryModeDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceDetached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceRecoveryMode] forKey:MDNotificationDeviceTypeKey]];
}

- (void)dfuDeviceAttached:(AMDFUModeDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceAttached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceDFUMode] forKey:MDNotificationDeviceTypeKey]];
}

- (void)dfuDeviceDetached:(AMDFUModeDeviceRef)device {
    [self sendMessageToListeners:_cmd withDevice:device];
    [[NSNotificationCenter defaultCenter] postNotificationName:MDNotificationDeviceDetached object:nil userInfo:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kAMDeviceDFUMode] forKey:MDNotificationDeviceTypeKey]];
}

@end

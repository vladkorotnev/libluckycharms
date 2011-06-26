//
//  MDNotificationCenter.h
//  iOS Restore
//
//  Created by John Heaton on 4/15/11.
//  Copyright 2011 GJB Software. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MDListener.h"

@interface MDNotificationCenter : NSObject <MDListener> {
@private
    NSMutableSet *_listeners;
}

+ (MDNotificationCenter *)sharedInstance;

- (void)addListener:(id<MDListener>)listener;
- (void)removeListener:(id<MDListener>)listener;

- (void)clearAllListeners;

@end

// For classes that don't want to implement the listener protocol, but are interested in when a device is attached/detached
extern NSString *MDNotificationDeviceAttached;
extern NSString *MDNotificationDeviceDetached;

// User info keys
extern NSString *MDNotificationDeviceTypeKey; // NSNumber, value is an AMDeviceMode
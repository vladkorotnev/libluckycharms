//
//  MDDeviceManager.m
//  iOS Restore
//
//  Created by John Heaton on 4/17/11.
//  Copyright 2011 GJB Software. All rights reserved.
//

#import "MDDeviceManager.h"
#import "MDNotificationCenter.h"


@implementation MDDeviceManager

static MDDeviceManager *sharedMDDeviceManager = nil;

@synthesize currentDeviceMode;
@synthesize currentDFUDevice;
@synthesize currentNormalDevice;
@synthesize currentRestoreDevice;
@synthesize currentRecoveryDevice;

@synthesize currentDeviceType;

+ (MDDeviceManager *)sharedInstance {
    @synchronized(self) {
        if (!sharedMDDeviceManager) {
            sharedMDDeviceManager = [[self alloc] init];
        }
    }
    
	return sharedMDDeviceManager;
}

- (NSUInteger)retainCount {
    return NSUIntegerMax;
}

- (oneway void)release {}

- (id)retain {
    return sharedMDDeviceManager;
}

- (id)autorelease {
    return sharedMDDeviceManager;
}

- (void)normalDeviceAttached:(AMDeviceRef)device {
    currentNormalDevice = device;
    currentDeviceMode = kAMDeviceNormalMode;
    currentDeviceType = iOSGetDeviceType(AMDeviceUSBProductID(device), 0);
}

- (void)normalDeviceDetached:(AMDeviceRef)device {
    currentNormalDevice = NULL;
    currentDeviceMode = kAMDeviceNoMode;
    currentDeviceType = NULL;
}

- (void)restoreDeviceAttached:(AMRestoreModeDeviceRef)device {
    currentRestoreDevice = device;
    currentDeviceMode = kAMDeviceRestoreMode;
    currentDeviceType = iOSGetDeviceType(AMDeviceUSBProductID((AMDeviceRef)device), 0);
}

- (void)restoreDeviceDetached:(AMRestoreModeDeviceRef)device {
    currentRestoreDevice = NULL;
    currentDeviceMode = kAMDeviceNoMode;
    currentDeviceType = NULL;
}

- (void)recoveryDeviceAttached:(AMRecoveryModeDeviceRef)device {
    currentRecoveryDevice = device;
    currentDeviceMode = kAMDeviceRecoveryMode;
    currentDeviceType = iOSGetDeviceType(AMRecoveryModeDeviceGetProductID(device), AMRecoveryModeDeviceGetProductType(device));
}

- (void)recoveryDeviceDetached:(AMRecoveryModeDeviceRef)device {
    currentRecoveryDevice = NULL;
    currentDeviceMode = kAMDeviceNoMode;
    currentDeviceType = NULL;
}

- (void)dfuDeviceAttached:(AMDFUModeDeviceRef)device {
    currentDFUDevice = device;
    currentDeviceMode = kAMDeviceDFUMode;
    currentDeviceType = iOSGetDeviceType(AMDFUModeDeviceGetProductID(device), AMDFUModeDeviceGetProductType(device));
}

- (void)dfuDeviceDetached:(AMDFUModeDeviceRef)device {
    currentDFUDevice = NULL;
    currentDeviceMode = kAMDeviceNoMode;
    currentDeviceType = NULL;
}

- (BOOL)deviceIsPluggedIn {
    return (self.currentDeviceType != NULL);
}

@end

//
//  MDDeviceManager.h
//  iOS Restore
//
//  Created by John Heaton on 4/17/11.
//  Copyright 2011 GJB Software. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MobileDevice.h"
#import "MDListener.h"


@interface MDDeviceManager : NSObject <MDListener> {
@private
    AMDeviceMode currentDeviceMode;
    AMDeviceRef currentNormalDevice;
    AMRestoreModeDeviceRef currentRestoreDevice;
    AMRecoveryModeDeviceRef currentRecoveryDevice;
    AMDFUModeDeviceRef currentDFUDevice;
    APPLE_MOBILE_DEVICE *currentDeviceType;
}

+ (MDDeviceManager *)sharedInstance;

- (BOOL)deviceIsPluggedIn;

@property (readonly) AMDeviceMode currentDeviceMode;
@property (readonly) AMDeviceRef currentNormalDevice;
@property (readonly) AMRestoreModeDeviceRef currentRestoreDevice;
@property (readonly) AMRecoveryModeDeviceRef currentRecoveryDevice;
@property (readonly) AMDFUModeDeviceRef currentDFUDevice;

@property (readonly) APPLE_MOBILE_DEVICE *currentDeviceType;

@end
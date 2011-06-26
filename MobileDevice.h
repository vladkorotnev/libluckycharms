/*
    MobileDevice.h
    Header for the MobileDevice framework used by iTunes + some extras

    Created by John Heaton(Gojohnnyboi) on Sunday, April 3, 2011
    Contact: gojohnnyboi@me.com
 
    Contributions are welcome.
*/

#include <CoreFoundation/CoreFoundation.h>

typedef struct {
    const char *name;
    uint16_t productID;
    const char *model;
    const char *board;
    uint32_t dfuDeviceID;
    uint32_t recoveryDeviceID;
} APPLE_MOBILE_DEVICE;

#define NUM_APPLE_MOBILE_DEVICES 13
#define AMSVC_AFC                   CFSTR("com.apple.afc")
#define AMSVC_BACKUP                CFSTR("com.apple.mobilebackup")
#define AMSVC_CRASH_REPORT_COPY     CFSTR("com.apple.crashreportcopy")
#define AMSVC_DEBUG_IMAGE_MOUNT     CFSTR("com.apple.mobile.debug_image_mount")
#define AMSVC_NOTIFICATION_PROXY    CFSTR("com.apple.mobile.notification_proxy")
#define AMSVC_PURPLE_TEST           CFSTR("com.apple.purpletestr")
#define AMSVC_SOFTWARE_UPDATE       CFSTR("com.apple.mobile.software_update")
#define AMSVC_SYNC                  CFSTR("com.apple.mobilesync")
#define AMSVC_SCREENSHOT            CFSTR("com.apple.screenshotr")
#define AMSVC_SYSLOG_RELAY          CFSTR("com.apple.syslog_relay")
#define AMSVC_SYSTEM_PROFILER       CFSTR("com.apple.mobile.system_profiler")
typedef unsigned int afc_error_t;

struct afc_connection {
    unsigned int handle;            /* 0 */
    unsigned int unknown0;          /* 4 */
    unsigned char unknown1;         /* 8 */
    unsigned char padding[3];       /* 9 */
    unsigned int unknown2;          /* 12 */
    unsigned int unknown3;          /* 16 */
    unsigned int unknown4;          /* 20 */
    unsigned int fs_block_size;     /* 24 */
    unsigned int sock_block_size;   /* 28: always 0x3c */
    unsigned int io_timeout;        /* 32: from AFCConnectionOpen, usu. 0 */
    void *afc_lock;                 /* 36 */
    unsigned int context;           /* 40 */
};

struct afc_device_info {
    unsigned char unknown[12];  /* 0 */
};

struct afc_directory {
    unsigned char unknown[0];   /* size unknown */
};

struct afc_dictionary {
    unsigned char unknown[0];   /* size unknown */
};

typedef unsigned long long afc_file_ref;



static APPLE_MOBILE_DEVICE APPLE_MOBILE_DEVICES[NUM_APPLE_MOBILE_DEVICES] = {
    { "iPhone",         0x1290, "iPhone1,1",   "m68ap", 304222464,  310382848 },
    { "iPhone 3G",      0x1292, "iPhone1,2",   "n82ap", 304230656,  310391040 },
    { "iPhone 3G[s]",   0x1294, "iPhone2,1",   "n88ap", 35104,      35104 },
    { "iPhone 4(GSM)",  0x1297, "iPhone3,1",   "n90ap", 35120,      35120 },
    { "iPhone 4(CDMA)", 0x129c, "iPhone3,3",   "n92ap", 100698416,  100698416 },
    { "iPod touch 1G",  0x1291, "iPod1,1",     "n45ap", 304226560,  310386944 },
    { "iPod touch 2G",  0x1293, "iPod2,1",     "n72ap", 34592,      34592 },
    { "iPod touch 3G",  0x1299, "iPod3,1",     "n18ap", 33589538,   33589538 },
    { "iPod touch 4G",  0x129e, "iPod4,1",     "n81ap", 134252848,  134252848 },
    { "iPad",           0x129a, "iPad1,1",     "k48ap", 33589552,   33589552 },
    { "iPad 2(WiFi)",   0x129f, "iPad2,1",     "k93ap", 67144000,   67144000 },
    { "iPad 2(GSM)",    0x12a2, "iPad2,2",     "k94ap", 100698432,  100698432 },
    { "iPad 2(CDMA)",   0x12a3, "iPad2,3",     "k95ap", 33589568,   33589568 }
};

typedef struct {
    const char *name;
    uint16_t productID;
} APPLE_USB_INTERFACE_TYPE;

#define NUM_APPLE_USB_INTERFACES 6

static APPLE_USB_INTERFACE_TYPE APPLE_USB_INTERFACES[NUM_APPLE_USB_INTERFACES] = {
    { "Recovery Mode v1",   0x1280 },
    { "Recovery Mode v2",   0x1281 },
    { "Recovery Mode v3",   0x1282 },
    { "Recovery Mode v4",   0x1283 },
    { "DFU/WTF v1",         0x1222 },
    { "DFU/WTF v2",         0x1227 }
};

#define AMDeviceInterfaceIsRecovery(productID) \
    ((productID == APPLE_USB_INTERFACES[0].productID) || \
    (productID == APPLE_USB_INTERFACES[1].productID) || \
    (productID == APPLE_USB_INTERFACES[2].productID) || \
    (productID == APPLE_USB_INTERFACES[3].productID))

#define AMDeviceInterfaceIsDFU(productID) \
    ((productID == APPLE_USB_INTERFACES[4].productID) || \
    (productID == APPLE_USB_INTERFACES[5].productID))

enum {
    kAMDeviceNormalMode = 0,
    kAMDeviceRestoreMode = 1,
    kAMDeviceRecoveryMode = 2,
    kAMDeviceDFUMode = 3,
    kAMDeviceNoMode = 4
};
typedef NSInteger AMDeviceMode;

// Statuses for functions -- more will be added
typedef enum {
    kAMStatusSuccess = 0,
    kAMStatusFailure = !kAMStatusSuccess
} AMStatus;

// Possible messages in the AMDeviceNotificationRef sent to a AMDeviceConnectionCallback function
typedef enum {
    kAMDeviceNotificationMessageConnected = 1,
    kAMDeviceNotificationMessageDisconnected = 2,
    kAMDeviceNotificationMessageUnsubscribed = 3
} AMDeviceNotificationMessage;



// List of known services on the device

// MobileDevice types. Struct names based off of CoreFoundation
// NOTE: You may cast from dfu to a recovery device 
typedef struct __AMDevice *AMDeviceRef;
typedef struct __AMRecoveryModeDevice *AMRecoveryModeDeviceRef;
typedef struct __AMDFUModeDevice *AMDFUModeDeviceRef;
typedef struct __AMRestoreModeDevice *AMRestoreModeDeviceRef;

typedef unsigned char *AMDeviceSubscriptionRef;

// It is necessary for this object's struct to be public. There are no functions to properly obtain its elements
typedef struct {
    AMDeviceRef                     device; // can be cast to AMRestoreModeDeviceRef if notification is restore device
    AMDeviceNotificationMessage     message;
    AMDeviceSubscriptionRef         subscription;
} *AMDeviceNotificationRef;

// Callback declarations
typedef void (* AMRecoveryModeDeviceConnectionCallback)(AMRecoveryModeDeviceRef device);
typedef void (* AMDFUModeDeviceConnectionCallback)(AMDFUModeDeviceRef device);
typedef void (* AMDeviceConnectionCallback)(AMDeviceNotificationRef notification);

#ifdef __cplusplus
extern "C" {
#endif

// Connection subscription functions. These will allow you to set up callback functions that are called upon connection/disconnection of a device.
AMStatus AMRestoreRegisterForDeviceNotifications(AMDFUModeDeviceConnectionCallback DFUConnectCallback, AMRecoveryModeDeviceConnectionCallback recoveryConnectCallback, AMDFUModeDeviceConnectionCallback DFUDisconnectCallback, AMRecoveryModeDeviceConnectionCallback recoveryDisconnectCallback, unsigned int alwaysZero, void *userInfo);

AMStatus AMDeviceNotificationSubscribe(AMDeviceConnectionCallback callback, int alwaysZero_1, int alwaysZero_2, int alwaysZero_3, AMDeviceSubscriptionRef *subscription);
 mach_error_t AMDeviceDeactivate(AMDeviceRef *device);
mach_error_t AMDeviceActivate(AMDeviceRef *device, CFDictionaryRef dict);
    int performOperation(AMRestoreModeDeviceRef *rdev, CFMutableDictionaryRef message);
	int socketForPort(AMRestoreModeDeviceRef *rdev, unsigned int portnum);
	int sendCommandToDevice(AMRestoreModeDeviceRef *rdev, CFStringRef cfs, int block);
	int sendFileToDevice(AMRestoreModeDeviceRef *rdev, CFStringRef filename); 
    
// Functions for use with AMDeviceRef objects(normal interface)
AMStatus AMDeviceConnect(AMDeviceRef device);
AMStatus AMDeviceDisconnect(AMDeviceRef device);
int AMDeviceGetConnectionID(AMDeviceRef device);

Boolean AMDeviceIsPaired(AMDeviceRef device);
AMStatus AMDevicePair(AMDeviceRef device);
AMStatus AMDeviceValidatePairing(AMDeviceRef device);

CFStringRef AMDeviceCopyDeviceIdentifier(AMDeviceRef device);

AMStatus AMDeviceEnterRecovery(AMDeviceRef device);

AMStatus AMDeviceStartSession(AMDeviceRef device);
AMStatus AMDeviceStopSession(AMDeviceRef device);

CFTypeRef AMDeviceCopyValue(AMDeviceRef device, CFStringRef domain, CFStringRef valueName);

AMStatus AMDeviceStartService(AMDeviceRef device, CFStringRef serviceName, CFSocketNativeHandle *socketDescriptor);

AMStatus AMDPostNotification(CFSocketNativeHandle socket, CFStringRef notification, CFStringRef userinfo);
AMStatus AMDObserveNotification(CFSocketNativeHandle socket, CFStringRef notification);
AMStatus AMDListenForNotifications(CFSocketNativeHandle socket, CFNotificationCallback cb, id data);
AMStatus AMDShutdownNotificationProxy(CFSocketNativeHandle socket);
    
    
uint16_t AMDeviceUSBProductID(AMDeviceRef device);

void AMDeviceRelease(AMDeviceRef device);
void AMDeviceRetain(AMDeviceRef device);

// Functions for use with AMRecoveryModeDeviceRef objects(recovery interface)
CFStringRef AMRecoveryModeDeviceCopyEnvironmentVariableFromDevice(AMRecoveryModeDeviceRef device, CFStringRef variable);

AMStatus AMRecoveryModeDeviceSendCommandToDevice(AMRecoveryModeDeviceRef device, CFStringRef command);
AMStatus AMRecoveryModeDeviceSendBlindCommandToDevice(AMRecoveryModeDeviceRef device, CFStringRef command);

AMStatus AMRecoveryModeDeviceSendFileToDevice(AMRecoveryModeDeviceRef device, CFStringRef filename);

AMStatus AMRecoveryModeDeviceSetAutoBoot(AMRecoveryModeDeviceRef device, Boolean autoBoot);
AMStatus AMRecoveryModeDeviceReboot(AMRecoveryModeDeviceRef device);
AMStatus AMRestoreModeDeviceReboot(AMRestoreModeDeviceRef *rdev);
    
uint16_t AMRecoveryModeDeviceGetProductID(AMRecoveryModeDeviceRef device);
uint32_t AMRecoveryModeDeviceGetProductType(AMRecoveryModeDeviceRef device);

CFTypeID AMRecoveryModeDeviceGetTypeID(AMRecoveryModeDeviceRef device);

AMStatus AMRecoveryModeDeviceCopyAuthInstallPreflightOptions(AMRecoveryModeDeviceRef device, CFDictionaryRef inputOptions, CFDictionaryRef *newRestoreOptions);
AMStatus AMRestorePerformRecoveryModeRestore(AMRecoveryModeDeviceRef device, CFDictionaryRef restoreOptions, void *callback, void *userInfo);

// Functions for use with AMRestoreModeDeviceRef objects(restore interface)

AMRestoreModeDeviceRef AMRestoreModeDeviceCreate(int alwaysZero_1, int connectionID, int alwaysZero_2);

CFDictionaryRef AMRestoreCreateDefaultOptions(CFAllocatorRef allocator); // may also be used for recovery/dfu restores
AMStatus AMRestorePerformRestoreModeRestore(AMRecoveryModeDeviceRef device, CFDictionaryRef restoreOptions, void *callback, void *userInfo);

// Functions for use with AMDFUModeDeviceRef objects(DFU/WTF interface)
uint16_t AMDFUModeDeviceGetProductID(AMDFUModeDeviceRef device);
uint32_t AMDFUModeDeviceGetProductType(AMDFUModeDeviceRef device);
    
AMStatus USBMuxConnectByPort(int connectionID, int phonePort, CFSocketNativeHandle *outHandle);

    /* Opens an Apple File Connection. You must start the appropriate service
	 * first with AMDeviceStartService(). In iTunes, io_timeout is 0.
	 *
	 * Returns:
	 *      MDERR_OK                if successful
	 *      MDERR_AFC_OUT_OF_MEMORY if malloc() failed
	 */
    afc_error_t AFCConnectionOpen(int socket_fd, unsigned int io_timeout,
                                              struct afc_connection **conn);
    
    afc_error_t AFCDeviceInfoOpen(struct afc_connection *conn, struct
								  afc_dictionary **info);
    
#if defined(__APPLE__)
	void AFCPlatformInitialize();
#endif
    
    afc_error_t AFCDirectoryOpen(struct afc_connection *conn, char *path, struct
								 afc_directory **dir);

	afc_error_t AFCDirectoryRead(struct afc_connection *conn, struct afc_directory *dir,
                                             char **dirent);
	afc_error_t AFCDirectoryClose(struct afc_connection *conn, struct afc_directory *dir);
	afc_error_t AFCDirectoryCreate(struct afc_connection *conn, char *dirname);
    afc_error_t AFCRemovePath(struct afc_connection *conn, char *dirname);
	afc_error_t AFCRenamePath(struct afc_connection *conn, char *oldpath, char *newpath);
    
    
    /* Opens file for reading or writing without locking it in any way. afc_file_ref should not be shared between threads - 
     * opening file in one thread and closing it in another will lead to possible crash.
	 * path - UTF-8 encoded absolute path to file
	 * mode 2 = read, mode 3 = write; unknown = 0 
	 * ref - receives file handle
	 */
	 afc_error_t AFCFileRefOpen(struct afc_connection *conn, char *path, unsigned
                                           long long int mode, afc_file_ref *ref);
	/* Reads specified amount (len) of bytes from file into buf. Puts actual count of read bytes into len on return
	 */
	 afc_error_t AFCFileRefRead(struct afc_connection *conn, afc_file_ref ref,
                                           void *buf, unsigned int *len);
	/* Writes specified amount (len) of bytes from buf into file.
	 */
	 afc_error_t AFCFileRefWrite(struct afc_connection *conn, afc_file_ref ref,
                                            void *buf, unsigned int len);
	/* Moves the file pointer to a specified location.
	 * offset - Number of bytes from origin (int64)
	 * origin - 0 = from beginning, 1 = from current position, 2 = from end
	 */
	 afc_error_t AFCFileRefSeek(struct afc_connection *conn, afc_file_ref ref,
                                           unsigned long long offset, int origin, int unused);
    
	/* Gets the current position of a file pointer into offset argument.
	 */
	 afc_error_t AFCFileRefTell(struct afc_connection *conn, afc_file_ref ref,
                                           unsigned long long* offset);
    
	/*  Truncates a file at the specified offset.
	 */
	 afc_error_t AFCFileRefSetFileSize(struct afc_connection *conn, afc_file_ref ref,
                                                  unsigned long long offset);
    
    
	 afc_error_t AFCFileRefLock(struct afc_connection *conn, afc_file_ref ref);
	 afc_error_t AFCFileRefUnlock(struct afc_connection *conn, afc_file_ref ref);
	 afc_error_t AFCFileRefClose(struct afc_connection *conn, afc_file_ref ref);
    
	/* Opens dictionary describing specified file or directory (iTunes below 8.2 allowed using AFCGetFileInfo
     to get the same information)
     */
	 afc_error_t AFCFileInfoOpen(struct afc_connection *conn, char *path, struct
                                            afc_dictionary **info);
    
	/* Reads next entry from dictionary. When last entry is read, function returns NULL in key argument
     Possible keys:
     "st_size":     val - size in bytes
     "st_blocks":   val - size in blocks
     "st_nlink":    val - number of hardlinks
     "st_ifmt":     val - "S_IFDIR" for folders
     "S_IFLNK" for symlinks
     "LinkTarget":  val - path to symlink target
     */
	 afc_error_t AFCKeyValueRead(struct afc_dictionary *dict, char **key, char **
                                            val);
	/* Closes dictionary
     */
	 afc_error_t AFCKeyValueClose(struct afc_dictionary *dict);
    
	
	/* Returns the context field of the given AFC connection. */
	 unsigned int AFCConnectionGetContext(struct afc_connection *conn);
	
	/* Returns the fs_block_size field of the given AFC connection. */
	 unsigned int AFCConnectionGetFSBlockSize(struct afc_connection *conn);
	
	/* Returns the io_timeout field of the given AFC connection. In iTunes this is
	 * 0. */
	 unsigned int AFCConnectionGetIOTimeout(struct afc_connection *conn);
	
	/* Returns the sock_block_size field of the given AFC connection. */
	 unsigned int AFCConnectionGetSocketBlockSize(struct afc_connection *conn);
	
	/* Closes the given AFC connection. */
	 afc_error_t AFCConnectionClose(struct afc_connection *conn);
    
// custom functions
static APPLE_MOBILE_DEVICE *iOSGetDeviceType(uint16_t productID, uint32_t deviceID) {
    for(int i=0;i<NUM_APPLE_USB_INTERFACES;++i) {
        if(APPLE_USB_INTERFACES[i].productID == productID) {
            if(i < 4) {
                for(int i=0;i<NUM_APPLE_MOBILE_DEVICES;++i) {
                    if(APPLE_MOBILE_DEVICES[i].recoveryDeviceID == deviceID) {
                        return &APPLE_MOBILE_DEVICES[i];
                    }
                }
            } else {
                for(int i=0;i<NUM_APPLE_MOBILE_DEVICES;++i) {
                    if(APPLE_MOBILE_DEVICES[i].dfuDeviceID == deviceID) {
                        return &APPLE_MOBILE_DEVICES[i];
                    }
                }
            }
        }
    }
    
    for(int i=0;i<NUM_APPLE_MOBILE_DEVICES;++i) {
        if(APPLE_MOBILE_DEVICES[i].productID == productID) {
            return &APPLE_MOBILE_DEVICES[i];
        }
    }
    
    return NULL;
}

static NSString *iOSGetDeviceConnectionType(uint16_t productID, uint32_t deviceID, BOOL isRestoreMode) {
    NSString *modeName = (isRestoreMode ? @"Restore" : @"Normal");
    NSString *deviceName = @"Unknown Device";
    
    APPLE_MOBILE_DEVICE *device = iOSGetDeviceType(productID, deviceID);
    deviceName = [NSString stringWithUTF8String:device->name];
    
    for(int i=0;i<NUM_APPLE_USB_INTERFACES;++i) {
        if(APPLE_USB_INTERFACES[i].productID == productID) {
            if(i < 4)
                modeName = @"Recovery";
            else
                modeName = @"DFU";
        }
    }
    
    return [NSString stringWithFormat:@"%@: %@ Mode", deviceName, modeName];
}

static NSString *iOSGetDeviceClassName(APPLE_MOBILE_DEVICE *deviceType) {
    if(strstr(deviceType->model, "iPod") != NULL) {
        return @"iPod";
    } else if(strstr(deviceType->model, "iPhone") != NULL) {
        return @"iPhone";
    } else if(strstr(deviceType->model, "iPad") != NULL) {
        return @"iPad";
    }
    
    return @"Unknown";
}

#ifdef __cplusplus 
}
#endif
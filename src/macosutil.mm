#include "macosutil.h"
#import <AppKit/AppKit.h>

void MacOSUtil::hideDockIcon() {
    [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];
}

void MacOSUtil::activateApplication() {
    [NSApp activateIgnoringOtherApps:YES];
}

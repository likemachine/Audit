#pragma once

#if PLATFORM_IOS
#import <ARKit/ARKit.h>
@class QRCodeReader;

@interface QRCodeARSessionDelegate : NSObject<ARSessionDelegate>
- (instancetype)initWithReader:(QRCodeReader*)reader;
@property (nonatomic, weak) QRCodeReader* reader;
@end

#endif

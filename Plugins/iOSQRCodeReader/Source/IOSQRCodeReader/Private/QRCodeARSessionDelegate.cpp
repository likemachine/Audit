#include "QRCodeARSessionDelegate.h"
#include "QRCodeReader.h"

#if PLATFORM_IOS

@implementation QRCodeARSessionDelegate

- (instancetype)initWithReader:(QRCodeReader*)reader
{
    self = [super init];
    if (self)
    {
        _reader = reader;
    }
    return self;
}

- (void)session:(ARSession *)session didUpdateFrame:(ARFrame *)frame
{
    [_reader processARFrame:frame];
}

@end

#endif


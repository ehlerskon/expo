/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#import <Foundation/Foundation.h>

#import "ABI11_0_0RCTDefines.h"
#import "ABI11_0_0RCTAssert.h"

/**
 * ABI11_0_0RCTProfile
 *
 * This file provides a set of functions and macros for performance profiling
 *
 * NOTE: This API is a work in progress, please consider carefully before
 * using it.
 */

ABI11_0_0RCT_EXTERN NSString *const ABI11_0_0RCTProfileDidStartProfiling;
ABI11_0_0RCT_EXTERN NSString *const ABI11_0_0RCTProfileDidEndProfiling;

ABI11_0_0RCT_EXTERN const uint64_t ABI11_0_0RCTProfileTagAlways;

#if ABI11_0_0RCT_PROFILE

@class ABI11_0_0RCTBridge;

#define ABI11_0_0RCTProfileBeginFlowEvent() \
_Pragma("clang diagnostic push") \
_Pragma("clang diagnostic ignored \"-Wshadow\"") \
NSUInteger __rct_profile_flow_id = _ABI11_0_0RCTProfileBeginFlowEvent(); \
_Pragma("clang diagnostic pop")

#define ABI11_0_0RCTProfileEndFlowEvent() \
_ABI11_0_0RCTProfileEndFlowEvent(__rct_profile_flow_id)

ABI11_0_0RCT_EXTERN dispatch_queue_t ABI11_0_0RCTProfileGetQueue(void);

ABI11_0_0RCT_EXTERN NSUInteger _ABI11_0_0RCTProfileBeginFlowEvent(void);
ABI11_0_0RCT_EXTERN void _ABI11_0_0RCTProfileEndFlowEvent(NSUInteger);

/**
 * Returns YES if the profiling information is currently being collected
 */
ABI11_0_0RCT_EXTERN BOOL ABI11_0_0RCTProfileIsProfiling(void);

/**
 * Start collecting profiling information
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileInit(ABI11_0_0RCTBridge *);

/**
 * Stop profiling and return a JSON string of the collected data - The data
 * returned is compliant with google's trace event format - the format used
 * as input to trace-viewer
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileEnd(ABI11_0_0RCTBridge *, void (^)(NSString *));

/**
 * Collects the initial event information for the event and returns a reference ID
 */
ABI11_0_0RCT_EXTERN void _ABI11_0_0RCTProfileBeginEvent(NSThread *calleeThread,
                                      NSTimeInterval time,
                                      uint64_t tag,
                                      NSString *name,
                                      NSDictionary *args);
#define ABI11_0_0RCT_PROFILE_BEGIN_EVENT(tag, name, args) \
  do { \
    if (ABI11_0_0RCTProfileIsProfiling()) { \
      NSThread *__calleeThread = [NSThread currentThread]; \
      NSTimeInterval __time = CACurrentMediaTime(); \
      _ABI11_0_0RCTProfileBeginEvent(__calleeThread, __time, tag, name, args); \
    } \
  } while(0)

/**
 * The ID returned by BeginEvent should then be passed into EndEvent, with the
 * rest of the event information. Just at this point the event will actually be
 * registered
 */
ABI11_0_0RCT_EXTERN void _ABI11_0_0RCTProfileEndEvent(NSThread *calleeThread,
                                    NSString *threadName,
                                    NSTimeInterval time,
                                    uint64_t tag,
                                    NSString *category);

#define ABI11_0_0RCT_PROFILE_END_EVENT(tag, category) \
  do { \
    if (ABI11_0_0RCTProfileIsProfiling()) { \
      NSThread *__calleeThread = [NSThread currentThread]; \
      NSString *__threadName = ABI11_0_0RCTCurrentThreadName(); \
      NSTimeInterval __time = CACurrentMediaTime(); \
      _ABI11_0_0RCTProfileEndEvent(__calleeThread, __threadName, __time, tag, category); \
    } \
  } while(0)

/**
 * Collects the initial event information for the event and returns a reference ID
 */
ABI11_0_0RCT_EXTERN NSUInteger ABI11_0_0RCTProfileBeginAsyncEvent(uint64_t tag,
                                                NSString *name,
                                                NSDictionary *args);

/**
 * The ID returned by BeginEvent should then be passed into EndEvent, with the
 * rest of the event information. Just at this point the event will actually be
 * registered
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileEndAsyncEvent(uint64_t tag,
                                        NSString *category,
                                        NSUInteger cookie,
                                        NSString *name,
                                        NSString *threadName);

/**
 * An event that doesn't have a duration (i.e. Notification, VSync, etc)
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileImmediateEvent(uint64_t tag,
                                         NSString *name,
                                         NSTimeInterval time,
                                         char scope);

/**
 * Helper to profile the duration of the execution of a block. This method uses
 * self and _cmd to name this event for simplicity sake.
 *
 * NOTE: The block can't expect any argument
 *
 * DEPRECATED: this approach breaks debugging and stepping through instrumented block functions
 */
#define ABI11_0_0RCTProfileBlock(block, tag, category, arguments) \
^{ \
  ABI11_0_0RCT_PROFILE_BEGIN_EVENT(tag, @(__PRETTY_FUNCTION__), nil); \
  block(); \
  ABI11_0_0RCT_PROFILE_END_EVENT(tag, category, arguments); \
}

/**
 * Hook into a bridge instance to log all bridge module's method calls
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileHookModules(ABI11_0_0RCTBridge *);

/**
 * Unhook from a given bridge instance's modules
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileUnhookModules(ABI11_0_0RCTBridge *);

/**
 * Hook into all of a module's methods
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileHookInstance(id instance);

/**
 * Send systrace or cpu profiling information to the packager
 * to present to the user
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileSendResult(ABI11_0_0RCTBridge *bridge, NSString *route, NSData *profileData);

/**
 * Systrace gluecode
 *
 * allow to use systrace to back ABI11_0_0RCTProfile
 */

typedef struct {
  const char *key;
  int key_len;
  const char *value;
  int value_len;
} systrace_arg_t;

typedef struct {
  void (*start)(uint64_t enabledTags, char *buffer, size_t bufferSize);
  void (*stop)(void);

  void (*begin_section)(uint64_t tag, const char *name, size_t numArgs, systrace_arg_t *args);
  void (*end_section)(uint64_t tag, size_t numArgs, systrace_arg_t *args);

  void (*begin_async_section)(uint64_t tag, const char *name, int cookie, size_t numArgs, systrace_arg_t *args);
  void (*end_async_section)(uint64_t tag, const char *name, int cookie, size_t numArgs, systrace_arg_t *args);

  void (*instant_section)(uint64_t tag, const char *name, char scope);

  void (*begin_async_flow)(uint64_t tag, const char *name, int cookie);
  void (*end_async_flow)(uint64_t tag, const char *name, int cookie);
} ABI11_0_0RCTProfileCallbacks;

ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileRegisterCallbacks(ABI11_0_0RCTProfileCallbacks *);

/**
 * Systrace control window
 */
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileShowControls(void);
ABI11_0_0RCT_EXTERN void ABI11_0_0RCTProfileHideControls(void);

#else

#define ABI11_0_0RCTProfileBeginFlowEvent()
#define _ABI11_0_0RCTProfileBeginFlowEvent() @0

#define ABI11_0_0RCTProfileEndFlowEvent()
#define _ABI11_0_0RCTProfileEndFlowEvent(...)

#define ABI11_0_0RCTProfileIsProfiling(...) NO
#define ABI11_0_0RCTProfileInit(...)
#define ABI11_0_0RCTProfileEnd(...) @""

#define _ABI11_0_0RCTProfileBeginEvent(...)
#define _ABI11_0_0RCTProfileEndEvent(...)

#define ABI11_0_0RCT_PROFILE_BEGIN_EVENT(...)
#define ABI11_0_0RCT_PROFILE_END_EVENT(...)

#define ABI11_0_0RCTProfileBeginAsyncEvent(...) 0
#define ABI11_0_0RCTProfileEndAsyncEvent(...)

#define ABI11_0_0RCTProfileImmediateEvent(...)

#define ABI11_0_0RCTProfileBlock(block, ...) block

#define ABI11_0_0RCTProfileHookModules(...)
#define ABI11_0_0RCTProfileHookInstance(...)
#define ABI11_0_0RCTProfileUnhookModules(...)

#define ABI11_0_0RCTProfileSendResult(...)

#define ABI11_0_0RCTProfileShowControls(...)
#define ABI11_0_0RCTProfileHideControls(...)

#endif

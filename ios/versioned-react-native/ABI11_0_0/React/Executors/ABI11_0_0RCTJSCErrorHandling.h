/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#import <JavaScriptCore/JavaScriptCore.h>

#import "ABI11_0_0RCTDefines.h"

typedef struct ABI11_0_0RCTJSCWrapper ABI11_0_0RCTJSCWrapper;

/**
 Translates a given exception into an NSError.

 @param exception The JavaScript exception object to translate into an NSError. This must be
 a JavaScript Error object, otherwise no stack trace information will be available.

 @return The translated NSError object

 - The JS exception's name property is incorporated in the NSError's localized description
 - The JS exception's message property is the NSError's failure reason
 - The JS exception's unsymbolicated stack trace is available via the NSError userInfo's ABI11_0_0RCTJSExceptionUnsymbolicatedStackTraceKey
 */
ABI11_0_0RCT_EXTERN NSError *ABI11_0_0RCTNSErrorFromJSError(JSValue *exception);

/**
 Translates a given exception into an NSError.

 @see ABI11_0_0RCTNSErrorFromJSError for details
 */
ABI11_0_0RCT_EXTERN NSError *ABI11_0_0RCTNSErrorFromJSErrorRef(JSValueRef exception, JSGlobalContextRef ctx, ABI11_0_0RCTJSCWrapper *jscWrapper);

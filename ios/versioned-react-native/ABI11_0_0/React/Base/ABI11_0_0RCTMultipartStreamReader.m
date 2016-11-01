/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#import "ABI11_0_0RCTMultipartStreamReader.h"

#define CRLF @"\r\n"

@implementation ABI11_0_0RCTMultipartStreamReader {
  __strong NSInputStream *_stream;
  __strong NSString *_boundary;
}

- (instancetype)initWithInputStream:(NSInputStream *)stream boundary:(NSString *)boundary
{
  if (self = [super init]) {
    _stream = stream;
    _boundary = boundary;
  }
  return self;
}

- (NSDictionary *)parseHeaders:(NSData *)data
{
  NSMutableDictionary *headers = [NSMutableDictionary new];
  NSString *text = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
  NSArray<NSString *> *lines = [text componentsSeparatedByString:CRLF];
  for (NSString *line in lines) {
    NSUInteger location = [line rangeOfString:@":"].location;
    if (location == NSNotFound) {
      continue;
    }
    NSString *key = [line substringToIndex:location];
    NSString *value = [[line substringFromIndex:location + 1] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    [headers setValue:value forKey:key];
  }
  return headers;
}

- (void)emitChunk:(NSData *)data callback:(ABI11_0_0RCTMultipartCallback)callback done:(BOOL)done
{
  NSData *marker = [CRLF CRLF dataUsingEncoding:NSUTF8StringEncoding];
  NSRange range = [data rangeOfData:marker options:0 range:NSMakeRange(0, data.length)];
  if (range.location == NSNotFound) {
    callback(nil, data, done);
  } else {
    NSData *headersData = [data subdataWithRange:NSMakeRange(0, range.location)];
    NSInteger bodyStart = range.location + marker.length;
    NSData *bodyData = [data subdataWithRange:NSMakeRange(bodyStart, data.length - bodyStart)];
    callback([self parseHeaders:headersData], bodyData, done);
  }
}

- (BOOL)readAllParts:(ABI11_0_0RCTMultipartCallback)callback
{
  NSInteger start = 0;
  NSData *delimiter = [[NSString stringWithFormat:@"%@--%@%@", CRLF, _boundary, CRLF] dataUsingEncoding:NSUTF8StringEncoding];
  NSData *closeDelimiter = [[NSString stringWithFormat:@"%@--%@--%@", CRLF, _boundary, CRLF] dataUsingEncoding:NSUTF8StringEncoding];
  NSMutableData *content = [[NSMutableData alloc] initWithCapacity:1];

  const NSUInteger bufferLen = 4 * 1024;
  uint8_t buffer[bufferLen];

  [_stream open];
  while (true) {
    BOOL isCloseDelimiter = NO;
    NSRange remainingBufferRange = NSMakeRange(start, content.length - start);
    NSRange range = [content rangeOfData:delimiter options:0 range:remainingBufferRange];
    if (range.location == NSNotFound) {
      isCloseDelimiter = YES;
      range = [content rangeOfData:closeDelimiter options:0 range:remainingBufferRange];
    }

    if (range.location == NSNotFound) {
      NSInteger bytesRead = [_stream read:buffer maxLength:bufferLen];
      if (bytesRead <= 0 || _stream.streamError) {
        return NO;
      }
      [content appendBytes:buffer length:bytesRead];
      continue;
    }

    NSInteger end = range.location;
    NSInteger length = end - start;

    // Ignore preamble
    if (start > 0) {
      NSData *chunk = [content subdataWithRange:NSMakeRange(start, length)];
      [self emitChunk:chunk callback:callback done:isCloseDelimiter];
    }

    if (isCloseDelimiter) {
      return YES;
    }

    start = end + delimiter.length;
  }
}

@end

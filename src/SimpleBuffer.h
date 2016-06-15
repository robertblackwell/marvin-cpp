//
//  SimpleBuffer.h
//  http-parse-test
//
//  Created by ROBERT BLACKWELL on 7/17/14.
//  Copyright (c) 2014 Blackwellapps. All rights reserved.
//

#include <string>
#pragma once
#pragma mark - SimpleBuffer

#define kSIMPLE_BUFFER_SIZE 1000

typedef struct{
    char  open_guard[5];
    char* buffer;
    int   used;
    int   length;
    char  close_guard[5];
} SimpleBuffer;

SimpleBuffer* SimpleBufferNew();
void SimpleBufferFree(SimpleBuffer* sb);
void SimpleBufferAppend(SimpleBuffer* sb, char* at, size_t length );
std::string SimpleBufferToNSString(SimpleBuffer* sb);



#include <stdlib.h>
#include <string>
#include <stdbool.h>
#include "SimpleBuffer.h"

void
SimpleBufferDump(SimpleBuffer* sb, std::string message)
{
    //NSLog(@"SB Dump  %@ SB: %x buf: %x used: %d length: %d", message, (int)sb, (int)sb->buffer, sb->used, sb->length);
}

bool xstrncmp(char* one, char* two, int n){
    for(int i=0; i < n; i++){
        if( one[i] != two[i])
            return false;
    }
    return true;
}

void SBTestGuards(SimpleBuffer* sb, std::string msg)
{
    char* opn = "OPN";
    char* cls = "CLS";
    if( xstrncmp(sb->open_guard, opn, 3) && xstrncmp(sb->close_guard, cls, 3) )
        return;
    
//    [NSException raise:@"SimpleBuffer" format:@"%@  ", msg];
 
}

SimpleBuffer* SimpleBufferNew()
{
    SimpleBuffer* sb = (SimpleBuffer*)malloc(sizeof(SimpleBuffer));
    strcpy(sb->open_guard, "OPN");
    strcpy(sb->close_guard, "CLS");
    sb->length = kSIMPLE_BUFFER_SIZE;
    sb->used = 0;
    sb->buffer = (char*)malloc(kSIMPLE_BUFFER_SIZE);
    SBTestGuards(sb, "SB New");
    SimpleBufferDump(sb, "SBNew");
    return sb;
}
void SimpleBufferFree(SimpleBuffer* sb)
{
    SBTestGuards(sb, "Free");
    SimpleBufferDump(sb, " SBFREE");
    free(sb->buffer);
    free(sb);
}
void SimpleBufferAppend(SimpleBuffer* sb, char* at, size_t length )
{    
    SBTestGuards(sb, "Append begin");
    SimpleBufferDump(sb, "Append ");
    
    if(sb->used + length > sb->length){
        // extend buffer
        
        unsigned long min_space_needed = length + sb->used;// - sb->length;
        
        int new_size = ()(int)MAX(kSIMPLE_BUFFER_SIZE, min_space_needed);
        
        char* b = (char*)malloc(new_size);
        sb->length = new_size;
        
//        char* b = malloc(sb->length + MAX(kSIMPLE_BUFFER_SIZE, min_space_needed));
        memcpy(b, &(sb->buffer[0]), sb->used);
        char* tmp = sb->buffer;
        free(tmp);
        sb->buffer = b;
    }
    memcpy(&sb->buffer[sb->used], at, length );
    sb->used += length;
    SBTestGuards(sb, "Append end");
    SimpleBufferDump(sb, "After append");
}

//NSString* SimpleBufferToNSString(SimpleBuffer* sb)
//{
//    if( sb == nil )
//    {
//        std::string str = "Is Nil";
//        return str;
//    }
//    else
//    {
//        char* str = malloc(sb->used+1);
//        strncpy(str, sb->buffer, sb->used);
//        str[sb->used] = 0;
//        std::string nStr = std::string(str);
//        free(str);
//        return nStr;
//    }
//}


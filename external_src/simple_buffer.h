#ifndef SIMPLE_BUFFER_H
#define SIMPLE_BUFFER_H

#include <stdlib.h>

#define kSIMPLE_BUFFER_SIZE 100000

typedef struct simple_buffer_s{
    char  open_guard[5];
    char* buffer;
    int   used;
    int   length;
    char  close_guard[5];
} simple_buffer_t;

#ifdef __cplusplus
extern "C"
{
#endif

simple_buffer_t*    sb_create();
void                sb_free(simple_buffer_t* sb);
void                sb_append(simple_buffer_t* sb, char* at, size_t length );
int                 sb_get_used(simple_buffer_t* sb);

int                 sb_to_string(simple_buffer_t* sb, char** buf);

#ifdef __cplusplus
}
#endif



#endif

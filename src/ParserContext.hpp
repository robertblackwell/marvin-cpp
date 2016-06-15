#include "simple_buffer.h"
#include "HTTPMessage.hpp"
#pragma once

/*
 * We are going to attach an instance of this object to the http_parser struct.
 * To do this without pulling an ARC error we need to do some type casting
 * The two functions PointerToParserContext and ParserContextToPointer
 * make the type casts
 */

#define kHEADER_STATE_NOTHING 10
#define kHEADER_STATE_FIELD   11
#define kHEADER_STATE_VALUE   12


class ParserContext
{
public:

//	BufferPtr    	body;
    //
    // This is a reference to the same messages as is referenced from
    // the HTTPParsers last_message property
    //
    HTTPMessage*   	message;
	int				header_state;
	///////////////////////////////////////////////////////////////////////////////////
	// These are C structures and must be allocated and freed explicitly with SBFree()
    // They are also temporary working variables used only during the parsing process
	///////////////////////////////////////////////////////////////////////////////////
	simple_buffer_t*   url_buf;
	simple_buffer_t*   status_buf;
	simple_buffer_t*   name;
	simple_buffer_t*   value;
	////////////////////////////////////////////////////////////////////

	void*             objc_parser; //There are reasons this cannot be more strongly typed
    ParserContext();
    ~ParserContext();
};

/*
 * saved a name : value pair into the header map of the contextRef object
 */
void saveNameValuePair(http_parser* parser, simple_buffer_t* name, simple_buffer_t* value);

/*
 * Casts a point to a ParserContext*
 */
//ParserContext*  PointerToParserContext(void* pointer);

/*
 * reverses the process and casts a ParserRef to void*
 */
//void*   ParserContextRefToPointer(ParserContext* ref);


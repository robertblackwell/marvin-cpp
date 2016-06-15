#include <string>
#include <map>
#include <iostream>
#include "http_parser.h"

#pragma once
#pragma mark - http method constants and strings
/****************************************************************************/
/*
 * The next three lines are about having objc constants that match up with
 * the values used in http_parser.h
 */

/*
#define YY(num, name, string) kHttpMethod##name = num,

enum _HttpMethodEnum
 {
    
HTTP_METHOD_MAP(YY)

 };

#undef YY

typedef enum _HttpMethodEnum HttpMethodType;
*/
/*
 * these can be turned into NSStrings* by:
 */



typedef std::map<std::string, std::string> MessageHeaders;

typedef std::string Buffer;
typedef std::shared_ptr<Buffer>	BufferPtr;


class HTTPMessage
{
public:
	//
	// Request specific fields, HTTP Method and destination URL
	//
	enum http_method  					method;
	std::string							url;
	
	//
	// Response specific fields, HTTP status (numeric) and text description
	//	
	int									status_code;
	std::string							status;

	//
	// Common element of both request and response as in http/major.minor
	//
	int									http_major;
	int									http_minor;
	
    /**
     * The message headers (not the first line), they are key value pairs.
     * Currently no text transformis applied but probably at some point
     * will convert everything to camel-case
     */
    static const std::string 			CONTENT_LENGTH;
    static const std::string 			CONNECTION;
	MessageHeaders						headers;
    
    /**
     * A pointer to a buffer holding the message body as an octet stream
     */
    BufferPtr                            body;
    
    /**
     * meta fields - not part of the physical message but derived from the message
     */
    std::string     connect_host;
    int             connect_port;


	HTTPMessage();
	~HTTPMessage();
    
    /**
     * factory methods to create some special response messages
     */
	static HTTPMessage ConnectOKMessage();
	static HTTPMessage ConnectFailedMessage();

	std::string		methodAsString();
    
    /**
     * General purpose header management methods
     */
    std::string     headersAsString();
    bool            hasHeader( std::string key);
    void            removeHeader( std::string key);
    void            setHeader(std::string key, std::string value);
    std::string     getHeader(std::string key);
    void            dumpHeaders(std::ostream& os);
    
	/**
     * Special or convenience methods for accesing header/url information.
     *
     * Specifically - get the host and port for a HTTP "CONNECT" request that
     * would be typically sent when a browser knows a proxy is operating
     */
    std::string 	getConnectHost();
	int				getConnectPort();

    /**
     * Other convenience methods for determining features of the message
     */
	bool isRequest();
	bool isHTTPS();
	bool isHTTP();
    
    /**
     * Various serialization methods
     */
    std::string     firstLineAsString();
    std::string 	bodyAsString();
    std::string     messageAsString();
	BufferPtr       serialize();

};

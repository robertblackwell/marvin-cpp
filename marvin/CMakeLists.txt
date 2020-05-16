##
## marvin/CMakeLists.txt
##
## this directory exports marvin_library
##
## Note includes 4 files from vendor/src
##
set(EXTSRC ${MARVIN_VENDOR_SRCDIR})

set(MARVIN_LIBRARY_HEADER_FILES 
	boost_stuff.hpp 
	callback_typedefs.hpp 

	buffer/buffer.hpp
	buffer/buffer_chain.hpp
	buffer/m_buffer.hpp
	
	certificates/certificates.hpp
	certificates/env_utils.hpp

	client/client.hpp
	client/request.hpp 

	collector/collector_base.hpp
	collector/pipe_collector.hpp
	collector/collector_interface.hpp


	connection/half_tunnel.hpp
	connection/socket_factory.hpp
	connection/socket_interface.hpp
	connection/connection.hpp
	connection/timeout.hpp
	connection/tunnel_handler.hpp

	error/marvin_error.hpp 

	error_handler/exception.hpp
	error_handler/error_handler.hpp
	
	${EXTSRC}/CxxUrl/url.hpp 
	${EXTSRC}/uri-parser/UriParser.hpp 
	${EXTSRC}/http-parser/http_parser.h 

	helpers/helpers_fs.hpp
	helpers/mitm.hpp
	
	http/headers_v2.hpp 
	http/http_method.hpp 
	http/message_base.hpp 
	http/message_factory.hpp
	http/parser.hpp
	http/uri.hpp 
	http/uri_query.hpp

	message/full_msg_rdr.hpp
	message/message_reader.hpp 
	message/message_writer.hpp 
	
	server_v3/adapter.hpp 
	server_v3/connection_handler.hpp 
	server_v3/tcp_server.hpp 
	server_v3/request_handler_interface.hpp
	server_v3/server_connection_manager.hpp 
	server_v3/server_context.hpp
	server_v3/mitm_app.hpp
	server_v3/mitm_https.hpp
	server_v3/mitm_http.hpp
	server_v3/mitm_tunnel.hpp

	)

set(MARVIN_LIBRARY_SOURCE_FILES 

	${EXTSRC}/CxxUrl/url.cpp
	${EXTSRC}/uri-parser/UriCodec.cpp
	${EXTSRC}/http-parser/http_parser.c

	buffer/buffer_chain.cpp			
	buffer/m_buffer.cpp				
	
	certificates/certificates.cpp
	certificates/env_utils.cpp

	client/client.cpp  	
	client/request.cpp 	
	client/request_headers.cpp 			
	client/request_msg.cpp 			
	client/request_hbc.cpp 			
	client/request_response.cpp 			
	client/request_chunk.cpp 			
	
	collector/collector_base.cpp
	collector/collector_interface.cpp
	collector/pipe_collector.cpp

	connection/half_tunnel.cpp     		
	connection/socket_factory.cpp		
	connection/connection.cpp		
	connection/timeout.cpp				
	connection/tunnel_handler.cpp  		
	
	error/marvin_error.cpp 	

	error_handler/exception.cpp
	error_handler/error_handler.cpp

	helpers/helpers_fs.cpp
	helpers/mitm.cpp
	
	http/headers_v2.cpp 		
	http/http_method.cpp 		
	http/message_base.cpp
	http/message_factory.cpp 		
	http/parser.cpp 
	http/uri.cpp
	http/uri_query.cpp
	
	message/full_msg_rdr.cpp
	message/message_reader.cpp 	
	message/message_writer.cpp 	

	server_v3/connection_handler.cpp 
	server_v3/tcp_server.cpp 
	server_v3/request_handler_interface.cpp
	server_v3/server_connection_manager.cpp 
	server_v3/server_context.cpp
	server_v3/mitm_app.cpp
	server_v3/mitm_https.cpp
	server_v3/mitm_http.cpp
	server_v3/mitm_tunnel.cpp

	)

set_source_files_properties(${MARVIN_LIBRARY_SOURCE_FILES} PROPERTIES LANGUAGE CXX)

add_library(marvin_library 
	STATIC 
		${MARVIN_LIBRARY_SOURCE_FILES} 
		${MARVIN_LIBRARY_HEADER_FILES}
)

target_include_directories(marvin_library 
	SYSTEM PUBLIC 
	${MARVIN_INCLUDE_PATHS}
)

target_precompile_headers(marvin_library PUBLIC
	<marvin/configure_trog.hpp>
  	<marvin/boost_stuff.hpp>
	<memory>
	<stddef.h>
	<vector>
	<string>
	<iostream>
	<sstream>
	<functional>
	<iterator>
	<thread>
	<pthread.h>
	<regex>
	<map>
	<boost/asio.hpp>
	<boost/asio/ssl.hpp>
	<boost/system/error_code.hpp>
	<boost/asio/error.hpp>
	<boost/bind.hpp>
	<boost/function.hpp>
	<boost/date_time/posix_time/posix_time.hpp>
	<boost/algorithm/string.hpp>
	<boost/uuid/uuid.hpp>
	<boost/uuid/uuid_generators.hpp>
	<boost/uuid/uuid_io.hpp>
	<boost/filesystem.hpp>
	<boost/unordered_set.hpp>	
	<marvin/error/marvin_error.hpp>
	<boost/process.hpp>
	)

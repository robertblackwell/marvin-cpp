
# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
message("Marvin_SOURCE_DIR: ${Marvin_SOURCE_DIR}")
message("Vendir Dir: ${VENDOR_DIR}")
set(PROJECT_DIR ${Marvin_SOURCE_DIR})
# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS include/*.hpp)

file(GLOB HEADERS 
	boost_stuff.hpp 
	callback_typedefs.hpp 
	m_log.hpp

	buffer/buffer.hpp
	buffer/buffer_chain.hpp
	buffer/m_buffer.hpp
	
	certificates/certificates.hpp

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
	error/exception.hpp
	
	external_src/CxxUrl/url.hpp 
	external_src/uri-parser/UriParser.hpp 
	external_src/http-parser/http_parser.h 
	external_src/rb_logger/rb_logger.hpp  
	external_src/simple_buffer/simple_buffer.h
	
	forwarding/forward_helpers.hpp 
	forwarding/forwarding_handler.hpp
	forwarding/ssl_forwarding_handler.hpp

	helpers/helpers_fs.hpp
	helpers/error.hpp
	helpers/macros.hpp
	
	http/http_header.hpp 
	http/headers_v2.hpp 
	http/http_method.hpp 
	http/message_base.hpp 
	http/message_factory.hpp
	http/message_interface.hpp 
	http/ordered_key_value.hpp 
	http/parser.hpp
	http/uri.hpp 
	http/uri_query.hpp
	http/uri_v2.hpp

	message/message_reader.hpp 
	message/message_writer.hpp 
	
	server/connection_handler.hpp 
	server/http_server.hpp 
	server/request_handler_base.hpp
	server/server_connection_manager.hpp 
	server/server_context.hpp

	server_v2/connection_handler.hpp 
	server_v2/http_server.hpp 
	server_v2/request_handler_base.hpp
	server_v2/server_connection_manager.hpp 
	server_v2/server_context.hpp


	server_v3/adapter.hpp 
	server_v3/connection_handler.hpp 
	server_v3/server.hpp 
	server_v3/request_handler_interface.hpp
	server_v3/server_connection_manager.hpp 
	server_v3/server_context.hpp
	)

set(SOURCES 

	m_log.cpp
	
	buffer/buffer_chain.cpp			
	buffer/m_buffer.cpp				
	
	certificates/certificates.cpp

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
	error/exception.cpp		

	external_src/CxxUrl/url.cpp
	external_src/uri-parser/UriCodec.cpp
	external_src/http-parser/http_parser.c
	external_src/rb_logger/rb_logger.cpp
	external_src/simple_buffer/simple_buffer.c

	forwarding/forward_helpers.cpp 		
	forwarding/forwarding_handler.cpp 	
	forwarding/ssl_forwarding_handler.cpp 	

	helpers/helpers_fs.cpp
	helpers/error.cpp
	
	http/http_header.cpp 		
	http/headers_v2.cpp 		
	http/http_method.cpp 		
	http/message_base.cpp
	http/message_factory.cpp 		
	http/ordered_key_value.cpp 
	http/ordered_key_value.cpp  			
	http/parser.cpp 
	http/uri.cpp
	http/uri_query.cpp		
	http/uri_v2.cpp
	
	message/message_reader.cpp 	
	message/message_writer.cpp 	
	
	# server/connection_handler.cpp 			
	# server/request_handler_base.cpp
	# server/http_server.cpp 					
	# server/server_connection_manager.cpp 
	# # leave out server_context - no functions in this file - produces library warning that is a bit obscure
	# # server/server_context.cpp

	# server_v2/connection_handler.cpp 
	# server_v2/http_server.cpp 
	# server_v2/request_handler_base.cpp
	# server_v2/server_connection_manager.cpp 
	# server_v2/server_context.cpp

	# server_v3/adapter.cpp
	server_v3/connection_handler.cpp 
	server_v3/server.cpp 
	server_v3/request_handler_interface.cpp
	server_v3/server_connection_manager.cpp 
	server_v3/server_context.cpp

	)

set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

set(HEADER_DIRS ${PROJECT_DIR}
)
	# ${PROJECT_DIR}/marvin/include
	# ${PROJECT_DIR}/marvin/boost_ext
	# ${PROJECT_DIR}/marvin/buffer
	# ${PROJECT_DIR}/marvin/client
	# ${PROJECT_DIR}/marvin/collector
	# ${PROJECT_DIR}/marvin/config
	# ${PROJECT_DIR}/marvin/connection
	# ${PROJECT_DIR}/marvin/error
	# ${PROJECT_DIR}/marvin/forwarding
	# ${PROJECT_DIR}/marvin/http
	# ${PROJECT_DIR}/marvin/message
	# ${PROJECT_DIR}/marvin/server
	# ${PROJECT_DIR}/marvin/src
	# ${PROJECT_DIR}/marvin/helpers
	# ${PROJECT_DIR}/marvin/external_src
	# ${PROJECT_DIR}/marvin/external_src/uri
	# ${PROJECT_DIR}/marvin/external_src/CxxUrl
	# )

# set(HEADER_LIST "${CertificateLibrary_SOURCE_DIR}/include/cert/x509/cert.hpp")
message("HEADER_LIST ${HEADERS}")
message("Sources: ${SOURCES}")

add_library(marvin_library STATIC ${SOURCES} ${HEADERS})
# set_property(TARGET marvin_library PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
# We need this directory, and users of our library will need it too

target_include_directories(marvin_library SYSTEM PUBLIC ${HEADER_DIRS} ${VENDOR_DIR}/include)

target_precompile_headers(marvin_library PUBLIC
	<marvin/external_src/rb_logger/rb_logger.hpp>
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
	# )
# <marvin/server/request_handler_base.hpp>
# <marvin/external_src/uri-parser/UriParser.hpp>
# <marvin/client/client.hpp>
# <marvin/http/message_base.hpp>
# <marvin/connection/connection.hpp>
# <marvin/http/http_header.hpp>
# <marvin/connection/tunnel_handler.hpp>
# <marvin/forwarding/ssl_forwarding_handler.hpp>
# <marvin/collector/collector_interface.hpp>

	)

# This depends on (header only) boost
# target_link_libraries(cert_library PRIVATE Boost::boost)

# All users of this library will need at least C++11
target_compile_features(marvin_library PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
# source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})
# source_group(headers FILES ${HEADER_LIST})
# source_group(headers REGULAR_EXPRESSION include/cert/*.hpp})


##
## marvin/CMakeList.txt 
##
## this directory exports a library target - marvin_library
##


# message("Marvin_SOURCE_DIR: ${Marvin_SOURCE_DIR}")
# message("Vendir Dir: ${VENDOR_DIR}")
# set(PROJECT_DIR ${Marvin_SOURCE_DIR})
set(EXT_SRC ${MARVIN_VENDOR_SRCDIR})

# # Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS include/*.hpp)

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

	error_handler/error_handler.hpp
	
	${EXT_SRC}/CxxUrl/url.hpp
	${EXT_SRC}/uri-parser/UriParser.hpp 
	${EXT_SRC}/http-parser/http_parser.h 
	${EXT_SRC}/simple_buffer/simple_buffer.h
	
	helpers/helpers_fs.hpp
	helpers/mitm.hpp
	
	http/headers_v2.hpp 
	http/http_method.hpp 
	http/message_base.hpp 
	http/message_factory.hpp
	http/message_interface.hpp 
	http/parser.hpp
	http/uri.hpp 
	http/uri_query.hpp

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
	error_handler/error_handler.cpp

	${EXT_SRC}/CxxUrl/url.cpp
	${EXT_SRC}/uri-parser/UriCodec.cpp

	${EXT_SRC}/http-parser/http_parser.c 
	${EXT_SRC}/simple_buffer/simple_buffer.c

	helpers/helpers_fs.cpp
	helpers/mitm.cpp
	
	http/headers_v2.cpp 		
	http/http_method.cpp 		
	http/message_base.cpp
	http/message_factory.cpp 		
	http/parser.cpp 
	http/uri.cpp
	http/uri_query.cpp
	
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

message("MARVIN_LIBRARY_SOURCE_FILES               ${MARVIN_LIBRARY_SOURCE_FILES}")
# message("MARVIN_LIBRARY_HEADER_FILES               ${MARVIN_LIBRARY_HEADER_FILES}")
if(OFF)
set_source_files_properties(${EXT_SRC}/http_parser/http_parser.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(${EXT_SRC}/simple_buffer/simple_buffer.c PROPERTIES LANGUAGE CXX)

add_library(marvin_c_library
	STATIC 
		external_src/simple_buffer/simple_buffer.c
		external_src/simple_buffer/simple_buffer.h
		external_src/http-parser/http_parser.c
		external_src/http-parser/http_parser.h
	)
# set_target_properties(marvin_c_library PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(marvin_c_library 
	SYSTEM PUBLIC
		${MARVIN_INCLUDE_PATHS} 
		# ${MARVIN_PROJECT_DIR}
		# ${MARVIN_VENDOR_INCLUDEDIR}
		# ${MARVIN_VENDOR_SRCDIR}
)
endif()
if (ON)
add_library(marvin_library 
	STATIC  
		${MARVIN_LIBRARY_SOURCE_FILES}
		${MARVIN_LIBRARY_HEADER_FILES}
)
set_target_properties(marvin_library PROPERTIES LINKER_LANGUAGE CXX)
# set_property(TARGET marvin_library PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
# We need this directory, and users of our library will need it too

target_include_directories(marvin_library 
	SYSTEM PUBLIC
		${MARVIN_INCLUDE_PATHS} 
		# ${MARVIN_PROJECT_DIR}
		# ${MARVIN_VENDOR_INCLUDEDIR}
		# ${MARVIN_VENDOR_SRCDIR}
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
endif()

# This depends on (header only) boost
# target_link_libraries(cert_library PRIVATE Boost::boost)

# All users of this library will need at least C++11
# target_compile_features(marvin_library PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
# source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})
# source_group(headers FILES ${HEADER_LIST})
# source_group(headers REGULAR_EXPRESSION include/cert/*.hpp})

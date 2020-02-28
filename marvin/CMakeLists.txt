
# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
message("Marvin_SOURCE_DIR: ${Marvin_SOURCE_DIR}")
message("Vendir Dir: ${VENDOR_DIR}")
set(PROJECT_DIR ${Marvin_SOURCE_DIR})
# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS include/*.hpp)

file(GLOB HEADERS 
	include/boost_stuff.hpp 
	include/callback_typedefs.hpp 

	boost_ext/io_service_pool.hpp

	buffer/buffer.hpp
	buffer/buffer_chain.hpp
	buffer/m_buffer.hpp
	
	client/client.hpp

	collector/collector_base.hpp
	collector/pipe_collector.hpp
	collector/collector_interface.hpp


	connection/half_tunnel.hpp
	connection/socket_factory.hpp
	connection/tcp_connection.hpp
	connection/timeout.hpp
	connection/tls_connection.hpp
	connection/tunnel_handler.hpp
	
	external_src/CxxUrl/url.hpp 
	external_src/uri-parser/uri-parser/UriParser.hpp 
	external_src/http-parser/http_parser.h 
	external_src/rb_logger/rb_logger.hpp  
	external_src/simple_buffer/simple_buffer.h

	error/marvin_error.hpp 
	
	forwarding/forward_helpers.hpp 
	forwarding/forwarding_handler.hpp
	
	http/http_header.hpp 
	http/marvin_http.hpp 
	http/marvin_okv.hpp 
	http/message.hpp 
	http/parser.hpp
	
	message/http_method.hpp 
	message/marvin_uri.hpp 
	message/message_reader.hpp 
	message/message_writer.hpp 
	message/uri_query.hpp
	
	server/request_handler_base.hpp
	server/connection_handler.hpp 
	server/http_server.hpp 
	server/request_handler.hpp 
	server/server_connection_manager.hpp 
	server/server_context.hpp

	helpers/helpers_fs.hpp
	helpers/error.hpp
	helpers/macros.hpp
	)
	# include/forwarding_handler.hpp 
	# include/forwarding_handler.ipp 
	# include/utest.hpp
	# src/HTTPMessage src/request_handler.hpp
	# collector/collector_base.cpp
	# collector/i_collector.cpp 
	# collector/pipe_collector
	
	# client/request.hpp
	# config/marvin.hpp
	# connection/i_socket.hpp
	
	# )


set(SOURCES 
	external_src/CxxUrl/url.cpp
	external_src/uri-parser/UriCodec.cpp
	external_src/http-parser/http_parser.c
	external_src/rb_logger/rb_logger.cpp
	external_src/simple_buffer/simple_buffer.c

	boost_ext/io_service_pool.cpp

	buffer/buffer_chain.cpp			
	buffer/m_buffer.cpp				
	
	client/client.cpp  				
	
	collector/collector_base.cpp
	collector/pipe_collector.cpp
	collector/collector_interface.cpp

	connection/half_tunnel.cpp     		
	connection/socket_factory.cpp		
	connection/tcp_connection.cpp		
	connection/timeout.cpp				
	connection/tls_connection.cpp		
	connection/tunnel_handler.cpp  		
	
	error/marvin_error.cpp 				

	forwarding/forward_helpers.cpp 		
	forwarding/forwarding_handler.cpp 	
	
	http/http_header.cpp 		
	http/marvin_http.cpp 		
	http/marvin_okv.cpp 		
	http/message.cpp  			
	http/parser.cpp 			
	
	message/http_method.cpp 	
	message/marvin_uri.cpp
	message/message_reader.cpp 	
	message/message_writer.cpp 	
	message/uri_query.cpp 		
	
	server/request_handler_base.cpp
	server/connection_handler.cpp 			
	server/http_server.cpp 					
	server/server_connection_manager.cpp 	 

	helpers/helpers_fs.cpp
	helpers/error.cpp
	)
	# server/server_context.cpp 				
	# client/request.cpp  X
	
	# collector/collector_base.cpp
	# collector/i_collector.cpp 
	# collector/pipe_collector.cpp
	
	# config/marvin.hpp

	# error/test_error.hpp           X
	# http/i_message.hpp 			X
	# message/marvin_uri.hpp        X 
	# connection/i_socket.cpp 			X
	# src/HTTPMessage src/request_handler.hpp X

	# )
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

set(HEADER_DIRS
	${PROJECT_DIR}
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

# We need this directory, and users of our library will need it too
target_include_directories(marvin_library PUBLIC ${HEADER_DIRS} ${VENDOR_DIR}/include)
# This depends on (header only) boost
# target_link_libraries(cert_library PRIVATE Boost::boost)

# All users of this library will need at least C++11
target_compile_features(marvin_library PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
# source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})
# source_group(headers FILES ${HEADER_LIST})
# source_group(headers REGULAR_EXPRESSION include/cert/*.hpp})

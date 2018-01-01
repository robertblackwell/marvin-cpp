SRC=external_src

DEP_INCLUDE_DIR=deps/include
DEP_LIB_DIR=deps/lib

CLONE_DIR=packages

LOCAL_REPO_DIR=$(HOME)/git-repos

CXXURL_DIR=$(SRC)/CxxUrl
URLPARSER_DIR=$(SRC)/uri
SIMPLEBUFFER=simple_buffer.h simple_buffer.c
HTTP_PARSER_C=http_parser.c http_parser.h

MYINSTALLS=$(HOME)/MyInstalls
OPENSSL_DIR=$(MYINSTALLS)/openssl/openssl-1.0.1g
OPENSSL_INCLUDE = $(OPENSSL_DIR)/include
OPENSSL_LIBS=$(OPENSSL_DIR)
JSON=https://github.com/nlohmann/json.git
BIN=~/MyCurrentProjects/Pixie/MarvinCpp/DerivedData/MarvinCpp/Build/Products/Debug

.PHONY: run_tests
tests_run:
	$(BIN)/test_server_client_body_format
	$(BIN)/test_server_client_body_buffering
	$(BIN)/test_timeout
	$(BIN)/test_connect
	$(BIN)/test_reader
	$(BIN)/test_client_request

.PHONY: tests_all
tests_all: tests_clean tests_build

.PHONY: tests_clean
tests_clean:
	xcodebuild -scheme test_server_client_body_buffering clean 
	xcodebuild -scheme test_server_client_body_format clean
	xcodebuild -scheme test_timeout clean
	xcodebuild -scheme test_connect clean
	xcodebuild -scheme test_reader clean
	xcodebuild -scheme test_client_request clean


.PHONY: tests_build
tests_build:
	@xcodebuild -quiet -scheme test_server_client_body_buffering build
	@ xcodebuild -quiet -scheme test_server_client_body_format build 
	@ xcodebuild -quiet -scheme test_timeout build 
	@ xcodebuild -quiet -scheme test_connect build 
	@ xcodebuild -quiet -scheme test_reader build 
	# this was tricky - watch the escaping

$(CLONE_DIR):
	mkdir $(CLONE_DIR)
	
$(SRC):
	mkdir $(SRC)

$(CXXURL_DIR):	
	mkdir $(CXXURL_DIR)

$(URLPARSER_DIR):	
	mkdir $(URLPARSER_DIR)
json:
	rm -rfv $(CLONE_DIR)/json
	git clone $(JSON) --branch master --single-branch  $(CLONE_DIR)/json
	cp $(CLONE_DIR)/json/src/json.hpp $(SRC)/

catch2:
	rm -rfv $(CLONE_DIR)/Catch2
	# git clone https://github.com/catchorg/Catch2.git $(CLONE_DIR)/cpp_catch
	git clone $(LOCAL_REPO_DIR)/Catch2 $(CLONE_DIR)/Catch2
	cp $(CLONE_DIR)/Catch2/single_include/catch.hpp  $(SRC)/


simple_buffer:
	git clone $(LOCAL_REPO_DIR)/simple_buffer $(CLONE_DIR)/simple_buffer
	cp $(CLONE_DIR)/simple_buffer/src/*.c  $(SRC)/
	cp $(CLONE_DIR)/simple_buffer/src/*.h  $(SRC)/

http_parser:
	git clone $(LOCAL_REPO_DIR)/http-parser $(CLONE_DIR)/http-parser
	cp $(CLONE_DIR)/http-parser/http_parser.c  $(SRC)/
	cp $(CLONE_DIR)/http-parser/http_parser.h  $(SRC)/

cxxurl: $(CXXURL_DIR)
	rm -rf $(CLONE_DIR)/cxxurl
	git clone https://github.com/chmike/CxxUrl.git $(CLONE_DIR)/cxxurl
	# git clone $(REPO_DIR)/http-parser $(CLONE_DIR)/http-parser
	cp $(CLONE_DIR)/CxxUrl/url.hpp  $(CXXURL_DIR)/
	cp $(CLONE_DIR)/CxxUrl/url.cpp  $(CXXURL_DIR)/

urlparser:: $(URLPARSER_DIR)
	rm -rf $(CLONE_DIR)/urlparser
	git clone https://github.com/CovenantEyes/uri-parser.git  $(CLONE_DIR)/urlparser
	cp $(CLONE_DIR)/urlparser/UriParser.hpp  $(URLPARSER_DIR)/

openssl_include:
	-rm -rf $(DEP_INCLUDE_DIR)/openssl
	ln -s $(OPENSSL_INCLUDE)/openssl $(DEP_INCLUDE_DIR)/openssl

openssl_libs:
	-rm $(DEP_LIB_DIR)/libcrypto.a
	ln -s $(OPENSSL_LIBS)/libcrypto.a $(DEP_LIB_DIR)/libcrypto.a
	-rm $(DEP_LIB_DIR)/libssl.a
	ln -s $(OPENSSL_LIBS)/libssl.a $(DEP_LIB_DIR)/libssl.a


openssl: openssl_include openssl_libs


install_deps: $(CLONE_DIR)  $(SRC) simple_buffer http_parser catch2 cxxurl urlparser
	
	
clean_CLONE_DIR:	
	rm -Rf $(SRC)/*
	rm -Rf $(CLONE_DIR)/*
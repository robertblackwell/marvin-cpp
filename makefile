SRC=external_src

CXXURL_DIR=$(SRC)/CxxUrl

URLPARSER_DIR=$(SRC)/uri

DEPS=deps

REPO_DIR=$(HOME)/git-repos

SIMPLEBUFFER=simple_buffer.h simple_buffer.c

HTTP_PARSER_C=http_parser.c http_parser.h

$(DEPS):
	mkdir $(DEPS)
	
$(SRC):
	mkdir $(SRC)

$(CXXURL_DIR):	
	mkdir $(CXXURL_DIR)

$(URLPARSER_DIR):	
	mkdir $(URLPARSER_DIR)

cpp-catch:
	git clone $(REPO_DIR)/cpp-catch $(DEPS)/cpp-catch
	cp $(DEPS)/cpp-catch/*.hpp  $(SRC)/


simple_buffer:
	git clone $(REPO_DIR)/simple_buffer $(DEPS)/simple_buffer
	cp $(DEPS)/simple_buffer/src/*.c  $(SRC)/
	cp $(DEPS)/simple_buffer/src/*.h  $(SRC)/

http_parser:
	git clone $(REPO_DIR)/http-parser $(DEPS)/http-parser
	cp $(DEPS)/http-parser/http_parser.c  $(SRC)/
	cp $(DEPS)/http-parser/http_parser.h  $(SRC)/

cxxurl: $(CXXURL_DIR)
	rm -rf $(DEPS)/cxxurl
	git clone https://github.com/chmike/CxxUrl.git $(DEPS)/cxxurl
	# git clone $(REPO_DIR)/http-parser $(DEPS)/http-parser
	cp $(DEPS)/CxxUrl/url.hpp  $(CXXURL_DIR)/
	cp $(DEPS)/CxxUrl/url.cpp  $(CXXURL_DIR)/

urlparser:: $(URLPARSER_DIR)
	rm -rf $(DEPS)/urlparser
	git clone https://github.com/CovenantEyes/uri-parser.git  $(DEPS)/urlparser
	cp $(DEPS)/urlparser/UriParser.hpp  $(URLPARSER_DIR)/

install_deps: $(DEPS)  $(SRC) simple_buffer http_parser cpp-catch cxxurl urlparser
	
	
clean_deps:	
	rm -Rf $(SRC)/*
	rm -Rf $(DEPS)/*
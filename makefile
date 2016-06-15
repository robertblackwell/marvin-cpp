SRC=external_src

DEPS=deps

REPO_DIR=$(HOME)/git-repos

SIMPLEBUFFER=simple_buffer.h simple_buffer.c

HTTP_PARSER_C=http_parser.c http_parser.h

$(DEPS):
	mkdir $(DEPS)
	
$(SRC):
	mkdir $(SRC)

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

install_deps: $(DEPS)  $(SRC) simple_buffer http_parser cpp-catch
	
	
clean_deps:	
	rm -Rf $(SRC)/*
	rm -Rf $(DEPS)/*
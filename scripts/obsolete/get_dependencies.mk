# 
# This makefile gets all project dependencies and loads includes/libraries into the ${project_dir}/deps
# directory. 
# The dependencies come either from the homebrew cellar or are directlt loaded from
# the dependency's github repo.
# Verson numbers of those dependencies copied from the homebrew cellar are hardcoded below
# and if that version of the dependcy is not available in the cellar this build will fail
#
# run from the directory holding this make file
#

# github tags for required packages
OPEN_SSL_TAG = OpenSSL_1_1_1d
BOOST_TAG
CATCH2_TAG=master
JSON_TAG=master

project_dir=$(realpath ${shell PWD}/.. )

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(dir $(mkfile_path))

dump:
	echo ${project_dir}
	echo ${current_dir}
	echo ${mkfile_path}

EXTERNAL_SRC=external_src
DEPENDENCIES_DIR_NAME=vendor
DEPENDENCIES_DIR=${project_dir}/external_code/${DEPENDENCIES_DIR_NAME}
DEPENDENCIES_DIR=${project_dir}/${DEPENDENCIES_DIR_NAME}
CLONE_DIR=${current_dir}/cloned_repos

DEPENDENCIES_INCLUDE_DIR=${DEPENDENCIES_DIR}/include
DEPENDENCIES_LIB_DIR=${DEPENDENCIES_DIR}/lib


LOCAL_REPO_DIR=$(HOME)/git-repos

CXXURL_DIR=$(SRC)/CxxUrl
URLPARSER_DIR=$(SRC)/uri
SIMPLEBUFFER=simple_buffer.h simple_buffer.c
HTTP_PARSER_C=http_parser.c http_parser.h

# setup location of libboost, openssl, catch2
ifeq ($(OS),Darwin)
	CELLAR=/usr/local/Cellar
else
	CELLAR=somethingthatcausesfailer
endif

# installs all dependencies 
.PHONY: install
install: boost_install openssl_install catch2_install json_install
	rm -rf ${CLONE_DIR}/*
#duplicate with a different name
.PHONY: install_deps
install_deps:  install

# becareful NOT to delete the ${project_dir}/deps directory
.PHONY: clean
clean:
	-rm -rf $(CLONE_DIR)
	-rm -rf ${DEPENDENCIES_INCLUDE_DIR}
	-rm -rf ${DEPENDENCIES_LIB_DIR}
	
# utilities targets to create directories as required
$(EXTERNAL_SRC):
	mkdir $(EXTERNAL_SRC)

${DEPENDENCIES_LIB_DIR}:
	mkdir -p ${DEPENDENCIES_LIB_DIR}

${DEPENDENCIES_INCLUDE_DIR}:
	mkdir -p ${DEPENDENCIES_INCLUDE_DIR}

${CLONE_DIR}:
	mkdir -p ${CLONE_DIR}


############################################################################################################
# json-nlohmann package commands
# json is installed in the project by cloning the github repo and extracting the required bits
############################################################################################################
JSON_VERSION=xxxxx
JSON_DIR=${CELLAR}/json_nolhmann/${JSON_VERSION}
JSON_INCLUDE_DIR=${JSON_DIR}/include
JSON_LIB_DIR=${JSON_DIR}/lib
JSON_GITHUB=https://github.com/nlohmann/json.git
JSON_BRANCH_OR_TAG=$(JSON_TAG)
.PHONY: json_dump
json_dump:
	@echo ${JSON_VERSION}
	@echo ${JSON_DIR}
	@echo ${JSON_INCLUDE_DIR}
	@echo ${JSON_LIB_DIR}
	@echo ${JSON_GITHUB}


.PHONY: json_install
json_install: json_include

.PHONY: json_clean
json_clean:
	rm -rf $(CLONE_DIR)/json || true
	rm -rf $(DEPENDENCIES_INCLUDE_DIR)/json || true

# .PHONY: json_include
# json_include: ${DEPENDENCIES_INCLUDE_DIR}
# 	-rm -rf $(DEPENDENCIES_INCLUDE_DIR)/json || true
# 	cp -rv $(JSON_INCLUDE_DIR)/* $(DEPENDENCIES_INCLUDE_DIR)

json_clone:
	rm -rf $(CLONE_DIR)/json
	git clone $(JSON_GITHUB) --branch ${JSON_BRANCH_OR_TAG} --single-branch  $(CLONE_DIR)/json

.PHONY: json_include
json_include: json_clone
	rm -rf $(DEPENDENCIES_INCLUDE_DIR)/json || true
	[ -d $(DEPENDENCIES_INCLUDE_DIR)/json ] || mkdir -p $(DEPENDENCIES_INCLUDE_DIR)/json
	cp $(CLONE_DIR)/json/single_include/nlohmann/json.hpp $(DEPENDENCIES_INCLUDE_DIR)/json/
############################################################################################################
# catch2 package commands
# catch2 is installed in the project by copying from the brew install cellar
############################################################################################################
CATCH2_VERSION=2.11.1
CATCH2_DIR=${CELLAR}/catch2/${CATCH2_VERSION}
CATCH2_INCLUDE_DIR=${CATCH2_DIR}/include
CATCH2_LIB_DIR=${CATCH2_DIR}/lib

.PHONY: catch2_dump
catch2_dump:
	@echo ${CATCH2_VERSION}
	@echo ${CATCH2_DIR}
	@echo ${CATCH2_INCLUDE_DIR}
	@echo ${CATCH2_LIB_DIR}
	@echo ${CATCH2_DEPS_INCLUDE_DIR}


.PHONY: catch2_install
catch2_install: catch2_include

.PHONY: catch2_clean
catch2_clean:
	rm -rf $(DEPENDENCIES_INCLUDE_DIR)/catch2 || true

.PHONY: catch2_include
catch2_include: ${DEPENDENCIES_INCLUDE_DIR}
	-rm -rf $(DEPENDENCIES_INCLUDE_DIR)/catch2 || true
	cp -r $(CATCH2_INCLUDE_DIR)/* $(DEPENDENCIES_INCLUDE_DIR)/catch2

############################################################################################################
# openssl package commands
# openssl is installed in the project by copying from the brew install cellar
############################################################################################################
OPENSSL_VERSION=1.1.1d
OPENSSL_DIR=$(CELLAR)/openssl@1.1/${OPENSSL_VERSION}
OPENSSL_INCLUDE_DIR = $(OPENSSL_DIR)/include
OPENSSL_LIB_DIR=$(OPENSSL_DIR)/lib

.PHONY: openssl_dump
openssl_dump:
	@echo ${OPENSSL_VERSION}
	@echo ${OPENSSL_DIR}
	@echo ${OPENSSL_INCLUDE_DIR}
	@echo ${OPENSSL_LIB_DIR}
	@echo ${OPENSSL_DEPS_INCLUDE_DIR}

.PHONY: openssl_install
openssl_install: openssl_include openssl_libs

.PHONY: openssl
openssl: openssl_include openssl_libs

.PHONY: openssl_clean
openssl_clean:
	-rm -rf $(DEPENDENCIES_INCLUDE_DIR)/openssl
	-rm $(DEPENDENCIES_LIB_DIR)/libcrypto.a || true
	-rm $(DEPENDENCIES_LIB_DIR)/libssl.a || true


.PHONY: openssl_include
openssl_include: ${DEPENDENCIES_INCLUDE_DIR}
	-rm -rf $(DEPENDENCIES_INCLUDE_DIR)/openssl || true
	cp -rv $(OPENSSL_INCLUDE_DIR)/* $(DEPENDENCIES_INCLUDE_DIR)

.PHONY: openssl_libs
openssl_libs: ${DEPENDENCIES_LIB_DIR}
	-rm $(DEPENDENCIES_LIB_DIR)/libcrypto.a || true
	-rm $(DEPENDENCIES_LIB_DIR)/libssl.a || true
	cp -r $(OPENSSL_LIB_DIR)/lib*.a $(DEPENDENCIES_LIB_DIR)

############################################################################################################
# boost package commands
# boost is installed in the project by copying from the brew install cellar
# 
# If building boost from source use command
#  ./b2 --link=static --threading=multi --variant=debug --layout=system
# 
############################################################################################################
BOOST_VERSION=1.72.0
BOOST_DIR=${CELLAR}/boost/${BOOST_VERSION}
BOOST_INCLUDE_DIR=${BOOST_DIR}/include
BOOST_LIB_DIR=${BOOST_DIR}/lib
BOOST_DEPS_INCLUDE_DIR = ${DEPENDENCIES_INCLUDE_DIR}/boost

.PHONY: boost_dump
boost_dump:
	@echo ${BOOST_VERSION}
	@echo ${BOOST_DIR}
	@echo ${BOOST_INCLUDE_DIR}
	@echo ${BOOST_LIB_DIR}
	@echo ${BOOST_DEPS_INCLUDE_DIR}

.PHONY: boost_wget
boost_wget:
	https://dl.bintray.com/boostorg/release/1.72.0/source/boost_1_72_0.tar.gz
	tar xvzf ${boost_name}.tar.gz 

.PHONY: boost_install
boost_install: boost_include boost_libs

.PHONY: boost_install
boost_clean:
	-rm -rf $(DEPENDENCIES_INCLUDE_DIR)/boost || true
	-rm -rf $(DEPENDENCIES_LIB_DIR)/libboost* || true

.PHONY: boost_include
boost_include: ${DEPENDENCIES_INCLUDE_DIR}
	rm -rf $(DEPENDENCIES_INCLUDE_DIR)/boost || true
	cp -r $(BOOST_INCLUDE_DIR)/* $(DEPENDENCIES_INCLUDE_DIR)

.PHONY: boost_libs
boost_libs: ${DEPENDENCIES_LIB_DIR}
	-rm -rf $(DEPENDENCIES_LIB_DIR)/libboost* || true
	cp -r $(BOOST_LIB_DIR)/libboost_*.a $(DEPENDENCIES_LIB_DIR)

############################################################################################################


clean_CLONE_DIR:
	rm -Rf $(SRC)/*
	rm -Rf $(CLONE_DIR)/*


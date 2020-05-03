#!/bin/bash
# https://github.com/openssl/openssl.git
pwd=`pwd`
project_dir=$pwd
project_name=$(basename $project_dir)
vendor=${pwd}/vendor
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
openssl_name=openssl-1.1.1f

# 
# requires the following variables to be set
# 	project_name
# 	project_dir_name
# 	clone-dir
# 	openssl_name
# 	stage_dir
# 		and include lib ssl  -  via hard coding
# 	vendor_dir
# 		vendor_dir/include
# 		vendor_dir/lib
# 		vendor_dir/ssl 
# 
verify_print_variables \
	project_name \
	project_dir_name \
	clone_dir \
	stage_dir \
	vendor_dir \
	openssl_name

debug=
function help() {
	echo Install openssl_1.1.1f
	echo Usage:
	echo 	stage_openssl_1.1.1f.sh [arg]
	echo
	echo	args is either
	echo		help 	Print this help message
	echo		install After build copy include and libs to final destination
	echo
	echo 	The required package is downloaded into a temp dir inside the scripts dir
	echo	If required the package is built and the headers and libs copied either
	echo 	to a temporary "stage" directory or to the final location		
	exit 0
}
function verify_project_name() {
	if [ $project_name != ${project_dir_name} ] ; then
		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}
function get_package_into() {
	rm -rf ${clone_dir}/${openssl_name}
	cd ${clone_dir}
	echo wget https://www.openssl.org/source/${openssl_name}.tar.gz
	wget https://www.openssl.org/source/${openssl_name}.tar.gz
	tar xvzf ${openssl_name}.tar.gz
	cd ${openssl_name}
	ls -al
}
function stage_package_to() {
	mkdir -p ${stage_dir}/include
	mkdir -p ${sstage_dir}/lib
	mkdir -p ${sstage_dir}/ssl

	# ./Configure --prefix=${script_dir}/stage --openssldir=${script_dir}/stage --debug darwin64-x86_64-cc
	if [[ "$OSTYPE" == "linux-gnu" ]]; then
        ARCHITECTURE=linux-x86_64
	elif [[ "$OSTYPE" == "darwin"* ]]; then
	    ARCHITECTURE=darwin64-x86_64-cc
	elif [[ "$OSTYPE" == "cygwin" ]]; then
	        echo ${OSTYPE} is unknown OS type - will not build
	elif [[ "$OSTYPE" == "msys" ]]; then
	        echo ${OSTYPE} is unknown OS type - will not build
	elif [[ "$OSTYPE" == "win32" ]]; then
	        echo ${OSTYPE} is unknown OS type - will not build
	elif [[ "$OSTYPE" == "freebsd"* ]]; then
	        echo ${OSTYPE} is unknown OS type - will not build
	else
	        echo ${OSTYPE} is unknown OS type - will not build
	fi
	arch
	./Configure --prefix=${stage_dir} --openssldir=${vendor_dir}/ssl --debug ${ARCHITECTURE} #// darwin64-x86_64-cc
	make all
	make install
}
function install_package() {
	mkdir -p ${vendor_dir}/include/openssl
	mkdir -p ${vendor_dir}/lib
	rm -rf ${vendor_dir}/include/openssl/*
	cp -rv ${stage_dir}/include/openssl/* ${vendor}/include/openssl
	cp -rv ${stage_dir}/lib/*.a ${vendor}/lib/
}


if [ "$1" == "help" ] ; then help; fi

echo 
echo INSTALL ${openssl_name} beginning ========================================================
echo

if [ "$1" == "stage" ] || [ "$1" == "install" ]; then
	verify_project_name
	get_package
	stage_package
	echo 
	echo STAGE ${package_name} complete ========================================================
	echo
fi


if [ "$1" == "stage" ] || [ "$1" == "install" ] || [ "$1" == "" ] ; then
	verify_package_name
	get_package_into
	stage_package_to
fi

if [ "$1" == "install" ] || [ "$1" == "install_only" ] ; then
	install_package
	echo 
	echo INSTALL ${package_name} complete ========================================================
	echo
fi


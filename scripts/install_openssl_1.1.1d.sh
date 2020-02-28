#!/bin/bash
# https://github.com/openssl/openssl.git
debug=
function help() {
	echo Install openssl_1.1.1d
	echo Usage:
	echo 	stage_openssl_1.1.1d.sh [arg]
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
	if [ $project_name != "marvin++" ] ; then
		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}
function get_package() {
	rm -rf ${clone_dir}/${openssl_name}
	cd ${clone_dir}
	echo wget https://www.openssl.org/source/${openssl_name}.tar.gz
	wget https://www.openssl.org/source/${openssl_name}.tar.gz
	tar xvzf ${openssl_name}.tar.gz
	cd ${openssl_name}
	ls -al
}
function stage_package() {
	mkdir -p ${script_dir}/stage/include
	mkdir -p ${script_dir}/stage/lib
	mkdir -p ${script_dir}/stage/ssl

	# ./Configure --prefix=${script_dir}/stage --openssldir=${script_dir}/stage --debug darwin64-x86_64-cc

	./Configure --prefix=${script_dir}/stage --openssldir=${vendor}/ssl --debug darwin64-x86_64-cc
	make all
	make install
}
function install_package() {
	mkdir -p ${vendor}/include/openssl
	mkdir -p ${vendor}/lib
	rm -rf ${vendor}/include/openssl/*
	cp -rv ${script_dir}/stage/include/openssl/* ${vendor}/include/openssl
	cp -rv ${script_dir}/stage/lib/*.a ${vendor}/lib/
}


if [ "$1" == "help" ] ; then help; fi

pwd=`pwd`
project_dir=$pwd
project_name=$(basename $project_dir)
vendor=${pwd}/vendor
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
openssl_name=openssl-1.1.1d
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


#!/bin/bash

function install_package {
	mkdir -p ${vendor}/include/cert
	mkdir -p ${vendor}/lib
	rm -rfv ${vendor}/include/cert/*
	rm -rfv ${vendor}/lib/libcert*
	cp -rv ${script_dir}/stage/include/cert/* ${vendor}/include/cert

	cp -rv ${script_dir}/stage/lib/libcert*.a ${vendor}/lib/
	echo 
	echo INSTALL $package complete ========================================================
	echo
}

function get_package {
	cd ${clone_dir}
	rm -rfv ${clone_dir}/${package_name}
	${git_clone}
	cd ${package_name}
	ls -al
}

function stage_package {
	stage_dir=${script_dir}/stage
	mkdir -p ${script_dir}/stage/include
	mkdir -p ${script_dir}/stage/lib
	cd ${clone_dir}/${package_name}
	if [ -d cmake-build-debug ] ; then 
		rm -rf cmake-build-debug/*
	else
		mkdir -p cmake-build-debug
	fi
	cd cmake-build-debug
	pwd
	cmake -DVENDOR_DIR=${stage_dir} -DSTAGE_DIR=${stage_dir} ..
	# cmake  --build . --target cert_library
	make -j 8 cert_library
	cmake --build . --target install -j 8
}

function verify_package_name() {
	if [ $project_name != "marvin++" ] ; then
		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}

function help() {
	echo Install package ${package_name}
	echo Usage:
	echo 	install_${package_name}.sh [arg]
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

debug=

package_name=libcert

if [ "$1" == "help" ] ; then help; fi


pwd=`pwd`
vendor=${pwd}/vendor
project_dir=$pwd
project_name=$(basename $project_dir)
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
git_clone="git clone https://github.com/robertblackwell/x509_certificate_library.git ${release} ${clone_dir}/${package_name}"

echo 
echo INSTALL $package_name begin ========================================================
echo

if [ "$1" == "stage" ] || [ "$1" == "install" ] || [ "$1" == "" ] ; then
	verify_package_name
	get_package
	stage_package
fi

if [ "$1" == "install" ] || [ "$1" == "install_only" ] ; then
	install_package
	echo 
	echo INSTALL ${package_name} complete ========================================================
	echo
fi



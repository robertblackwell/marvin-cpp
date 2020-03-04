#!/bin/bash

# This script installs a header only project into the vendor/include directory

echo 
echo INSTALL ${package_description} beginning ========================================================
echo

function help() {
	echo Install ${package_description}
	echo Usage:
	echo 	stage_${package_description} [arg]
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

function verify_package_name() {
	if [ $project_name != "marvin++" ] ; then

		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}

function get_package_into() {
	cd ${clone_dir}
	rm -rfv ${clone_dir}/${package_name}

	${git_clone}
	# --branch ${json_release} 
	cd ${package_clone_stem}
	ls -al
}

function stage_package_to() {
	mkdir -p ${script_dir}/stage/include
	mkdir -p ${script_dir}/stage/lib
	rm -rfv ${script_dir}/stage/include/${package_name}
	mkdir -p ${script_dir}/stage/include/${package_name}
	cp -rv ${clone_dir}/${package_clone_stem}/${header_cp_pattern} ${script_dir}/stage/include/${package_name}
}

function install_package() {
	mkdir -p ${vendor}/include/${package_name}
	rm -rfv ${vendor}/include/${package_name}/*
	cp -rv ${script_dir}/stage/include/${package_name}/* ${vendor}/include/${package_name}
}

# 
# main
# 
if [ "$1" == "help" ] ; then help; fi
pwd=`pwd`
project_dir=$pwd
project_name=$(basename $project_dir)
source_dir=${project_dir}/marvin
vendor=${project_dir}/vendor
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
stage_dir=${script_dir}/stage
package_stage_dir=${stage_dir}/${package_name}

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


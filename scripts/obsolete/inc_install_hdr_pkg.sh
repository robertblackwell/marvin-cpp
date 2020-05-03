#!/bin/bash

# This script installs a header only project into the vendor/include directory
# 
# requires
# 	project_dir_name
# 	project_name
# 	clone_dir
# 	package_clone_stem
# 	stage_dir
# 	header_cp_pattern
# 	vendor_dir
# 

[ -z "${package_name}" ] && echo WARNING: variable package_name is empty && exit 1
[ -z "${project_name}" ] && echo WARNING: variable project_name is empty && exit 1
[ -z "${project_dir_name}" ] && echo WARNING: variable project_dir_name is empty && exit 1
[ -z "${clone_dir}" ] && echo WARNING: variable clone_dir is empty && exit 1
[ -z "${package_clone_stem}" ] && echo WARNING: variable package_clone_stem is empty && exit 1
[ -z "${stage_dir}" ] && echo WARNING: variable stage_dir is empty && exit 1
[ -z "${header_cp_pattern}" ] && echo WARNING: variable header_cp_pattern is empty && exit 1

echo package_name.......................................... "${package_name}"
echo project_name.......................................... "${project_name}"
echo project_dir_name...................................... "${project_dir_name}"
echo clone_dir............................................. "${clone_dir}"
echo package_clone_stem.................................... "${package_clone_stem}"
echo stage_dir............................................. "${stage_dir}"
echo header_cp_pattern..................................... "${header_cp_pattern}"

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

# 
# requires
# 	project_dir_name
# 	project_name
# 
function verify_package_name() {
	if [ $project_name != ${project_dir_name} ] ; then

		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}
# 
# requires
# 	clone_dir
# 	package_name
# 	package_clone_stem
# 
function get_package_into() {
	cd ${clone_dir}
	rm -rfv ${clone_dir}/${package_name}

	${git_clone}
	# --branch ${json_release} 
	cd ${package_clone_stem}
	ls -al
}
# 
# requires
# 	stage_dir
# 	package_name
# 	package_clone_stem
# 	header_cp_pattern
# 
function stage_package_to() {
	mkdir -p ${stage_dir}/include
	mkdir -p ${stage_dir}/lib
	rm -rfv ${stage_dir}/include/${package_name}
	mkdir -p ${stage_dir}/include/${package_name}
	cp -rv ${clone_dir}/${package_clone_stem}/${header_cp_pattern} ${stage_dir}/include/${package_name}
}
# 
# requires
# 	vendor_dir
# 	package_name
# 	header_cp_pattern
# 
function install_package() {
	mkdir -p ${vendor_dir}/include/${package_name}
	rm -rfv ${vendor_dir}/include/${package_name}/*
	cp -rv ${stage_dir}/include/${package_name}/* ${vendor_dir}/include/${package_name}
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


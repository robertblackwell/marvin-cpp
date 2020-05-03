#!/bin/bash
# https://github.com/json/json.git
# This script installs a source package, both headers and c/cpp files, into external_src

# 
# This file requires that the following variables be set
# before entering this file
# 
# 	package_name - the base name of the dirctory into which the repo will be cloned
# 	project_dir_name
# 	clone_dir
# 	package_stage_dir
# 	clone_dir_stem_name
# 	header_cp_pattern
# 	source_cp_pattern
# 	external_src
# 

[ -z "${package_name}" ] && echo WARNING: variable package_name is empty  $0 && exit 1
[ -z "${project_dir_name}" ] && echo WARNING: variable project_dir_name is empty  $0 && exit 1
[ -z "${clone_dir}" ] && echo WARNING: variable clone_dir is empty  $0 && exit 1
[ -z "${package_stage_dir}" ] && echo WARNING: variable package_stage_dir is empty  $0 && exit 1
[ -z "${clone_dir_stem_name}" ] && echo WARNING: variable clone_dir_stem_name is empty  $0 && exit 1
[ -z "${header_cp_pattern}" ] && echo WARNING: variable header_cp_pattern is empty  $0 && exit 1
[ -z "${source_cp_pattern}" ] && echo WARNING: variable source_cp_pattern is empty  $0 && exit 1
[ -z "${external_src}" ] && echo WARNING: variable external_src is empty  $0 && exit 1

echo package_name....................................... "${package_name}"
echo project_dir_name................................... "${project_dir_name}"
echo clone_dir.......................................... "${clone_dir}"
echo package_stage_dir.................................. "${package_stage_dir}"
echo clone_dir_stem_name................................ "${clone_dir_stem_name}"
echo header_cp_pattern.................................. "${header_cp_pattern}"
echo source_cp_pattern.................................. "${source_cp_pattern}"
echo external_src....................................... "${external_src}"


exit 1

function help() {
	echo Install ${package_name}
	echo Usage:
	echo 	install_${package_name} [arg]
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
# 	package_name
# 	project_dir_name
# 
function verify_project_name() {
	if [ $project_name != ${project_dir_name} ] ; then
		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}
# 
#  requires
# 		clone_dir
# 		package_name - the base name of the dirctory into which the repo will be cloned
# 
function get_package_into() {
	rm -rfv ${clone_dir}/${package_name}

	if [ "$git_clone" == "" ] ; then
		git clone https://github.com/${git_repo} ${git_branch} ${clone_dir}/${package_name}
	else
		cd ${clone_dir}
		${git_clone}
	fi
	ls -al ${clone_dir}
}
# 
# Copy the source from the downloaded package into the stage directory  stage/external_src/${package_name}
# 
# requires
# 	clone_dir
# 	package_stage_dir
# 	clone_dir_stem_name
# 	header_cp_pattern
# 	source_cp_pattern
# 
function stage_package_to() {
	rm -f  ${package_stage_dir}/*
	mkdir -p ${package_stage_dir}
	cp -rv ${clone_dir}/${clone_dir_stem_name}/${header_cp_pattern} ${package_stage_dir}
	cp -rv ${clone_dir}/${clone_dir_stem_name}/${source_cp_pattern} ${package_stage_dir}
}
# 
# Install the source and headers into ${source_dir}/external_src/${package_name}
# 
# requires 
# 	external_src
# 	package_name
# 	package_stage_dir
# 
function install_package() {
	# cleanup and copy to install dir
	if [ -d ${external_src}/${package_name} ] ; then 
		rm -rfv ${external_src}/${package_name}/*
	else
		mkdir -p ${external_src}/${package_name}
	fi
	cp -rv ${package_stage_dir}/* ${external_src}/${package_name}
}

# 
# main
# 
echo 
echo INSTALL ${package_name} beginning ========================================================
echo
if [ "$1" == "help" ] ; then help ; fi

pwd=`pwd`
if [ "$debug" != "" ] ; then echo $pwd; fi
vendor=${pwd}/vendor
vendor_dir=${vendor}

echo vendor: ${vendor} vendor_dir : ${vendor_dir}
echo

project_dir=$pwd
project_name=$(basename $project_dir)
source_dir=${project_dir}/marvin
# external_src=${source_dir}/external_src
external_src=${vendor_dir}/src
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos
stage_dir=${script_dir}/stage
package_stage_dir=${script_dir}/stage/external_src/${package_name}

echo In cwd=${pwd} installing ${package_name} into ${package_stage_dir} then into ${external_src}
echo 
if [ "$1" == "stage" ] || [ "$1" == "install" ] || [ "$1" == "" ] ; then
	verify_project_name ${project_name} "marvin"
	get_package_into ${clone_dir}
	stage_package_to ${stage_dir}
fi

if [ "$1" == "install" ] || [ "$1" == "install_only" ] ; then
	install_package
fi


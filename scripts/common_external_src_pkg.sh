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

# [ -z "${package_name}" ] && echo WARNING: variable package_name is empty  $0 && exit 1
# [ -z "${project_dir_name}" ] && echo WARNING: variable project_dir_name is empty  $0 && exit 1
# [ -z "${clone_dir}" ] && echo WARNING: variable clone_dir is empty  $0 && exit 1
# [ -z "${package_stage_dir}" ] && echo WARNING: variable package_stage_dir is empty  $0 && exit 1
# [ -z "${clone_dir_stem_name}" ] && echo WARNING: variable clone_dir_stem_name is empty  $0 && exit 1
# [ -z "${header_cp_pattern}" ] && echo WARNING: variable header_cp_pattern is empty  $0 && exit 1
# [ -z "${source_cp_pattern}" ] && echo WARNING: variable source_cp_pattern is empty  $0 && exit 1
# [ -z "${external_src}" ] && echo WARNING: variable external_src is empty  $0 && exit 1

verify_print_variables \
	package_name \
	project_dir_name \
	clone_dir \
	stage_dir \
	clone_dir_stem_name \
	header_cp_pattern \
	source_cp_pattern \
	external_src


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
function verify_package_name() {
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
function get_package() {
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
function stage_package() {
	rm -f  ${stage_src_dir}/${package_name}
	mkdir -p ${stage_src_dir}/${package_name}
	cp -rv ${clone_dir}/${clone_dir_stem_name}/${header_cp_pattern} ${stage_src_dir}/${package_name}
	cp -rv ${clone_dir}/${clone_dir_stem_name}/${source_cp_pattern} ${stage_src_dir}/${package_name}
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
	cp -rv ${stage_src_dir}/${package_name}/* ${external_src}/${package_name}
}


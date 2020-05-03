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

verify_print_variables \
	package_name \
	project_dir_name \
	clone_dir \
	stage_dir \
	clone_dir_stem_name \
	header_cp_pattern \
	source_cp_pattern \
	external_src

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


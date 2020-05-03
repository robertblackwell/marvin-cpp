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

verify_print_variables \
	package_name project_name \
	project_dir_name \
	clone_dir \
	package_clone_stem stage_dir \
	header_cp_pattern

echo 
echo INSTALL ${package_description} beginning ========================================================
echo
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
function get_package() {
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
function stage_package() {
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


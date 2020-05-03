# 
# requires
# 	vendor_dir
# 	vendor_include_dir	-	hard coded
#	vendor_lib_dir		-	hard coded
# 	stage_dir
# 	stage_include_dir	-	hard coded
# 	stage_lib_dir		- 	hard coded
# 	package_repo_name 	- 	hard coded
# 	package_name 		- 	hard coded
# 	package_name
# 	libcert-cmake-build-debug
# 	libsert name

verify_print_variables \
	clone_dir \
 	vendor_dir \
 	stage_dir \
 	package_name \
 	project_dir_name 


function install_package {
	mkdir -p ${vendor_dir}/include/cert
	mkdir -p ${vendor_dir}/lib
	rm -rfv ${vendor_dir}/include/cert/*
	rm -rfv ${vendor_dir}/lib/libcert*
	cp -rv ${stage_dir}/include/cert/* ${vendor_dir}/include/cert

	cp -rv ${stage_dir}/lib/libcert*.a ${vendor_dir}/lib/
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
	if [ $project_name != ${project_dir_name} ] ; then
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
source ${basedir}/common_run.sh

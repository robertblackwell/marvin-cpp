#!/bin/bash
# https://github.com/boost/boost.git

function install_package {
	mkdir -p ${vendor}/include/boost
	mkdir -p ${vendor}/lib
	rm -rfv ${vendor}/include/boost/*
	rm -rfv ${vendor}/lib/libboost*
	cp -rv ${script_dir}/stage/include/boost/* ${vendor}/include/boost

	cp -rv ${script_dir}/stage/lib/*.a ${vendor}/lib/
	echo 
	echo INSTALL $boost_name complete ========================================================
	echo
}

function get_package {
	cd ${clone_dir}
	rm -rfv ${clone_dir}/${boost_targz_file}*
	rm -rfv ${clone_dir}/${boost_name}
	# wget https://dl.bintray.com/boostorg/release/${boost_release}/source/boost_1_72_0.tar.gz
	wget ${boost_url}
	tar xvzf ${boost_targz_file}
	cd ${boost_name}
	ls -al
}

function stage_package {
	mkdir -p ${script_dir}/stage/include
	mkdir -p ${script_dir}/stage/lib

	cd ${clone_dir}/${boost_name}

	./bootstrap.sh --prefix=${script_dir}/stage  darwin64-x86_64-cc

	./b2 --link=static --threading=multi --variant=debug --layout=system install
}

function verify_package_name() {
	if [ $project_name != "marvin++" ] ; then
		echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
		exit 1
	fi
}

function help() {
	echo Install package ${boost_name}
	echo Usage:
	echo 	stage_boost_1.72.0.sh [arg]
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
boost_release=1.72.0
boost_name=boost_1_72_0
boost_targz_name=boost_1_72_0
boost_targz_file=${boost_targz_name}.tar.gz
boost_url=https://dl.bintray.com/boostorg/release/${boost_release}/source/${boost_targz_file}

pwd=`pwd`
vendor=${pwd}/vendor
project_dir=$pwd
project_name=$(basename $project_dir)
script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos

if [ "$1" == "help" ] ; then help; fi

echo 
echo INSTALL $boost_name begin ========================================================
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



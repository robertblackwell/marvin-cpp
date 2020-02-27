#!/bin/bash
# https://github.com/json/json.git
debug=
cxxurl_release=v3.7.3
echo 
echo INSTALL cxxurl beginning ========================================================
echo


if [ "$1" == "help" ] ; then
	echo Install cxxurl
	echo Usage:
	echo 	install_cxxurl [arg]
	echo
	echo	args is either
	echo		help 	Print this help message
	echo		install After build copy include and libs to final destination
	echo
	echo 	The required package is downloaded into a temp dir inside the scripts dir
	echo	If required the package is built and the headers and libs copied either
	echo 	to a temporary "stage" directory or to the final location		
	exit 0
fi



pwd=`pwd`
if [ "$debug" != "" ] ; then echo $pwd; fi
vendor=${pwd}/vendor
external_src=${pwd}/external_src

project_dir=$pwd
project_name=$(basename $project_dir)

# echo $project_name $project_dir

if [ $project_name != "marvin++" ] ; then

	echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
	exit 1
fi


script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos

if [ "$debug" != "" ] ; then echo should be external_code:  ${script_dir}; fi
if [ "$debug" != "" ] ; then echo should be external_code/cloned_repos:   ${clone_dir}; fi
# exit 0

cd ${clone_dir}
rm -rfv ${clone_dir}/cxxurl

git clone https://github.com/chmike/CxxUrl.git
# --branch ${json_release} 
cd CxxUrl
ls -al

rm -f  ${script_dir}/stage/cxxurl/*
mkdir -p ${script_dir}/stage/cxxurl

mkdir -p ${script_dir}/stage/include/json
cp -rv ${clone_dir}/cxxurl/url*.hpp ${script_dir}/stage/cxxurl
cp -rv ${clone_dir}/cxxurl/url*.cpp ${script_dir}/stage/cxxurl
echo 
echo STAGE cxxurl complete ========================================================
echo

if [ "$1" != "install" ] ; then
	exit 0
fi


# cleanup and copy to install dir
mkdir -p ${external_src}/cxxurl
rm -rfv ${external_src}/cxxurl/*

cp -rv ${script_dir}/stage/cxxurl/json/* ${external_src}/cxxurl
echo 
echo INSTALL cxxurl complete ========================================================
echo


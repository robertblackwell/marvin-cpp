#!/bin/bash
# https://github.com/json/json.git
debug=
json_release=v3.7.3
echo 
echo INSTALL nlohman_json beginning ========================================================
echo


if [ "$1" == "help" ] ; then
	echo Install nlohmann_json_3.7.3
	echo Usage:
	echo 	stage_nlohmann_josn_3.7.3 [arg]
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

project_dir=$pwd
project_name=$(basename $project_dir)

# echo $project_name $project_dir

if [ $project_name != "x509" ] ; then

	echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
	exit 1
fi


script_dir=$(dirname $(realpath $0))
clone_dir=${script_dir}/cloned_repos

if [ "$debug" != "" ] ; then echo should be external_code:  ${script_dir}; fi
if [ "$debug" != "" ] ; then echo should be external_code/cloned_repos:   ${clone_dir}; fi
# exit 0

cd ${clone_dir}
rm -rfv ${clone_dir}/nlohmann
rm -rfv ${clone_dir}/json

git clone https://github.com/nlohmann/json.git 
# --branch ${json_release} 
cd json
ls -al

mkdir -p ${script_dir}/stage/include
mkdir -p ${script_dir}/stage/lib

mkdir -p ${script_dir}/stage/include/json
cp -rv ${clone_dir}/json/single_include/nlohmann/json.hpp ${script_dir}/stage/include/json
echo 
echo STAGE nlohman_json complete ========================================================
echo

if [ "$1" != "install" ] ; then
	exit 0
fi


# cleanup and copy to install dir
mkdir -p ${vendor}/include/json
rm -rfv ${vendor}/include/json/*

cp -rv ${script_dir}/stage/include/json/* ${vendor}/include/json
echo 
echo INSTALL nlohman_json complete ========================================================
echo


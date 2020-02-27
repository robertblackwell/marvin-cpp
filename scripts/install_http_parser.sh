#!/bin/bash
# https://github.com/json/json.git
debug=
http_parser_release=v2.9.3
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
rm -rfv ${clone_dir}/http-parser

git clone git://github.com/nodejs/http-parser.git  --branch ${http_parser_release}

# --branch ${json_release} 
cd http-parser
ls -al

rm -f ${script_dir}/stage/http-parser/*
mkdir -p ${script_dir}/stage/http-parser

cp -rv ${clone_dir}/http-parser/http*.h ${script_dir}/stage/http-parser
cp -rv ${clone_dir}/http-parser/http*.c ${script_dir}/stage/http-parser
echo 
echo STAGE http-parser complete ========================================================
echo

if [ "$1" != "install" ] ; then
	exit 0
fi


# cleanup and copy to install dir
mkdir -p ${external_src}/http-parser
rm -rfv ${external_src}/http-parser/*

cp -rv ${script_dir}/stage/http_parser/* ${external_src}/http_parser
echo 
echo INSTALL http_parser complete ========================================================
echo


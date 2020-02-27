#!/bin/bash
# https://github.com/json/json.git
debug=
url_parser_release=v3.7.3
echo 
echo INSTALL url_parser beginning ========================================================
echo


if [ "$1" == "help" ] ; then
	echo Install url_parser
	echo Usage:
	echo 	install_url_parser [arg]
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
rm -rfv ${clone_dir}/covenanteyes
rm -rfv ${clone_dir}/uri-parser

git clone https://github.com/CovenantEyes/uri-parser.git  

# --branch ${json_release} 
cd uri-parser
ls -al

rm -vf ${script_dir}/stage/uri-parser/*
mkdir -p ${script_dir}/stage/uri-parser

cp -rv ${clone_dir}/uri-parser/Uri*.hpp ${script_dir}/stage/uri-parser
cp -rv ${clone_dir}/uri-parser/Uri*.cpp ${script_dir}/stage/uri-parser
echo 
echo STAGE simple_buffer complete ========================================================
echo

if [ "$1" != "install" ] ; then
	exit 0
fi


# cleanup and copy to install dir
mkdir -p ${external_src}/uri-parser
rm -rfv ${external_src}/uri-parser/*

cp -rv ${script_dir}/stage/uri-parser/* ${external_src}/uri-parser
echo 
echo INSTALL uri-parser complete ========================================================
echo


#!/bin/bash
# https://github.com/json/json.git
# This script installs a source package, both headers and c/cpp files, into external_src

debug=

echo 
echo INSTALL ${package_name} beginning ========================================================
echo


if [ "$1" == "help" ] ; then
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
fi



pwd=`pwd`
if [ "$debug" != "" ] ; then echo $pwd; fi
vendor=${pwd}/vendor

project_dir=$pwd
project_name=$(basename $project_dir)
source_dir=${project_dir}/marvin
external_src=${source_dir}/external_src

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
rm -rfv ${clone_dir}/${package_name}

if [ "$git_clone" == "" ] ; then
	git clone https://github.com/${git_repo} ${git_branch}
else
	${git_clone}
fi
	# --branch ${json_release} 
cd ${clone_dir_stem_name}
ls -al

stage_dir=${script_dir}/stage/external_src/${package_name}
rm -f  ${stage_dir}/*
mkdir -p ${stage_dir}

cp -rv ${clone_dir}/${clone_dir_stem_name}/${header_cp_pattern} ${stage_dir}
cp -rv ${clone_dir}/${clone_dir_stem_name}/${source_cp_pattern} ${stage_dir}
echo 
echo STAGE ${package_name} complete ========================================================
echo

if [ "$1" != "install" ] ; then
	exit 0
fi


# cleanup and copy to install dir
mkdir -p ${external_src}/${package_name}
rm -rfv ${external_src}/${package_name}/*

cp -rv ${stage_dir}/* ${external_src}/${package_name}
echo 
echo INSTALL ${package_name} complete ========================================================
echo


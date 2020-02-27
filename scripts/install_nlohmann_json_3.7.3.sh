#!/bin/bash
# https://github.com/json/json.git
debug=
project_name=marvin
json_release=v3=json
package_name=json
package_clone_stem=json
package_description=nlohman_json_${json_release}
git_clone="git clone https://github.com/nlohmann/json.git"
header_cp_pattern=single_include/nlohmann/json.hpp

basedir=$(dirname "$0")
source ${basedir}/inc_install_hdr_pkg.sh


# echo 
# echo INSTALL nlohman_json beginning ========================================================
# echo


# if [ "$1" == "help" ] ; then
# 	echo Install ${package_description}
# 	echo Usage:
# 	echo 	stage_${package_description} [arg]
# 	echo
# 	echo	args is either
# 	echo		help 	Print this help message
# 	echo		install After build copy include and libs to final destination
# 	echo
# 	echo 	The required package is downloaded into a temp dir inside the scripts dir
# 	echo	If required the package is built and the headers and libs copied either
# 	echo 	to a temporary "stage" directory or to the final location		
# 	exit 0
# fi



# pwd=`pwd`
# if [ "$debug" != "" ] ; then echo $pwd; fi

# project_dir=$pwd
# project_name=$(basename $project_dir)
# source_dir=${project_dir}/marvin
# vendor=${project_dir}/vendor
# # echo $project_name $project_dir

# if [ $project_name != "marvin++" ] ; then

# 	echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
# 	exit 1
# fi


# script_dir=$(dirname $(realpath $0))
# clone_dir=${script_dir}/cloned_repos

# if [ "$debug" != "" ] ; then echo should be external_code:  ${script_dir}; fi
# if [ "$debug" != "" ] ; then echo should be external_code/cloned_repos:   ${clone_dir}; fi
# # exit 0

# cd ${clone_dir}
# rm -rfv ${clone_dir}/${package_name}

# ${git_clone}
# # --branch ${json_release} 
# cd ${package_name}
# ls -al

# mkdir -p ${script_dir}/stage/include
# mkdir -p ${script_dir}/stage/lib

# mkdir -p ${script_dir}/stage/include/${package_name}
# cp -rv ${clone_dir}/json/${header_cp_patter} ${script_dir}/stage/include/${package_name}
# echo 
# echo STAGE ${package_description} complete ========================================================
# echo

# if [ "$1" != "install" ] ; then
# 	exit 0
# fi


# # cleanup and copy to install dir
# mkdir -p ${vendor}/include/${package_name}
# rm -rfv ${vendor}/include/${package_name}/*

# cp -rv ${script_dir}/stage/include/${package_name}/* ${vendor}/include/${package_name}
# echo 
# echo INSTALL ${package_name} complete ========================================================
# echo


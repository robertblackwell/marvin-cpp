#!/bin/bash
# https://github.com/catch/catch.git

debug=

catch_release=v2.11.1
package_description=catch_v2.11.1
package_name=catch2

package_clone_stem=Catch2
git_clone="git clone https://github.com/catchorg/Catch2.git --branch ${catch_release}"
header_cp_pattern=single_include/catch2/*


basedir=$(dirname "$0")
source ${basedir}/inc_install_hdr_pkg.sh

# echo 
# echo INSTALL $catch2_name beginning ========================================================
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


# catch_url=https://dl.bintray.com/catchorg/release/${catch_release}/source/catch_1_72_0.tar.gz

# pwd=`pwd`
# if [ "$debug" != "" ] ; then echo $pwd; fi
# vendor=${pwd}/vendor

# project_dir=$pwd
# project_name=$(basename $project_dir)

# # echo $project_name $project_dir

# if [ $project_name != "x509" ] ; then

# 	echo "You are in the wrong directory : [" ${project_name} "] should be at project root "
# 	exit 1
# fi


# script_dir=$(dirname $(realpath $0))
# clone_dir=${script_dir}/cloned_repos

# if [ "$debug" != "" ] ; then echo should be external_code:  ${script_dir}; fi
# if [ "$debug" != "" ] ; then echo should be external_code/cloned_repos:   ${clone_dir}; fi
# # exit 0

# cd ${clone_dir}
# rm -rfv ${clone_dir}/Catch2

# git clone https://github.com/catchorg/Catch2.git --branch ${catch_release} 
# cd Catch2
# ls -al

# mkdir -p ${script_dir}/stage/include
# mkdir -p ${script_dir}/stage/lib

# cp -rv ${clone_dir}/Catch2/single_include/* ${script_dir}/stage/include

# echo 
# echo STAGE $catch2_name complete ========================================================
# echo
# if [ "$1" != "install" ] ; then
# 	exit 0
# fi


# # cleanup and copy to install dir
# echo mkdir -p ${vendor}/include/catch2
# mkdir -p ${vendor}/include/catch2
# rm -rfv ${vendor}/include/catch2/*
# echo cp -rv ${script_dir}/stage/include/catch2/* ${vendor}/include/catch2

# cp -rv ${script_dir}/stage/include/catch2/* ${vendor}/include/catch2

# echo 
# echo INSTALL $catch2_name complete ========================================================
# echo
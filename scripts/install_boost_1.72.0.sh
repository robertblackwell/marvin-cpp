#!/bin/bash
# https://github.com/boost/boost.git

debug=

boost_release=1.72.0
boost_name=boost_1_72_0

echo 
echo INSTALL $boost_name begin ========================================================
echo

if [ "$1" == "help" ] ; then
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
fi

boost_url=https://dl.bintray.com/boostorg/release/${boost_release}/source/boost_1_72_0.tar.gz
boost_targz_file=boost_1_72_0.tar.gz

pwd=`pwd`

if [ "$debug" != "" ] ; then echo $pwd; fi

vendor=${pwd}/vendor

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
rm -rfv ${clone_dir}/${boost_targz_file}*
rm -rfv ${clone_dir}/${boost_name}
# wget https://dl.bintray.com/boostorg/release/${boost_release}/source/boost_1_72_0.tar.gz
wget ${boost_url}
tar xvzf ${boost_targz_file}

cd ${boost_name}
ls -al

mkdir -p ${script_dir}/stage/include
mkdir -p ${script_dir}/stage/lib

cd ${clone_dir}/${boost_name}

./bootstrap.sh --prefix=${script_dir}/stage  darwin64-x86_64-cc

./b2 --link=static --threading=multi --variant=debug --layout=system install

echo 
echo STAGING $boost_name complete ========================================================
echo

# only install if asked to do so
if [  "$1" != "install" ] ; then
	exit 0
fi
mkdir -p ${vendor}/include/boost
mkdir -p ${vendor}/lib
rm -rfv ${vendor}/include/boost/*
rm -rfv ${vendor}/lib/libboost*
cp -rv ${script_dir}/stage/include/boost/* ${vendor}/include/boost

cp -rv ${script_dir}/stage/lib/*.a ${vendor}/lib/


echo 
echo INSTALL $boost_name complete ========================================================
echo


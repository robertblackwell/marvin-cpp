#!/bin/bash
# https://github.com/openssl/openssl.git
debug=
openssl_name=openssl-1.1.1d
echo 
echo INSTALL ${openssl_name} beginning ========================================================
echo

if [ "$1" == "help" ] ; then
	echo Install openssl_1.1.1d
	echo Usage:
	echo 	stage_openssl_1.1.1d.sh [arg]
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

project_dir=$pwd
project_name=$(basename $project_dir)
vendor=${pwd}/vendor

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

rm -rf ${clone_dir}/${openssl_name}
cd ${clone_dir}

echo wget https://www.openssl.org/source/${openssl_name}.tar.gz

wget https://www.openssl.org/source/${openssl_name}.tar.gz

tar xvzf ${openssl_name}.tar.gz

cd ${openssl_name}
ls -al

mkdir -p ${script_dir}/stage/include
mkdir -p ${script_dir}/stage/lib
mkdir -p ${script_dir}/stage/ssl

# ./Configure --prefix=${script_dir}/stage --openssldir=${script_dir}/stage --debug darwin64-x86_64-cc

./Configure --prefix=${script_dir}/stage --openssldir=${vendor}/ssl --debug darwin64-x86_64-cc
make all
make install
echo 
echo STAGE ${openssl_name} complete ========================================================
echo

if [ "$1" != "install" ] ; then
	exit 0
fi

mkdir -p ${vendor}/include/openssl
mkdir -p ${vendor}/lib
rm -rf ${vendor}/include/openssl/*
cp -rv ${script_dir}/stage/include/openssl/* ${vendor}/include/openssl

cp -rv ${script_dir}/stage/lib/*.a ${vendor}/lib/
echo 
echo INSTALL ${openssl_name} complete ========================================================
echo


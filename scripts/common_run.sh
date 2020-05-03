


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



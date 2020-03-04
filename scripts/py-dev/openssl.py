import sys
import json
import datetime
import os
import pprint
import shutil
from util import run

from package import LibraryPackage

package_name = "openssl"
openssl_name = "openssl-1.1.1d"

# package_url = "https://dl.bintray.com/boostorg/release/{}/source/boost_1_72_0.tar.gz".format(boost_release)
package_url = "https://www.openssl.org/source/{}.tar.gz".format(openssl_name)
package_targz_file = "tar xvzf {}.tar.gz".format(openssl_name)


class OpenSSL(LibraryPackage):
	def __init__(self, name, version, the_defaults):
		super().__init__(package_name, the_defaults)
		self.name = name
		self.version = version

		self.package_targz_file_path = os.path.join(self.defaults.clone_dir, package_targz_file)
		self.wget_output_path = self.defaults.clone_dir
		self.package_targz_file_path = os.path.join(self.defaults.clone_dir, package_targz_file)
		self.vendor_ssl_dir = os.path.join(self.defaults.vendor_dir, "ssl")

	def get_package(self):
		print("here")
		super().get_package_before()
		run("rm -rfv {}/{}".format(self.defaults.clone_dir, package_targz_file))
		run("rm -rfv {}/{}/*".format(self.defaults.clone_dir, package_name))
		run("wget -O {} {}".format(self.wget_output_path, package_url))
		run("tar xvzf {} -C {}".format(self.package_targz_file_path, self.package_clone_dir_path))
		run("ls -al {}".format(self.package_clone_dir_path))
		super().get_package_after()
	
	def stage_package(self):
		super().stage_package_before()
		run("mkdir -p {}".format(self.stage_include_dir_path))
		run("rm -rf {}/*".format(self.package_stage_include_dir_path))
		run("mkdir -p {}".format(self.stage_lib_dir_path))
		run("rm -rf {}/libboost*".format(self.stage_lib_dir_path))

		run(
			"./Configure --prefix={} --openssldir={} --debug darwin64-x86_64-cc"
			.format(self.defaults.stage_dir, self.vendor_ssl_dir), 
			self.package_clone_dir_path)
		run("make all")
		run("make install")
		super().stage_package_after()

	def install_package(self):
		super().install_package_before()
		run("mkdir -p {}".format(self.vendor_lib_dir_path))
		run("mkdir -p {}".format(self.package_vendor_include_dir_path))
		run("rm -rf {}/*".format(self.package_vendor_include_dir_path))
		run("rm -rf {}/libcrypto*".format(self.vendor_lib_dir_path))
		run("rm -rf {}/libssl*".format(self.vendor_lib_dir_path))

		run("cp -rv {}/* {}".format(self.package_stage_include_dir_path,  self.package_vendor_include_dir_path))
		run("cp -rv {}/lib* {}".format(self.stage_lib_dir_path,  self.vendor_lib_dir_path))
		super().install_package_after()

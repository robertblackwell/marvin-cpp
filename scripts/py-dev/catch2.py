import sys
import json
import datetime
import os
import pprint
import shutil
from util import run

from package import HeadersOnlyPackage

package_name = "catch"

catch_release="v2.11.1"
package_description="catch_v2.11.1"
package_name="catch2"

package_clone_stem="Catch2"
git_clone="git clone https://github.com/catchorg/Catch2.git --branch ${catch_release}"
header_cp_pattern="single_include/catch2/*"

class Catch2(HeadersOnlyPackage):
	def __init__(self, name, version, the_defaults):
		super().__init__(package_name, the_defaults)
		self.name = name
		self.version = version

	def get_package(self):
		print("here")
		super().get_package_before()
		run("rm -rfv {}/{}".format(self.defaults.clone_dir, package_clone_stem))
		run(git_clone, self.defaults.clone_dir)

		super().get_package_after()
	
	def stage_package(self):
		super().stage_package_before()
		run("mkdir -p {}".format(self.stage_include_dir_path))
		run("rm -rf {}/*".format(self.package_stage_include_dir_path))
		run("cp -rv {}/{} {}".format(self.package_clone_dir_path, header_cp_pattern, self.package_stage_include_dir_path))

		super().stage_package_after()

	def install_package(self):
		super().install_package_before()
		run("mkdir -p {}".format(self.package_vendor_include_dir_path))
		run("rm -rf {}/*".format(self.package_vendor_include_dir_path))

		run("cp -rv {}/* {}".format(self.package_stage_include_dir_path,  self.package_vendor_include_dir_path))
		super().install_package_after()

import sys
import json
import datetime
import os
import pprint
import shutil
from util import run

from package import SourcePackage

package_name = "cxxurl"


class CxxUrl(SourcePackage):
	def __init__(self, name, version, the_defaults):
		super().__init__(package_name, the_defaults)
		self.name = name
		self.version = version

	def get_package(self):
		print("here")
		super().get_package_before()
		run("git clone https://github.com/chmike/CxxUrl.git ", self.defaults.clone_dir)
		super().get_package_after()
	
	def stage_package(self):
		super().stage_package_before()
		run("cp -rv {}/*.hpp {}".format(self.package_clone_dir_path, self.package_stage_external_src_dir_path))
		run("cp -rv {}/url*.cpp {}".format(self.package_clone_dir_path, self.package_stage_external_src_dir_path))

	def install_package(self):
		super().install_package_before()
		run("cp -rv {}/* {}".format(self.package_stage_external_src_dir_path,  self.package_external_src_dir_path))

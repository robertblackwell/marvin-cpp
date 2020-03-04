import sys
import json
import datetime
import os
import pprint
import shutil
from util import run

from package import SourcePackage

package_name = "simple_buffer"

class SimpleBuffer(SourcePackage):
	def __init__(self, name, version, the_defaults):
		super().__init__(package_name, the_defaults)
		self.name = name
		self.version = version

	def get_package(self):
		print("here")
		super().get_package_before()
		run("git clone file://$\{HOME\}/git-repos/simple_buffer", self.defaults.clone_dir)
		super().get_package_after()
	
	def stage_package(self):
		super().stage_package_before()
		run("cp -rv {}/src/simple*.h* {}".format(self.package_clone_dir_path, self.package_stage_external_src_dir_path))
		run("cp -rv {}/src/simple*.c* {}".format(self.package_clone_dir_path, self.package_stage_external_src_dir_path))
		super().stage_package_after()

	def install_package(self):
		super().install_package_before()

		run("cp -rv {}/* {}".format(self.package_stage_external_src_dir_path,  self.package_external_src_dir_path))
		super().install_package_after()

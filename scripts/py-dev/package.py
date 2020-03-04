import sys
import json
import datetime
import pprint
import os
from util import run

class PackageBase(object):
	def __init__(self, package_name, the_defaults):
		self.defaults = the_defaults
		self.package_name = package_name
		self.package_clone_dir_path = os.path.join(self.defaults.clone_dir, package_name)
		self.stage_include_dir_path = os.path.join(self.defaults.script_dir, "stage", "include")
		self.stage_lib_dir_path = os.path.join(self.defaults.script_dir, "stage", "lib")
		self.vendor_include_dir_path = os.path.join(self.defaults.vendor_dir, "include")
		self.vendor_lib_dir_path = os.path.join(self.defaults.vendor_dir, "lib")

		self.package_stage_include_dir_path = os.path.join(self.stage_include_dir_path, package_name)
		self.package_vendor_include_dir_path = os.path.join(self.vendor_include_dir_path, package_name)

class LibraryPackage(PackageBase):
	def __init__(self, package_name, the_defaults):
		super().__init__(package_name, the_defaults)
		print("LibraryPackage")

	def get_package_before(self):
		pass
	
	def get_package_after(self):
		pass
	
	def stage_package_before(self):
		pass

	def stage_package_after(self):
		pass

	def install_package_before(self):
		pass

	def install_package_after(self):
		pass

class SourcePackage(PackageBase):
	def __init__(self, package_name, the_defaults):
		super().__init__(package_name, the_defaults)
		print("SourcePackage")
		self.stage_external_src_dir_path = os.path.join(self.defaults.stage_dir,"external_src")
		self.package_stage_external_src_dir_path = os.path.join(self.stage_external_src_dir_path, self.package_name)
		self.package_external_src_dir_path = os.path.join(self.defaults.source_dir, "external_src", self.package_name)

	def get_package_before(self):
		run("rm -rfv {}/{}/*".format(self.defaults.clone_dir, self.package_name))
		pass
	
	def get_package_after(self):
		run("ls -al {}".format(self.package_clone_dir_path))
	
	def stage_package_before(self):
		run("mkdir -p {}".format(self.package_stage_external_src_dir_path))
		run("rm -rf {}/*".format(self.package_stage_external_src_dir_path))
		pass

	def stage_package_after(self):
		pass

	def install_package_before(self):
		run("mkdir -p {}".format(self.package_external_src_dir_path))
		run("rm -rf {}/*".format(self.package_external_src_dir_path))
		pass

	def install_package_after(self):
		pass

class HeadersOnlyPackage(PackageBase):
	def __init__(self, package_name, the_defaults):
		super().__init__(package_name, the_defaults)
		print("SourcePackage")

	def get_package_before(self):
		run("rm -rfv {}/{}/*".format(self.defaults.clone_dir, self.package_name))
		pass
	
	def get_package_after(self):
		run("ls -al {}".format(self.package_clone_dir_path))
	
	def stage_package_before(self):
		pass

	def stage_package_after(self):
		pass

	def install_package_before(self):
		pass

	def install_package_after(self):
		pass
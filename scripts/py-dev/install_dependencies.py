#!/usr/bin/env python3

import sys
import argparse
import datetime
import pprint
import optparse
import os

pp = pprint.PrettyPrinter(indent=4)

project_name = "marvin++"
project_dir = os.getcwd()
source_dir = os.path.join(project_dir, "marvin")
clone_dir = os.path.join(project_dir, "scripts", "clone_dir")
stage_dir = os.path.join(project_dir, "scripts", "stage_dir")

__version__ = "0.3.5"

debug = True
logfile = False

class Defaults:
    def __init__(the_project_name, the_project_dir, the_source_dir_name):
        clone_name = "clone"
        stage_name = "stage"
        external_name = "external"
        vendor_name = "vendor"
        scripts_name = "scripts"

        project_name = the_project_name
        project_dir = the_project_dir
        unpack_dir = os.path.join(project_dir, scripts_name, clone_name)
        source_dir = os.path.join(project_dir, the_project_name)
        external_dir = os.path.join(project_dir, the_project_name, external_name)



def doit(openssl_version, output_dir, dryrun_flag):
    print("Hello")
    pp.pprint([openssl_version, output_dir, dryrun_flag])


def main():
    project_name = None
    project_dir = None
    source_dir_name = None

    parser = argparse.ArgumentParser(
        description="Install dependencies for project.")
    parser.add_argument('-v', '--version', 	action="store_true",
                        help="Prints the version number.")

    parser.add_argument('--install', 	dest="install_flag", action="store_true",
                        help='Installs the staged install to the final destination')

    parser.add_argument('--project-name', 	   dest='project_name', help='The name of the project. Required')
    parser.add_argument('--project-dir',       dest='project_dir', help='Path to the project top level directory. Defaults to pwd. ')
    parser.add_argument('--source-dir-name',   dest='project_dir_name', help='Stem name of the project source directory, should be same as project name lowercased')

    parser.add_argument('--clone-dir', 		dest='clone_dir_path', help='The path for directory into which packages are cloned/unpacked. Default to scripts/clone_dir ')
    parser.add_argument('--stage-dir',      dest='stage_dir_path', help='The path for directory into which package headers amd archives are copied after building.'
            + 'Default to scripts/stage ')
    parser.add_argument('--vendor-dir',      dest='vendor_dir_path', help='The path for directory into which package headers amd archives are installed locally to the project.\n'
            +'Mustb always be an immediate subdirectory of the project-dir and defaults to {project-dir}/vendor.')
    # 													+ 'and libraries will be in {install-dir}/lib')
    parser.add_argument('--external-dir',      dest='external_dir_path', help='The path for directory into packages delivered as copied source files will be installed.\n'
        'Must be inside the project source directory, Defaults to project_dir/source_dir_name/external.')


    # parser.add_argument('--stage-dir', 		dest='stage_dir_path',
    # 	help='Sets the openssl stage directory.' +
    # 		'\nOn completion openssl headers will be placed in {install-dir}/include, '	+ 'and libraries will be in {install-dir}/lib')

    # parser.add_argument('--list', 		dest='infile_path',
    # 	help='Path to input file, each line has arguments for command. Cannot parse both a file and a string')
    # parser.add_argument('--log', 		dest='logfile_path',
    # 	help='Path to log file, default is ncm_results_YYYYMMDDHHMMSS.txt in the working directory')
    # parser.add_argument('--debug', 		action="store_true",
    # 	help="Prints out the command to be executed rather than execute the command, to help problem solve")
    # parser.add_argument('--dryrun', 	action="store_true",
    # 	help="Does NOT execute group changes but does provide logs")
    args = parser.parse_args()
    pp.pprint(args)


    # if debug:
    #     pp.pprint(args)

    # if args.version:
    #     print(__version__)
    #     sys.exit(0)

    # if args.show_openssl_version:
    #     print(default_openssl_version)

    # if args.openssl_version:
    #     active_openssl_version = args.openssl_version
    # else:
    #     active_openssl_version = default_openssl_version

    # output_dir = None
    # if args.stage_dir_path:
    #     output_dir = args.stage_dir

    # if args.install_dir_path:
    #     output_dir = args.install_dir_path

    # logpath = "./log"
    # if args.logfile_path:
    #     logpath = args.logfile_path
    # else:
    #     ts = datetime.datetime.now().isoformat()
    #     if debug:
    #         pp.pprint(ts)
    #     if debug:
    #         pp.pprint(logpath)
    #     global logfile
    #     logfile = open(logpath, "w")
    # if debug:
    #     pp.pprint(logfile)

    # doit(active_openssl_version, output_dir, args.dryrun)


if __name__ == "__main__":
    main()

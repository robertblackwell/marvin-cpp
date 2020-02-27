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


def doit(openssl_version, output_dir, dryrun_flag):
    print("Hello")
    pp.pprint([openssl_version, output_dir, dryrun_flag])


def main():
    default_openssl_version = "openssl@1.1.1d"

    parser = argparse.ArgumentParser(
        description="Install dependencies for project.")
    parser.add_argument('-v', '--version', 	action="store_true",
                        help="Prints the version number.")

    parser.add_argument('--install', 	dest="install_flag", action="store_true",
                        help='Installs the staged install to the final destination')

    # parser.add_argument('--openssl-version', 		dest='openssl_version', 	default="openssl@1.1.1d",
    # 		help='Sets the openssl version to get, must be full version string, the tag used in the github repo. Default is: openssl@1.1.1d')

    # parser.add_argument('--install-dir', 		dest='install_dir_path',
    # 	help='Sets the openssl installation directory.' + '\nOn completion openssl headers will be placed in {install-dir}/include, '
    # 													+ 'and libraries will be in {install-dir}/lib')

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

    if debug:
        pp.pprint(args)

    if args.version:
        print(__version__)
        sys.exit(0)

    if args.show_openssl_version:
        print(default_openssl_version)

    if args.openssl_version:
        active_openssl_version = args.openssl_version
    else:
        active_openssl_version = default_openssl_version

    output_dir = None
    if args.stage_dir_path:
        output_dir = args.stage_dir

    if args.install_dir_path:
        output_dir = args.install_dir_path

    logpath = "./log"
    if args.logfile_path:
        logpath = args.logfile_path
    else:
        ts = datetime.datetime.now().isoformat()
        if debug:
            pp.pprint(ts)
        if debug:
            pp.pprint(logpath)
        global logfile
        logfile = open(logpath, "w")
    if debug:
        pp.pprint(logfile)

    doit(active_openssl_version, output_dir, args.dryrun)


if __name__ == "__main__":
    main()

#!/usr/bin/python3
# -*- coding: UTF-8 -*-

# Copyright 2019 Xilinx Inc.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import sys, os, argparse, logging

import vaitraceCfgManager
import vaitraceSetting
import vaitraceCppRunner
import vaitracePyRunner
import vaitraceDefaults

VAITRACE_VER = "v1.2-20200526"

def parseCmdLine():
    """
    -o: trace file: save trace file [default: trace.txt]
    -c: conf: input config file
    -t: time: tracing time in second(s)
    -v: show version
    """

    default_conf_json = ""
    cmd_parser = argparse.ArgumentParser(prog="Xilinx Vitis AI Trace")
    cmd_parser.add_argument("cmd", nargs=argparse.REMAINDER)
    cmd_parser.add_argument("-c", dest="config", nargs='?', default=default_conf_json, help="Specify the config file")
    cmd_parser.add_argument("-d", dest='debug', action='store_true', help="Enable debug")
    cmd_parser.add_argument("-o", dest="traceSaveTo", nargs='?', help="Save trace file to")
    cmd_parser.add_argument("-t", dest='timeout', nargs='?', type=int, default=3, help="Tracing time limitation")
    cmd_parser.add_argument("-v", dest='showversion', action='store_true', help="Show version")
    cmd_parser.add_argument("-b", dest='bypass', action='store_true', help="Bypass vaitrace, just run command")

    args = cmd_parser.parse_args()

    if args.showversion:
        print("Xilinx Vitis AI Profiler Tracer Ver %s" % VAITRACE_VER)
        exit(0)

    return args, cmd_parser


def printHelpExit(parser):
    parser.print_help()
    exit(-1)

def main(args, cmd_parser):
    options = vaitraceDefaults.traceCfgDefaule

    """Configuration priority: Configuration File > Command Line > Default"""
    options['cmdline_args'] = {}
    options['cmdline_args']['cmd'] = args.cmd
    options['cmdline_args']['timeout'] = args.timeout
    options['cmdline_args']['output'] = args.traceSaveTo
    options['cmdline_args']['debug'] = args.debug
    options['cmdline_args']['config'] = args.config
    options['cmdline_args']['bypass'] = args.bypass

    if args.debug:
        logging.basicConfig(level=logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
    
    logging.debug("Cmd line args: ")
    logging.debug(args)

    if vaitraceCfgManager.parseCfg(options) == False:
        printHelpExit(cmd_parser)

    vaitraceSetting.setting(options)
    if vaitraceCfgManager.saityCheck(options) == False:
        printHelpExit(cmd_parser)

    vaitraceCppRunner.run(options)

if __name__ == '__main__':

    """Checking Permission"""
    if os.getgid() != 0:
        logging.error("This tool need run as 'root'")
        exit(-1)

    main(*parseCmdLine())

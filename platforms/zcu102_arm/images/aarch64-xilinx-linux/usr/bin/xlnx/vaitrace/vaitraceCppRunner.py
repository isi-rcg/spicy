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


import os,re,csv,sys,string,time,argparse,json,signal,platform,gzip,pickle,time, logging
from subprocess import Popen, PIPE

import collector, tracer
import vaitraceCfgManager
import vaitraceSetting

def run(globalOptions: dict):
	options = globalOptions

	if options.get('cmdline_args').get('bypass', False):
		cmd = options.get('control').get('cmd')
		logging.info("Bypass vaitrace, just run cmd")
		proc = Popen(cmd)
		proc.wait()
		exit(0)
 	
	"""Preparing"""
	tracer.prepare(options)
	tracer.start()
	
	"""requirememt format: ["tracerName", "tracerName1", "hwInfo", ...]"""
	collector.prepare(options, tracer.getSourceRequirement())
	collector.start()

	"""Start Running"""
	cmd = options.get('control').get('cmd')
	timeout = options.get('control').get('timeout')
	proc = Popen(cmd)
	
	options['control']['pid'] = proc.pid
	
	if timeout <= 0:
	    proc.wait()
	else:
	    while timeout > 0:
	        time.sleep(1)
	        timeout -= 1
	        p = proc.poll()
	        if p is not None:
	            break
	
	collector.stop()
	tracer.stop()
	proc.wait()
	
	tracer.process(collector.getData())
	
	"""Dump Data"""
	saveTo = options.get('control').get('xat').get('filename')
	compress = options.get('control').get('xat').get('compress', True)
	
	if compress:
	    xat = gzip.open(saveTo, 'wb+')
	else:
	    xat = open(saveTo, 'wb+')
	
	pickle.dump(tracer.getData(), xat)
	
	print(".xat File Saved to [%s]" % os.path.abspath(saveTo))
	xat.close()

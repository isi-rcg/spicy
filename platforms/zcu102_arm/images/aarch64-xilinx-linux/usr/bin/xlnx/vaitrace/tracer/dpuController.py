#!/usr/bin/python3
# -*- coding:utf-8 -*-

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


import os, sys, re, json, pickle
import tracer.tracerBase


DPU_CONTROLLER_FILTER = """EVENT_"""

class DPUControllerTracer(tracer.tracerBase.Tracer):
    def __init__(self):
        super().__init__('DPUController', source=['tracepoint'], compatible = {'machine': ["x86_64", "aarch64"]})
        self.traceLog = []

    def process(self, data):
        for s in self.source:
            d = data.get(s, None)
            if d is None:
                continue
            for l in d:
                if l.find(DPU_CONTROLLER_FILTER) < 0:
                    continue
                self.traceLog.append(l)

    def start(self):
        super().start()

    def stop(self):
        super().stop()

    def prepare(self, options: dict, debug: bool):
        self.saveTo = None
        if debug:
            self.saveTo = "./dpurt.trace"

    def getData(self):
        if self.saveTo != None:
            with open(self.saveTo, "w+t") as save:
                save.writelines(self.traceLog)
        return self.traceLog

tracer.tracerBase.register(DPUControllerTracer())

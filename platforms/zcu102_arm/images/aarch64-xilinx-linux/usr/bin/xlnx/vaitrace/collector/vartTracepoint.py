
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

import sys, os
import collector
from tempfile import TemporaryDirectory



class tracepointCollector(collector.collectorBase.Collector):
    def __init__(self, stdout="", stderr=""):
        super().__init__(name='tracepoint')
        self.logdir = TemporaryDirectory()
        self.logdirName = self.logdir.name + '/'

    def __del__(self):
        self.logdir.cleanup()

    def prepare(self, conf: dict) -> dict:

        # "VAI_TRACE_ENABLE"=1
        # "VAI_TRACE_DIR"="/tmp/vai/"
        # "VAI_TRACE_TS" = "boot" or "x86_tsc"

        os.environ.setdefault("VAI_TRACE_ENABLE", "1")
        os.environ.setdefault("VAI_TRACE_DIR", self.logdirName)

        # preferClocks = ["boot", "x86-tsc", "global"]
        ts_mode = conf.get("control").get("traceClock")
        print(ts_mode)
        if ts_mode not in ["boot", "x86-tsc"]:
            assert()
        os.environ.setdefault("VAI_TRACE_TS", ts_mode)

        return conf

    def start(self):
        super().start()

    def stop(self):
        super().stop()

    def getData(self):
        traceEvents = []

        for root, dirs, files in os.walk(self.logdirName):
            for name in files:
                f = os.path.join(root, name)
                traceEvents += open(f).readlines()

        return traceEvents


collector.collectorBase.register(tracepointCollector())

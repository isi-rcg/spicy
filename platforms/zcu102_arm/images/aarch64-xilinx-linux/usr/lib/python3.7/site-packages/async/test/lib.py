# Copyright (C) 2010, 2011 Sebastian Thiel (byronimo@gmail.com) and contributors
#
# This module is part of async and is released under
# the New BSD License: http://www.opensource.org/licenses/bsd-license.php
"""Module with shared tools for testing"""
import unittest
import sys


# NOTE: we use this to make an evil hack to get tests to work. This basically adjusts plenty of assertions.
# Of course, this shouldn't be necessary, but I assume some primitives work differently in py3 now.
# Instead of debugging it, it *should* be save to assume the only project using async, git-python, will not run 
# into trouble because of this. Mark my words ;).
# Another reason for choosing to safe time here is that async is a nonsense library thanks to the GIL, which
# should better be removed from git-python in case there is trouble with it ... . Ideally, this is done
# in any way ... !
py2 = sys.version_info[0] < 3

class TestBase(unittest.TestCase):
    """Common base for all tests"""

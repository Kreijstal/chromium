# Copyright 2014 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Helper script for GN to run an arbitrary binary. See compiled_action.gni.

Run with:
  python gn_run_binary.py <binary_name> [args ...]
"""

import os
import subprocess
import sys

# This script is designed to run binaries produced by the current build. We
# may prefix it with "./" to avoid picking up system versions that might
# also be on the path.
path = sys.argv[1]
if not os.path.isabs(path):
    path = './' + path

def _NormalizeNasmPathArg(arg):
  if arg.startswith('-I') and len(arg) > 2:
    include = arg[2:]
    trailing_slash = '/' if include.endswith(('/', '\\')) else ''
    include = include.rstrip('/\\')
    if include and not os.path.isabs(include):
      include = os.path.abspath(include)
    return '-I' + include.replace('\\', '/') + trailing_slash

  if arg.endswith('.asm') and not os.path.isabs(arg):
    return os.path.abspath(arg).replace('\\', '/')

  return arg

# The rest of the arguments are passed directly to the executable.
args = [path] + sys.argv[2:]
if os.path.basename(path).lower() == 'nasm.exe':
  args = [path] + [_NormalizeNasmPathArg(arg) for arg in sys.argv[2:]]

ret = subprocess.call(args)
if ret != 0:
    if ret <= -100:
        # Windows error codes such as 0xC0000005 and 0xC0000409 are much easier to
        # recognize and differentiate in hex. In order to print them as unsigned
        # hex we need to add 4 Gig to them.
        print(
            '%s failed with exit code 0x%08X' % (sys.argv[1], ret + (1 << 32))
        )
    else:
        print('%s failed with exit code %d' % (sys.argv[1], ret))
sys.exit(ret)


# Responsible for compiling the software,
# less OS-dependent than a makefile

import os
import sys
import subprocess
import shutil
import random

# Our own tools
import util


def main(args=sys.argv):

  supported_compilers = ['clang++', 'g++']

  # Randomly pick one so on machines w/ both we test both compilers
  # to make sure our instructions downstairs don't rely on flags
  # only supported by one compiler.
  cpp = random.choice(supported_compilers)
  for _ in range(0, 100):
    if shutil.which(cpp):
      break
    cpp = random.choice(supported_compilers)

  if not shutil.which(cpp):
    raise Exception('No supported compiler found! (need g++ or clang++ on PATH')

  other_required_programs = [
    'make',
  ]
  for p in other_required_programs:
    if not shutil.which(p):
      raise Exception('Required program not found: {}'.format(p))

  # Download SDL library + headers so we don't need to rely
  # on the developer having them under /usr or C:\Windows
  sdl_lib_path = os.path.join('libs', 'sdl2')
  if not os.path.exists(sdl_lib_path):
    util.dl_archive_to(
      'http://www.libsdl.org/release/SDL2-2.0.14.tar.gz',
      sdl_lib_path
    )

  # Compile SDL2 (~5 minutes)
  sdl_static_lib_dir = os.path.join(sdl_lib_path, 'build', '.libs')
  sdl_include_dir = os.path.join(sdl_lib_path, 'include')
  if not os.path.exists(sdl_static_lib_dir):
    subprocess.run([
      os.path.join('.', 'configure'),
    ], cwd=sdl_lib_path, check=True)
    subprocess.run([
      'make',
    ], cwd=sdl_lib_path, check=True)
  
  # Compile the code
  all_src_files = [
    'main.cpp',
  ]

  if os.name == 'nt':
    lib_flags = ['-Wl,-Bstatic', '-lSDL2', '-Wl,-Bdynamic', '-ldl', '-lpthread']
  else:
    lib_flags = ['-Wl,-Bstatic', '-lSDL2', '-Wl,-Bdynamic', '-ldl', '-lpthread']

  subprocess.run([
    cpp,
    # enable a ton of modern C++ tools
    '--std=c++2a', '-Wall', '-Werror',
    # Add header files
    '-I{}'.format(sdl_include_dir),
    # Add lib search path
    '-L{}'.format(sdl_static_lib_dir),
    # Link against OS graphics libraries (must be installed by dev + end users)
    *lib_flags,
    # Code code code
    *all_src_files,
    *lib_flags,
    # And a.out is a lame name for a program
    '-o', 'main',
  ], check=True)

  # Run the output executable
  if not 'norun' in args:
    subprocess.run([os.path.join('.', 'main')])


if __name__ == '__main__':
  main()


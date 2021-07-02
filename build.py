
# Responsible for compiling the software,
# less OS-dependent than a makefile

import os
import sys
import subprocess
import shutil

# Our own tools
import util

def main(args=sys.argv):

  cpp = 'clang++'
  if not shutil.which(cpp):
    cpp = 'g++'

  if not shutil.which(cpp):
    raise Exception('No supported compiler found! (need g++ or clang++ on PATH')

  # Download OpenGL headers so we don't need to rely
  # on the developer having them under /usr or C:\Windows
  mesa_lib_path = os.path.join('libs', 'mesa')
  if not os.path.exists(mesa_lib_path):
    util.dl_archive_to(
      'https://archive.mesa3d.org//mesa-21.1.4.tar.xz',
      mesa_lib_path
    )

  mesa_glut_lib_path = os.path.join('libs', 'mesa_glut')
  if not os.path.exists(mesa_glut_lib_path):
    util.dl_archive_to(
      'https://archive.mesa3d.org//glut/MesaGLUT-7.9.2.tar.gz',
      mesa_glut_lib_path
    )

  gl_include_path = os.path.join(mesa_lib_path, 'include')
  glut_include_path = os.path.join(mesa_glut_lib_path, 'include')

  # Compile the code
  all_src_files = [
    'main.cpp',
  ]

  # Windows names their proprietary 64-bit GL implementation "opengl32" -_-
  # so we link against libraries differently for each OS
  if os.name == 'nt':
    lib_flags = ['-lopengl32',]
    # TODO see https://stackoverflow.com/a/2568108/9252743 for how to use GLUT on windows these days
  else:
    lib_flags = ['-lGL', '-lglut',]

  subprocess.run([
    cpp,
    # enable a ton of modern C++ tools
    '--std=c++20',
    # Add header files
    '-I{}'.format(gl_include_path),
    '-I{}'.format(glut_include_path),
    # Link against OS graphics libraries (must be installed by dev + end users)
    *lib_flags,
    # Code code code
    *all_src_files,
    # And a.out is a lame name for a program
    '-o', 'main',
  ], check=True)

  # Run the output executable
  subprocess.run([os.path.join('.', 'main')])


if __name__ == '__main__':
  main()


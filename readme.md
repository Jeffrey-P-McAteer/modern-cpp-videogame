
# Modern CPP Videogame

This is a quick effort to use the most modern
reasonably-sane cross-platform standards
to build a networked 2D platformer.

By "reasonably-sane" I mean recent C++ tools
which can be expected to run out of the box with a modern
compiler. This excludes very clean abstractions like [modules](https://en.cppreference.com/w/cpp/language/modules).

# Dev environment setup

Windows has issues running `configure` scripts, and lots of people say the only way
they can get the OS to compile libraries like SDL2 is using an intermediate shell like [MSYS2](https://www.msys2.org/).
Proceed at your own peril.

# MSYS2 Install

MSYS2 gives the following packages to windorks users: https://packages.msys2.org/search

```bash
pacman -Syu python python-pip clang make git
```

## General setup

```bash
# Install g++ or clang++
# Install make
# Install python

# Install the requests lib to download dependencies
python -m pip install --user requests

```

# Building & Running

```bash
python -m build
```

To alter the build process, edit `build.py` to run different compile
commands or pre-process things (make sure any downloaded 3rd-party code is `.gitignore`-ed
and for godssake use `os.path.join('a', 'b')` instead of hardcoding `a/b` or `a\\b` everywhere).

# Lessons Learned

GL is great for graphics primitives, and in modern languages is easy to use. In C++ it's horrific to setup for multi-platform development,
thus we currently download and compile a static copy of [SDL](https://github.com/libsdl-org/SDL) to embed for graphics.
SDL in turn will perform rather dynamic os-specific lookups for a GL runtime (scans around for `GL.so` or `opengl32.dll`, etc.).

`cmd.exe` is aweful, `powershell.exe` is slow, and apparently the best windows dev environment is [MSYS2](https://www.msys2.org/),
which looks aweful but works for lots of people? I don't speak 25-year-old operating-system-without-a-linker though, the more I study
windows the more it looks like a sony PSP with 16GB of ram and the world's slowest filesystem hammered on top.

MSYS2 is essentially the terminal + package manager microsoft should have built, delivered by some ex-Arch engineers.
It's stellar and I don't think I'm ever using `cmd.exe` or chocolatey ever again.



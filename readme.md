
# Modern CPP Videogame

This is a quick effort to use the most modern
reasonably-sane cross-platform standards
to build a networked 2D platformer.

By "reasonably-sane" I mean recent C++ tools
which can be expected to run out of the box with a modern
compiler. This excludes very clean abstractions like [modules](https://en.cppreference.com/w/cpp/language/modules).

# Dev environment setup

```bash
# Install g++ or clang++

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



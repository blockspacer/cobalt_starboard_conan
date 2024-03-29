﻿# About

Modified `starboard` library from cobalt.foo

## NOTE

```
# duplicate entries not supported by emscripten => log.cc renamed to common_log.cc
# duplicate entries not supported by emscripten => new.cc renamed to common_new.cc
```

## EXTRA FEATURES

- ported to CMake
- supports Conan
- added CMake options to disable some features
- supports starboard from cobalt.foo
- ported to WASM (emscripten) with threading support
- ported to WASM (emscripten) without threading support

## DOCS

cobalt.foo

The Chromium Projects https://www.chromium.org/developers

Chromium docs https://chromium.googlesource.com/chromium/src/+/master/docs

## HOW TO BUILD FROM SOURCE

Create conan profile, see https://github.com/blockspacer/CXXCTP#install-conan---a-crossplatform-dependency-manager-for-c

Build with created conan profile:

```
export CONAN_REVISIONS_ENABLED=1

sudo -E apt-get install build-essential libxmu-dev libxi-dev libgl-dev -y
sudo -E apt install libegl1-mesa-dev -y
sudo apt-get install libgles2-mesa-dev -y

conan create . conan/stable --profile clang

# clean build cache
conan remove "*" --build --force
```

## HOW TO INSTALL FROM CONAN

```
conan install --build=missing --profile clang -o enable_tests=False ..
```

## USAGE EXAMPLES

TODO

## LICENSE

TODO

## PORT ISSUES

- Some flags from `declare_args` may be not supported.
- Some platforms may be not supported.

## Used cobalt version

```bash
cab7770533804d582eaa66c713a1582f361182d3
```

# Commutators
C++ lib for commuting operators and some python bindings

# Installation

Has dependencies [boost, spdlog, pybind11, virtualenv]
 - for boost `apt install libboost-all-dev` should be enough (only used for tests).
 - need to ensure have venv module with `apt-get install python3-venv`
 - for spdlog, pybind, abseil and mimalloc I've added them as submodules so shouldn't
   have to do anything to get them.
 - for testing python api I manually create a virtualenv and run
   the tests inside this environment. warning gets deleted and rebuild
   when running ninja.

```
git clone --recurse-submodules https://github.com/Sam-Hornby/Commutators.git
cd Commutators
mkdir build
cd build
cmake .. -GNinja
ninja
ctest -L operators tests/
```

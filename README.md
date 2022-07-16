# Commutators
C++ lib for commuting operators and some python bindings

# Installation

Has dependencies [boost, spdlog, pybind11, virtualenv, setuptools]
 - for boost `apt install libboost-all-dev` should be enough (only used for tests).
 - need to ensure have venv module with `apt-get install python3-venv`
 - need to install several pip packages with `pip install setuptools wheel`
 - for spdlog, pybind, abseil and mimalloc I've added them as submodules so shouldn't
   have to do anything to get them.
 - for testing python api I manually create a virtualenv and run
   the tests inside this environment. warning gets deleted and rebuild
   when running ninja.

```
sudo apt install libboost-all-dev python3-venv ninja-build
pip install setuptools wheel
git clone --recurse-submodules https://github.com/Sam-Hornby/Commutators.git
cd Commutators
mkdir build
cd build
cmake .. -GNinja
ninja
ctest -L operators tests/
```

# Commutators
C++ lib for commuting operators and some python bindings

# Installation

Has dependencies [boost, spdlog, pybind11, pytest]
 - for boost `apt install libboost-all-dev` should be enough
 - for spdlog and pybind I've added them as submodules so shouldn't
   have to do anything to get them
 - for pytest pip install. I suggest a virtualenv as good practice

```
virtualenv -p python3 operators_venv
source operators_venv/bin/activate
pip install pytest wheel setuptools
git clone --recurse-submodules https://github.com/Sam-Hornby/Commutators.git
cd Commutators
mkdir build
cd build
cmake .. -GNinja
ninja
ctest tests -j2
```

# Commutators
C++ lib for commuting operators

# Installation

Has dependencies [boost, spdlog, pybind11, pytest]
for boost `apt install libboost-all-dev` should be enough
for spdlog and pybind clone it into the directory and my CMakelists should pick it up
for pytest pip install. I suggest a virtualenv as good practice

```
virtualenv -p python3 operators_venv
source operators_venv/bin/activate
pip install pytest
git clone ${this_repo} # https://github.com/Sam-Hornby/Commutators.git
cd Commutators
git clone ${spdlog}  # https://github.com/gabime/spdlog.git
git clone ${pybind11}
mkdir build
cd build
cmake .. -GNinja
ninja
ctest tests
```

# Commutators
C++ lib for commuting operators

# Installation

Has 2 dependencies: boost and spdlog
for boost `apt install libboost-all-dev` should be enough
for spdlog clone it into the directory and my CMakelists should pick it up

```
git clone ${this_repo} # https://github.com/Sam-Hornby/Commutators.git
cd Commutators
git clone ${spdlog}  # https://github.com/gabime/spdlog.git
mkdir build
cd build
cmake .. -GNinja
ninja
ctest tests
```

#from setuptools import setup, find_packages

#setup(
    #this will be the package name you will see, e.g. the output of 'conda list' in anaconda prompt
#    name = "FockModule", 
    #some version number you may wish to add - increment this after every update
#    version="1.0", 
  
    # Use one of the below approach to define package and/or module names:
  
    #if there are only handful of modules placed in root directory, and no packages/directories exist then can use below syntax
#     packages=[''], #have to import modules directly in code after installing this wheel, like import mod2 (respective file name in this case is mod2.py) - no direct use of distribution name while importing
  
    #can list down each package names - no need to keep __init__.py under packages / directories
#    packages=["FockModule"], #importing is like: from package1 import mod2, or import package1.mod2 as m2
  
    #this approach automatically finds out all directories (packages) - those must contain a file named __init__.py (can be empty)
    #packages=find_packages(), #include/exclude arguments take * as wildcard, . for any sub-package names
#)

from setuptools import setup
from setuptools.dist import Distribution
import os

class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    def has_ext_modules(foo):
        return True

def find_binary_dir():
    src_dir = os.path.dirname(os.path.abspath(__file__))
    return src_dir + "/../build/lib/operators"

def find_pybind_output():
    bin_dir = find_binary_dir()
    for cand in os.listdir(bin_dir):
        print(cand)
        if cand.startswith("FockOperators.cpython") and cand.endswith(".so"):
            return os.path.join(bin_dir, cand)
    raise Exception(f'Could not find pybind output in {bin_dir}')


setup(name="FockModule",
      description="",
      maintainer="Sam H",
      maintainer_email="samhornby0@gmail.com",
      packages=["FockModule"],
      package_dir={"FockModule": find_binary_dir()},
      package_data={"FockModule": [find_pybind_output()]},
      distclass=BinaryDistribution)

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

#DISTNAME = "FockModule"
#DESCRIPTION = ""
#MAINTAINER = "sam h"
#MAINTAINER_EMAIL = "samhornby0@gmail.com"
#URL = ""
#LICENSE = ""
#DOWNLOAD_URL = ""
#VERSION = '1.0'
#PYTHON_VERSION = (3, 6)


# Tested with wheel v0.29.0
class BinaryDistribution(Distribution):
    """Distribution which always forces a binary package with platform name"""
    def has_ext_modules(foo):
        return True


binary_directory = os.path.abspath("../lib/operators")

setup(name="FockModule",
      description="",
      maintainer="Sam H",
      maintainer_email="samhornby0@gmail.com",
      #url=URL,
      #license=LICENSE,
      #download_url=DOWNLOAD_URL,
      #version=(3, 6),
      packages=["FockModule"],
      package_dir={"FockModule": binary_directory},

      # Include pre-compiled extension
      #package_data={"FockModule": ["FockModule/FockOperators.cpython-36m-x86_64-linux-gnu.so"]},
      package_data={"FockModule": [binary_directory + "/FockOperators.cpython-36m-x86_64-linux-gnu.so"]},
      distclass=BinaryDistribution)

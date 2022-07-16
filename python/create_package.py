from setuptools import setup
from setuptools.dist import Distribution
#from setuptools import sandbox
import os
import shutil
import subprocess
import sys

def check_configuation():
  # check that the build directory exists
  src_dir = os.path.dirname(os.path.abspath(__file__))
  build_dir = os.path.abspath(src_dir + "/../build")
  if not os.path.exists(build_dir):
    raise Exception(f"Expected build dir at: {build_dir}")
  # check the binary produced exists
  cython_binding_path = os.path.join(
            build_dir,
            "lib/operators/FockOperators.cpython-36m-x86_64-linux-gnu.so")
  if not os.path.exists(cython_binding_path):
    raise Exception(f"Expected to find binary at: f{cython_binding_path}")

# Create empty directory to create wheel in, delete directory
# if it already exists
def create_install_directory():
  src_dir = os.path.dirname(os.path.abspath(__file__))
  # TODO: get cmake to pass the build directory into this scrript
  #       instead of working it out from the src location
  install_dir = os.path.abspath(src_dir + "/../build/python_install")
  shutil.rmtree(install_dir, ignore_errors=True)
  os.makedirs(install_dir)
  return install_dir

def create_pip_package(install_directory_path):
  src_dir = os.path.dirname(os.path.abspath(__file__))
  setup_script = os.path.join(src_dir, "setup.py")
  command = f'install --install-lib {install_directory_path} bdist_wheel'
  print(command)
  subprocess.run([sys.executable,
                   setup_script,
                   'install',
                   '--install-lib',
                   f'{install_directory_path}',
                   'bdist_wheel'])

def create_virtualenv(install_path, operators_package):
  # create virtualenv
  # install pip packages
  return None

if __name__ == "__main__":
  check_configuation()
  install_directory_path = create_install_directory()
  pip_package_path = create_pip_package(install_directory_path)
  create_virtualenv(install_directory_path, pip_package_path)

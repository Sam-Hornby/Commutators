import os
import shutil
import subprocess
import sys
import venv

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
  subprocess.run([
                      sys.executable,
                      setup_script,
                      'build',
                      '-b',
                      f'{install_directory_path}',
                      'bdist_wheel',
                  ],
                  cwd=install_directory_path,
                  stdout=sys.stdout,
                  stderr=sys.stderr)
  # Check sucessfully created and return path to install
  for folder, _, files in os.walk(install_directory_path):
    for name in files:
      if name.endswith('.whl'):
        return os.path.join(folder, name)
  raise Exception("Could not find .whl file")

def create_virtualenv():
  src_dir = os.path.dirname(os.path.abspath(__file__))
  venv_dir = os.path.abspath(src_dir + "/../build/operators_venv")
  if not os.path.exists(venv_dir):
    os.makedirs(venv_dir)
  venv.create(venv_dir, with_pip=True)
  return venv_dir

other_pip_dependencies  = [
  "pytest",
  "wheel",
  "setuptools",
]

def install_packages(venv_path, operators_package):
  venv_binary = os.path.join(venv_path, "bin/python3")
  subprocess.run([
    f'{venv_binary}',
    '-m',
    'pip',
    'install',
    f'{operators_package}',
    '--force-reinstall',
  ],
  stdout=sys.stdout,
  stderr=sys.stderr)

  for dep in other_pip_dependencies:
    subprocess.run([
      f'{venv_binary}',
      '-m',
      'pip',
      'install',
      f'{dep}',
    ],
    stdout=sys.stdout,
    stderr=sys.stderr)

if __name__ == "__main__":
  check_configuation()
  install_directory_path = create_install_directory()
  pip_package_path = create_pip_package(install_directory_path)
  venv_path = create_virtualenv()
  install_packages(venv_path, pip_package_path)


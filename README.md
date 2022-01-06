# oppt_cyber
This repository presents a POMDP-based autonomous pen-tester using the ABT solver within OPPT. Running the model will first require a working installation of [OPPT](https://github.com/rdllab/oppt). Instructions here are courtesy of [OPPT Examples Repository](https://github.com/RDLLab/oppt_examples).

## Repository Structure
- `cfg/` : This directory contains the config files for OPPT problem implementations
- `scenarios/` : This directory contains the cyber scenario files referred to by the config files
- `src/plugins/` : This directory will contain the cyber plugins used to implement autonomous pen-testing within OPPT

## Compiling the plugins

### If OPPT was installed outside of systems library:
If <oppt_install_location> used for the installation of oppt was not in a systems library, 
compiling plugins separately might present the problem of not being able to find the oppt libraries. 

To get around this issue, we can include installed oppt libraries by setting the environment variable "oppt_DIR"
```
export oppt_DIR=<oppt_install_location>/lib/cmake/oppt
```

The separate compilation of the plugins can then be done by:
```
cd <oppt_repo_location>/src/plugins
mkdir build
cd build

# Have to set the CMAKE_INSTALL_DATADIR flag to select install dir
cmake -DCMAKE_INSTALL_DATADIR=${PWD} ..
make 
make install
```	

### If OPPT installed within systems library:
Set the environment variable to include installed system oppt libraries 
```
export oppt_DIR="/usr/local/lib/cmake/oppt"
```

Compiling and installing the plugins can then be done by:
```
cd oppt_cyber/src/plugins
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_DATADIR=/usr/local/share ..
make
sudo make install
```

## Running the pen-tester
To execute the pen-tester POMDP run:
```
# Go to executable dir
cd <oppt_repo_location>/bin/

# Execute abt
./abt --cfg <oppt_cyber_location>/cfg/TestES1.cfg
```

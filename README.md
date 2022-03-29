# Phantom OS Genode build environment

This reposetory contains files required to build Phantom OS inside Genode. Currently, Phantom uses native Genode build system and integrated inside it as a ported app (inside `ports` repository). Though, it should be adapted to use goa tool in the future.

Repository structure:

- `/src` folder containing build scripts and resources
- `/run` folder containing .run scenarios
- `/ports` folder containing .port and .hash

## Clone repositories


```bash
#
# Cloning repositories 
#

git clone git@github.com:S7rizh/phantom_build_env.git

# this repository is supposed to be used as a working dir
cd phantom_build_env

# Repository with Genode build container wrapper
git clone git@github.com:skalk/genode-devel-docker.git
# Repostory with Phantom OS port
git clone git@github.com:S7rizh/phantomuserland.git
# Repostory with Genode
git clone git@github.com:genodelabs/genode.git

cd genode
git checkout 21.11

cd ..
```

## Setting up Docker container

Genode development team provides a convinient tool that can import or build a container that will have an installed Genode toolchain. Phantom can be built either using the development container or using the virtual machine with manually built toolchain. However, we suggest to use the container.

```bash
cd genode-devel-docker

./docker import SUDO=sudo

# Or you can build the container on your machine using the following command
# ./docker build SUDO=sudo
``` 

## Starting the container

```bash
#
# Starting the container
#

./docker run SUDO=sudo DOCKER_CONTAINER_ARGS=" --network host "

#
# Go to the working directory
#

cd <PATH_TO_DIR>
```

> `--network host` argument is used to avoid the process of setting up the network for debugging

## Initial setup

This section contains commands that would prepare the environment to build Phantom OS. 

> Following commands should be executed inside the container!

```bash

#
# Setting up Genode
#

cd ./genode/

# Creating build directory
./tool/create_builddir x86_64

# Preparing other required ports
./tool/ports/prepare_port libc nova grub2 gdb stdcxx

# Need to enable optional repositories in build.conf
sed -i 's/#REPOSITORIES/REPOSITORIES/g' build/x86_64/etc/build.conf

```

## Linking files related to Phantom to Genode repo

```bash
#
# Creating soft links to run and port files and copying files with build recipes
#

cd ../
cp -as "$(pwd)/src/phantom" genode/repos/ports/src/app/
cp -as "$(pwd)/src/phantom_env" genode/repos/ports/src/app/
cp -as "$(pwd)/src/phantom_vm" genode/repos/ports/src/app/
ln -s $(pwd)/run/* genode/repos/ports/run/
ln -s $(pwd)/ports/* genode/repos/ports/ports/


# Pretending that we prepared the port of Phantom
mkdir -p genode/contrib/phantom-7b5692dcbe87fc7e4fb528e33c5522f8f832c56d/src/app
echo 7b5692dcbe87fc7e4fb528e33c5522f8f832c56d > genode/contrib/phantom-7b5692dcbe87fc7e4fb528e33c5522f8f832c56d/phantom.hash

# Soft link directory with the sources
ln -s $(pwd)/phantomuserland genode/contrib/phantom-7b5692dcbe87fc7e4fb528e33c5522f8f832c56d/src/app/phantom


# Creating soft link to resources required in runtime
mkdir -p genode/build/x86_64/bin
ln -s $(pwd)/src/phantom/phantom_bins.tar genode/build/x86_64/bin

# Going back to Genode dir
cd ./genode
```

## Building and running Phantom OS

Following command can be used to build and run Phantom:

`make -C build/x86_64 KERNEL=nova VERBOSE= run/phantom`

## Debugging instructions

Phantom can be debugged using the run script with gdb monitor. It will setup the server on port 5555 to which gdb can be attached. 

Following command can be used to build Phantom with gdb monitor:

`make -C build/x86_64 KERNEL=nova VERBOSE= run/phantom_debug`

> Important: on Genode 21.05 and 21.11 `CC_OLEVEL=-O0` option will cause the following error `Error: __cxa_pure_virtual called, return addr is 0x12c139`

> Also important: version of gdb should be exactly the same to the one defined in `/usr/local/genode/tool/current/bin`. Currently it is 10.2 for Genode 21.11

The following command can be used to run gdb from inside the container

```bash
cd <genode_dir>/build/x86_64

usr/local/genode/tool/21.05/bin/genode-x86-gdb debug/ld.lib.so -n
-ex "target remote localhost:5555"
-ex "b binary_ready_hook_for_gdb"
-ex "c"
-ex "delete 1"
-ex "file debug/test-gdb_monitor"
-ex "set solib-search-path debug"
-ex "sharedlibrary"
```

Also, VS Code's Native Debug extension can be used. It is suggested to open genode directory as a project folder. The following configuration can be added to `launch.json`

```json
{
  "type": "gdb",
  "gdbpath": "<GDB_PATH>/bin/gdb",
  "request": "attach",
  "printCalls": true,
  "name": "Native debug: Attach to gdb monitor",
  "executable": "./debug/ld.lib.so",
  "target": "localhost:5555",
  "remote": true,
  "cwd": "<PATH_TO_GENODE_REPOSITORY>/build/x86_64",
  "valuesFormatting": "parseText",
  "autorun": [
    "b binary_ready_hook_for_gdb",
    "c",
    "delete 1",
    "cd build/x86_64/",
    "file debug/isomem",
    "set solib-search-path debug",
    "b setup_adapters",
    "sharedlibrary",
    "add-symbol-file debug/isomem -o 0x1000000",
    "add-symbol-file debug/ld.lib.so -o 0x30000",
    "add-symbol-file debug/gdbserver_platform-nova.lib.so -o 0x10d0000",
    "add-symbol-file debug/libc.lib.so -o 0x10e2c000",
    "add-symbol-file debug/vfs.lib.so -o 0x10d87000",
    "add-symbol-file debug/libm.lib.so -o 0x10d45000",
    "add-symbol-file debug/stdcxx.lib.so -o 0x10f9000",
    "add-symbol-file debug/vfs_pipe.lib.so -o 0x10d26000",
  ]
}

``` 

Note that adresses for `add-symbol-file` commands might be different for diffent machines. When debug scenario is started it outputs addresses of binaries. Example output looks as follows:

```
[init -> gdb_monitor]   0x40000000 .. 0x4fffffff: stack area
[init -> gdb_monitor]   0x30000 .. 0x14dfff: ld.lib.so
[init -> gdb_monitor]   0x10d0000 .. 0x10f8fff: gdbserver_platform.lib.so
[init -> gdb_monitor]   0x10e2c000 .. 0x10ffffff: libc.lib.so
[init -> gdb_monitor]   0x10d87000 .. 0x10e2bfff: vfs.lib.so
[init -> gdb_monitor]   0x10d45000 .. 0x10d86fff: libm.lib.so
[init -> gdb_monitor]   0x10f9000 .. 0x12b9fff: stdcxx.lib.so
[init -> gdb_monitor]   0x10d26000 .. 0x10d3cfff: vfs_pipe.lib.so
```

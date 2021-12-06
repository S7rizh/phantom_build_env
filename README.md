# Phantom OS Genode build environment

This reposetory contains files required to build Phantom OS inside Genode. Currently, Phantom uses native Genode build system and integrated inside it as a ported app (inside `ports` repository). Though, it should be adapted to use goa tool in the future.

Repository structure:

- `/src` folder containing build scripts and resources
- `/run` folder containing .run scenarios
- `/ports` folder containing .port and .hash

## Setting up Docker container

Genode provides development container that is used to build the system. This container can be either pulled from the Docker Hub (`sgmakarov/genode-toolchain`) or built manually using corresponding tool (https://github.com/skalk/genode-devel-docker).

```bash
git clone git@github.com:S7rizh/phantomuserland.git

git clone git@github.com:genodelabs/genode.git
git checkout 21.05

sudo docker run --name genode_phantom -it -v $(pwd):$(pwd) sgmakarov/genode-toolchain
```

## Genode initial setup

Creating soft links to run and port files and copying files with build recipes

```bash
cd <path_to_dir>
cp -R $(pwd)/src/* genode/repos/ports/src/app/
ln -s $(pwd)/run/* genode/repos/ports/run/
ln -s $(pwd)/ports/* genode/repos/ports/ports/
```

Setting up Genode

```bash
cd ./genode/

# Creating build directory
./tool/create_builddir x86_64

# Creating soft link to resources required in runtime
mkdir -p build/x86_64/bin
ln -s $(pwd)/../src/phantom/phantom_bins.tar build/x86_64/bin

# Preparing port of Phantom OS and then substituting the directory with soft link
./tool/ports/prepare_port phantom CHECK_HASH=no
rm -r contrib/phantom-7b5692dcbe87fc7e4fb528e33c5522f8f832c56d/src/app/phantom/
ln -s $(pwd)/../phantomuserland contrib/phantom-7b5692dcbe87fc7e4fb528e33c5522f8f832c56d/src/app/phantom

# Preparing other required ports
./tool/ports/prepare_port libc nova grub2 gdb stdcxx

# Need to enable optional repositories in build.conf. Can be done manually or the premade build.conf file can be copied
cp ../build.conf build/x86_64/etc/build.conf
```

## Build instructions

Following command can be used to build Phantom:

`make -C build/x86_64 CC_OLEVEL=-O0 KERNEL=nova VERBOSE= run/phantom`

Following command can be used to build Phantom with gdb monitor:

`make -C build/x86_64 CC_OLEVEL=-O0 KERNEL=nova VERBOSE= run/phantom_debug`



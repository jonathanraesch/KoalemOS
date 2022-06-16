#!/bin/bash

set -e


TARGET="x86_64-elf"
PREFIX=${PREFIX:-/usr/local/x86_64-elf-toolchain}
BINUTILS_VER=${BINUTILS_VER:-2.35}
GCC_VER=${GCC_VER:-10.2.0}
BUILD_PATH=$(realpath ${BUILD_PATH:-./build-cross})


if [ -d $BUILD_PATH ]; then
	if [ ! -w $BUILD_PATH ]; then
		echo "insufficient permissions in \"${BUILD_PATH}\""
		exit 1
	fi
else
	if [ ! -w $(dirname $BUILD_PATH) ]; then
		echo "insufficient permissions in \"$(dirname $BUILD_PATH)\" to create build directory"
		exit 1
	fi
fi

if [ -d $PREFIX ]; then
	if [ ! -w $PREFIX ]; then
		echo "insufficient permissions in ${PREFIX} to install compiler"
		exit 1
	fi
else
	if [ ! -w $(dirname $PREFIX) ]; then
		echo "insufficient permissions in \"$(dirname $PREFIX)\" to create installation directory"
		exit 1
	fi
fi


function cleanup() {
	rm -rf $BUILD_PATH
}

trap cleanup EXIT


mkdir -p $BUILD_PATH
cd $BUILD_PATH


wget https://ftpmirror.gnu.org/binutils/binutils-$BINUTILS_VER.tar.gz
tar -xf binutils-$BINUTILS_VER.tar.gz

wget https://ftpmirror.gnu.org/gcc/gcc-$GCC_VER/gcc-$GCC_VER.tar.gz
tar -xf gcc-$GCC_VER.tar.gz

mkdir -p binutils-objdir
cd binutils-objdir
../binutils-$BINUTILS_VER/configure --prefix $PREFIX --target $TARGET --with-sysroot --disable-werror --disable-nls
make
make install
cd ..

mkdir -p gcc-objdir
cd gcc-objdir
../gcc-$GCC_VER/configure --prefix $PREFIX --target $TARGET --enable-languages=c,c++ --disable-werror --disable-nls --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc


rm -r $BUILD_PATH

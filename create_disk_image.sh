#!/bin/bash

set -o xtrace

dd if=/dev/zero of=build/koalemos-uefi.img count=10000
/sbin/sfdisk build/koalemos-uefi.img < config/sfdisk_script

# 512 byte/sector * 34 sectors = 17408 byte
sudo /sbin/losetup -o 17408 /dev/loop0 build/koalemos-uefi.img
sudo mkfs -t fat /dev/loop0
sudo mount /dev/loop0 /mnt
sudo mkdir -p /mnt/EFI/BOOT
sudo cp build/boot/BOOTX64.efi /mnt/EFI/BOOT
sudo cp build/kernel/koalemos.elf /mnt/EFI/BOOT
sudo umount /dev/loop0
sudo /sbin/losetup -d /dev/loop0

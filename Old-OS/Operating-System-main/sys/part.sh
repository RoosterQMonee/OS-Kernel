# Partition the disk
dd if=/dev/zero bs=1M count=0 seek=64 of=image.hdd
 
parted -s image.hdd mklabel gpt
 
parted -s image.hdd mkpart ESP fat32 2048s 100%
parted -s image.hdd set 1 esp on
 
git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1
 
make -C limine
 
./limine/limine-install image.hdd
USED_LOOPBACK=$(sudo losetup -Pf --show image.hdd)

sudo mkfs.fat -F 32 ${USED_LOOPBACK}p1
mkdir -p img_mount
sudo mount ${USED_LOOPBACK}p1 img_mount
 
sudo mkdir -p img_mount/EFI/BOOT
sudo cp -v myos.elf limine.cfg limine/limine.sys img_mount/
sudo cp -v limine/BOOTX64.EFI img_mount/EFI/BOOT/
 
sync
sudo umount img_mount
sudo losetup -d ${USED_LOOPBACK}
{ pkgs }: {
    deps = [
      pkgs.tree
      pkgs.gdb
      pkgs.busybox
      pkgs.xorriso
      pkgs.llvmPackages.bintools-unwrapped
      pkgs.qemu
      pkgs.nasm
      pkgs.llvmPackages.clang
      pkgs.grub2_light
    ];
}
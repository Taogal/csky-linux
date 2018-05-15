# C-SKY Linux Arch Porting

* Directory arch/csky is the C-SKY Linux Arch Porting.
* Directory arch/arch-csky-drivers is the drivers of interrupt-controller and timer.

# How to use
* You also can use buildroot to quick start with simple steps:

```sh
    $ git clone https://gitlab.com/c-sky/buildroot.git
    $ cd buildroot
    $ make qemu_csky_ck810_uclibc_bt_defconfig
    $ make
```

# Upstream Cover letter :)
```sh
This patchset adds architecture support to Linux for C-SKY's 32-bit embedded
CPU cores and the patches are based on linux-4.16-rc5.

There are two ABI versions with several CPU cores in this patchset:
  ABIv1: ck610 (16-bit instruction, 32-bit data path, VIPT Cache ...)
  ABIv2: ck807 ck810 (16/32-bit variable length instruction, PIPT Cache ...)

More information: http://en.c-sky.com

I'm from Hangzhou,China C-SKY Microsystems and responsible for C-SKY Linux
port. My development repo is github.com/c-sky/csky-linux and use buildroot
as our CI-test enviornment. "LTP, Lmbench, uclibc-ng-test ..." will be tested
for every commit. See here for more details:
  https://gitlab.com/c-sky/buildroot/pipelines

You can try C-SKY linux in a few steps:
  $ git clone https://github.com/c-sky/buildroot.git
  $ cd buildroot
  $ make qemu_csky_ck807_uclibc_bt_defconfig
  $ make
It will download "linux uclibc-ng gcc binutils qemu busybox" source code and build
them into one image. How to run, See:
  https://github.com/c-sky/buildroot/blob/master/board/qemu/csky/readme.txt

I've finished uClibc-ng.org upstream and "gcc glibc binutils qemu ..." upstream is
on going and the source code is here:
  https://github.com/c-sky

It's my first patchset to linux and any feedback is welcome :)

Best Regards
  Guo Ren
```

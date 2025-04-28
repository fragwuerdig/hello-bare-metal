## Hello Bare Metal

A bare metal program that is able to:

- boot into x86-64bit mode with paging enabled
- allocate 1:1 mapped memory for kernel structs
- display by pushing into the VGA buffer
- accept key input via interrupt handling.

... more to come. This is a hobby project to recall my times as embedded dev. Don't use it for any activities other than playing around. And certainly don't boot it on your actual machine :P

## Run it

To run it let's assume you got gcc (for x86) installed and also qemu to run this thing. This is gonna build the kernel, package it into an iso and throw it at the VM's CD-ROM drive:

```
make run
```
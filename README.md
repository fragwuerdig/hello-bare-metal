## Hello Bare Metal

A bare metal program featuring:

- boot into x86-64bit mode with paging enabled
- minimal bump allocator with identity mapping
- interrupts (to be extended/configured properly)
- cooperative multitasking
- basic formatted vga output

... more to come. This is a hobby project to recall my times as embedded dev. Don't use it for any activities other than playing around. And certainly don't boot it on your actual machine :P

Currently the kernel main entrypoint is more used like a "playground" to test out new fetures and play around with them. Don't expect it to be the same on every commit to this repository.

## Run it

To run it let's assume you got gcc (for x86_64) installed and also qemu to run this thing. This is gonna build the kernel, package it into an iso and throw it at the VM's CD-ROM drive:

```
make run
```

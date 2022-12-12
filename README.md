# Containd
Containd is a contanerization utility meant to run a process in a container. Containd relies on the `clone` system call. 

## Setup
Run `make setup` as root to install the rootfs (alpine linux). Disable cgroups (no root) with the `--no-cgroups` option. Run `make` to build

## Usage
Run `sudo ./containd` 

| Option              | Usage |
| ------------------- | ----- |
| `-p`                | int for maximum number of processes- default `max`|
| `-m`                | int for maximum memory usage - default `max` |
| `-r`                | string for rootfs path - default `./rootfs`      |
| `-c`                | string for command to be run - default `/bin/sh`      |
| `-stack`            | int for stack size - default `65536`      |
| `--use-shell`       | option to use `system` instead of `execvp`      |
| `--disable-network` | option to disable networking       |
| `--no-cgroup`       | option to not use `cgroups` (voids `-p` and `-m`)      |
# LFSR Kernel Module

This project introduces a pseudorandom generator device for Linux, based on a 64-bit Fibonacci [LFSR](https://en.wikipedia.org/wiki/Linear-feedback_shift_register).

The feedback polynomial is:

> 2⁶⁴ + 2⁶² + 2⁶¹ + 2⁵⁹ + 1

## Kernel module

This kernel module installs a device file at _/dev/lfsr_.

### Operations

|Operation|Effect|
|---|---|
|Read|Consumes data from the device.|
|Write|Sets the generator seed (requires 8 bytes or more).|

### Setup

#### Dependencies

- Linux Kernel headers.
- GNU Make.
- gcc compiler.


|Platform|Command|
|---|---|
|Debian / Ubuntu|``apt install make gcc linux-headers-`uname -r` ``|
|Red Hat / Fedora|`dnf install make gcc kernel-devel elfutils-libelf-devel`|

#### Build

```shell
make
```

### Manage

#### Insert module

```shell
insmod lfsr.ko
```

> **Note:** Inserting an unsigned module into the Linux Kernel will taint it.

#### Remove module

```shell
rmmod lfsr
```

## Test tool

### Setup

#### Dependencies

- GNU Make
- g++ compiler.

|Platform|Command|
|---|---|
|Debian / Ubuntu|`apt install make g++`|
|Red Hat / Fedora|`dnf install make gcc-c++`|

#### Build

```shell
make
```

### Syntax

```
rand-test [-h] [-s SIZE] [ GEN ... ]
```

|Option|Range|Default|Description|
|---|---|---|---|
|`-h`|||Print help.|
|`-s`|4-9223372036854775807|`1073741824` (1 GiB)|Define the data length.|

#### Generators

|Generator|Description|
|---|---|
|`CRandom`|C linear congruential generator.|
|`LFSR`|_/dev/lfsr_|
|`Mersenne`|MT19937 Mersenne Twister.|
|`URandom`|_/dev/urandom_|

### Sample

|Generator|Time|Performance|
|---|---|---|
|CRandom|711.984 ms.|1508.1 MB/s.|
|LFSR|416.378 ms.|2746.55 MB/s.|
|Mersenne|1241.43 ms.|864.923 MB/s.|
|URandom|2122.14 ms.|505.97 MB/s.|

![Throughput](https://user-images.githubusercontent.com/10536251/204366187-22c96183-b359-4b7e-9e05-61984583064a.png)

## Performance comparison

|Version|Peak throughput|Cost per shift|Improvement over v0.1|
|---|---|---|---|
|0.1|84.2231 MB/s.|576 ops.|-|
|0.2|140.919 MB/s.|560 ops.|67.32 %|
|0.3|184.154 MB/s.|400 ops.|2.187 X|
|0.4|2336.51 MB/s.|18 ops.|32.61 X|

|Peak throughput (higher is better)|Cost per shifting (lower is better)|
|:-:|:-:|
|![Peak throughput](https://user-images.githubusercontent.com/10536251/204159565-40518f5d-2839-4c05-ad4e-bbc69e05b23a.png)|![Cost per shifting](https://user-images.githubusercontent.com/10536251/204131158-9438e0aa-208e-437e-940b-92a9cefeab0b.png)|

### Testing platform

|Processor|Memory|OS|
|---|---|---|
|Intel i7 11700 2.5 GHz|DDR4 32 GB 3200 MHz|Ubuntu 22.04 @ VirtualBox 7|

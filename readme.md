# binDfuSer

Create a .dfu file from a .bin file for STM32 MCU's.

## How to compile:

```
make
```

## How to use:

binDfuSer takes two input arguments
```
$ binDfuser <input.bin> <output.dfu>
```
The .dfu file can then be used with dfu-util or ST's "DefuSe Demo" to upload to the microcontroller.

## Current Limitations:

* Single binary (element)

* Element Address is currently fixed at 0x8000000

* VID fixed to 0x483

* PID fixed to 0xdf11

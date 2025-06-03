# UEFI T-Rex Runner

yeah so i ported the chrome dino game to UEFI because why not

![screenshot](screenshot.png)
*(screenshot coming soon™)*

## What is this?

It's literally the Chrome offline dinosaur game but running directly on UEFI firmware. No OS needed. Just pure, raw, dinosaur-jumping action at boot time.

## Why?

I was bored 

## Building

You need MinGW-w64. On Windows:

```bash
# Compile
x86_64-w64-mingw32-gcc -O3 -Wall -Wextra -std=c11 -ffreestanding 
    -fno-stack-protector -fpic -fshort-wchar -mno-red-zone 
    -c trex.c -o trex.o

# Link
x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared 
    -Wl,--subsystem,10 -e efi_main -o trex.efi trex.o
```

If you're on Linux, just install mingw-w64 from your package manager and you're good to go.

## Running

### Real Hardware (at your own risk lol)
1. Format a USB drive as FAT32
2. Create `/EFI/BOOT/` directory
3. Copy `trex.efi` to `/EFI/BOOT/BOOTX64.EFI`
4. Boot from USB
5. Jump over cacti

### QEMU (the safe way)
```bash
# Get OVMF (UEFI firmware for QEMU)
# On Windows, download from: https://github.com/retrage/edk2-nightly
# On Linux: sudo apt install ovmf

# Run it
qemu-system-x86_64 -bios OVMF.fd -drive file=fat:rw:. -m 256M -vga std
```

## Controls

- **SPACE** or **UP** - Jump / Start / Restart
- **ESC** - Quit (because ctrl+c doesn't work in UEFI)

## Known Issues

- Timer events are flaky on some UEFI implementations (falls back to polling)
- Sometimes the text doesn't clear properly (UEFI text mode is janky)
- No sound because UEFI doesn't really do audio
- Might not work on your specific firmware because UEFI implementations are all special snowflakes

## How it Works

Basically:
1. Get a pointer to the framebuffer through Graphics Output Protocol
2. Draw pixels directly to memory
3. Poll keyboard input
4. Try not to crash the firmware

The hardest part was figuring out UEFI's 500 different struct definitions. Fun times

## TODO

- [ ] Port Doom next (jk... unless?)

## License

Do whatever you want with it. If you brick your motherboard running this, that's on you.

## Credits

Chrome dino game by Google (sorry for stealing your sprites)

github@vladamisici
# ATARI ST Sidecart Test suite for the ROM Emulator

This repository contains a test program designed to validate the functionality and performance of the Sidecart ROM Emulator on the Atari ST series computers.

## Introduction

The Sidecart ROM Emulator emulates the behavior of Atari ST cartridges and the ROM memory contained. To ensure its flawless operation and accurate timing, this test suite offers several read tests, presenting the results visually on-screen.

For those new to the Sidecart ROM Emulator, we recommend visiting the official [Sidecart ROM Emulator website](https://sidecartridge.com) for a comprehensive overview.

## Getting Started

To get started with the testing process, you'll need two main files, both of which can be located in the [Release page](https://github.com/diegoparrilla/atarist-sidecart-test-rom/releases) of this repository:

- **`TESTSCRT.TOS`**: The primary test program, which should be transferred to your Atari ST machine.
  
- **`TESTROM.BIN`**: A binary file containing randomized data across two ROM banks. This file needs to be copied to both the microSD card used by the Sidecart ROM Emulator and your Atari ST computer.

## Setup Instructions

1. **File Preparation**: Ensure that `TESTROM.BIN` resides in the same directory as `TESTSCRT.TOS` on your Atari ST. Files can be transferred using emulated floppy disks, hard drives, or other preferred methods.

2. **Sidecart Setup**: Copy `TESTROM.BIN` to the microSD card of the Sidecart ROM Emulator. Insert the emulator into your powered-off Atari ST computer. As you power on the system, hold down the `SELECT` button on the Sidecart ROM Emulator. Upon successful boot, navigate the list of ROMs and choose `TESTROM.BIN`. Either reset or power cycle your Atari ST to boot into the default desktop.

3. **Running the Test**: With the setup complete, simply launch the `TESTSCRT.TOS` program. It will autonomously perform a series of read tests on the emulated ROM memory, displaying each result on-screen.


## Requirements

- An Atari STe computer (or emulator). There are several emulators available for Windows, Linux, and Mac. I recommend [Hatari](http://hatari.tuxfamily.org/), and I'm also a big fan of [MiSTer](https://misterfpga.org/). It should work on any Atari STe with at least 1MB of RAM.

- The [atarist-toolkit-docker](https://github.com/diegoparrilla/atarist-toolkit-docker): You should read first how to install it and how to use it. It's very easy.

- A `git` client. You can use the command line or a GUI client.

- A Makefile compatible with GNU Make.

## Building

Once you have your real Atari ST computer, Hatari emulator or MiSTer Atari ST up and running, you can build the program using the following steps:

1. Clone this repository:

```
$ git clone https://github.com/diegoparrilla/atarist-sidecart-test-rom.git
```

2. Export the `ST_WORKING_FOLDER` environment variable with the absolute path of the folder where you cloned the repository:

```
export ST_WORKING_FOLDER=<ABSOLUTE_PATH_TO_THE_FOLDER_WHERE_YOU_CLONED_THE_REPO>
```

3. Build the test program. Enter the `atarist-sidecart-test-rom` folder and run the `make` script:

```
cd atarist-sidecart-test-rom
make
```

4. Copy the `dist/TESTSCRT.TOS` file to your Atari ST computer, Hatari emulator or MiSTer Atari ST.

5. Run it!

## Resources 


## License
This project is licenses under the GNU General Public License v3.0. See the [LICENSE](LICENSE) file for details.

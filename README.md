# atarist-sidecart-test-rom
Atari ST test program for testing the Sidecart ROM Emulator

## Introduction

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

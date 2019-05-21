# KETCube - remote terminal application

The KETCube platform consist of multiple parts, for the overall view on the KETCube platform, see the [KETCube documentation](https://github.com/SmartCAMPUSZCU/KETCube-docs) repository.

This repository contains the remote terminal application. KETCube remote terminal application is a frontend for MQTT-based remote terminal subsystem of KETCube.

## Prerequisites

- compiler with C++14 support (GCC 5.0+, Clang 3.4+, MS Visual Studio 2015+)
- CMake
- OpenSSL
- fetched KETCube firmware repository 

## Build

After cloning the repository, navigate to its root directory. The following command sequence is an example of how to build the application:

```
git submodule init
git submodule update
mkdir build && cd build
cmake .. -DKETCUBE_FW_ROOT=/path/to/KETCube-fw/
make
```

## Running

To run the application, you need configuration file called config.ini. Please, refer to `samples/config-example.ini` example for all possible options

Having the file named `config.ini` in the same directory, as the executable, one could simply run the following command to start the session:

```
./ketcube-remote-terminal
```

Possible command line options are:
- `--config <file>` or `-c <file>` - specifies the config file path to be loaded
- `--input <file>` or `-i <file>` - specifies the input file with commands to be sent
- `--output <file>` or `-o <file>` - specifies the output file to store responses to

## Terminal commands

Terminal command set is the same, as the one contained in project in `KETCUBE_FW_ROOT` path.

In addition to standard commands, one may choose to use batch mode, which is started using `!batch`, confirmed using `!commit` and aborted using `!abort`. For example, one may use the following command sequence:

```
!batch
enable ADC
disable TxDisplay
set core basePeriod 360000
!commit
reload
```

This command sequence would take only 2 base periods to execute - one for the command batch, one for `reload` command.

The `reload` command is an exception from the rest of commands executed remotely. It is completely asynchronnous and the remote terminal application does not wait for reply, as the node performs reset much earlier, than the response mechanism is scheduled.

## Developed by

[![SmartCAMPUS ZCU](https://github.com/SmartCAMPUSZCU/KETCube-docs/blob/master/resources/images/smartCAMPUSZCU_logo.svg)](https://www.smartcampus.cz/en)
[![ZCU](https://github.com/SmartCAMPUSZCU/KETCube-docs/blob/master/resources/images/ZCU_logotype.svg)](https://www.zcu.cz/en)

See also the list of current and past 
[Contributors](https://github.com/SmartCAMPUSZCU/KETCube-fw/blob/master/CONTRIBUTORS).

## License

KETCube firmware in general an KETCube Core and KETCube Drivers in particular 
are distributed under the MIT-like University of Illinois/NCSA Open Source 
License. 
See also 
[LICENSE](https://github.com/SmartCAMPUSZCU/KETCube-fw/blob/master/LICENSE) file.

The specific parts (see comments in source files) of the KETCube firmware are 
distributed under BSD-like Semtech and ST Microelectronics licenses.
See also 
[SEMTECH](https://github.com/SmartCAMPUSZCU/KETCube-fw/blob/master/LICENSE_SEMTECH)
and 
[STM](https://github.com/SmartCAMPUSZCU/KETCube-fw/blob/master/LICENSE_STM) 
LICENSE files.
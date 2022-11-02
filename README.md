# Virtual SNMP Agent
A toolkit to easily run and manage multiple virtual SNMP agents on a single host

![Badge](https://img.shields.io/badge/version-v1.0.0-blue) ![Badge](https://img.shields.io/badge/doxygen-missing-red) ![Badge](https://img.shields.io/badge/tests-missing-red)

# About
The Virtual SNMP Agent comprises a set of utilities to manage and run simulations of any network device responding to SNMP queries. Its major functionality is provided by the vsa program, which parses the SNMP walk output of some target agent and runs an exact copy of it on port 161. The API used by vsa to achieve this task is also exported for extension through its headers files and the static library libvsa. If one intends to further the vsa capabilities, Virtual SNMP Agent also delivers docker facilities for running multiple agents on a single host.

# Installing
Virtual SNMP Agent depends on two external libraries
1. net-snmp
2. glib-2.0

After downloading this project, the basic installation is done by:
```
autoreconf -i
./configure
make
make install
```

You can also use the auxiliary script files autogen.sh and clean.sh when experimenting with source code. They speed up the project's building and cleaning process.
```
# (Edit some code ...)
./autogen.sh
# (Make some tests ...)
./clean.sh
```

Even though libvsa is automatically built and installed along with its header files for use, if you intend to write software that links against it, it is advisable, but not required, to have pkg-config installed.

Also, to run multiple agents on a single host, you must enable Docker support with
```
./configure --enable-docker=yes
```
and, obviously, have docker installed.

# vsa
Running vsa is a simple task, it just needs to be given a file containing the SNMP walk output of some target agent. After parsing that file, vsa starts responding to SNMP queries on port 161. For example, using the net-snmp utility:
```
snmpwalk -On -v2c -cpublic <target agent address> . > state.mib
vsa state.mib
```
You can also pass the file name through the environment variable VSA_FILE.

__Attention:__
1. Since vsa only parses numeric OIDs, with the exception of a .iso prefix, you must use the -On flag.
2. vsa also requires a vsa.conf file following the same snmpd.conf rules (there is a sample version along with the source code).

# libvsa
libvsa provides all the objects and functions required by vsa to parse and build SNMP objects. Its interface is exported to --prefix/include/vsa (default path is /usr/local/include/vsa) and, along with the static library created, can be used to build new applications. The libvsa functions never abort. When something wrong occurs, they return error values and log messages to stderr as warnings and debugs. Those messages can be disabled by defining -DNVSA_WARN and -DNVSA_DEBUG at building time:
`./configure CPPFLAGS="-DNVSA_WARN -DNVSA_DEBUG"`

The pkg-config utility can be used to link against libvsa:
```
pkg-config --cflags vsa
pkg-config --libs vsa
```

# Using Docker to Run Multiple vsa Agents
If you want to further the vsa capabilities and run multiple agents on different ports, enable Docker support with `./configure --enable-docker=yes` at building time. This will create a vsa image and the auxiliary script vsa-docker-manager for running multiple vsa agents inside Docker containers.

## vsa-docker-manager
The vsa-docker-manager script provides facilities to manage multiple vsa agents running inside Docker containers through two main actions:
run and stop.

When used to run an agent, vsa-docker-manager requires a file name and, optionally, its path, a port number, and the path
of the configuration file vsa.conf.

If used to stop an agent, vsa-docker-manager only requires the port number where the agent is running. If no port number
is given, then all the agents running are stopped.

### Examples
```
# Running vsa on port 8888 with file foo.mib located in the current directory
vsa-docker-manager -f foo.mib -p 8888

# Stopping vsa on port 8888
vsa-docker-manager -p 8888

# Running vsa on any port available with file foo.mib located in the home directory
vsa-docker-manager -f foo.mib -d ~

# Running vsa on any port available with file bar.mib located in $HOME/.vsa
vsa-docker-manager -f bar.mib

# Stopping all vsa containers
vsa-docker-manager
```

For more details run `vsa-docker-manager -h`.

## Using the Latest vsa Image from Docker Hub
If you just want to use a pre-built image, get the latest version from Docker Hub:
```
docker pull daltonvlm/vsa
```

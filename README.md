# Virtual SNMP Agent
A toolset to run and manage multiple SNMP agents simulations

![Badge](https://img.shields.io/badge/version-v1.0-blue) ![Badge](https://img.shields.io/badge/todo-doxygen-blue)

# About
The Virtual SNMP Agent comprises a set of utilities to manage and run simulations of any network device responding to SNMP queries. Its major functionality is provided by the vsa program, which parses the SNMP walk output of some target agent and runs an exact copy of it on port 161. The API used by vsa to achieve this task is also exported for extension through its headers files and the static library libvsa. If one intends to further the vsa capabilities, Virtual SNMP Agent also delivers docker facilities for running multiple agents on a single host.

# Installing
Virtual SNMP Agent depends on two external libraries
1. net-snmp
2. glib

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

Also, to run multiple agents in a single host, you should enable Docker support with
```
./configure --enable-docker=yes
```
and, obviously, have docker installed.

# vsa
Running vsa is a simple task, it just needs to be passed the file containing the SNMP walk output of the target agent. After parsing the file, vsa starts responding SNMP queries on port 161. For example, using the net-snmp utility:
```
snmpwalk -On -v2c -cpublic <target agent address> . > state.mib
vsa state.mib
```
You can also pass the file name through the environment variable VSA_FILE. The argument option has precedence over the environment variable.

__Attention:__
1. Since vsa only parses numeric OIDs, with the exception of a .iso prefix, you must use the -On flag.
2. vsa also requires a vsa.conf file following the same snmpd.conf rules (there is a sample version along with the source code)

# libvsa
libvsa provides all the objects and functions required by vsa to parse and build SNMP objects. Its interface is exported to --prefix/include/vsa (default path is /usr/local/include/vsa) and, along with the static library created, can be used to build new applications. The libvsa functions never abort. When something wrong occurs, they return error values and log messages to stderr as warnings and debugs. Those messages can be disabled by defining -DNVSA_WARN and -DNVSA_DEBUG at building time:
`./configure CPPFLAGS="-DNVSA_WARN -DNVSA_DEBUG"`

The pkg-config utility can be used to link against libvsa:
```
pkg-config --cflags vsa
pkg-config --libs vsa
```

# Using Docker to Run Multiple Agents
If you want to further the vsa capabilities and run multiple agents on different ports, enable Docker support with `./configure --enable-docker=yes` at building time. This will create a vsa image and the auxiliary script vsa-docker-manager to run multiple vsa agents in Docker containers.

## vsa-docker-manager
You can opt to use the Docker tools to directly manage vsa containers, but vsa-docker-manager facilitates the task of running and stopping them. When used to run vsa on a container, vsa-docker-manager requires a file name (to forward to the vsa agent), and, optionally, a directory to locate that file and a port number to run on. If no directory is passed, the script will first look for the file in $HOME/.vsa and then in '.'. If no port number is passed, the first number available starting by 1024 will be picked. Now, if used to stop a container, vsa-docker-manager must receive the port number where that container is running. If no port is received, then all containers will be stopped.

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

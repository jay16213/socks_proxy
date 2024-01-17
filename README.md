# SOCKS Proxy

This repository contains an implementation of a SOCKS4 proxy server, supporting both connect mode and bind mode.

## Installation
Run the following command to compile the source code and generate the executable `socks_server`
```
make
```

## Usage
Once built, you can start the SOCKS4 server by running:

```bash
./socks_server <port> # Listen on specified port
```

## Features
This proxy server provides two operations as defined in the SOCKS4 protocol:
- Connect Mode: Allows clients to establish a connection through the proxy to an arbitrary network service.
- Bind Mode: Enables the proxy to accept incoming connections from a specified server on a designated port, facilitating reverse connections as needed, especially useful in scenarios like passive FTP.

## Development
For developers who wish to contribute to this project and require debugging capabilities, use the debug option when building the project:
```
make debug
```

This enables the display of debug messages from the program.

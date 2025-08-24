# Multithreaded TCP Echo Server in C

## Overview
This is a simple multithreaded TCP server implemented in C that handles multiple clients concurrently. Each client connection is managed in a separate POSIX thread. The server echoes back any message received from a client.

## Features
- Uses POSIX threads (`pthread`) to handle multiple clients concurrently.
- TCP socket programming for client-server communication.
- Tested in a Linux environment.
- Clients can connect using tools like `telnet` or `nc`.

## Usage

### Compile
```bash
gcc -pthread server.c -o server

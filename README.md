
# Bomberstudent - M1 RES

## Table of Contents

1. [RES Game Project](#res-game-project)

2. [How to use](#how-to-use)

    2.1. [V-0.1.311023](#v-01311023)

3. [Scripts](#scripts)

    3.1. [Stress testing the TCP server](#stress-testing-the-server)

## RES Game Project

***Contributors**: Hadrien Belleville & Ugo Courche*

***Date of first commit**: October 31, 2023*

> This Git repository only contains the source code of the C server for the Bomberstudent game project.

### How to use

> This section contains the methods of how to use this code at current state.
>
> How to read the versions name: `V-X.Y.Z`
>
> + `X`: This indicates the major version. It is incremented for significant changes that may introduce backward-incompatible features.
> + `Y`: This is the minor version. It is typically incremented for smaller, backward-compatible changes or feature additions.
> + `Z`: This represents the patch version. It is used for bug fixes or small improvements that are backward-compatible. In our case this will always be the date of the first commit of the patch version.

#### V-0.1.311023

To test the code at this current state follow these steps:

+ Open a Terminal for the server and navigate to the Makefile directory. Run:

```bash
    make && ./main
```

+ In separate Terminal windows for each client, establish a connection to the server and initiate the stream flow:

```bash
    nc localhost 42069
```

+ Feel free to send messages to the server from any connected client terminal. Don't forget to start with a polite greeting - say 'hello' first!

### Scripts

> This section covers all scripts that can be used for testing purposes or anything that we may find usefull...

#### Stress Testing the server

The following script can be used if needed to create multiple instances of clients with connections in the server. It can be used for debugging and testing purposes, especially to stress testing the TCP server.

```bash
#!/bin/bash

# Check if an argument is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <number_of_iterationsX>"
    exit 1
fi

# Retrieve the number of iterations from the first argument
n=$1

# Loop and execute command in a child process
for ((i=0; i<n; i++)); do
    (
        nc localhost 42069
    ) &
done

# Wait for all child process to end
wait
```

# Echo Server

This program creates a TCP server socket, binds, and listens to a random port.
Once a client connection has been established the socket echoes any incoming data.

### Building
To build, execute the command `make` in the project directory

### Running
To run, execute the command `./echo_s` in the project directory
    - You may need to grant execute permissions by running the command `chmod +x echo_s`
    - You can use the optional `-d` flag to run in verbose mode
        - This flag has a mandatory argument, which can be an integer value in the range of 0-6
        - Higher argument value = increased verbosity
        - Example of running with the flag: `./echo_s -d 5`

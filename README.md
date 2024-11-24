# Web Server
### Cristian Madrazo
This program creates a TCP server socket, binds, and listens to a random port.
Once a client connection has been established the socket serves files that meet
    assignment guidelines using HTTP/1.0. HTTP/1.1 GET requests are also 
    answered with an HTTP/1.0 response since most web browsers are not easily
    configured. 

This builds off of another project of mine, see `echo-server/`

### Building
To build, execute the command `make` in the project directory

### Running
To run, execute the command `./web_server` in the project directory
    - You may need to grant execute permissions by running the command `chmod +x web_server`
    - You can use the optional `-d` flag to run in verbose mode
        - This flag has a mandatory argument, which can be an integer value in the range of 0-6
        - Higher argument value = increased verbosity
        - Example of running with the flag: `./web_server -d 5`

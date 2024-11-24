// **************************************************************************************
// * Echo Strings (echo_s.cc)
// * -- Accepts TCP connections and then echos back each string sent.
// **************************************************************************************
#include "echo_s.h"

// **************************************************************************************
// constants and macros
#define BUFFER_SIZE 1024
#define DEFAULT_PORT 1748

// **************************************************************************************
// * processConnection()
// * - Handles reading the line from the network and sending it back to the client.
// * - Returns true if the client sends "QUIT" command, false if the client sends "CLOSE".
// **************************************************************************************
int processConnection(int sockFd) {
  bool quitProgram = true;
  bool keepGoing = true;
  char buffer[BUFFER_SIZE];
  while (keepGoing) {

    //
    // Call read() call to get a buffer/line from the client.
    // Hint - don't forget to zero out the buffer each time you use it.
    // int bytesRead;
    
    bzero(buffer, BUFFER_SIZE);
    int bytesRead = read(sockFd, buffer, BUFFER_SIZE);

    std::string message = buffer;
    INFO << "New chunk of data received with: " << message << ENDL;

    //
    // Check for quit command
    if (message.substr(0, 4) == "QUIT") {
      DEBUG << "Chunk starts with QUIT command" << ENDL;
      keepGoing = false;
      quitProgram = true;
    }

    //
    // Check for close command
    if (message.substr(0, 5) == "CLOSE") {
      DEBUG << "Chunk starts with CLOSE command" << ENDL;
      keepGoing = false;
      quitProgram = false;
    }

    //
    // Call write() to send line back to the client.
    ssize_t bytes_sent = write(sockFd, buffer, strlen(buffer));

  }

  return quitProgram;
}

// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connection until processConnection() returns 1
// **************************************************************************************

int main (int argc, char *argv[]) {

  // Obtain a pseudo random number to seed the dist generator
  std::random_device rd;  
  std::mt19937 eng(rd());

  // ********************************************************************
  // * Process the command line arguments
  // ********************************************************************
  Argparser parser(argc, argv);
  parser.add_option('d', true, false, 1, 1);
  parser.parse();
  std::vector<int> arg_values = parser.get_values_int('d');

  if (arg_values.size() == 0) {
    LOG_LEVEL = 0;
  }
  else {
    LOG_LEVEL = arg_values.at(0);
  }

  // *******************************************************************
  // * Creating the inital socket is the same as in a client.
  // ********************************************************************
  int listenFd = -1;
  // Call socket() to create the socket you will use for lisening.
  if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    
    FATAL << "Failed to create listening socket" << ENDL;
    return -1;
  }

  DEBUG << "Calling Socket() assigned file descriptor " << listenFd << ENDL;
  
  // ********************************************************************
  // * The bind() and calls take a structure that specifies the
  // * address to be used for the connection. On the cient it contains
  // * the address of the server to connect to. On the server it specifies
  // * which IP address and port to lisen for connections.
  // ********************************************************************
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));

  // *** assign 3 fields in the servadd struct sin_family, sin_addr.s_addr and sin_port
  // *** the value your port can be any value > 1024.
  servaddr.sin_family = AF_INET;  // IPv4
  servaddr.sin_addr.s_addr = INADDR_ANY;  // listen to any address
  servaddr.sin_port = htons(DEFAULT_PORT); // htons converts the int to network byte order
  


  // ********************************************************************
  // * Binding configures the socket with the parameters we have
  // * specified in the servaddr structure.  This step is implicit in
  // * the connect() call, but must be explicitly listed for servers.
  // ********************************************************************
  bool bindSuccesful = false;

  std::uniform_int_distribution<> distr(1025, 65535);
  while (!bindSuccesful) {
    // ** Call bind()
    // You may have to call bind multiple times if another process is already using the port
    // your program selects.
    if (bind(listenFd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
      DEBUG << "Bind failed" << ENDL;
      servaddr.sin_port = htons(distr(eng));
    }
    else {
      bindSuccesful = true;
      DEBUG << "Bind succesfull" << ENDL;
    }
  }
  std::cout << "Using port: " << ntohs(servaddr.sin_port) << std::endl;
  // *** DON'T FORGET TO PRINT OUT WHAT PORT YOUR SERVER PICKED SO YOU KNOW HOW TO CONNECT.


  // ********************************************************************
  // * Setting the socket to the listening state is the second step
  // * needed to being accepting connections.  This creates a queue for
  // * connections and starts the kernel listening for connections.
  // ********************************************************************
  int listenQueueLength = 1;
  // ** Cal listen()
  if (listen(listenFd, listenQueueLength) < 0) {
        FATAL << "Listen() failed" << std::endl << ENDL;
        close(listenFd);
        return -1;
    }

  // ********************************************************************
  // * The accept call will sleep, waiting for a connection.  When 
  // * a connection request comes in the accept() call creates a NEW
  // * socket with a new fd that will be used for the communication.
  // ********************************************************************
  bool quitProgram = false;
  while (!quitProgram) {
    int connFd = 0;

    // Call the accept() call to check the listening queue for connection requests.
    // If a client has already tried to connect accept() will complete the
    // connection and return a file descriptor that you can read from and
    // write to. If there is no connection waiting accept() will block and
    // not return until there is a connection.
    // ** call accept() 
    int addrlen = sizeof(servaddr);
    DEBUG << "Calling accept(" << listenFd << ", " << (struct sockaddr *)&servaddr << ", " << (socklen_t *)&addrlen << ENDL;
    int new_socket = accept(listenFd, (struct sockaddr *)&servaddr, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        FATAL << "Accept() failed" << ENDL;
        close(listenFd);
        return -1;
    }

    DEBUG << "Connection accepted" << ENDL;
    
    // Now we have a connection, so you can call processConnection() to do the work.
    quitProgram = processConnection(new_socket);
   
    close(new_socket);
  }

  close(listenFd);

}

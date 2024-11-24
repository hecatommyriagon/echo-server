// **************************************************************************************
// * Echo Strings (echo_s.cc)
// * -- Accepts TCP connections and then echos back each string sent.
// **************************************************************************************
#include "web_server.h"

// **************************************************************************************
// constants and macros
#define BUFFER_SIZE 1024
#define DEFAULT_PORT 1748
#define DEFAULT_HTTP_CODE 400
#define PREVIEW_LEN 30
#define EMPTY_MSG_LIMIT 5

// **************************************************************************************
// sig_handler()
// handles the CTRL + C signal
// **************************************************************************************
void sig_handler (int signum) {
    std::cout << std::endl;
    INFO << "Caught signal " << signum << ENDL;
    INFO << "Closing file descriptors 3-31" << ENDL;

    for (int i = 3; i < 32; i++) {
        close (i);
    }

    exit (1);
}

// **************************************************************************************
// createPreview
// creates a preview message, which is the first few characters and a " ... " appended
// length of preview is a constant
// **************************************************************************************
std::string create_preview (std::string message) {
    std::string message_preview =
    ((message.size () < PREVIEW_LEN) ? message.substr (0, message.size ()) :
                                       message.substr (0, PREVIEW_LEN) + "...");

    return message_preview;
}

// **************************************************************************************
// sendFile()
// Takes a socket and an ifstream object representing the file to send
// Sends the file in chunks of BUFFER_SIZE (which should already be done by the inet lib
//  but good practice)
// Does not check for file errors! Do that before calling
// **************************************************************************************
int sendFile (int sockFd, std::ifstream& file) {

    // set pointer to beginnign of file
    file.seekg (0, std::ios::beg);

    // Read the file and send it in chunks
    char buffer[BUFFER_SIZE];
    while (file.read (buffer, sizeof (buffer))) {


        DEBUG << "Sending chunk of file to client: "
              << create_preview (string_to_literal (std::string (buffer))) << ENDL;
        send (sockFd, buffer, file.gcount (), 0);
    }

    // Send any remaining bytes
    if (file.gcount () > 0) {
        DEBUG << "Sending chunk of file to client: "
              << create_preview (string_to_literal (std::string (buffer))) << ENDL;
        send (sockFd, buffer, file.gcount (), 0);
    }

    return 0;
}

// **************************************************************************************
// * sendLine()
// * - Takes an arbitrary std::string, converts it to a char array, adds the
// line terminator and sends it to the client
// **************************************************************************************
int sendLine (int sockFd, std::string data) {
    // add http special chars to mark end of packet and transform to c string
    const char* data_arr = data.c_str (); // C style string

    DEBUG << "Sending line to client: " << string_to_literal (data) << ENDL;

    // Call write() to send line back to the client.
    ssize_t bytes_sent = write (sockFd, data_arr, strlen (data_arr));

    return 0;
}

// **************************************************************************************
// sendResponse
// sends a typical http response packet to client
// expecting header with status line and content type at least
// will add content length and blank line
// returns 0 if succesful or a status code of a suggested alternative
// **************************************************************************************
int sendResponse (int sockFd, std::string filepath, std::string headers) {

    // Open the file in binary mode
    std::ifstream file (filepath, std::ios::binary);

    // Check if the file opened successfully
    if (!file) {
        ERROR << "File \"" << filepath << "\" could not be opened, check permissions" << ENDL;
        return 404;
    }

    // if file opened okay
    else {

        // Get the size of the file
        file.seekg (0, std::ios::end);
        std::streamsize size = file.tellg ();
        file.seekg (0, std::ios::beg);

        // Create a char buffer to hold the file contents
        std::vector<char> buffer (size);

        // Read the contents of the file into the buffer and save it as string
        if (file.read (buffer.data (), size)) {

            // send headers first which should contain status line
            sendLine (sockFd, std::string (headers));

            // send file size info to client
            sendLine (sockFd, std::string ("Content-Length: " + std::to_string (size) + "\r\n"));

            // Send blank line to separate body from headers
            sendLine (sockFd, "\r\n");

            // // Send file contents
            // std::string file_contents (buffer.data ());
            // sendLine (sockFd, file_contents);

            sendFile (sockFd, file);
        }

        // if error reading file that seems to exist and was opened succesfully
        else {
            return 500;
        }

        // Close the file
        file.close ();
    }

    return 0;
}

// **************************************************************************************
// Uses sendLine() to send back the 505 error code and message.
// Indicates unsuported http version
// **************************************************************************************
void send505 (int sockFd) {

    // set headers to send
    std::string headers = "HTTP/1.0 505 HTTP Version Not Supported\r\n";
    headers += "Content-Type: text/html; charset=UTF-8\r\n";

    // Resource file
    std::string filepath = "http/505.html";

    int respond = sendResponse (sockFd, filepath, headers);

    // if sending back nice response failed we send back bare bones response
    if (sendResponse (sockFd, filepath, headers)) {
        sendLine (sockFd, "HTTP/1.0 505 HTTP Version Not Supported\r\n\r\n");
    }
}

// **************************************************************************************
// Uses sendLine() to send back the 500 error code and message.
// Indicates an internal server error
// **************************************************************************************
void send500 (int sockFd) {
    ERROR << "Calling unimplemented function: HTTP/1.1 500 Internal Server Error to client" << ENDL;
}

// **************************************************************************************
// * send400()
// * - Uses sendLine() to send back the 400 error code and message.
// **************************************************************************************
void send400 (int sockFd) {

    // set headers to send
    std::string headers = "HTTP/1.0 400 Bad Request\r\n";
    headers += "Content-Type: text/html; charset=UTF-8\r\n";

    // Resource file
    std::string filepath = "http/400.html";

    // if sending back nice response failed we send back bare bones response
    if (sendResponse (sockFd, filepath, headers)) {
        sendLine (sockFd, "HTTP/1.0 400 Bad Request\r\n\r\n");
    }
}

// **************************************************************************************
// * send404()
// * - Uses sendLine() to send back the 404 error code and message.
// **************************************************************************************
void send404 (int sockFd) {

    // set headers to send
    std::string headers = "HTTP/1.0 404 Not Found\r\n";
    headers += "Content-Type: text/html; charset=UTF-8\r\n";

    // Resource file
    std::string filepath = "http/404.html";

    // if sending back nice response failed we send back bare bones response
    if (sendResponse (sockFd, filepath, headers)) {
        sendLine (sockFd, "HTTP/1.0 404 Not Found\r\n\r\n");
    }
}

// **************************************************************************************
// * send200()
// * - Uses sendLine() to send back the 200 code and contents of the file
// * - If file not found then send 404
// **************************************************************************************
void send200 (int sockFd, std::string filepath) {
    DEBUG << "Verifying request" << ENDL;

    // Define the regex patterns for fileX.html and imageX.jpg
    std::string f_pattern = R"(file[0-9]\.html)";
    std::regex file_pattern (f_pattern);
    std::string i_pattern = R"(image[0-9]\.jpg)";
    std::regex image_pattern (i_pattern);

    // Check if the filename matches the regex patterns
    if (!(std::regex_match (filepath, file_pattern) || std::regex_match (filepath, image_pattern))) {
        DEBUG << "Request format doesn't meet assignment guidelines" << ENDL;

        // we still send 404 because while the file may exist, the assignment
        // specifies only certain files should be returned
        send404 (sockFd);
    }

    // Check if file doesn't exists in current working dir
    else if (!std::filesystem::exists (filepath)) {
        DEBUG << "Requested file doesn't exist" << ENDL;
        send404 (sockFd);
    }

    // if file exists
    else {
        DEBUG << "Request verified succesfully" << ENDL;

        std::string headers = "HTTP/1.0 200 OK\r\n";

        // send proper content header to client
        // if html file
        if (std::regex_match (filepath, file_pattern)) {
            headers += "Content-Type: text/html; charset=UTF-8\r\n";
        }
        // else must be jpg image
        else {
            headers += "Content-Type: image/jpeg\r\n";
        }

        int response = sendResponse (sockFd, filepath, headers);

        switch (response) {

        case (404): send404 (sockFd); break;

        case (500): send500 (sockFd); break;

        default: return;
        }
    }
}

// **************************************************************************************
// readRequest()
// Read the request and return a status code and file name if we can find one
// Returns 0 if not a complete request
// **************************************************************************************
int readRequest (int sockFd, std::string& filename) {
    std::string full_message = "";
    int default_code         = DEFAULT_HTTP_CODE;
    bool quitProgram         = true;
    bool keepGoing           = true;
    char buffer[BUFFER_SIZE];
    int empty_messages = 0;

    while (keepGoing) {

        std::string filepath;

        // Call read() call to get a buffer/line from the client.
        // Hint - don't forget to zero out the buffer each time you use it.
        // int bytesRead;
        bzero (buffer, BUFFER_SIZE);
        int bytesRead = read (sockFd, buffer, BUFFER_SIZE);

        // Receive message
        std::string message = buffer;
        INFO << "New message received: " << create_preview (string_to_literal (message)) << ENDL;

        // if error reading from socket
        if (bytesRead < 0) {
            ERROR << "Error reading from socket, closing connection" << ENDL;
            quitProgram = false;
            break;
        }

        // if this message is empty then add 1 to keep track
        if (bytesRead == 0) {
            INFO << "Client disconnected, closing connection" << ENDL;
            quitProgram = false;
            break;
        }

        // When client side connection closes unexpectedly on nc, we get hit with repeated
        //  empty messages so the connection is closed after a certain limit
        if (empty_messages >= EMPTY_MSG_LIMIT) {
            keepGoing = false;
        }

        // Append message to full message
        full_message += message;

        // checks for end of request \r\n\r\n
        if (string_exists (full_message, "\r\n\r\n")) {
            message = full_message;
            DEBUG << "CLRFCLRF found, processing full request: " << create_preview (string_to_literal (message))
                  << ENDL;

            // stores all header lines + request line
            std::vector<std::string> headers;

            // split headers if any
            int t = 0;
            for (int i = 0; i < message.size (); i++) {
                if (message[i] == '\r' && i < message.size () - 1 && message[i + 1] == '\n') {
                    std::string header = message.substr (t, i);
                    headers.push_back (header);
                    t = i + 2;
                }
            }

            // split up request line by spaces
            std::vector<std::string> request_line = string_tokenize (headers.at (0), ' ');

            // Check if GET message meets assignment requirements and get filepath
            //  (eg. space separated line starts with GET, then has filepath, then
            //  specifies http 1.0)
            // Also support 1.1 requests for this assignment since its a simple file request
            //  and makes it easier to test on Safari but responses are sent back in 1.0
            if (request_line.at (0).substr (0, 3) == "GET" &&
            (request_line.at (2).substr (0, 8) == "HTTP/1.0" || request_line.at (2).substr (0, 8) == "HTTP/1.1")) {
                filepath = request_line.at (1);

                // clean up filepath a bit
                filepath = remove_padding (filepath, '/', true, false);
                // filepath = remove_padding(filepath, ' ', false, true);

                DEBUG << "Request line parsed succesfully, HTTP/1.0 requesting " << filepath << ENDL;

                filename  = filepath;
                keepGoing = false;
                return 200;
            }

            // if GET message is valid but http version is not 1.0 like assignment specifies
            else if ((request_line.at (0).substr (0, 3) == "GET" && request_line.at (2).substr (0, 5) == "HTTP/")) {
                DEBUG << "Request line parsed succesfully, unsupported HTTP version. Preparing to "
                         "return 505"
                      << ENDL;
                keepGoing = false;
                return 505;
            }

            // If request line can't be parsed than we send back Bad-Request
            else {
                DEBUG << "Request line was not parsed succesfully, preparing to return 400" << ENDL;
                keepGoing = false;
                return 400;
            }
        }

        // \r\n\r\n not found
        else {
            DEBUG << "Message did not contain CLRFCLRF, waiting for more messages from this connection" << ENDL;

            // this is implicit but it's restated for clarity
            keepGoing = true;
        }
    }

    return quitProgram;
}

int processConnection (int sockFd) {
    // file name being requested, will be parsed
    std::string filename;
    std::string full_message = "";

    // get status code from request
    int status_code = readRequest (sockFd, filename);

    switch (status_code) {
    // request OK
    case (200): send200 (sockFd, filename); break;

    // bad request
    case (400): send400 (sockFd); break;

    // http version not supported
    // this needed because most browsers send http/1.1 by default and this
    // assignment specifies only http/1.0
    case (505): send505 (sockFd); break;

    // file not found
    case (404): send404 (sockFd); break;
    }

    return 0;
}

// **************************************************************************************
// * main()
// * - Sets up the sockets and accepts new connection until processConnection()
// returns 1
// **************************************************************************************

int main (int argc, char* argv[]) {

    // catch SIGINT and send to sig_handler
    signal (SIGINT, sig_handler);

    // Obtain a pseudo random number to seed the dist generator
    std::random_device rd;
    std::mt19937 eng (rd ());

    // ********************************************************************
    // * Process the command line arguments
    // ********************************************************************
    Argparser parser (argc, argv);
    parser.add_option ('d', true, false, 1, 1);
    parser.parse ();
    std::vector<int> arg_values = parser.get_values_int ('d');

    if (arg_values.size () == 0) {
        LOG_LEVEL = 0;
    } else {
        LOG_LEVEL = arg_values.at (0);
    }

    // *******************************************************************
    // * Creating the inital socket is the same as in a client.
    // ********************************************************************
    int listenFd = -1;
    // Call socket() to create the socket you will use for lisening.
    if ((listenFd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
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
    memset (&servaddr, 0, sizeof (servaddr));

    // *** assign 3 fields in the servadd struct sin_family, sin_addr.s_addr and
    // sin_port
    // *** the value your port can be any value > 1024.
    servaddr.sin_family      = AF_INET;              // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;           // listen to any address
    servaddr.sin_port        = htons (DEFAULT_PORT); // htons converts the int to network byte order

    // ********************************************************************
    // * Binding configures the socket with the parameters we have
    // * specified in the servaddr structure.  This step is implicit in
    // * the connect() call, but must be explicitly listed for servers.
    // ********************************************************************
    bool bindSuccesful = false;

    std::uniform_int_distribution<> distr (1025, 65535);
    while (!bindSuccesful) {
        // ** Call bind()
        // You may have to call bind multiple times if another process is
        // already using the port your program selects.
        if (bind (listenFd, (struct sockaddr*)&servaddr, sizeof (servaddr)) < 0) {
            DEBUG << "Bind failed" << ENDL;
            servaddr.sin_port = htons (distr (eng));
        } else {
            bindSuccesful = true;
            DEBUG << "Bind succesfull" << ENDL;
        }
    }
    std::cout << "Using port: " << ntohs (servaddr.sin_port) << std::endl;
    // *** DON'T FORGET TO PRINT OUT WHAT PORT YOUR SERVER PICKED SO YOU KNOW
    // HOW TO CONNECT.

    // ********************************************************************
    // * Setting the socket to the listening state is the second step
    // * needed to being accepting connections.  This creates a queue for
    // * connections and starts the kernel listening for connections.
    // ********************************************************************
    int listenQueueLength = 1;
    // ** Cal listen()
    if (listen (listenFd, listenQueueLength) < 0) {
        FATAL << "Listen() failed" << std::endl << ENDL;
        close (listenFd);
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

        // Call the accept() call to check the listening queue for connection
        // requests. If a client has already tried to connect accept() will
        // complete the connection and return a file descriptor that you can
        // read from and write to. If there is no connection waiting accept()
        // will block and not return until there is a connection.
        // ** call accept()
        int addrlen = sizeof (servaddr);
        DEBUG << "Calling accept(" << listenFd << ", " << (struct sockaddr*)&servaddr << ", "
              << (socklen_t*)&addrlen << ENDL;
        int new_socket = accept (listenFd, (struct sockaddr*)&servaddr, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            FATAL << "Accept() failed" << ENDL;
            close (listenFd);
            return -1;
        }

        DEBUG << "Connection accepted" << ENDL;

        // Now we have a connection, so you can call processConnection() to do
        // the work.
        quitProgram = processConnection (new_socket);

        close (new_socket);
    }

    close (listenFd);
}

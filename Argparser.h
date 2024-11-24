// Created by Cristian Madrazo
// Argparser library headers to help parse command line arguments
// Version 1.0

#ifndef ARGPARSER
#define ARGPARSER

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept> // For std::runtime_error

// constant defining max arguments for a flag with "unlimited" arguments
const int MAX_ARGS = 30;

struct Option {
    // a unique letter to identify this option
    char flag;

    // does this flag allow arguments, used during creating an option
    bool args_allowed;

    // minimum number of arguments
    int min;

    // maximum number of arguments
    int max;

    // if this flag required, will throw exception if parse() is called and user did not use it
    bool required;

    // stores value true if flag is present during parsing
    bool on;

    // stores arguments related to this flag after parsing
    std::vector<std::string> values;

    // addes a value to values vector
    void add_value(std::string);

    // Constructor
    Option(char, bool, bool, int, int);

    // Destructor
    ~Option();
};

class Argparser {
private:
    // number of arguments
    int argc;

    // arguments
    std::vector<std::string> argv;

    // options, and their minimum and maximum
    std::vector<Option> options;

    // Verifies that everything is as expected after parsing
    bool verify();


public:
    // Constructor
    Argparser(int, char *[]);

    // Destructor
    ~Argparser();

    // Prints the nth argument
    void print(int n);

    // Add a flag option
    void add_option(char flag, bool args_allowed, bool required, int max_if_args, int min_if_args);

    // Parses clargs
    void parse();

    // Displays all parsed flags/info
    void display_all();

    // Returns all values for an option as integers
    std::vector<int> get_values_int(char flag);

    // Returns all values for an option as strings
    std::vector<std::string> get_values_string(char flag);
};

#endif

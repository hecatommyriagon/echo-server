// Created by Cristian Madrazo
// Argparser library implementations to help parse command line arguments
// Version 1.0

#include "Argparser.h"

// Constructor
Option::Option(char flag, bool args, bool required, int max = -1, int min = -1) {
    this->flag = flag;
    this->required = required;
    this->max = max;
    this->min = min;
    this->on = false; // set during parsing
    this->args_allowed = args;

    // if args allowed is false, then min max are -1 regardless
    if (!args) {
        min = -1; 
        max = -1;
    }
}

// Destructor
Option::~Option(){
}

// addes a value to values vector
void Option::add_value(std::string val) {
    values.push_back(val);
}


// Constructor implementation
Argparser::Argparser(int c, char *v[]) {

    // copy number of arguments
    argc = c;
    
    // Copy each argument
    for (int i = 0; i < c; ++i) {
        std::string arg = v[i];
        argv.push_back(arg);
    }
}

// Destructor implementation
Argparser::~Argparser(){
}

// Prints the nth argument
void Argparser::print(int n) {

    // check that n is a valid argument index
    if (n < 0 || n >= argc) {
        std::cerr << n << " is not a valid argument index.\n";
    }
    // prints argument at index n
    else {
        std::cout << argv.at(n) << std::endl;
    }
}

// Add a flag option
void Argparser::add_option(char opt, bool args_allowed, bool required, int max = -1, int min = -1) {

    if (min > max || max <= 0 || min < 0) {
        throw std::runtime_error("Invalid argument min/max argument settings for flag" + std::string(1, opt));
    }
    
    for (int i = 0; i < options.size(); i++) {
        if (options.at(i).flag == opt) {
            throw std::runtime_error("Flag option -" + std::string(1, opt) + " already defined");
        }
    }

    Option new_option(opt, args_allowed, required, max, min);
    options.push_back(new_option);

}

// Parses clargs
void Argparser::parse(){
    // iterate through all arguments except 0 (program name)
    for (int i = 1; i < argv.size(); i++) {

        bool valid_arg = false;

        // check if current argument is a flag
        if (argv.at(i)[0] == '-') {

            // check if it is one of the user specified options
            for (int j = 0; j < options.size(); j++){
                if (argv.at(i) == "-" + std::string(1, options.at(j).flag)) {

                    valid_arg = true;

                    int skip = 0;

                    // check if this flag accepts arguments
                    if (options.at(j).args_allowed == true) {

                        // set on value to true
                        options.at(j).on = true;

                        // if there are more arguments
                        if (i + 1 < argv.size()) {

                            // iterate through next arguments till we hit max num args or another flag or end of args
                            for (int k = 1; k <= options.at(j).max; k++) {
                                int l = i + k;

                                // check if end of args
                                if (l >= argv.size()) {
                                    break;
                                }

                                // check if we hit another flag
                                if (argv.at(l)[0] == '-') {
                                    break;
                                }

                                // otherwise add as value to current flag
                                options.at(j).add_value(argv.at(l));
                                skip++;
                            }
                        }
                        
                    }

                    // if does not accept arguments then we set its value to be 1 for true
                    else {  
                        options.at(j).on = true;
                    }

                    // end loop since we found a match, and skip iterating through the next few args if 
                    // they applied to this flag
                    i += skip;
                    break;
                }
            }
        }

        // throw exception if invalid argument
        if (valid_arg == false) {
            throw std::runtime_error("Invalid argument " + argv.at(i));
        }
    }
    verify();
}

// Displays all parsed flags/info
void Argparser::display_all() {

    std::cout << "\n------------------------";
    std::cout << "\nDisplaying Flag Information\n\n";
    std::cout << "\n\tUser Defined Options: " << options.size();
    // iterate through all user defined options
    for (Option option : options) {

        std::cout << "\n\tFlag -" + std::string(1, option.flag);

        // required message
        std::cout << "\n\t\tRequired: ";

        if (option.required == true) {

            std::cout << "YES";
        }

        else {
        
            std::cout << "NO";
        }

        // args allowed message
        std::cout << "\n\t\tArgs Allowed: ";

        if (option.args_allowed == true) {

            std::cout << "YES";
        }

        else {
        
            std::cout << "NO";
        }

        // on message
        std::cout << "\n\t\tOn: ";

        if (option.on == true) {
        
            std::cout << "YES";
        }

        else {
        
            std::cout << "NO";
        }
        
        // arguments message
        std::cout << "\n\t\tArg Values: ";
        if (option.values.size() == 0) {
            std::cout << "None";
        }
        else {
            
            // iterate through all values and print them
            for (std::string val : option.values) {
                std::cout << "\n\t\t\t" << val;
            }

            std::cout << std::endl;
        }

    }

    std::cout << "\n------------------------\n";
}

bool Argparser::verify() {

    // iterate through all user defined options
    for (Option option : options) {

        // if this flag is required but not used throw error
        if (option.required == true) {

            if (option.on == false) {

                throw std::runtime_error("Flag -" + std::string(1, option.flag) + " is required but not used");
                return false;
            }
        }
    
        // if this flag does not meet min or max argument values
        else if (option.args_allowed == true && option.on == true) {

            if (option.values.size() < option.min) {

                throw std::runtime_error("Flag -" + std::string(1, option.flag) + " does not have enough arguments");
                return false;
            }

            if (option.values.size() > option.max) {

                throw std::runtime_error("Flag -" + std::string(1, option.flag) + " as too many arguments");
                return false;
            }
        } 
    }

    return true;
}

std::vector<int> Argparser::get_values_int(char flag) {

    // iterate through all user defined options, cast to int and add to vector
    for (Option option : options) {
        
        // if match, collect all values cast to int and return the new vector
        if (flag == option.flag) {
            std::vector<int> return_values;

            for (std::string arg : option.values) {
                return_values.push_back(std::stoi(arg));
            }

            return return_values;
        }
    }

    throw std::runtime_error("Option -" + std::string(1, flag) + " is not a user-specified option");

}

std::vector<std::string> Argparser::get_values_string(char flag) {

    // iterate through all user defined options, cast to int and add to vector
    for (Option option : options) {
        
        // if match, collect all values cast to int and return the new vector
        if (flag == option.flag) {

            return option.values;
        }
    }

    throw std::runtime_error("Option -" + std::string(1, flag) + " is not a user-specified option");

}

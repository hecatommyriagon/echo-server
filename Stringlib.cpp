/**
 * @file Stringlib.cpp
 * @author Cristian Madrazo
 * @brief Implementations for Stringlib
 * @version 1.1
 *
 */

#include "Stringlib.h"
#include <cstring>
#include <iostream>
#include <vector>

unsigned long string_length (const std::string STR) {
    unsigned long result = -1;
    result               = STR.length (); // set result to the length of the string
    return result;
}

char string_char_at (const std::string STR, const int IDX) {
    // Set result to the character of a string at a given index
    return STR.at (IDX);
}

std::string string_append (const std::string LEFT, const std::string RIGHT) {
    // set result to the concatenation of strings LEFT and RIGHT
    std::string result = LEFT + RIGHT;
    return result;
}

std::string string_insert (const std::string STR, const std::string TO_INSERT, const int IDX) {
    // set result to the result of inserting a string into another
    std::string result = STR;
    result             = STR.substr (0, IDX) + TO_INSERT + STR.substr (IDX, STR.length ());

    return result;
}

size_t string_find (const std::string STR, const char C) {
    // set result to the index of the first occurrence of the character
    size_t result = STR.find_first_of (C);
    return result;
}

bool string_exists (const std::string STR, const std::string SBSTR) {
    // return true if substring is found in str

    // call string_find
    size_t pos = STR.find (SBSTR);

    if (pos == std::string::npos) {
        return false;
    }

    else {
        return true;
    }

    return 0;
}


std::string string_substring (const std::string STR, const int IDX, const int LEN) {
    // set result to be a substring starting at index of given length
    std::string result = STR;
    result             = result.substr (IDX, LEN);
    return result;
}

std::string string_replace (const std::string STR, const std::string TEXT_TO_REPLACE, const std::string REPLACE_WITH) {
    // set result to be the string with the given text replaced
    std::string result = STR;
    try {
        result = result.replace (STR.find (TEXT_TO_REPLACE), TEXT_TO_REPLACE.length (), REPLACE_WITH);
    } catch (std::out_of_range) {
        // if TEXT_TO_REPLACE is not found in STR
        // reset result
        result = STR;
    }
    return result;
}

std::string string_first_word (const std::string STR) {
    // set result to the first word from the string
    std::string result = STR;
    result             = string_nth_word (result, 1);
    return result;
}

std::string string_remove_first_word (const std::string STR) {
    // set result to be the string with the first word removed
    std::string result = STR;
    bool onlyOneWord   = true;

    for (int i = 0; i < (int)STR.size () - 1; i++) {
        if (result[i] == ' ') {
            onlyOneWord = false;
            break;
        }
    }

    if (onlyOneWord == true) {
        return "";
    }

    // sets firstWord to first word in string
    std::string firstWord = string_first_word (STR);

    // deletes firstWord from string
    // result = string_replace(result, firstWord, "");
    result = string_replace (result, firstWord + " ", "");

    return result;
}

std::string string_second_word (const std::string STR) {
    // set result to be the second word from the string
    std::string result = STR;
    result             = string_nth_word (result, 2);

    return result;
}

std::string string_third_word (const std::string STR) {
    // set result to be the third word from the string
    std::string result = STR;
    result             = string_nth_word (result, 3);

    return result;
}

std::string string_nth_word (const std::string STR, const int N) {
    // set result to be the nth word from the string
    std::string result = STR;
    std::vector<std::string> word;
    int k = -1;

    if (result == "") {
        return "";
    }

    // iterates through letters
    for (int i = 0; i < (int)result.size (); i++) {
        if (result[i] == ' ' || k == -1) {
            word.push_back ("");
            k++;
        }
        if (result[i] != ' ') {
            word[k].push_back (result[i]);
        }
    }

    return word[N - 1];
}

std::vector<std::string> string_tokenize (const std::string STR, const char DELIMINATOR) {
    // split the string by the given deliminator
    std::string result = STR;
    std::vector<std::string> word;
    int k = -1;

    if (result == "") {
        word.push_back ("");
        return word;
    }

    // iterates through letters
    for (int i = 0; i < (int)result.size (); i++) {
        if (result[i] == DELIMINATOR || k == -1) {
            word.push_back ("");
            k++;
        }

        if (result[i] != DELIMINATOR) {
            word[k].push_back (result[i]);
        }
    }

    return word;
}

// set result to be the string with all instances of TARGET replaced
std::string string_substitute (const std::string STR, const char TARGET, const char REPLACEMENT) {
    std::string result = STR;

    for (int i = 0; i < (int)STR.size (); i++) {
        if (result[i] == TARGET) {
            result[i] = REPLACEMENT;
        }
    }

    return result;
}

// convert all characters to lower case
std::string string_to_lower (const std::string STR) {
    std::string result = STR;
    for (int i = 0; i < (int)STR.size (); i++) {
        result[i] = tolower (result[i]);
    }
    return result;
}

// convert all characters to upper case
std::string string_to_upper (const std::string STR) {
    std::string result = STR;
    for (int i = 0; i < (int)STR.size (); i++) {
        result[i] = toupper (result[i]);
    }
    return result;
}

// Compare LHS and RHS
int string_compare (const std::string LHS, const std::string RHS) {
    int result  = 0;
    char* left  = new char[(int)LHS.size ()];
    char* right = new char[(int)RHS.size ()];
    for (int i = 0; i < (int)LHS.size (); i++) {
        left[i] = LHS[i];
    }

    for (int i = 0; i < (int)RHS.size (); i++) {
        right[i] = RHS[i];
    }

    result = std::strcmp (left, right);

    return result / abs (result);
}

// Turn a string into a string literal
std::string string_to_literal (const std::string str) {

    std::string literal_str = "";
    int i                   = 1;
    for (char u : str) {
        if (i < str.size ()) {

            switch (u) {

            case '\n': literal_str = literal_str + "\\n"; break;

            case '\t': literal_str = literal_str + "\\t"; break;

            case '\r': literal_str = literal_str + "\\r"; break;

            default: literal_str = literal_str + u; break;
            }
        }
    }

    return literal_str;
}

// remove pad characters from front and/or back of string
std::string remove_padding (const std::string str, char pad, bool front, bool back) {

    // string to be returned
    std::string return_string = "";

    // if front padding on
    if (front) {
        for (int i = 0; i < str.size (); i++) {

            // if we found nonmatch, split str and stop loop
            if (str.at (i) != pad) {
                return_string = return_string + str.substr (i, str.size () - i + 1);
                break;
            }
        }
    }

    // if front padding not on
    else {
        return_string = str;
    }

    if (back) {

        for (int i = return_string.size () - 1; i >= 0; i--) {

            // if we found nonmatch, split str and stop loop
            if (return_string.at (i) != pad) {
                return_string = return_string.substr (0, i + 1);
                break;
            }
        }
    }

    if (!front && !back) {
        return str;
    }

    return return_string;
}

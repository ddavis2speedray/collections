//
// Created by Donald Lamar Davis II on 4/22/15.
//

#ifndef COLLECTIONS_COMMANDLINEFLAGS_H
#define COLLECTIONS_COMMANDLINEFLAGS_H

#include <stddef.h>
#include <string>

#include "CommonDefines.h"

namespace collections {
    // Parses a string as a command line flag.  The string should have
    // the format "--flag=value".  When def_optional is true, the "=value"
    // part can be omitted.
    //
    // Returns the value of the flag, or NULL if the parsing failed.
    const char* ParseFlagValue(const char* str,
                               const char* flag,
                               bool def_optional) {
        // str and flag must not be NULL.
        if (str == NULL || flag == NULL) return NULL;

        // The flag must start with "--".
        const std::string flag_str = std::string("--") + flag;
        const size_t flag_len = flag_str.length();
        if (strncmp(str, flag_str.c_str(), flag_len) != 0) return NULL;

        // Skips the flag name.
        const char* flag_end = str + flag_len;

        // When def_optional is true, it's OK to not have a "=value" part.
        if (def_optional && (flag_end[0] == '\0')) {
            return flag_end;
        }

        // If def_optional is true and there are more characters after the
        // flag name, or if def_optional is false, there must be a '=' after
        // the flag name.
        if (flag_end[0] != '=') return NULL;

        // Returns the string after "=".
        return flag_end + 1;
    }
    // Parses a string for a string flag, in the form of
    // "--flag=value".
    //
    // On success, stores the value of the flag in *value, and returns
    // true.  On failure, returns false without changing *value.
    bool ParseStringFlag(const char* str, const std::string& flag, std::string* value) {
        // Gets the value of the flag as a string.
        const char* const value_str = ParseFlagValue(str, flag.c_str(), false);

        // Aborts if the parsing failed.
        if (value_str == NULL) return false;

        // Sets *value to the value of the flag.
        *value = value_str;
        return true;
    }
    // Parses a string for a bool flag, in the form of either
    // "--flag=value" or "--flag".
    //
    // In the former case, the value is taken as true as long as it does
    // not start with '0', 'f', or 'F'.
    //
    // In the latter case, the value is taken as true.
    //
    // On success, stores the value of the flag in *value, and returns
    // true.  On failure, returns false without changing *value.
    bool ParseBoolFlag(const char* str, const std::string& flag, bool* value) {
        // Gets the value of the flag as a string.
        const char* const value_str = ParseFlagValue(str, flag.c_str(), true);

        // Aborts if the parsing failed.
        if (value_str == NULL) return false;

        // Converts the string value to a bool.
        *value = !(*value_str == '0' || *value_str == 'f' || *value_str == 'F');
        return true;
    }

    // Parses a string for an Int32 flag, in the form of
    // "--flag=value".
    //
    // On success, stores the value of the flag in *value, and returns
    // true.  On failure, returns false without changing *value.
    bool ParseInt32Flag(const char* str, const std::string& flag, Int32* value) {
        // Gets the value of the flag as a string.
        const char* const value_str = ParseFlagValue(str, flag.c_str(), false);

        // Aborts if the parsing failed.
        if (value_str == NULL) return false;

        // Sets *value to the value of the flag.
        return ParseInt32("The value of flag --"+flag, value_str, value);
    }

}

#endif //COLLECTIONS_COMMANDLINEFLAGS_H

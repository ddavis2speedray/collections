//
// Created by Donald Lamar Davis II on 5/4/15.
//

#ifndef COLLECTIONS_JSONSCHEMA_H
#define COLLECTIONS_JSONSCHEMA_H


#include <mongo/client/dbclient.h>

namespace collections {
    class JsonSchema {
    public:
        explicit JsonSchema(const mongo::StringData& str);
        /*
        * Notation: All-uppercase symbols denote non-terminals; all other
        * symbols are literals.
        */

        /*
         * VALUE :
         *     STRING
         *   | NUMBER
         *   | NUMBERINT
         *   | NUMBERLONG
         *   | OBJECT
         *   | ARRAY
         *
         *   | true
         *   | false
         *   | null
         *   | undefined
         *
         *   | NaN
         *   | Infinity
         *   | -Infinity
        */
    private:
        mongo::Status value(const mongo::StringData& fieldName, mongo::BSONObjBuilder&);
        /*
         * OBJECT :
         *     {}
         *   | { MEMBERS }
         *   | SPECIALOBJECT
         *
         * MEMBERS :
         *     PAIR
         *   | PAIR , MEMBERS
         *
         * PAIR :
         *     FIELD : VALUE
         *
         */
    public:
        mongo::Status object(const mongo::StringData& fieldName, mongo::BSONObjBuilder&, bool subObj=true);
        mongo::Status parse(mongo::BSONObjBuilder& builder);
        bool isArray();
    private:
        /*
         * ARRAY :
         *     []
         *   | [ ELEMENTS ]
         *
         * ELEMENTS :
         *     VALUE
         *   | VALUE , ELEMENTS
         */
        mongo::Status array(const mongo::StringData& fieldName, mongo::BSONObjBuilder&, bool subObj=true);
        /*
         * NUMBER :
         *
         * NOTE: Number parsing is based on standard library functions, not
         * necessarily on the JSON numeric grammar.
         *
         * Number as value - strtoll and strtod
         * Date - strtoll
         * Timestamp - strtoul for both timestamp and increment and '-'
         * before a number explicity disallowed
         */
        mongo::Status number(const mongo::StringData& fieldName, mongo::BSONObjBuilder&);

        /*
         * FIELD :
         *     STRING
         *   | [a-zA-Z$_] FIELDCHARS
         *
         * FIELDCHARS :
         *     [a-zA-Z0-9$_]
         *   | [a-zA-Z0-9$_] FIELDCHARS
         */
        mongo::Status field(std::string* result);

        /*
         * STRING :
         *     " "
         *   | ' '
         *   | " CHARS "
         *   | ' CHARS '
         */
        mongo::Status quotedString(std::string* result);

        /*
         * CHARS :
         *     CHAR
         *   | CHAR CHARS
         *
         * Note: " or ' may be allowed depending on whether the string is
         * double or single quoted
         *
         * CHAR :
         *     any-Unicode-character-except-"-or-'-or-\-or-CONTROLCHAR
         *   | \"
         *   | \'
         *   | \\
         *   | \/
         *   | \b
         *   | \f
         *   | \n
         *   | \r
         *   | \t
         *   | \v
         *   | \u HEXDIGIT HEXDIGIT HEXDIGIT HEXDIGIT
         *   | \any-Unicode-character-except-x-or-[0-9]
         *
         * HEXDIGIT : [0..9a..fA..F]
         *
         * per http://www.ietf.org/rfc/rfc4627.txt, control characters are
         * (U+0000 through U+001F).  U+007F is not mentioned as a control
         * character.
         * CONTROLCHAR : [0x00..0x1F]
         *
         * If there is not an error, result will contain a null terminated
         * string, but there is no guarantee that it will not contain other
         * null characters.
         */
        mongo::Status chars(std::string* result, const char* terminalSet, const char* allowedSet=NULL);

        /**
         * Converts the two byte Unicode code point to its UTF8 character
         * encoding representation.  This function returns a string because
         * UTF8 encodings for code points from 0x0000 to 0xFFFF can range
         * from one to three characters.
         */
        std::string encodeUTF8(unsigned char first, unsigned char second) const;

        /**
         * @return true if the given token matches the next non whitespace
         * sequence in our buffer, and false if the token doesn't match or
         * we reach the end of our buffer.  Do not update the pointer to our
         * buffer (same as calling readTokenImpl with advance=false).
         */
        inline bool peekToken(const char* token);

        /**
         * @return true if the given token matches the next non whitespace
         * sequence in our buffer, and false if the token doesn't match or
         * we reach the end of our buffer.  Updates the pointer to our
         * buffer (same as calling readTokenImpl with advance=true).
         */
        inline bool readToken(const char* token);

        /**
         * @return true if the given token matches the next non whitespace
         * sequence in our buffer, and false if the token doesn't match or
         * we reach the end of our buffer.  Do not update the pointer to our
         * buffer if advance is false.
         */
        bool readTokenImpl(const char* token, bool advance=true);

        /**
         * @return true if the next field in our stream matches field.
         * Handles single quoted, double quoted, and unquoted field names
         */
        bool readField(const mongo::StringData& field);

        /**
         * @return true if matchChar is in matchSet
         * @return true if matchSet is NULL and false if it is an empty string
         */
        bool match(char matchChar, const char* matchSet) const;

        /**
         * @return true if every character in the string is a hex digit
         */
        bool isHexString(const mongo::StringData&) const;

        /**
         * @return true if every character in the string is a valid base64
         * character
         */
        bool isBase64String(const mongo::StringData&) const;

        /**
         * @return FailedToParse status with the given message and some
         * additional context information
         */
        mongo::Status parseError(const mongo::StringData& msg);
    public:
        inline int offset() { return (_input - _buf); }

    private:
        /*
         * _buf - start of our input buffer
         * _input - cursor we advance in our input buffer
         * _input_end - sentinel for the end of our input buffer
         *
         * _buf is the null terminated buffer containing the JSON string we
         * are parsing.  _input_end points to the null byte at the end of
         * the buffer.  strtoll, strtol, and strtod will access the null
         * byte at the end of the buffer because they are assuming a c-style
         * string.
         */
        const char* const _buf;
        const char* _input;
        const char* const _input_end;

    };
    mongo::BSONObj fromSchemaJson(const std::string& str);
    mongo::BSONObj fromSchemaJson(const char* jsonString, int* len);
}
#endif //COLLECTIONS_JSONSCHEMA_H



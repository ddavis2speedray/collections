//
// Created by Donald Lamar Davis II on 5/4/15.
//

# define verify(expression) MONGO_verify(expression)

#include <mongo/util/base64.h>
#include "JsonSchema.h"
#include "mongo/util/hex.h"

using namespace mongo;
using namespace std;

using boost::scoped_ptr;

namespace collections {
#if 0
#define MONGO_JSON_DEBUG(message) log() << "JSON DEBUG @ " << __FILE__\
    << ":" << __LINE__ << " " << __FUNCTION__ << ": " << message << endl;
#else
#define MONGO_JSON_DEBUG(message)
#endif

#define ALPHA "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define DIGIT "0123456789"
#define CONTROL "\a\b\f\n\r\t\v"
#define JOPTIONS "gims"

// Size hints given to char vectors
    enum {
        FIELD_RESERVE_SIZE = 4096,
        STRINGVAL_RESERVE_SIZE = 4096
    };

    static const char* LBRACE = "{",
            *RBRACE = "}",
            *LBRACKET = "[",
            *RBRACKET = "]",
            *LPAREN = "(",
            *RPAREN = ")",
            *COLON = ":",
            *COMMA = ",",
            *FORWARDSLASH = "/",
            *SINGLEQUOTE = "'",
            *DOUBLEQUOTE = "\"";

    JsonSchema::JsonSchema(const StringData& str)
            : _buf(str.rawData())
            , _input(_buf)
            , _input_end(_input + str.size())
    {}

    Status JsonSchema::parseError(const StringData& msg) {
        std::ostringstream ossmsg;
        ossmsg << msg;
        ossmsg << ": offset:";
        ossmsg << offset();
        ossmsg << " of:";
        ossmsg << _buf;
        return Status(ErrorCodes::FailedToParse, ossmsg.str());
    }

    Status JsonSchema::parse(BSONObjBuilder& builder) {
        return isArray() ? array("UNUSED", builder, false) : object("UNUSED", builder, false);
    }

    Status JsonSchema::object(const StringData& fieldName, BSONObjBuilder& builder, bool subObject) {
        MONGO_JSON_DEBUG("fieldName: " << fieldName);
        if (!readToken(LBRACE)) {
            return parseError("Expecting '{'");
        }

        // Empty object
        if (readToken(RBRACE)) {
            if (subObject) {
                BSONObjBuilder empty(builder.subobjStart(fieldName));
                empty.done();
            }
            return Status::OK();
        }

        // Special object
        std::string firstField;
        firstField.reserve(FIELD_RESERVE_SIZE);
        Status ret = field(&firstField);
        if (ret != Status::OK()) {
            return ret;
        }

        if (firstField == "$ref") {
            if (!subObject) {
                return parseError("Reserved field name in base object: $ref");
            }
        }
        else { // firstField != <reserved field name>
            // Normal object

            // Only create a sub builder if this is not the base object
            BSONObjBuilder* objBuilder = &builder;
            scoped_ptr<BSONObjBuilder> subObjBuilder;
            if (subObject) {
                subObjBuilder.reset(new BSONObjBuilder(builder.subobjStart(fieldName)));
                objBuilder = subObjBuilder.get();
            }

            if (!readToken(COLON)) {
                return parseError("Expecting ':'");
            }
            Status valueRet = value(firstField, *objBuilder);
            if (valueRet != Status::OK()) {
                return valueRet;
            }
            while (readToken(COMMA)) {
                std::string fieldName;
                fieldName.reserve(FIELD_RESERVE_SIZE);
                Status fieldRet = field(&fieldName);
                if (fieldRet != Status::OK()) {
                    return fieldRet;
                }
                if (!readToken(COLON)) {
                    return parseError("Expecting ':'");
                }
                Status valueRet = value(fieldName, *objBuilder);
                if (valueRet != Status::OK()) {
                    return valueRet;
                }
            }
        }
        if (!readToken(RBRACE)) {
            return parseError("Expecting '}' or ','");
        }
        return Status::OK();
    }

    Status JsonSchema::value(const StringData& fieldName, BSONObjBuilder& builder) {
        MONGO_JSON_DEBUG("fieldName: " << fieldName);
        if (peekToken(LBRACE)) {
            Status ret = object(fieldName, builder);
            if (ret != Status::OK()) {
                return ret;
            }
        }
        else if (peekToken(LBRACKET)) {
            Status ret = array(fieldName, builder);
            if (ret != Status::OK()) {
                return ret;
            }
        }
        else if (peekToken(DOUBLEQUOTE) || peekToken(SINGLEQUOTE)) {
            std::string valueString;
            valueString.reserve(STRINGVAL_RESERVE_SIZE);
            Status ret = quotedString(&valueString);
            if (ret != Status::OK()) {
                return ret;
            }
            builder.append(fieldName, valueString);
        }
        else if (readToken("true")) {
            builder.append(fieldName, true);
        }
        else if (readToken("false")) {
            builder.append(fieldName, false);
        }
        else if (readToken("null")) {
            builder.appendNull(fieldName);
        }
        else if (readToken("undefined")) {
            builder.appendUndefined(fieldName);
        }
        else if (readToken("NaN")) {
            builder.append(fieldName, std::numeric_limits<double>::quiet_NaN());
        }
        else if (readToken("Infinity")) {
            builder.append(fieldName, std::numeric_limits<double>::infinity());
        }
        else if (readToken("-Infinity")) {
            builder.append(fieldName, -std::numeric_limits<double>::infinity());
        }
        else {
            Status ret = number(fieldName, builder);
            if (ret != Status::OK()) {
                return ret;
            }
        }
        return Status::OK();
    }

    Status JsonSchema::array(const StringData& fieldName, BSONObjBuilder& builder, bool subObject) {
        MONGO_JSON_DEBUG("fieldName: " << fieldName);
        uint32_t index(0);
        if (!readToken(LBRACKET)) {
            return parseError("Expecting '['");
        }

        BSONObjBuilder* arrayBuilder = &builder;
        scoped_ptr<BSONObjBuilder> subObjBuilder;
        if (subObject) {
            subObjBuilder.reset(new BSONObjBuilder(builder.subarrayStart(fieldName)));
            arrayBuilder = subObjBuilder.get();
        }

        if (!peekToken(RBRACKET)) {
            do {
                Status ret = value(builder.numStr(index), *arrayBuilder);
                if (ret != Status::OK()) {
                    return ret;
                }
                index++;
            } while (readToken(COMMA));
        }
        arrayBuilder->done();
        if (!readToken(RBRACKET)) {
            return parseError("Expecting ']' or ','");
        }
        return Status::OK();
    }

    Status JsonSchema::number(const StringData& fieldName, BSONObjBuilder& builder) {
        char* endptrll;
        char* endptrd;
        long long retll;
        double retd;

        // reset errno to make sure that we are getting it from strtod
        errno = 0;
        // SERVER-11920: We should use parseNumberFromString here, but that function requires that
        // we know ahead of time where the number ends, which is not currently the case.
        retd = strtod(_input, &endptrd);
        // if pointer does not move, we found no digits
        if (_input == endptrd) {
            return parseError("Bad characters in value");
        }
        if (errno == ERANGE) {
            return parseError("Value cannot fit in double");
        }
        // reset errno to make sure that we are getting it from strtoll
        errno = 0;
        // SERVER-11920: We should use parseNumberFromString here, but that function requires that
        // we know ahead of time where the number ends, which is not currently the case.
        retll = strtoll(_input, &endptrll, 10);
        if (endptrll < endptrd || errno == ERANGE) {
            // The number either had characters only meaningful for a double or
            // could not fit in a 64 bit int
            MONGO_JSON_DEBUG("Type: double");
            builder.append(fieldName, retd);
        }
        else if (retll == static_cast<int>(retll)) {
            // The number can fit in a 32 bit int
            MONGO_JSON_DEBUG("Type: 32 bit int");
            builder.append(fieldName, static_cast<int>(retll));
        }
        else {
            // The number can fit in a 64 bit int
            MONGO_JSON_DEBUG("Type: 64 bit int");
            builder.append(fieldName, retll);
        }
        _input = endptrd;
        if (_input >= _input_end) {
            return parseError("Trailing number at end of input");
        }
        return Status::OK();
    }

    Status JsonSchema::field(std::string* result) {
        MONGO_JSON_DEBUG("");
        if (peekToken(DOUBLEQUOTE) || peekToken(SINGLEQUOTE)) {
            // Quoted key
            // TODO: make sure quoted field names cannot contain null characters
            return quotedString(result);
        }
        else {
            // Unquoted key
            // 'isspace()' takes an 'int' (signed), so (default signed) 'char's get sign-extended
            // and therefore 'corrupted' unless we force them to be unsigned ... 0x80 becomes
            // 0xffffff80 as seen by isspace when sign-extended ... we want it to be 0x00000080
            while (_input < _input_end &&
                   isspace(*reinterpret_cast<const unsigned char*>(_input))) {
                ++_input;
            }
            if (_input >= _input_end) {
                return parseError("Field name expected");
            }
            if (!match(*_input, ALPHA "_$")) {
                return parseError("First character in field must be [A-Za-z$_]");
            }
            return chars(result, "", ALPHA DIGIT "_$");
        }
    }

    Status JsonSchema::quotedString(std::string* result) {
        MONGO_JSON_DEBUG("");
        if (readToken(DOUBLEQUOTE)) {
            Status ret = chars(result, "\"");
            if (ret != Status::OK()) {
                return ret;
            }
            if (!readToken(DOUBLEQUOTE)) {
                return parseError("Expecting '\"'");
            }
        }
        else if (readToken(SINGLEQUOTE)) {
            Status ret = chars(result, "'");
            if (ret != Status::OK()) {
                return ret;
            }
            if (!readToken(SINGLEQUOTE)) {
                return parseError("Expecting '''");
            }
        }
        else {
            return parseError("Expecting quoted string");
        }
        return Status::OK();
    }

/*
 * terminalSet are characters that signal end of string (e.g.) [ :\0]
 * allowedSet are the characters that are allowed, if this is set
 */
    Status JsonSchema::chars(std::string* result, const char* terminalSet,
                             const char* allowedSet) {
        MONGO_JSON_DEBUG("terminalSet: " << terminalSet);
        if (_input >= _input_end) {
            return parseError("Unexpected end of input");
        }
        const char* q = _input;
        while (q < _input_end && !match(*q, terminalSet)) {
            MONGO_JSON_DEBUG("q: " << q);
            if (allowedSet != NULL) {
                if (!match(*q, allowedSet)) {
                    _input = q;
                    return Status::OK();
                }
            }
            if (0x00 <= *q && *q <= 0x1F) {
                return parseError("Invalid control character");
            }
            if (*q == '\\' && q + 1 < _input_end) {
                switch (*(++q)) {
                    // Escape characters allowed by the JSON spec
                    case '"':  result->push_back('"');  break;
                    case '\'': result->push_back('\''); break;
                    case '\\': result->push_back('\\'); break;
                    case '/':  result->push_back('/');  break;
                    case 'b':  result->push_back('\b'); break;
                    case 'f':  result->push_back('\f'); break;
                    case 'n':  result->push_back('\n'); break;
                    case 'r':  result->push_back('\r'); break;
                    case 't':  result->push_back('\t'); break;
                    case 'u': { //expect 4 hexdigits
                        // TODO: handle UTF-16 surrogate characters
                        ++q;
                        if (q + 4 >= _input_end) {
                            return parseError("Expecting 4 hex digits");
                        }
                        if (!isHexString(StringData(q, 4))) {
                            return parseError("Expecting 4 hex digits");
                        }
                        unsigned char first = fromHex(q);
                        unsigned char second = fromHex(q += 2);
                        const std::string& utf8str = encodeUTF8(first, second);
                        for (unsigned int i = 0; i < utf8str.size(); i++) {
                            result->push_back(utf8str[i]);
                        }
                        ++q;
                        break;
                    }
                        // Vertical tab character.  Not in JSON spec but allowed in
                        // our implementation according to test suite.
                    case 'v':  result->push_back('\v'); break;
                        // Escape characters we explicity disallow
                    case 'x':  return parseError("Hex escape not supported");
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':  return parseError("Octal escape not supported");
                        // By default pass on the unescaped character
                    default:   result->push_back(*q); break;
                        // TODO: check for escaped control characters
                }
                ++q;
            }
            else {
                result->push_back(*q++);
            }
        }
        if (q < _input_end) {
            _input = q;
            return Status::OK();
        }
        return parseError("Unexpected end of input");
    }

    std::string JsonSchema::encodeUTF8(unsigned char first, unsigned char second) const {
        std::ostringstream oss;
        if (first == 0 && second < 0x80) {
            oss << second;
        }
        else if (first < 0x08) {
            oss << char( 0xc0 | (first << 2 | second >> 6) );
            oss << char( 0x80 | (~0xc0 & second) );
        }
        else {
            oss << char( 0xe0 | (first >> 4) );
            oss << char( 0x80 | (~0xc0 & (first << 2 | second >> 6) ) );
            oss << char( 0x80 | (~0xc0 & second) );
        }
        return oss.str();
    }

    inline bool JsonSchema::peekToken(const char* token) {
        return readTokenImpl(token, false);
    }

    inline bool JsonSchema::readToken(const char* token) {
        return readTokenImpl(token, true);
    }

    bool JsonSchema::readTokenImpl(const char* token, bool advance) {
        MONGO_JSON_DEBUG("token: " << token);
        const char* check = _input;
        if (token == NULL) {
            return false;
        }
        // 'isspace()' takes an 'int' (signed), so (default signed) 'char's get sign-extended
        // and therefore 'corrupted' unless we force them to be unsigned ... 0x80 becomes
        // 0xffffff80 as seen by isspace when sign-extended ... we want it to be 0x00000080
        while (check < _input_end && isspace(*reinterpret_cast<const unsigned char*>(check))) {
            ++check;
        }
        while (*token != '\0') {
            if (check >= _input_end) {
                return false;
            }
            if (*token++ != *check++) {
                return false;
            }
        }
        if (advance) { _input = check; }
        return true;
    }

    bool JsonSchema::readField(const StringData& expectedField) {
        MONGO_JSON_DEBUG("expectedField: " << expectedField);
        std::string nextField;
        nextField.reserve(FIELD_RESERVE_SIZE);
        Status ret = field(&nextField);
        if (ret != Status::OK()) {
            return false;
        }
        if (expectedField != nextField) {
            return false;
        }
        return true;
    }

    inline bool JsonSchema::match(char matchChar, const char* matchSet) const {
        if (matchSet == NULL) {
            return true;
        }
        if (*matchSet == '\0') {
            return false;
        }
        return (strchr(matchSet, matchChar) != NULL);
    }

    bool JsonSchema::isHexString(const StringData& str) const {
        MONGO_JSON_DEBUG("str: " << str);
        std::size_t i;
        for (i = 0; i < str.size(); i++) {
            if (!isxdigit(str[i])) {
                return false;
            }
        }
        return true;
    }

    bool JsonSchema::isBase64String(const StringData& str) const {
        MONGO_JSON_DEBUG("str: " << str);
        std::size_t i;
        for (i = 0; i < str.size(); i++) {
            if (!match(str[i], base64::chars)) {
                return false;
            }
        }
        return true;
    }

    bool JsonSchema::isArray() {
        return peekToken(LBRACKET);
    }

    BSONObj fromSchemaJson(const char* jsonString, int* len) {
        MONGO_JSON_DEBUG("jsonString: " << jsonString);
        if (jsonString[0] == '\0') {
            if (len) *len = 0;
            return BSONObj();
        }
        JsonSchema jschema(jsonString);
        BSONObjBuilder builder;
        Status ret = Status::OK();
        try {
            ret = jschema.parse(builder);
        }
        catch(std::exception& e) {
            std::ostringstream message;
            message << "caught exception from within JSON parser: " << e.what();
            throw MsgAssertionException(17031, message.str());
        }

        if (ret != Status::OK()) {
            ostringstream message;
            message << "code " << ret.code() << ": " << ret.codeString() << ": " << ret.reason();
            throw MsgAssertionException(16619, message.str());
        }
        if (len) *len = jschema.offset();
        return builder.obj();
    }

    BSONObj fromSchemaJson(const std::string& str) {
        int len = str.size();
        return fromSchemaJson(str.c_str(),&len);
    }
}

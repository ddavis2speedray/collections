//
// Created by Donald Lamar Davis II on 4/17/15.
//

#ifndef COLLECTIONS_JSONSCHEMA_H
#define COLLECTIONS_JSONSCHEMA_H

#include <string>
#include <iostream>
#include <json/json.h>
#include <network/uri/uri.hpp>
#include "jsoncpp/include/json/value.h"

namespace jsonschema {
    enum primitivesId {
        PRIMITIVE_ARRAY=1,
        PRIMITIVE_BOOLEAN,
        PRIMITIVE_INTEGER,
        PRIMITIVE_NUMBER,
        PRIMITIVE_NULL,
        PRIMITIVE_OBJECT,
        PRIMITIVE_STRING
    };

    class value {
    public:
        value(primitivesId id, value& value);
    private:
        const primitivesId id;
        const value& val;
    };

    class schema {
    public:
        schema(const Json::Value& root);
        const auto parseSchema(std::istream& in);
    private:
        const Json::Value& root;
        const Json::Value& $schema;
        const Json::Value& id;
        const Json::Value& title;
    };

    class validation_error:public std::exception {
    public:
        validation_error() _NOEXCEPT :msgField(),valueField(){}
        validation_error(std::string msg, Json::Value value) _NOEXCEPT;
        virtual ~validation_error() _NOEXCEPT {};
        virtual const std::string msg() const _NOEXCEPT;
        virtual const Json::Value value() const _NOEXCEPT;
    private:
        const std::string msgField;
        const Json::Value valueField;
    };
}
#endif //COLLECTIONS_JSONSCHEMA_H

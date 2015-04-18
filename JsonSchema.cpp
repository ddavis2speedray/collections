//
// Created by Donald Lamar Davis II on 4/17/15.
//

#include "JsonSchema.h"
#include "jsoncpp/include/json/value.h"
#include "jsoncpp/include/json/reader.h"
#include <json/json.h>
#include <network/uri.hpp>

using namespace jsonschema;
using namespace std;

value::value(primitivesId id, value &value):id(id),val(value) { }

schema::schema(const Json::Value& root):root(root),title(root["title"]),$schema(root["$schema"]),id(root["id"]) {
    if(root.isNull()&&!root.isObject()) {
        throw new validation_error("schema does not start as an object",root);
    }
}

const auto schema::parseSchema(std::istream &in) {
    Json::Value root;
    Json::Reader reader;
    if(reader.parse(in,root,true)) {
        return unique_ptr<schema>(new schema(root));
    }
    return unique_ptr<schema>(nullptr);
}

validation_error::validation_error(string msg, Json::Value value) _NOEXCEPT:msgField(msg),valueField(value) { }
const string validation_error::msg() const _NOEXCEPT { return msgField; }
const Json::Value validation_error::value() const _NOEXCEPT { return valueField; }


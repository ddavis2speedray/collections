//
// Created by Donald Lamar Davis II on 4/17/15.
//

#include <boost/filesystem.hpp>
#include <fstream>
#include "CollectionsManager.h"
#include "JsonSchema.h"

using namespace mongo;
using namespace boost;
using namespace boost::filesystem;
using namespace std;

namespace collections {

    vector<Configuration> configurations = {Configuration(UNDEFINED,DEFAULT_DB,"default"),
                                                         Configuration(COLLECTION,DEFAULT_DB,DEFAULT_COLLECTION_COLLECTION),
                                                         Configuration(SCHEMA,DEFAULT_DB,DEFAULT_SCHEMA_COLLECTION)};
    vector<SchemaItem> schemas;

    auto findLocalSchema(const StringData& ns) {
        return find_if(schemas.begin(),schemas.end(),[ns](const SchemaItem& o)->bool {
            return ns.compare(o.getStringField("id"))==0;
        });
    }

    const auto initializeMongoDb() {
        client::Options options;
        options.setCallShutdownAtExit(true);
        static Status status = client::initialize(options);
        return status;
    }

    Manager::Manager(const mongo::ConnectionString& cs) _NOEXCEPT {
        const auto status = initializeMongoDb();
        string errMsg;
        conn.reset(cs.connect(errMsg));
        wc.reset(new WriteConcern());
    }

    Manager::Manager(mongo::DBClientBase* c) _NOEXCEPT {
        conn.reset(c);
    }

    Status Manager::add(Item &item) _NOEXCEPT {
        try {
            conn->insert(*(configurations[item.typeId()].nameSpace()),item,0,wc.get());
        } catch(DBException& e) {
            return Status(ErrorCodes::fromInt(e.getCode()),e.toString());
        }
        return Status::OK();
    }

    Status Manager::update(Item &item) _NOEXCEPT {
        return Status::OK();
    }

    Status Manager::remove(Item &item) _NOEXCEPT {
        return Status::OK();
    }

    Status Manager::addSchema(ifstream& ifs) {
        ifs.seekg(0, std::ios::end);
        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (ifs.read(buffer.data(), size))
        {
            SchemaItem schema(fromSchemaJson(string(buffer.begin(),buffer.end())));
            cout << schema.jsonString() << endl;
            string id(schema.getStringField("id"));
            if(findLocalSchema(id)!=schemas.end()) {
                // Diff for changes
            } else {
                // Store new entry
                Status status = add(schema);
                if(!status.isOK()) {
                    return status;
                }
                schemas.push_back(schema);
            }
        } else {
            return Status(ErrorCodes::Error::FileStreamFailed,"Unable to read schema");
        }
        return Status::OK();
    }

    Status Manager::loadSystemSchemas(const std::string schemaDir) {
        try
        {
            boost::filesystem::path path = boost::filesystem::path(schemaDir);
            if(filesystem::exists(path) || filesystem::is_directory(path) ) {
                for_each(directory_iterator(path),directory_iterator(),[&](const boost::filesystem::path& p) -> Status {
                    if(!filesystem::is_directory(p)) {
                        cout << "Reading " << p << endl;
                        auto ifs = ifstream(p.c_str(),ios::in | ios::binary | ios::ate);
                        return addSchema(ifs);
                    }
                    return Status::OK();
                });
            } else {
                return Status(ErrorCodes::Error::InvalidPath,path.generic_string());
            }
        }
        catch(const filesystem_error& ex)
        {
            return Status(ErrorCodes::Error::InternalError,ex.what());
        }
        return Status::OK();
    }
};

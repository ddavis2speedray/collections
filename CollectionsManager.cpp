//
// Created by Donald Lamar Davis II on 4/17/15.
//

#include <boost/filesystem.hpp>
#include <fstream>
#include "CollectionsManager.h"

using namespace mongo;
using namespace boost;
using namespace boost::filesystem;
using namespace std;

namespace collections {

    std::vector<Configuration> configurations = {Configuration(UNDEFINED,DEFAULT_DB,"default"),
                                                         Configuration(COLLECTION,DEFAULT_DB,DEFAULT_COLLECTION_COLLECTION),
                                                         Configuration(SCHEMA,DEFAULT_DB,DEFAULT_SCHEMA_COLLECTION)};

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

    ActionStatus Manager::add(Item& item) _NOEXCEPT {
        ActionStatus status;
        try {
            conn->insert(*(configurations[item.typeId()].nameSpace()),item,0,wc.get());
        } catch(DBException& e) {
            status = ActionStatus(ErrorCodes::fromInt(e.getCode()),e.toString());
        }
        return status;
    }

    ActionStatus Manager::update(Item& item) _NOEXCEPT {
        ActionStatus status;
        return status;
    }

    ActionStatus Manager::remove(Item& item) _NOEXCEPT {
        ActionStatus status;
        return status;
    }

    ActionStatus Manager::addSchema(ifstream& ifs) {
        ActionStatus status;
        ifs.seekg(0, std::ios::end);
        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (ifs.read(buffer.data(), size))
        {
            SchemaItem schema(mongo::fromjson(string(buffer.begin(),buffer.end())));
            status = add(schema);
        } else {
            status = ActionStatus(ErrorCodes::Error::FileStreamFailed,"Unable to read schema");
        }
        return status;
    }

    ActionStatus Manager::loadSystemSchemas(const std::string schemaDir) {
        ActionStatus status;
        try
        {
            boost::filesystem::path path = boost::filesystem::path(schemaDir);
            if(filesystem::exists(path) || filesystem::is_directory(path) ) {
                for_each(directory_iterator(path),directory_iterator(),[&](const boost::filesystem::path& p) -> ActionStatus {
                    ActionStatus status;
                    if(!filesystem::is_directory(p)) {
                        cout << "Reading " << p << endl;
                        auto ifs = ifstream(p.c_str(),ios::in | ios::binary | ios::ate);
                        status = addSchema(ifs);
                        return status;
                    }
                    return status;
                });
            } else {
                status = ActionStatus(ErrorCodes::Error::InvalidPath,path.generic_string());
            }
        }
        catch(const filesystem_error& ex)
        {
            status = ActionStatus(ErrorCodes::Error::InternalError,ex.what());
        }
        return status;
    }
};

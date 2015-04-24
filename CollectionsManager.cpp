//
// Created by Donald Lamar Davis II on 4/17/15.
//

#include <boost/filesystem.hpp>
#include "CollectionsManager.h"

using namespace mongo;
using namespace boost;
using namespace boost::filesystem;
using namespace std;

namespace collections {

    std::vector<Configuration> configurations = {Configuration(UNDEFINED,DEFAULT_DB,"default"),
                                                         Configuration(COLLECTION,DEFAULT_DB,DEFAULT_COLLECTION_COLLECTION),
                                                         Configuration(SCHEMA,DEFAULT_DB,DEFAULT_SCHEMA_COLLECTION)};

    const ActionStatus okStatus() {
        const static ActionStatus status(ErrorCodes::Error::OK,std::string("SUCCESS"));
        return status;
    }

    const Status& initializeMongoDb() {
        client::Options options;
        options.setCallShutdownAtExit(true);
        static Status status = client::initialize(options);
        return status;
    }

    Manager::Manager(const mongo::ConnectionString& cs) _NOEXCEPT {
        const Status& status = initializeMongoDb();
        string errMsg;
        conn.reset(cs.connect(errMsg));
        wc.reset(new WriteConcern());
    }

    Manager::Manager(mongo::DBClientBase* c) _NOEXCEPT {
        conn.reset(c);
    }

    ActionStatus Manager::add(Item& item) _NOEXCEPT {
        ActionStatus status = okStatus();
        try {
            conn->insert(*(configurations[item.typeId()].nameSpace()),item,0,wc.get());
        } catch(DBException& e) {
            status.reset(ErrorCodes::fromInt(e.getCode()),e.toString());
        }
        return status;
    }

    ActionStatus Manager::update(Item& item) _NOEXCEPT {
        ActionStatus status = okStatus();
        return status;
    }

    ActionStatus Manager::remove(Item& item) _NOEXCEPT {
        ActionStatus status = okStatus();
        return status;
    }

    ActionStatus Manager::loadSystemSchemas(const std::string schemaDir) {
        ActionStatus status = okStatus();
        try
        {
            boost::filesystem::path path = boost::filesystem::path(schemaDir);
            if(filesystem::exists(path) || filesystem::is_directory(path) ) {
                vector<boost::filesystem::path> paths;
                for_each(directory_iterator(path),directory_iterator(),[&](auto &p) {
                    if(!filesystem::is_directory(p)) {
                        paths.push_back(p);
                    }
                });
            } else {
                status.reset(ErrorCodes::Error::InvalidPath,path.generic_string());
            }
        }
        catch(const filesystem_error& ex)
        {
            status.reset(ErrorCodes::Error::InternalError,ex.what());
        }
        return status;
    }
};

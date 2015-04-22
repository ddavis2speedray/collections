//
// Created by Donald Lamar Davis II on 4/17/15.
//

#include "CollectionsManager.h"

using namespace mongo;
using namespace boost;
using namespace std;

namespace collections {

    std::vector<Configuration> createDefaultConfiguration() {
        std::vector<Configuration> confs(_MAX_ITEM_IDS);
        confs.push_back(Configuration(UNDEFINED,DEFAULT_DB,"default"));
        confs.push_back(Configuration(COLLECTION,DEFAULT_DB,DEFAULT_COLLECTION_COLLECTION));
        confs.push_back(Configuration(SCHEMA,DEFAULT_DB,DEFAULT_SCHEMA_COLLECTION));
    }
    static std::vector<Configuration> configurations = createDefaultConfiguration();

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
        ActionStatus status(ErrorCodes::Error::OK,"Success");
        try {
            conn->insert(configurations[item.typeId()].nameSpace(),item,0,wc.get());
        } catch(DBException& e) {
            ActionStatus status(ErrorCodes::fromInt(e.getCode()),e.toString());
            return status;
        }
        return status;
    }

    ActionStatus Manager::update(Item& item) _NOEXCEPT {
        ActionStatus status(ErrorCodes::Error::OK,"Success");
        return status;
    }

    ActionStatus Manager::remove(Item& item) _NOEXCEPT {
        ActionStatus status(ErrorCodes::Error::OK,"Success");
        return status;
    }
}

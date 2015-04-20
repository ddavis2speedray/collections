//
// Created by Donald Lamar Davis II on 4/17/15.
//

#include "CollectionsManager.h"
#include "CollectionItem.h"

using namespace collections;
using namespace mongo;
using namespace boost;
using namespace std;

void initializeMongoDb() {
    client::Options options;
    options.setCallShutdownAtExit(true);
    static Status status = client::initialize(options);
    if(!status.isOK()) {
        cerr << "failed to initialize with error: " << status.codeString() << endl;
        throw mongo_error(status.reason());
    }
}

Manager::Manager(const mongo::ConnectionString& cs) {
    initializeMongoDb();
    string errMsg;
    connection.reset(cs.connect(errMsg));
    if(!connection) {
        throw mongo_error(errMsg);
    }
    writeConcern.reset(new WriteConcern());
    nameSpace = "speedray.collections";
}

Manager::Manager(mongo::DBClientBase* c) {
    connection.reset(c);
}

ActionStatus Manager::addCollectionItem(CollectionItem& item) {
    ActionStatus status(ErrorCodes::Error::OK,"Success");
    try {
        connection->insert(nameSpace,item,0,writeConcern.get());
    } catch(DBException& e) {
        ActionStatus status(ErrorCodes::fromInt(e.getCode()),e.toString());
        return status;
    }
    return status;
}

ActionStatus Manager::updateCollectionItem(CollectionItem& item) {
    ActionStatus status(ErrorCodes::Error::OK,"Success");
    return status;
}

ActionStatus Manager::deleteCollectionItem(CollectionItem& item) {
    ActionStatus status(ErrorCodes::Error::OK,"Success");
    return status;
}
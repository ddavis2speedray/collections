//
// Created by Donald Lamar Davis II on 4/17/15.
//

#include "CollectionsManager.h"

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
}
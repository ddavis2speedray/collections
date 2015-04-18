#include <iostream>
#include <mongo/client/dbclient.h>
#include "CollectionsManager.h"

using namespace std;
using namespace mongo;
using namespace boost;
using namespace collections;

int main() {
    string uri("mongodb://192.168.99.100");
    string errMsg;
    ConnectionString cs = ConnectionString::parse(uri, errMsg);
    if (!cs.isValid()) {
        cerr << "Error parsing connection string " << uri << ": " << errMsg << endl;
        return -1;
    }
    try {
        Manager manager(cs);
        cout << "connected ok" << endl;
    } catch( const DBException &e ) {
        cerr << "caught " << e.what() << endl;
        return -1;
    } catch( const mongo_error &e) {
        cerr << "caught " << e.what() << endl;
        cerr << "reason " << e.msg() << endl;
        return -1;
    }
    return 0;
}
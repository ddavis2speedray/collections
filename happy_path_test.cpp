//
// Created by Donald Lamar Davis II on 4/18/15.
//

#include <gtest/gtest.h>
#include "CollectionsManager.h"
#include "CollectionItem.h"
#include "mongo/dbtests/mock/mock_dbclient_connection.h"

using namespace std;
using namespace mongo;
using namespace boost;
using namespace collections;

TEST(HappyPathTest,AddFolderCollection) {
    MockRemoteDBServer server("test");
    MockDBClientConnection conn(&server);
    string uri("mongodb://192.168.99.100");
    string errMsg;
    ConnectionString cs = ConnectionString::parse(uri, errMsg);
    EXPECT_TRUE(cs.isValid());
    Manager manager(cs);
    CollectionItem item(fromjson("{ \"id\":\"http://speedray.org/collections/folder\" }"));
    ActionStatus actionStatus = manager.addCollectionItem(item);
    EXPECT_EQ(ErrorCodes::Error::OK,actionStatus.error());
}
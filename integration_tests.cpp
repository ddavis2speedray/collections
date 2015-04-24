//
// Created by Donald Lamar Davis II on 4/18/15.
//

#include <gtest/gtest.h>
#include "CollectionsManager.h"
#include "CommandLineFlags.h"

using namespace std;
using namespace mongo;
using namespace boost;
using namespace collections;

static string testDb("mongodb://localhost");
static string schemasDir;

TEST(IntegrationTests,Configuration) {
    EXPECT_EQ(UNDEFINED,configurations[UNDEFINED].typeId());
    EXPECT_EQ(COLLECTION,configurations[COLLECTION].typeId());
    EXPECT_EQ(SCHEMA,configurations[SCHEMA].typeId());
}

TEST(IntegrationTests,AddSystemSchema) {
    string errMsg;
    ConnectionString cs = ConnectionString::parse(testDb, errMsg);
    EXPECT_TRUE(cs.isValid());
    Manager manager(cs);
    EXPECT_TRUE(manager.isValid());
    ActionStatus actionStatus = manager.loadSystemSchemas(schemasDir);
    EXPECT_EQ(ErrorCodes::Error::OK,actionStatus.error());
}

TEST(IntegrationTests,AddFolderCollection) {
    string errMsg;
    ConnectionString cs = ConnectionString::parse(testDb, errMsg);
    EXPECT_TRUE(cs.isValid());
    Manager manager(cs);
    EXPECT_TRUE(manager.isValid());
    CollectionItem item(fromjson("{ \"id\":\"http://speedray.org/collections/folder\" }"));
    ActionStatus actionStatus = manager.add(item);
    EXPECT_EQ(ErrorCodes::Error::OK,actionStatus.error());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    for (int i = 1; i < argc; i++) {
        const char* const arg = argv[i];
        if(ParseStringFlag(arg,"mongodb",&testDb)||
        ParseStringFlag(arg,"schemasDir",&schemasDir)) {}
    }
    cout << "Mongo DB URL is " << testDb << endl;
    cout << "Schemas directory is " << schemasDir << endl;
    return RUN_ALL_TESTS();
}
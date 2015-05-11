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

TEST(IntegrationTests,AddSystemSchema) {
    string errMsg;
    ConnectionString cs = ConnectionString::parse(testDb, errMsg);
    EXPECT_TRUE(cs.isValid());
    Manager manager(cs);
    EXPECT_TRUE(manager.isValid());
    Status status = manager.loadSystemSchemas(schemasDir);
    EXPECT_EQ(ErrorCodes::Error::OK,status.code());
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
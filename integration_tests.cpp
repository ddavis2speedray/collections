//
// Created by Donald Lamar Davis II on 4/18/15.
//

#include <gtest/gtest.h>
#include "CollectionsManager.h"
#include "CollectionItem.h"
#include "JsonSchema.h"
#include <cxxopts.hpp>

using namespace std;
using namespace mongo;
using namespace boost;
using namespace collections;

TEST(HappyPathTest,AddSystemSchema) {
    string uri("mongodb://192.168.99.100");
    string errMsg;
    ConnectionString cs = ConnectionString::parse(uri, errMsg);
    EXPECT_TRUE(cs.isValid());
    Manager manager(cs);
    Schema schema(fromjson("{ \"id\":\"http://speedray.org/collections/folder\" }"));
}

TEST(HappyPathTest,AddFolderCollection) {
    string uri("mongodb://192.168.99.100");
    string errMsg;
    ConnectionString cs = ConnectionString::parse(uri, errMsg);
    EXPECT_TRUE(cs.isValid());
    Manager manager(cs);
    CollectionItem item(fromjson("{ \"id\":\"http://speedray.org/collections/folder\" }"));
    ActionStatus actionStatus = manager.addCollectionItem(item);
    EXPECT_EQ(ErrorCodes::Error::OK,actionStatus.error());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    try {
        cxxopts::Options options(argv[0], " - integration test command line options");
        options.add_options()
                ("s,schema","JSON Schema directory",cxxopts::value<std::vector<std::string>>(),"schemas")
                ("help", "Print help");
        options.parse(argc, argv);

    } catch (const cxxopts::OptionException& e) {
        std::cerr << "error parsing options: " << e.what() << std::endl;
        return 1;
    }
    return RUN_ALL_TESTS();
}
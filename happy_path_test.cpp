//
// Created by Donald Lamar Davis II on 4/18/15.
//

#include <gtest/gtest.h>
#include "CollectionsManager.h"

using namespace std;
using namespace mongo;
using namespace boost;
using namespace collections;

TEST(HappyPathTest,Connect) {
    string uri("mongodb://192.168.99.100");
    string errMsg;
    ConnectionString cs = ConnectionString::parse(uri, errMsg);
    EXPECT_TRUE(cs.isValid());
    Manager manager(cs);
}
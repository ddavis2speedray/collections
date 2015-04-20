//
// Created by Donald Lamar Davis II on 4/19/15.
//

#include "CollectionItem.h"

using namespace std;
using namespace mongo;
using namespace network;
using namespace collections;

CollectionItem::CollectionItem(BSONObj o):BSONObj(o) {
}

CollectionItem::CollectionItem(std::string jsonString) {  }

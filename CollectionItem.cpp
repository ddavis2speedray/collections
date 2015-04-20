//
// Created by Donald Lamar Davis II on 4/19/15.
//

#include "CollectionItem.h"

using namespace std;
using namespace mongo;
using namespace network;
using namespace collections;

CollectionItem::CollectionItem(std::string uri) {
    network::uri _uri(uri);
    if(!_uri.is_absolute()) {
        throw network_exception("schema uri must be absolute");
    }
}
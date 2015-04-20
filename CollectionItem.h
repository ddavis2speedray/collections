//
// Created by Donald Lamar Davis II on 4/19/15.
//

#ifndef COLLECTIONS_COLLECTIONITEM_H
#define COLLECTIONS_COLLECTIONITEM_H

#include <mongo/bson/bsonobj.h>
#include <network/uri.hpp>

namespace collections {
    class CollectionItem {
    public:
        CollectionItem(std::string uri);
        explicit CollectionItem(const char *bsonData):obj(bsonData) { }
        explicit CollectionItem(mongo::SharedBuffer ownedBuffer):obj(ownedBuffer) { }
        CollectionItem(CollectionItem&& other):obj(other.obj) { }
        CollectionItem(const CollectionItem&)=default;

        mongo::BSONObj& dbObject() { return obj; }
    private:
        mongo::BSONObj obj;
    };

    class network_exception: public std::exception {
    public:
        virtual network_exception(std::string msg):msgField(msg) {};
        virtual ~network_exception() {};
    private:
        std::string msgField;
    };
}
#endif //COLLECTIONS_COLLECTIONITEM_H

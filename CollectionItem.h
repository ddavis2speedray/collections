//
// Created by Donald Lamar Davis II on 4/19/15.
//

#ifndef COLLECTIONS_COLLECTIONITEM_H
#define COLLECTIONS_COLLECTIONITEM_H

#include <mongo/bson/bson.h>
#include <network/uri.hpp>

namespace collections {
    class CollectionItem:public mongo::BSONObj {
    public:
        CollectionItem() {};
        CollectionItem(std::string jsonString);
        CollectionItem(mongo::BSONObj o);
        explicit CollectionItem(const char *bsonData):mongo::BSONObj(bsonData) { }
        explicit CollectionItem(mongo::SharedBuffer ownedBuffer):mongo::BSONObj(ownedBuffer) { }
        CollectionItem(CollectionItem&& other):mongo::BSONObj(other) { }
        CollectionItem(const CollectionItem&)=default;
    };

    class network_exception: public std::exception {
    public:
        network_exception(std::string msg):msgField(msg) {};
        virtual ~network_exception() {};
    private:
        std::string msgField;
    };
}
#endif //COLLECTIONS_COLLECTIONITEM_H

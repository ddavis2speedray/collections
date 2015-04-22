//
// Created by Donald Lamar Davis II on 4/17/15.
//

#ifndef COLLECTIONS_COLLECTIONSMANAGER_H
#define COLLECTIONS_COLLECTIONSMANAGER_H

#include <mongo/client/dbclient.h>

#define DEFAULT_DB "speedray"
#define DEFAULT_COLLECTION_COLLECTION "collections"
#define DEFAULT_SCHEMA_COLLECTION "schemas"

namespace collections {

    enum ItemId {
        UNDEFINED = 0,
        COLLECTION = 1,
        SCHEMA = 2,
        _MAX_ITEM_IDS = 99
    };

    class ActionStatus;

    class Configuration {
    public:
        Configuration(ItemId type,std::string db, std::string c) _NOEXCEPT:type(type),ns(db+"."+c) {};
        const auto typeId() _NOEXCEPT { return type; }
        const auto nameSpace() _NOEXCEPT { return &ns; }
        void nameSpace(std::string db,std::string c) { ns = db+"."+c; }
    private:
        ItemId type;
        std::string ns;
    };

    class Item:public mongo::BSONObj {
    public:
        Item(mongo::BSONObj o) _NOEXCEPT:BSONObj(o),type(UNDEFINED) {};
        Item(mongo::BSONObj o,ItemId typeId) _NOEXCEPT:BSONObj(o),type(typeId) {};
        const auto typeId() _NOEXCEPT { return type; };
    private:
        ItemId type;
    };

    class CollectionItem:public Item {
    public:
        CollectionItem(mongo::BSONObj o) _NOEXCEPT:Item(o,COLLECTION) {}
    };

    class SchemaItem:public Item {
    public:
        SchemaItem(mongo::BSONObj o) _NOEXCEPT:Item(o,SCHEMA) {}
    private:
    };

    class Manager {
    public:
        Manager(const mongo::ConnectionString& cs) _NOEXCEPT;
        Manager(mongo::DBClientBase* c) _NOEXCEPT;
        virtual ~Manager() _NOEXCEPT { };
        const mongo::DBClientBase& connection()  _NOEXCEPT { return *conn; }
        const mongo::WriteConcern& writeConcern()  _NOEXCEPT { return *wc; }
        // Connection status, etc.
        const bool isValid() _NOEXCEPT { return conn?!conn->isFailed():false; }
        // Collection item actions add,update,delete
        ActionStatus add(Item& item) _NOEXCEPT;
        ActionStatus update(Item& item) _NOEXCEPT;
        ActionStatus remove(Item& item) _NOEXCEPT;
    private:
        std::unique_ptr<mongo::DBClientBase> conn;
        std::unique_ptr<mongo::WriteConcern> wc;
    };

    class ActionStatus {
    public:
        ActionStatus(mongo::ErrorCodes::Error error, const std::string &&msg) _NOEXCEPT :errorCode(error),errorMessage(msg) {};
        virtual ~ActionStatus() _NOEXCEPT {};
        const auto error() _NOEXCEPT { return errorCode; }
        const auto msg() _NOEXCEPT { return errorMessage; }
    private:
        const mongo::ErrorCodes::Error errorCode;
        const std::string& errorMessage;
    };
}
#endif //COLLECTIONS_COLLECTIONSMANAGER_H

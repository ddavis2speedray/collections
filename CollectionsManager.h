//
// Created by Donald Lamar Davis II on 4/17/15.
//

#ifndef COLLECTIONS_COLLECTIONSMANAGER_H
#define COLLECTIONS_COLLECTIONSMANAGER_H

#include <mongo/client/dbclient.h>

namespace collections {
    class Manager {
    public:
        Manager(const mongo::ConnectionString& cs);
        virtual ~Manager() _NOEXCEPT { };
    private:
        std::unique_ptr<mongo::DBClientBase> connection;
    };

    class mongo_error:public std::exception {
    public:
        mongo_error(const std::string& _msg) _NOEXCEPT:msgField(_msg) { }
        virtual ~mongo_error() _NOEXCEPT {}
        virtual const char* what() const _NOEXCEPT { return "collections::mongo_error"; };
        virtual const std::string& msg() const _NOEXCEPT { return msgField; };
    private:
        std::string msgField;
    };
}
#endif //COLLECTIONS_COLLECTIONSMANAGER_H

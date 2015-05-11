//
// Created by Donald Lamar Davis II on 5/2/15.
//

#ifndef COLLECTIONS_SCHEMAJSON_H
#define COLLECTIONS_SCHEMAJSON_H

#import <mongo/client/dbclient.h>

namespace collections {
    class SchemaJson {
    public:
        explicit SchemaJson(const mongo::StringData& str);

        /*
         * Notation: All-uppercase symbols denote non-terminals; all other
         * symbols are literals.
         */

        /*
         * VALUE :
         *     STRING
         *   | NUMBER
         *   | NUMBERINT
         *   | NUMBERLONG
         *   | OBJECT
         *   | ARRAY
         *
         *   | true
         *   | false
         *   | null
         *   | undefined
         *
         *   | NaN
         *   | Infinity
         *   | -Infinity
         *
         *   | new CONSTRUCTOR
         */
    private:
        Status value(const mongo::StringData& fieldName, mongo::BSONObjBuilder&);

        /*
         * OBJECT :
         *     {}
         *   | { MEMBERS }
         *   | SPECIALOBJECT
         *
         * MEMBERS :
         *     PAIR
         *   | PAIR , MEMBERS
         *
         * PAIR :
         *     FIELD : VALUE
         *
         * SPECIALOBJECT :
         *     IDOBJECT
         *   | REFOBJECT
         *
         */
    public:
        Status object(const mongo::StringData& fieldName, mongo::BSONObjBuilder&, bool subObj=true);
        Status parse(mongo::BSONObjBuilder& builder);
        bool isArray();

    };
}


#endif //COLLECTIONS_SCHEMAJSON_H

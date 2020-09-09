//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Attribute.hpp"

namespace ECE141 {

 //STUDENT: Implement the attribute class here... 
    Attribute::Attribute(DataType aType) {
        name = "";
        type = aType;
        size = 0;
        autoIncrement = 0;
        primary = 0;
        nullable = 1;
        defaultValue = -1;
    }
    Attribute::Attribute(std::string aName, DataType aType /*maybe more?*/) {
        name = aName;
        type = aType;
        size = 0;
        autoIncrement = 0;
        primary = 0;
        nullable = 1;
        defaultValue = -1;
    }
    Attribute::Attribute(const Attribute& aCopy) {
        name = aCopy.name;
        type = aCopy.type;
        size = aCopy.size;
        autoIncrement = aCopy.autoIncrement;
        primary = aCopy.primary;
        nullable = aCopy.nullable;
        defaultValue = aCopy.defaultValue;
    }
    Attribute::~Attribute() {

    }

    Attribute& Attribute::setName(std::string& aName) {
        name = aName;
        return *this;
    }
    Attribute& Attribute::setType(DataType aType) {
        type = aType;
        return *this;
    }
    Attribute& Attribute::setSize(int aSize) {
        size = aSize;
        return *this;
    }
    Attribute& Attribute::setAutoIncrement(bool anAuto) {
        autoIncrement = anAuto;
        return *this;
    }
    Attribute& Attribute::setPrimaryKey(bool aPri) {
        primary = aPri;
        return *this;
    }
    Attribute& Attribute::setNullable(bool aNullable) {
        nullable = aNullable;
        return *this;
    }

    bool          Attribute::isValid() {
        if (name.length() && size) {
            return true;
        }
        else {
            return false;
        }
    }


    MyRow Attribute::toRow() {
            MyRow rowObject;
            rowObject.row.push_back(getName());
            std::string theType = Attribute::dataTypeToString(getType());
            if (getSize() > 0) {
                theType += "(";
                theType += std::to_string((int)getSize());
                theType += ")";
            }
            rowObject.row.push_back(theType);
            rowObject.row.push_back(isNullable() ? std::string("YES") : std::string("NO"));
            rowObject.row.push_back(isPrimaryKey() ? std::string("YES") : std::string("NO"));
            rowObject.row.push_back(defaultValue);
            rowObject.row.push_back((isAutoIncrement() ? "auto_increment " : std::string("")) + (isPrimaryKey() ? "primary key " : std::string("")));
            return rowObject;
    }
}

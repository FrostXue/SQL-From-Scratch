//
//  Schema.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Schema.hpp"
#include <iostream>
#include "Helpers.hpp"
#include "ValueReader.hpp"

namespace ECE141 {

 //STUDENT: Implement the Schema class here...
    Schema::Schema(const std::string aName) {
        name = aName;
        blockNum = 0;
        changed = 0;
    }
    Schema::Schema(const Schema& aCopy) {
        attributes = aCopy.attributes;
        name = aCopy.name;
        blockNum = aCopy.blockNum;
        changed = aCopy.changed;
    }

    Schema::~Schema() {

    }

    Schema& Schema::addAttribute(const Attribute& anAttribute) {
        if (std::nullopt == getAttribute(anAttribute.getName())) {
            attributes.push_back(anAttribute);
        }
        return *this;
    }

    AttributeOpt      Schema::getAttribute(const std::string& aName) const {
        for (auto theAttr : attributes) {
            if (theAttr.getName() == aName) return theAttr;
        }
        return std::nullopt;
    }

    StringOpt           Schema::getPrimaryKeyName() const {
        for (int i = 0; i < attributes.size(); ++i) {
            if (attributes[i].isPrimaryKey()) {
                return attributes[i].getName();
            }
        }
        return std::nullopt;
    }

    uint32_t              Schema::getNextAutoIncrementValue() { // this function is incomplete
        return 0;
    }

    StatusResult  Schema::encode(std::ostream& aWriter) {
        std::string tableName = this->getName();
        int tableNameLength = tableName.length();

        aWriter << tableName;
        aWriter << '\n';
        
        /*for (int i = 0; i < this->getAttributes().size(); ++i) {
            std::string dataStr = "";
            std::string attributeName = this->getAttributes()[i].getName();
            DataType attributeType = this->getAttributes()[i].getType();
            uint32_t      size = this->getAttributes()[i].getSize();
            uint32_t      autoIncrement = this->getAttributes()[i].isAutoIncrement();
            uint32_t      primary = this->getAttributes()[i].isPrimaryKey();
            uint32_t      nullable = this->getAttributes()[i].isNullable();
            Entry theDefault = this->getAttributes()[i].defaultValue;
            //-------------------------------------------------------------------
            dataStr = dataStr + attributeName; dataStr += ' ';
            dataStr += static_cast<char>(attributeType); dataStr += ' ';
            dataStr += std::to_string(size); dataStr += ' ';
            if (autoIncrement) { dataStr += "1 "; }
            else { dataStr += "0 "; }
            if (primary) { dataStr += "1 "; }
            else { dataStr += "0 "; }
            if (nullable) { dataStr += "1 "; }
            else { dataStr += "0 "; }
            size_t theType = this->getAttributes()[i].defaultValue.index();
            dataStr += std::to_string(theType); dataStr += ' ';
            dataStr += Helpers::toString(theDefault); dataStr += '\n';
            //--------------------------------------------------------------------
            aWriter << dataStr;
        }*/

        for (int i = 0; i < this->getAttributes().size(); i++) {
            std::string attributeName = this->getAttributes()[i].getName();
            DataType    attributeType = this->getAttributes()[i].getType();
            uint32_t    size = this->getAttributes()[i].getSize();
            bool        autoIncrement = this->getAttributes()[i].isAutoIncrement();
            bool        primary = this->getAttributes()[i].isPrimaryKey();
            bool        nullable = this->getAttributes()[i].isNullable();
            Entry theDefault = this->getAttributes()[i].defaultValue;
            ValueWriter theWriter(aWriter,ValuePrefix::Included);

            theWriter.operator()(attributeName);
            theWriter.operator()(static_cast<char>(attributeType));
            theWriter.operator()(size);
            theWriter.operator()(autoIncrement);
            theWriter.operator()(primary);
            theWriter.operator()(nullable);
            theWriter.operator()(static_cast<int>(theDefault.index()));
            std::visit(theWriter, theDefault);

            aWriter << '\n';
        }

        return StatusResult(noError);
    }
    StatusResult  Schema::decode(std::istream& aReader) {
        std::getline(aReader, name);
        attributes.clear();
        while (aReader.good()) {
            aReader.ignore(1);//ignore the v before the attribute name
            std::string theAttributeName;
            std::getline(aReader, theAttributeName,'\037');//Set name of attribute
			if (theAttributeName[0]!='\0') {

                ValueReader theValueReader(aReader);

                char theType;
                theValueReader.binary_read(theType);//Set type of attribute
				attributes.push_back(Attribute(theAttributeName, static_cast<DataType>(theType)));

				int theSize = 0;//Set size
                theValueReader.binary_read(theSize);
				attributes.back().setSize(theSize);

				bool theAutoInc = false;//Set autoinc
                theValueReader.binary_read(theAutoInc);
				attributes.back().setAutoIncrement(theAutoInc);

				bool thePrimary = false;//Set primary
                theValueReader.binary_read(thePrimary);
				attributes.back().setPrimaryKey(thePrimary);

				bool theNullable = false;
                theValueReader.binary_read(theNullable);
				attributes.back().setNullable(theNullable);

				int theIndex = 0;
                theValueReader.binary_read(theIndex);

				Entry theDefaultValue;
                theValueReader.binary_read(theDefaultValue);
                attributes.back().defaultValue = theDefaultValue;

				std::getline(aReader, theAttributeName); //Spit out the \n
			}
			else break;
        }
        return StatusResult(noError);
    }

    void              Schema::show() {
        std::clog << "block num: " << blockNum << std::endl;
        std::clog << "schema name: " << name << std::endl;       
        for (int i = 0; i < attributes.size(); ++i) {
            std::clog << i << ": attribute name: " << attributes[i].getName() << std::endl;
            std::clog << i << ": attribute type: " << static_cast<char>(attributes[i].getType()) << std::endl;
            std::clog << i << ": attribute size: " << attributes[i].getSize() << std::endl;
            std::clog << i << ": attribute autoincrement: " << attributes[i].isAutoIncrement() << std::endl;
            std::clog << i << ": attribute nullable: " << attributes[i].isNullable() << std::endl;
            if (attributes[i].defaultValue.index() == 0) {
                std::clog << i << ": attribute default: " << std::get<int>(attributes[i].defaultValue) << std::endl;
            }
            if (attributes[i].defaultValue.index() == 1) {
                std::clog << i << ": attribute default: " << std::get<double>(attributes[i].defaultValue) << std::endl;
            }
            if (attributes[i].defaultValue.index() == 2) {
                std::clog << i << ": attribute default: " << std::get<char>(attributes[i].defaultValue) << std::endl;
            }
            if (attributes[i].defaultValue.index() == 3) {
                std::clog << i << ": attribute default: " << std::get<std::string>(attributes[i].defaultValue) << std::endl;
            }
            if (attributes[i].defaultValue.index() == 4) {
                std::clog << i << ": attribute default: " << std::get<bool>(attributes[i].defaultValue) << std::endl;
            }
        }
    }
    /*
    BlockType     Schema::getType() const {
        return BlockType::entity_block;
        //what kind of block is this?
    }
    */
    Storable* Schema::createSchema(std::stringstream& anInput) {
        Schema* toReturn = new Schema("");
        toReturn->decode(anInput);

        return toReturn;
    }
}

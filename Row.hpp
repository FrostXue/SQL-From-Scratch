//
//  Row.hpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
//#include "Storage.hpp"
//#include "Database.hpp"
#include "Value.hpp"
#include "Errors.hpp"
#include "ValueWriter.hpp"
#include "ValueReader.hpp"

namespace ECE141 {


  class Row : public Storable {
  public:

    Row(std::string aTableName = "");
    Row(const Row &aRow);
    ~Row();
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    std::string getTableName() {
        return tableName;
    }
    Row& setTableName(std::string aTableName) {
        tableName = aTableName;
    }
    NullableKVList& getKeyValueList() {
        return data;
    }
    void show();
    Row& setBlockNum(int anIndex) { /*std::clog << anIndex << std::endl;*/ blockNum = anIndex; /*std::clog << blockNum << std::endl;*/ return *this; } // FIXME not sure why, this function has bugs, show number incorrectly after storage load
    int              getBlockNum() const { /*std::clog << blockNum << std::endl;*/ return blockNum; } // FIXME not sure why, this function has bugs, show number incorrectly after storage load
    
      //STUDENT: What other methods do you require?
    virtual StatusResult  encode(std::ostream& aWriter);
    virtual StatusResult  decode(std::istream& aReader);
    virtual StorableType getType() {
        return StorableType::Row;
    }

    virtual void showView(std::ostream& aStream);//viewable interface

    static Storable* createRow(std::stringstream& aEncodedStream);//creator function to construct a new row object
  protected:  
    std::string tableName;
    NullableKVList data;  //you're free to change this if you like...
  };

}

#endif /* Row_hpp */

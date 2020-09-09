//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <iomanip>
#include "Storage.hpp"
#include"Schema.hpp"
#include "Tokenizer.hpp"
#include "Components.hpp"
#include "Row.hpp"
#include "Value.hpp"
#include "optional"
#include "MyView.hpp"
#include <utility> 
#include <algorithm>
#include <functional>
#include <array>
#include "Filters.hpp"
#include "RowCollection.hpp"
#include <map>
#include "IndexController.hpp"

namespace ECE141 {
    using TableBlockNum = uint32_t;
    using IndexBlockNum = uint32_t;
    //using TableName = std::string;

  class Database : public Storable  {
  public:
    
    Database(const std::string aPath, CreateNewStorage);
    Database(const std::string aPath, OpenExistingStorage);
    ~Database();
    
    Storage&          getStorage() {return storage;}
    std::string&      getName() {return name;}

    void createDatabase(std::string& aName);
    void describeDatabase(std::string& aName);
    void dropDatabase(std::string &aName);

    StatusResult createTable(Schema* aSchema);
    StatusResult dropTable(std::string aName);
    StatusResult deleteFrom(std::string aName, Filters& aFilter);

    Schema* getSchema(const std::string& aName);
    std::vector<Schema*> getSchemas();
    StatusResult insert(std::string tableName, std::vector<std::vector<Token>> rowTokens, Schema tableSchema);
    //std::vector<std::pair<std::pair<char, int>, int>> getFreeBlocks(std::string& aName);
    StatusResult select(std::string tableName, Filters& aFilter, RowCollection& aRowCollection);
    StatusResult update(std::string tableName, Filters& aFilter, RowCollection& aRowCollection);
    StatusResult updateARow(Row aRow);
    bool selectWithIndex(std::string tableName, Filters& aFilter, RowCollection& aRowCollection);
    StatusResult showIndexes();

    virtual StatusResult  encode(std::ostream& aWriter);
    virtual StatusResult  decode(std::istream& aReader);
    virtual StorableType getType() { return StorableType::Database; }
              
  protected:
      std::map<std::string, std::pair<TableBlockNum, IndexBlockNum>> tables;

    std::string     name;
    Storage         storage;
    IndexController indexCtrl;
  };
  
}

#endif /* Database_hpp */

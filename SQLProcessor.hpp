//
//  SQLProcessor.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "Schema.hpp"
#include "SQLStatements.hpp"
#include "Database.hpp"
#include "RowCollection.hpp"
#include "Filters.hpp"
#include "Timer.hpp"
#include "Components.hpp"
#include <map>

class Statement;
class Database; //define this later...

namespace ECE141 {

  class SQLProcessor : public CommandProcessor {
  public:
    
    SQLProcessor(CommandProcessor *aNext=nullptr);
    virtual ~SQLProcessor();
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);
    
    StatusResult createTable(const Schema &aSchema);
    StatusResult dropTable(const std::string &aName);
    StatusResult describeTable(const std::string &aName) const;
    StatusResult showTables() const;
    StatusResult insert(std::string tableName, std::vector<std::vector<Token>> rowTokens);
    StatusResult deleteFrom(std::string tableName, std::vector<std::vector<Token>> expressions);
    StatusResult select(std::string tableName, std::vector<Token> fields, std::vector<std::vector<Token>> expressions);
    StatusResult join(std::string tableName, std::vector<Token> fields, Join aJoin);
    StatusResult update(std::string tableName, std::vector<std::vector<Token>> KVPairs, std::vector<std::vector<Token>> expressions);
    StatusResult showIndexes();
    StatusResult alterTable(const std::string tableName, const Attribute& toAlter);
    
/*  do these in next assignment
    StatusResult insert();
    StatusResult update();
    StatusResult delete();
*/
    
  protected:
    
    //do you need other data members?
      
  };

}
#endif /* SQLProcessor_hpp */

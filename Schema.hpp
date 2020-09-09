//
//  Schema.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Schema_hpp
#define Schema_hpp

#include <iostream>
#include <stdio.h>
#include <vector>
#include "Attribute.hpp"
#include "Errors.hpp"
//#include "Row.hpp"
#include <optional>
//#include "StorageBlock.hpp"
#include "Value.hpp"
namespace ECE141 {
  
  struct Block;
  struct Expression;
  class  Database;
  class  Tokenizer;
  class  Storage;
  using StringList = std::vector<std::string>;
  //using attribute_callback = std::function<bool(const Attribute &anAttribute)>;
  
  using AttributeList = std::vector<Attribute>;
  using AttributeOpt = std::optional<Attribute>;
  using StringOpt = std::optional<std::string>;
  //STUDENT: If you're using the Storable interface, add that to Schema class?

  class Schema : public Storable {
  public:
                          Schema(const std::string aName);
                          Schema(const Schema &aCopy);
    
                          ~Schema();
        
    const std::string&    getName() const {return name;}
    const AttributeList&  getAttributes() const {return attributes;}
    int              getBlockNum() const {return blockNum;}
    bool                  isChanged() {return changed;}
    
    Schema&               addAttribute(const Attribute &anAttribute);
    AttributeOpt      getAttribute(const std::string &aName) const;
    void              show();
    Schema& setBlockNum(int anIndex) { blockNum = anIndex; return *this; }
      
        //STUDENT: These methods will be implemented in the next assignment...
    
    //Value                 getDefaultValue(const Attribute &anAttribute) const;
    //StatusResult          validate(KeyValues &aList);
    
    StringOpt             getPrimaryKeyName() const;
    uint32_t              getNextAutoIncrementValue();
    
    
    //STUDENT: Do you want to provide an each() method for observers?
    
    //friend class Database; //is this helpful?
    virtual StatusResult  encode(std::ostream& aWriter);
    virtual StatusResult  decode(std::istream& aReader); // caution: not able to set block num, set block num by setter separately
    virtual StorableType getType() {
        return StorableType::Schema;
    }

    static Storable* createSchema(std::stringstream& anInput);
  protected:
    
    AttributeList   attributes;
    std::string     name;
    //uint32_t        blockNum;  //storage location.
    bool            changed;
  };
  
}
#endif /* Schema_hpp */

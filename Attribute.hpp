//
//  Attribute.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <variant>
#include <string>
#include "MyRow.hpp"
#include "Value.hpp"
#include "Row.hpp"


namespace ECE141 {
      


  class Attribute {
  protected:
    std::string   name;
    DataType      type;
    
    //STUDENT: What other data should you save in each attribute?
    uint32_t      size : 24;
    uint32_t      autoIncrement : 1;
    uint32_t      primary : 1;
    uint32_t      nullable : 1;
    
    
  public:
      Entry defaultValue;
          
    Attribute(DataType aType=DataType::no_type);
    Attribute(std::string aName, DataType aType /*maybe more?*/);
    Attribute(const Attribute &aCopy);
    ~Attribute();
    
    Attribute&    setName(std::string &aName);
    Attribute&    setType(DataType aType);
    Attribute&    setSize(int aSize);
    Attribute&    setAutoIncrement(bool anAuto);
    Attribute&    setPrimaryKey(bool aPri);
    Attribute&    setNullable(bool aNullable);

    bool          isValid(); //is this schema valid? Are all the attributes value?
    
    const std::string&  getName() const {return name;}
    DataType            getType() const {return type;}
    uint32_t            getSize() const { return size; }

    bool                isPrimaryKey() const { return primary; }
    bool                isNullable() const { return nullable; }
    bool                isAutoIncrement() const { return autoIncrement; }
    MyRow               toRow();

    //STUDENT: are there other getter/setters to manage other attribute properties?
    static std::string dataTypeToString(DataType aType) {

        switch (static_cast<int>(aType))
        {
        case 78:
            return std::string("null");
            break;
        case 66:
            return std::string("boolean");
            break;
        case 68:
            return std::string("data");
            break;
        case 70:
            return std::string("float");
            break;
        case 73:
            return std::string("integer");
            break;
        case 86:
            return std::string("varchar");
            break;
        default:
            return std::string("null");
            break;
        }
    }
  };
  

}


#endif /* Attribute_hpp */

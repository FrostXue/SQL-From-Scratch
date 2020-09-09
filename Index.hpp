//
//  Index.hpp
//  RGAssignment8
//
//  Created by rick gessner on 5/17/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Index_h
#define Index_h

#include "Storage.hpp"
#include "keywords.hpp"
//#include "Errors.hpp"

namespace ECE141 {
  
  using IntOpt = std::optional<uint32_t>;
  
  struct LessKey {
    bool operator()(const Entry& anLHS, const Entry& aRHS) const {
      return anLHS < aRHS;
    }
  };

  class Index : public Storable {
  public:

    using ValueMap = std::map<Entry, uint32_t, LessKey>;

    Index(const std::string &aField = "", DataType aType = DataType::no_type, size_t aBlockNum = 0)
      : field(aField), type(aType) {
        type=aType;
        changed=false;
        blockNum = aBlockNum;
        schemaId = 0;
      }
    
    virtual ~Index() {}
    
    ValueMap&           getList() {return list;}
    void                setChanged(bool aValue=true) {changed=aValue;}
    bool                isChanged() {return changed;}
    const std::string&  getFieldName() const {return field;}
    uint32_t            getBlockNum() const {return blockNum;}

    
      //manage keys/values in the index...
    Index& addKeyValue(const Entry& aKey, uint32_t aValue) {
        //list[aKey] = aValue;
        list.insert({ aKey, aValue });
        return *this;
    }
    Index& removeKeyValue(const Entry& aKey) {
        if (list.count(aKey)) {
            list.erase(aKey);
        }
        return *this;
    }
    bool contains(const Entry& aKey) {
        if (list.count(aKey)) {
            return true;
        }
        else {
            return false;
        }
    }
    uint32_t getValue(const Entry& aKey) {
        if (list.count(aKey)) {
            return list[aKey];
        }
        else {
            return 0; // not finding the index
        }
    }
    
      //don't forget to support the storable interface IF you want it...
    virtual StatusResult encode(std::ostream &aWriter);    
    virtual StatusResult decode(std::istream &aReader);
    //void initBlock(StorageBlock &aBlock);

      //and the blockIterator interface...
    //bool each(BlockVisitor &aVisitor) override;  
    //bool canIndexBy(const std::string &aField) override; 

  protected:
    
    std::string   field; //what field are we indexing? such as id
    DataType     type;
    uint32_t      schemaId;
    bool          changed;
    //uint32_t      blockNum;  //storage block# of index...
    
    ValueMap      list;
  };

}
#endif /* Index_h */

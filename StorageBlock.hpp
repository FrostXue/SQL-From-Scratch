//
//  StorageBlock.hpp
//  Assignment3
//
//  Created by rick gessner on 4/11/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef StorageBlock_hpp
#define StorageBlock_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <variant>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include "ValueWriter.hpp"
#include "ValueReader.hpp"
#include <sstream>
#include <fstream>

namespace ECE141 {

  //a "storage" file is comprised of fixed-sized blocks (defined below)

  const size_t kPayloadSize = 1012; //area reserved in storage-block for user data...
  
  //using ValueType = std::variant<int, double, std::string>;

  //using KeyValues = std::map<const std::string, ValueType>;

  enum class BlockType {
    meta_block='T',
    data_block='D',
    entity_block='E',
    free_block='F',
    index_block='I',
    unknown_block='V',
    continue_block='C'
  };

  using NamedBlockNums = std::map<std::string, uint32_t>;

  struct BlockHeader {
   
    BlockHeader(BlockType aType=BlockType::data_block)
      : type(static_cast<char>(aType)), id(0), id_char{'0'}, hasNext(false), next(0) {
      }
   
    BlockHeader(const BlockHeader &aCopy) {
      *this=aCopy;
    }
   
    BlockHeader& operator=(const BlockHeader &aCopy) {
      type=aCopy.type;
      id=aCopy.id;
      hasNext = aCopy.hasNext;
      next = aCopy.next;
      for (int i = 0; i < 11; ++i) {
          id_char[i] = aCopy.id_char[i];
      }
      return *this;
    }

    void load(char theHeader[12]) {
        std::stringstream theTempFile(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
        theTempFile.write(reinterpret_cast<const char*>(theHeader), 12);

        theTempFile.seekg(0, std::ios::beg);
        theTempFile.clear();
        ValueReader theReader(theTempFile);

        type = theTempFile.get(); // put type in the first char
        theReader.binary_read(id, ReaderPrefix::Naked);
        theReader.binary_read(hasNext, ReaderPrefix::Naked);
        theReader.binary_read(next, ReaderPrefix::Naked);

    }
   
    char      type;     //char version of block type {[D]ata, [F]ree... }
    uint32_t  id;       //use this anyway you like

    bool hasNext;
    uint32_t next;

    char id_char[11];
  };
 
  struct StorageBlock {
        
    StorageBlock(BlockType aType=BlockType::data_block);
    
    StorageBlock(const StorageBlock &aCopy);
    StorageBlock& operator=(const StorageBlock &aCopy);

    StorageBlock& store(std::ostream &aStream);
    void load(const char theBlock[1024]);
    void load(std::istream& aStream);
    void loadData(std::istream & aStream);
    void clearData();
    //we use attributes[0] as table name...
    BlockHeader   header;
    char          data[kPayloadSize];
  };
 
}

#endif /* StorageBlock_hpp */

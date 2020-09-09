//
//  StorageBlock.cpp
//  Assignment3
//
//  Created by rick gessner on 4/11/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "StorageBlock.hpp"
#include <cstring>
#include <stdio.h>
#include <string.h>

namespace ECE141 {

  StorageBlock::StorageBlock(BlockType aType) : header(aType) {
    memset(data,'\0',sizeof(data));
  }

  StorageBlock::StorageBlock(const StorageBlock &aCopy) : header(aCopy.header) {
    std::memcpy(reinterpret_cast<void*>(data), &aCopy.data, kPayloadSize);
  }

  StorageBlock& StorageBlock::operator=(const StorageBlock &aCopy) {
    header=aCopy.header;
    std::memcpy(reinterpret_cast<void*>(data), &aCopy.data, kPayloadSize);
    return *this;
  }
  
  //USE: save the given stream in this block data area.
  StorageBlock& StorageBlock::store(std::ostream &aStream) { // passed in file stream
      
      //aStream.seekp(1 * 1024);
    aStream << header.type; // header.type 1 char
    //std::clog << "block storing char: 1" << std::endl;
   // std::clog << "sizeof(header.id_char): " << sizeof(header.id_char) << std::endl;
    //for (int i = 0; i < sizeof(header.id_char); i++) { // header has 11 chars
        //std::clog << "block storing char: " << i + 2 << std::endl;
        ValueWriter theWriter(aStream, ValuePrefix::Naked);
        theWriter.operator()(header.id);
        theWriter.operator()(header.hasNext);
        theWriter.operator()(header.next);


        aStream << '\0';
        aStream << '\0';
    //}
    //std::clog << "sizeof(data): " << sizeof(data) << std::endl;
    //for (int i = 0; i < sizeof(data); ++i) { // store in the data
        //std::clog << "block storing char: " << i + 13 << std::endl;
        aStream.write(reinterpret_cast<const char*>(&data[0]),sizeof(data)); // write in 1012 bytes

        //for (int i = 0; i < kPayloadSize; i++)
            //aStream.write(reinterpret_cast<const char*>(&data), sizeof(data));
    //}
    return *this;
  }
  void StorageBlock::load(const char theBlock[1024]) { // load data in to char array passed in
      char theHeader[12];
      for (int i = 0; i < 12; i++) { // load in 12 bytes header
          theHeader[i] = theBlock[i];
      }
      header.load(theHeader);
      for (int i = 0; i < kPayloadSize; i++) { // load in the data
          data[i] = theBlock[i + 12];
      }
  }

  void StorageBlock::load(std::istream& aStream) {
      char theHeader[12];
      for (int i = 0; i < 12; i++) {
          aStream >> theHeader[i];
      }
      header.load(theHeader);
      loadData(aStream);
  }

  void StorageBlock::loadData(std::istream& aStream) {
      char temp = aStream.get();
      if (aStream.good()) { // after last ff no more good
          for (int i = 0; i < kPayloadSize; i++) {//TODO fix me
              if (aStream.good()) {
                  //std::clog << aStream.tellg() << "(";
                  data[i] = temp;
                  temp = aStream.get();
                  //std::clog << (int)data[i] << ") ";
              }
              else {
                  memset(&data[i], '\0', sizeof(data) - i);
                  break;
              }
          }
      }
      else clearData();
  }

  void StorageBlock::clearData() {
      memset(data, '\0', sizeof(data));
  }
}

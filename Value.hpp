//
//  Value.hpp
//  RGAssignment5
//
//  Created by rick gessner on 4/26/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Value_h
#define Value_h

#include <variant>
#include <optional>
#include <string>
#include <map>
#include <cstdint>
#include "Errors.hpp"
#include "StorageBlock.hpp"
#include "Components.hpp"

namespace ECE141 {
    enum class StorableType {
        Schema, Attribute, MyRow, Row, Unknown, Database
    };

  struct Storable {
     //virtual BlockType getType() = 0;
      virtual StatusResult  encode(std::ostream& aWriter) { return StatusResult(); }
      virtual StatusResult  decode(std::istream& aReader) { return StatusResult(); }
      virtual StorableType getType() { return StorableType::Unknown; }
     int blockNum = 0;
  };



  enum class DataType {
     no_type='N', bool_type='B', datetime_type='D', float_type='F', int_type='I', varchar_type='V',
   };

  //using ValueType = std::variant<uint32_t, float, bool, std::string>;

  //using KeyValues = std::map<const std::string, Entry>;

  using KVPair = std::pair<std::string, Entry>;

  using NullableKVPair = std::pair<std::string, std::pair<bool, Entry>>;

  using NullableKVList = std::vector<std::pair<std::string, std::pair<bool, Entry>>>;

  struct KeyValuePair {
      KeyValuePair(std::string aKeyName, Entry aValue) {
          aPair.first = aKeyName;
          aPair.second = aValue;
      }
      KeyValuePair(std::string aKey = "", int aVal = 0) {
          aPair.first = aKey;
          aPair.second = aVal;
      }
      KeyValuePair(std::string aKey = "", double aVal = 0) {
          aPair.first = aKey;
          aPair.second = aVal;
      }
      KeyValuePair(std::string aKey = "", char aVal = 0) {
          aPair.first = aKey;
          aPair.second = aVal;
      }
      KeyValuePair(std::string aKey = "", std::string aVal = "") {
          aPair.first = aKey;
          aPair.second = aVal;
      }
      KeyValuePair(std::string aKey = "", bool aVal = 0) {
          aPair.first = aKey;
          aPair.second = aVal;
      }
      KeyValuePair(const KeyValuePair& aCopy) {
          aPair.first = aCopy.aPair.first;
          aPair.second = aCopy.aPair.second;
      }
      KeyValuePair& operator=(const KeyValuePair& aCopy) {
          aPair.first = aCopy.aPair.first;
          aPair.second = aCopy.aPair.second;
          return *this;
      }
      ~KeyValuePair() {}
      bool operator==(const KeyValuePair& aCopy) const {
          if (aPair.first == aCopy.aPair.first) {
              if (aPair.second.index() == aCopy.aPair.second.index()) {
                  if (aPair.second == aCopy.aPair.second) {
                      return true;
                  }
              }
          }
          return false;
      }
      bool operator!=(const KeyValuePair& aCopy) const {
          if (*this == aCopy) {
              return false;
          }
          else {
              return true;
          }
      }
      /*KVPair& setKey(std::string aKey) {
          aPair.first = aKey;
          return *this;
      }
      KVPair& setValue(int aNum) {
          aPair.second = aNum;
          return *this;
      }
      KVPair& setValue(double aNum) {
          aPair.second = aNum;
          return *this;
      }
      KVPair& setValue(bool aNum) {
          aPair.second = aNum;
          return *this;
      }
      KVPair& setValue(std::string aNum) {
          aPair.second = aNum;
          return *this;
      }*/
      KVPair aPair;
  };

  using KeyValueList = std::vector<KeyValuePair>;

}

#endif /* Value_h */

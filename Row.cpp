//
//  Row.cpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Row.hpp"

namespace ECE141 {
  
  //STUDENT: You need to fully implement these methods...
 

    Row::Row(std::string aTableName) { tableName = aTableName; blockNum = 0; }
  Row::Row(const Row &aRow) {
      tableName = aRow.tableName;
      data.clear();
      for (int i = 0; i < aRow.data.size(); ++i) {
          data.push_back(aRow.data[i]);
      }
      blockNum = aRow.blockNum;
  }
  Row& Row::operator=(const Row &aRow) {
      tableName = aRow.tableName;
      data.clear();
      for (int i = 0; i < aRow.data.size(); ++i) {
          data.push_back(aRow.data[i]);
      }
      blockNum = aRow.blockNum;
      return *this;
  }
  bool Row::operator==(Row &aCopy) const {
      if (tableName == aCopy.tableName && data.size() == aCopy.data.size()) {
          for (int i = 0; i < data.size(); ++i) {
              if (data[i] != aCopy.data[i]) {
                  return false;
              }
          }
      }
      return true;
  }
  Row::~Row() {}

  StatusResult  Row::encode(std::ostream& aWriter) {
      std::string tableName = this->getTableName();
      int tableNameLength = tableName.length();
      //----------------------------------------------binary encode--------------------------------------------Haoru May8
      ValueWriter theBinaryWriter(aWriter);

      theBinaryWriter.operator()(tableName);//Write table that it belongs to

      for (int i = 0; i < this->getKeyValueList().size(); ++i) {
          theBinaryWriter.operator()(getKeyValueList()[i].first);//Write string key
          theBinaryWriter.operator() (getKeyValueList()[i].second.first);//Write isNull
          if (!getKeyValueList()[i].second.first) std::visit(theBinaryWriter, getKeyValueList()[i].second.second);
      }
      //---------------------------------------------legacy string encode------------------------------------------------
      /*
      aWriter << tableName;
      aWriter << '\n';

      for (int i = 0; i < this->getKeyValueList().size(); ++i) {
          aWriter << getKeyValueList()[i].aPair.first;
          aWriter << ' ';
          if (getKeyValueList()[i].aPair.second.index() == 0) {
              aWriter << "int ";
              aWriter << std::to_string(std::get<int>(getKeyValueList()[i].aPair.second));
              aWriter << '\n';
          }
          else if (getKeyValueList()[i].aPair.second.index() == 1) {
              aWriter << "double ";
              aWriter << std::to_string(std::get<double>(getKeyValueList()[i].aPair.second));
              aWriter << '\n';
          }
          else if (getKeyValueList()[i].aPair.second.index() == 3) {
              aWriter << "string ";
              aWriter << std::get<std::string>(getKeyValueList()[i].aPair.second);
              aWriter << '\n';
          }
          else if (getKeyValueList()[i].aPair.second.index() == 4) {
              aWriter << "bool ";
              aWriter << std::to_string(std::get<bool>(getKeyValueList()[i].aPair.second));
              aWriter << '\n';
          }
      }
      */
      return StatusResult(noError);
  }

  //-----------------------------------------------utility functions---------------------------------------------
  //if (aStr.find_first_of("0123456789") == std::string::npos) { return std::nullopt; }
  // this function converts a string with numbers to an integer, only the first appearing number is being converted to int
  int stringToInt1(std::string aStr) {
      int d = stoi(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
      return d;
  }

  // this function converts a string with numbers to a double, only the first appearing number is being converted to double
  double stringToDouble1(std::string aStr) {
      double d = stod(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
      return d;
  }

  // this function converts a string with numbers to a bool, only the first appearing number is being converted to bool
  bool stringToBool1(std::string aStr) {
      bool theBool = (aStr[0] == 't' || aStr[0] == 'T') ? true : false;
      return theBool;
  }
  //--------------------------------------------------------------------------------------------------------------------

  StatusResult  Row::decode(std::istream& aReader) {
      //flagship binary decode
      try {
          ValueReader theReader(aReader);
          theReader.binary_read(tableName);//Get table that it belongs to
          int theCount = 0;
          while (aReader.peek() != '\0') {
              std::string theKey;//Read key
              theReader.binary_read(theKey);
              bool isNull;//Read isNull
              theReader.binary_read(isNull);

              /*std::cout << "in storage decode row function, null indicating: " << std::endl;
              std::clog << "Key of the kv pair is: " << theKey << std::endl;
              std::clog << "null indicating: " << isNull << std::endl;*/

              Entry theValue;
              if (!isNull) {
                  theReader.binary_read(theValue);
              }

              data.push_back(NullableKVPair(theKey, std::pair<bool, Entry>(isNull, theValue)));//Add KVPair
              /*std::clog << std::endl;
              std::clog << "in row decode function, added one kv pair" << std::endl;
              show();
              std::clog << std::endl;*/
              theCount++;
          }

          return StatusResult(noError);
      }
      catch(std::exception){
          return StatusResult(readError);
      }
      /*---------------legacy string decode----------------
      std::getline(aReader, tableName);
      data.clear();
      while (aReader.good()) {
          std::string theKeyName;
          std::getline(aReader, theKeyName, ' ');//Set name of keyname
          if (theKeyName[0] == '\n') { theKeyName.erase(0,1); }
          if (theKeyName[0] != '\0') {
              std::string theType;
              std::string theValue;
              Entry theRealValue;
              aReader >> theType;
              aReader >> theValue;
              switch (theType[0]) {
              case 'i': {
                  theRealValue = stringToInt1(theValue);
                  break;}
              case 'd': {
                  theRealValue = stringToDouble1(theValue);
                  break;}
              case 's': {
                  theRealValue = theValue;
                  break;}
              case 'b': {
                  theRealValue = stringToBool1(theValue);
                  break;}}
              KeyValuePair theKeyValuePair(theKeyName, theRealValue);
              data.push_back(theKeyValuePair);
          }
          else break;
      }
      */
  }

  /*
  void Row::show() {
      std::clog << "blocknum: " << blockNum << std::endl;
      std::clog << "Row belongs to table: " << tableName << std::endl;
      for (int i = 0; i < data.size(); ++i) {
          std::clog << i << ": key name: " << data[i].aPair.first << " type: ";
          if (data[i].aPair.second.index() == 0) {
              std::clog << "int value: ";
              std::clog << std::get<int>(data[i].aPair.second) << std::endl;
          }
          else if (data[i].aPair.second.index() == 1) {
              std::clog << "double value: ";
              std::clog << std::get<double>(data[i].aPair.second) << std::endl;
          }
          else if (data[i].aPair.second.index() == 3) {
              std::clog << "std::string value: ";
              std::clog << std::get<std::string>(data[i].aPair.second) << std::endl;
          }
          else if (data[i].aPair.second.index() == 4) {
              std::clog << "bool value: ";
              std::clog << std::get<bool>(data[i].aPair.second) << std::endl;
          }
      }
  }*/

  void Row::showView(std::ostream& aStream) {

  }

  void Row::show() {
      std::clog << "blocknum: " << blockNum << std::endl;
      std::clog << "Row belongs to table: " << tableName << std::endl;
      for (int i = 0; i < data.size(); ++i) {
          std::clog << i << ": key name: " << data[i].first << " type: ";
          if (data[i].second.second.index() == 0) {
              std::clog << "int value: ";
              std::clog << std::get<int>(data[i].second.second) << std::endl;
          }
          else if (data[i].second.second.index() == 1) {
              std::clog << "double value: ";
              std::clog << std::get<double>(data[i].second.second) << std::endl;
          }
          else if (data[i].second.second.index() == 3) {
              std::clog << "std::string value: ";
              std::clog << std::get<std::string>(data[i].second.second) << std::endl;
          }
          else if (data[i].second.second.index() == 4) {
              std::clog << "bool value: ";
              std::clog << std::get<bool>(data[i].second.second) << std::endl;
          }
      }
  }

  Storable* Row::createRow(std::stringstream& aEncodedStream) {
      Row* theRow = new Row("");
      if (theRow->decode(aEncodedStream).code == noError) return theRow;
      else return nullptr;
      
  }
}

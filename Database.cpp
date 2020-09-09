//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <sstream>
#include "Database.hpp"
#include "View.hpp"
#include "Storage.hpp"
#include <sstream>
#include "RowCollection.hpp"

//this class represents the database object.
//This class should do actual database related work,
//we called upon by your db processor or commands

namespace ECE141 {
        //path to the folder where you want to store your DB's...
  std::string getDatabasePath2(const std::string &aDBName) {
    std::string thePath;
    
    //build a full path (in default storage location) to a given db file..
    StorageInfo Storage;
    std::string filePath = Storage.getDefaultStoragePath1();
    if (filePath[0] == '/') {
        thePath = filePath + "/" + aDBName + ".db";
    }
    else {
        thePath = filePath + "\\" + aDBName + ".db";
    }
    return thePath;
  }

  Database::Database(const std::string aName, CreateNewStorage)
    : name(aName), storage(aName, CreateNewStorage{}), indexCtrl(storage) {
        StorageInfo theInfo;
        std::string thePath = getDatabasePath2(aName);
      std::ofstream theDBFile(thePath);
      theDBFile.close();
        createDatabase(name);
        blockNum = 0;
  }
  
  Database::Database(const std::string aName, OpenExistingStorage)
    : name(aName), storage(aName, OpenExistingStorage{}), indexCtrl(storage) {
      blockNum = 0;
      StorageBlockCluster theCluster(storage);
      theCluster.load(0);
      decode(theCluster.getData());
  }
  
  Database::~Database() {
  }

  void Database::createDatabase(std::string& aName) { // putting meta block into the file
    //std::clog << "check why is called twice???" << std::endl;
    storage.createMeta(aName,CreateNewStorage{});

  }

  static std::map<ECE141::BlockType, std::string> blockType = {
    {ECE141::BlockType::meta_block, "Meta"},
    {ECE141::BlockType::data_block, "Data"},
    {ECE141::BlockType::entity_block, "Entity"},
    {ECE141::BlockType::free_block, "Free"},
    {ECE141::BlockType::index_block, "Index"},
    {ECE141::BlockType::unknown_block, "Unknown"}
  };
  //------------------------------------------------------------utility functions---------------------------------------------------------------
  //if (aStr.find_first_of("0123456789") == std::string::npos) { return std::nullopt; }
  // this function converts a string with numbers to an integer, only the first appearing number is being converted to int
  int stringToInt(std::string aStr) {
      int d = stoi(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
      return d;
  }

  // this function converts a string with numbers to a double, only the first appearing number is being converted to double
  double stringToDouble(std::string aStr) {
      double d = stod(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
      return d;
  }

  // this function converts a string with numbers to a bool, only the first appearing number is being converted to bool
  bool stringToBool(std::string aStr) {
      bool theBool = (aStr[0] == 't' || aStr[0] == 'T') ? true : false;
      return theBool;
  }

  bool compare(const std::pair<std::pair<char, int>, int>& i, const std::pair<std::pair<char, int>, int>& j)
  {
      return i.second < j.second;
  }
  //------------------------------------------------------------------------------------------------------------------------------------------
  //--------------------------------------------------------dont want to delete, look at these get some ideas--------------------------------------------------------------
      //  StorageInfo Storage;
      //std::string filePath = getDatabasePath2(aName);
      //std::ifstream ifs(filePath); // get to file
      //std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())); // put into string
      ////std::cout << content << std::endl;
      //std::cout << "Blk#  Type    Other\n----------------------------\n";
      //for (int i = 0; i < storage.getTotalBlockCount(); ++i) {
      //    std::string blockContent = content.substr(1024 * i, 1024);
      //    //std::clog << "blockContent: " << blockContent << std::endl;
      //    //std::string formatedString;
      //    std::cout << stringToInt(blockContent);
      //    if (0 <= stringToInt(blockContent) && stringToInt(blockContent) <= 9) {
      //        std::cout << "     " << blockType[static_cast<BlockType>(blockContent[0])] << std::endl;
      //    }
      //    else if (10 <= stringToInt(blockContent) && stringToInt(blockContent) <= 99) {
      //        std::cout << "    " << blockType[static_cast<BlockType>(blockContent[0])] << std::endl;
      //    }
      //    else if (100 <= stringToInt(blockContent) && stringToInt(blockContent) <= 999) {
      //        std::cout << "   " << blockType[static_cast<BlockType>(blockContent[0])] << std::endl;
      //    }
      //    else if (1000 <= stringToInt(blockContent) && stringToInt(blockContent) <= 9999) {
      //        std::cout << "  " << blockType[static_cast<BlockType>(blockContent[0])] << std::endl;
      //    }
      //    else if (10000 <= stringToInt(blockContent) && stringToInt(blockContent) <= 99999) {
      //        std::cout << " " << blockType[static_cast<BlockType>(blockContent[0])] << std::endl;
      //    }
      //}
  //---------------------------------------------------------------------------------------------------------------------------------------------------------------------

  //std::vector<std::pair<std::pair<char, int>, int>> Database::getFreeBlocks(std::string& aName) {
  //    std::vector<std::pair<std::pair<char, int>, int>> listOfFreeBlocks;
  //    StorageInfo Storage;
  //    std::string filePath = getDatabasePath2(aName);
  //    std::ifstream ifs(filePath); // get to file
  //    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())); // put into string
  //    for (int i = 0; i < storage.getTotalBlockCount(); ++i) {
  //        std::string blockContent = content.substr(1024 * i, 1024);
  //        int blockNum = stringToInt(blockContent);
  //        if (static_cast<BlockType>(blockContent[0]) == BlockType::free_block) {
  //            listOfFreeBlocks.push_back(std::make_pair(std::make_pair('F',0),blockNum));
  //        }
  //    }
  //    return listOfFreeBlocks;
  //}

  void Database::describeDatabase(std::string& aName) { // not able to display free blocks yet
        //Format the schema for display
        Variants theHeaderContent{ Entry(std::string("Block#")),Entry(std::string("Type")),Entry(std::string("Other")) };
        MyRow theHeader(theHeaderContent);
        Variants RowContent{ Entry(std::string("0")),Entry(std::string("meta")),Entry(std::string("")) };

        Sheet theBlocks;
        theBlocks.push_back(RowContent);

        std::vector<std::pair<std::pair<char, int>, int>> orderTwoVectors; // 'S' and 'R' and 'F'

        std::vector<Schema> listOfSchemas;
        std::vector<Row> listOfRows;
        storage.load(listOfSchemas, BlockType::entity_block); // get all schemas
        storage.load(listOfRows, BlockType::data_block); // get all rows
        for (int i = 0; i < listOfSchemas.size(); ++i) {
            orderTwoVectors.push_back(std::make_pair(std::make_pair('S', i), listOfSchemas[i].getBlockNum()));
            //listOfSchemas[i].show();
        }
        for (int i = 0; i < listOfRows.size(); ++i) {
            orderTwoVectors.push_back(std::make_pair(std::make_pair('R', i), listOfRows[i].getBlockNum()));
            //listOfRows[i].show();
        }
        //uncomment the following, FREE blocks will also be described
        /*std::vector<std::pair<std::pair<char, int>, int>> listOfFreeBlocks = getFreeBlocks(aName);
        for (int i = 0; i < listOfFreeBlocks.size(); ++i) {
            orderTwoVectors.push_back(listOfFreeBlocks[i]);
        }*/
        std::sort(orderTwoVectors.begin(), orderTwoVectors.end(), compare);
        for (int i = 0; i < orderTwoVectors.size(); ++i) {
            if (orderTwoVectors[i].first.first=='S') {
                Variants RowContent{ Entry(std::to_string(listOfSchemas[orderTwoVectors[i].first.second].getBlockNum())),Entry(std::string("schema")),Entry(std::string('\"'+listOfSchemas[orderTwoVectors[i].first.second].getName()+ '\"')) };
                theBlocks.push_back(RowContent);
            }
            else if (orderTwoVectors[i].first.first == 'R') {
                NullableKVList theListOfKV = listOfRows[orderTwoVectors[i].first.second].getKeyValueList();
                std::string theID;
                for (int j = 0; j < theListOfKV.size(); ++j) {
                    if (theListOfKV[j].first == "id") {
                        std::string theidContent = theListOfKV[j].second.second.index() == std::variant_npos ? "NULL" : std::to_string(std::get<int>(theListOfKV[j].second.second)) ;
                        theID = "id=" + theidContent + ",  ";
                    }
                }
                std::string theInformation = theID + '\"' + listOfRows[orderTwoVectors[i].first.second].getTableName() + '\"';
                Entry theInfo(theInformation);
                Variants RowContent{ Entry(std::to_string(listOfRows[orderTwoVectors[i].first.second].getBlockNum())),Entry(std::string("data")),theInfo };
                theBlocks.push_back(RowContent);
            }
            else if (orderTwoVectors[i].first.first == 'F') {
                Variants RowContent{ Entry(std::to_string(orderTwoVectors[i].second)),Entry(std::string("Free")),Entry(std::string("")) };
                theBlocks.push_back(RowContent);
            }
        }
        MyView theView(theHeader, theBlocks);
        theView.show(std::cout);
        //storage.checkFileSize();
        std::cout << "describe database " + name + " (ok)" << std::endl;
  }
    
  void Database::dropDatabase(std::string &aName){ // drop command is handled by DBCmdProcessor to delete files and delete in memory DB object, nothing here
      
  }

  StatusResult Database::createTable(Schema* aSchema) {
      //Check duplicate
      /*std::clog << "beginning create table function" << std::endl;
      aSchema->show();
      std::clog << "finish showing the schema passed in" << std::endl;*/
      if (!getSchema(aSchema->getName())) {
          //Encode
          std::stringstream theData(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
          aSchema->encode(theData);

          //Store
          StorageBlockCluster theCluster(storage);
          theCluster.store(theData, WriteMethod::WriteNew, BlockType::entity_block);

          //delete aSchema; // release 
          aSchema->blockNum = theCluster.getIndex();

          //Indexing
          uint32_t theSchemaIndex = indexCtrl.createIndex(*aSchema);
          tables[aSchema->getName()] = std::make_pair(aSchema->blockNum, theSchemaIndex); // store table name, schema block number and index block number in DB

          std::stringstream theMeta(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
          encode(theMeta);
          theCluster.load(0);
          theCluster.store(theMeta,WriteMethod::OverwriteExisting,BlockType::meta_block);


          return StatusResult(noError);
      }
      else {
          //delete aSchema; // release
          return StatusResult(tableExists);
      }

  }

  StatusResult Database::dropTable(std::string aName) {
      //std::clog << "database drop table function" << std::endl;
      //StatusResult theResult = storage.dropTable(aName);
      //return theResult;

      //New code
      //Find the schema and delete
      if (auto theSchema = getSchema(aName)) {
          uint32_t theIndexBlockNum = tables[aName].second;
          StorageBlockCluster theCluster(storage);
          theCluster.load(theIndexBlockNum);
          theCluster.freeBlocks();

          theCluster.load(theSchema->blockNum);
          theCluster.freeBlocks();

          tables.erase(aName);
          theCluster.load(0);
          std::stringstream theData(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
          encode(theData);
          theCluster.store(theData, WriteMethod::OverwriteExisting, BlockType::meta_block);
          
          //Find the rows and delete
          storage.loadEach([&aName](StorageBlockCluster& aCluster) {
              if (aCluster.getType() == static_cast<char>(BlockType::data_block)) {
                  Row* theRow = static_cast<Row*>(Row::createRow(aCluster.getData()));
                  //std::clog << (theRow && theRow->getTableName() == aName) << std::endl;
                  if (theRow && theRow->getTableName() == aName) {
                      aCluster.freeBlocks();
                  }
              }
              return true;
              });
          return StatusResult(noError);
      }
      else return StatusResult(unknownTable);
  }

  StatusResult Database::deleteFrom(std::string aName, Filters& aFilter) {
      //std::clog << "in database delete from function" << std::endl;
      //StatusResult theResult = storage.deleteFrom(aName);
      //return theResult;
      int count = 0;
      if (Schema* theSchema = getSchema(aName)) {
          storage.loadEach([&aFilter, &aName, &count](StorageBlockCluster& aCluster) {// have to user aFilter aRowCollection tableName to match rows and put into rowcollection, the closure function returns true and false to tell each function continue load or not, in the process
              if (aCluster.blocks[0].header.type == static_cast<char>(BlockType::data_block)) {
                  Row* theRow = dynamic_cast<Row*>(Row::createRow(aCluster.getData()));
                 
                  if (theRow != nullptr && theRow->getTableName() == aName && aFilter.rowMatches(theRow)) {
                      aCluster.freeBlocks(); // XXX HACKS
                      count++;
                      delete theRow;
                  }
                  
                  return true;
              }
              else return true;
              });

          std::cout << count << " rows affected ";
          //std::clog << "finished database delete from function" << std::endl;
          return StatusResult(noError, noError);
      }
      else {
          return StatusResult(unknownTable, unknownTable);
      }

      //if (auto theSchema = getSchema(aName)) {
      //    //Find the rows and delete
      //    std::vector<Storable*> theRows;
      //    storage.load(theRows, [&aName](StorageBlockCluster& aCluster) {
      //        Row* theRow = static_cast<Row*>(Row::createRow(aCluster.getData()));
      //        if (theRow && theRow->getTableName() == aName) {
      //            aCluster.freeBlocks();
      //            return theRow;
      //        }
      //        else return (Row*) nullptr;
      //        });
      //    delete theSchema;
      //    return StatusResult(noError);
      //}
      //else {
      //    return StatusResult(unknownTable, unknownTable);
      //}
  }

  Schema* Database::getSchema(const std::string& aName) {

      //Schema* theSchema = nullptr;
      //storage.loadEach([&theSchema, &aName](StorageBlockCluster& aCluster) {
      //    if (aCluster.getType() == static_cast<char>(BlockType::entity_block)) {
      //        theSchema = static_cast<Schema*>(Schema::createSchema(aCluster.getData()));
      //        theSchema->blockNum = aCluster.blocks[0].header.id;
      //        if (theSchema->getName() == aName) return false;
      //        else {
      //            delete theSchema;
      //            theSchema = nullptr;
      //        }
      //    }
      //    return true;
      //    });
      //return theSchema;

      if (tables.find(aName) != tables.end()) {
          Schema* theSchema = new Schema(aName);
          storage.load(*theSchema, tables[aName].first);
          return theSchema;
      }
      else return nullptr;

  }
  std::vector<Schema*> Database::getSchemas() {
      std::vector<Schema*> toReturn;
      //storage.loadEach([&theStorables](StorageBlockCluster& aCluster) {
      //    if (aCluster.getType() == static_cast<char>(BlockType::entity_block)) {
      //        theStorables.push_back(Schema::createSchema(aCluster.getData()));
      //        theStorables.back()->blockNum = aCluster.getIndex();
      //        return true;
      //    }
      //    else return true;
      //    });
      for (auto theMap : tables) {
          Schema* theSchema = new Schema(theMap.first);
          storage.load(*theSchema, theMap.second.first);
          toReturn.push_back(theSchema);
      }


      return toReturn;
  }

  StatusResult Database::insert(std::string tableName, std::vector<std::vector<Token>> rowTokens, Schema tableSchema) {
      std::clog << "in database insert function" << std::endl;
      //tableSchema.show();
      for (int k = 1; k < rowTokens.size(); ++k) { // iterate through values in rowTokens, starts with 1 since 0 is attribute names
          Row theRow(tableName);
          for (int i = 0; i < tableSchema.getAttributes().size(); ++i) { // iterate throught attributes like: id name status, each row has all these attributes
              NullableKVPair theKVPair(tableSchema.getAttributes()[i].getName(),std::pair<bool,Entry>(true, Entry()));

              //handle auto increment
              if (tableSchema.getAttributes()[i].isAutoIncrement()) {
                  std::vector<Row> listOfRows;
                  std::vector<Row> listOfRowsInThisTableName;
                  std::vector<int> numberList;
                  storage.load(listOfRows, BlockType::data_block);
                  for (int a = 0; a < listOfRows.size(); ++a) {
                      if (listOfRows[a].getTableName() == tableName) {
                          listOfRowsInThisTableName.push_back(listOfRows[a]); // push in existing rows in the table
                      }
                  }
                  for (int b = 0; b < listOfRowsInThisTableName.size(); ++b) { // iterate throught these existing rows in the table
                      for (int c = 0; c < listOfRowsInThisTableName[b].getKeyValueList().size(); ++c) { // iterate throught all the key value pairs in the row, find the auto-increment one
                          if (listOfRowsInThisTableName[b].getKeyValueList()[c].first == tableSchema.getAttributes()[i].getName()) { // found auto increment 
                              if (listOfRowsInThisTableName[b].getKeyValueList()[c].second.second.index() == 0) { // make sure is int type
                                  numberList.push_back(std::get<int>(listOfRowsInThisTableName[b].getKeyValueList()[c].second.second)); // push in existing int number
                              }
                          }
                      }
                  }
                  std::sort(numberList.begin(), numberList.end()); // biggest in the end
                  theKVPair.second.first = false;
                  if (numberList.size()) {
                      theKVPair.second.second = numberList[numberList.size() - 1] + 1; // increment the value
                  }
                  else {
                      theKVPair.second.second = 1;
                  }
                  theRow.getKeyValueList().push_back(theKVPair);
                  continue;
              }

              
              for (int j = 0; j < rowTokens[0].size(); ++j) { // iterate through tokens like: id name                  
                  if (tableSchema.getAttributes()[i].getName() == rowTokens[0][j].data) { // found token, j is the position                     
                      if (tableSchema.getAttributes()[i].getType() == DataType::int_type) {
                          //if (rowTokens[k][j].data.find_first_of("0123456789") != std::string::npos)
                          theKVPair.second.first = false;
                          theKVPair.second.second = stringToInt(rowTokens[k][j].data);
                      }
                      else if (tableSchema.getAttributes()[i].getType() == DataType::float_type) { // double
                          //if (rowTokens[k][j].data.find_first_of("0123456789") != std::string::npos) 
                          theKVPair.second.first = false;
                          theKVPair.second.second = stringToDouble(rowTokens[k][j].data);
                      }
                      else if (tableSchema.getAttributes()[i].getType() == DataType::varchar_type) {
                          theKVPair.second.first = false;
                          theKVPair.second.second = rowTokens[k][j].data;
                      }
                      else if (tableSchema.getAttributes()[i].getType() == DataType::bool_type) {
                          theKVPair.second.first = false;
                          theKVPair.second.second = stringToBool(rowTokens[k][j].data);
                      }
                      else {
                          theKVPair.second.second = tableSchema.getAttributes()[i].defaultValue;
                          theKVPair.second.first = theKVPair.second.second.index() == std::variant_npos ? true : false;
                      }
                  }
              }
              if (theKVPair.second.first) {
                  theKVPair.second.second = tableSchema.getAttributes()[i].defaultValue;
                  theKVPair.second.first = theKVPair.second.second.index() == std::variant_npos ? true : false;
              }


              theRow.getKeyValueList().push_back(theKVPair);
          }
          //theRow.show(); // show all the row objects made
          //storage.insertRow(&theRow);//Legacy code

          //Store onto disk -- Flagship code
          std::stringstream theData;
          theRow.encode(theData);

          /*std::clog << "database insert function made row obj:" << std::endl;
          theRow.show();*/
          
          uint32_t blockNum = storage.insertRow(&theRow);

          /*for (auto item : tables) {
              std::clog << item.first << std::endl;
          }*/

          /*for (auto item : indexCtrl.getIndexes()) {
              std::clog << item.first << std::endl;
              for (auto element : item.second) {
                  std::clog << element.first << std::endl;
                  std::clog << element.second.getFieldName() << std::endl;
                  for (auto each : element.second.getList()) {
                      std::clog << std::get<int>(each.first) << std::endl;
                  }
              }
          }*/

          //std::clog << tables.count(tableName) << std::endl;

          //if (tables.count(tableName)) { // tables exist the indexed table
          //    uint32_t tableId = tables[tableName].first; // get table/schema block num
          //    std::clog << tableId << std::endl;
          //    if (indexCtrl.getIndexes().count(tableId)) { // the table exist indexes
          //        std::clog << "reached here" << std::endl;
          //        std::clog << indexCtrl.getIndexes().count(tableId) << std::endl;
          //        std::clog << indexCtrl.getIndexes()[tableId].size() << std::endl;
          //        for (std::pair<field, Index> eachIndex : indexCtrl.getIndexes()[tableId]) { // iterate through indexes
          //            std::clog << "existing index fields for table: " << tableName << " field: " << eachIndex.first << std::endl;
          //        }
          //    }
          //}
          

          if (tables.count(tableName)) { // tables exist the indexed table
              uint32_t tableId = tables[tableName].first; // get table/schema block num
              if (indexCtrl.getIndexes().count(tableId)) { // the table exist indexes
                  for (std::pair<field, Index> eachIndex : indexCtrl.getIndexes()[tableId]) { // iterate through indexes
                      for (int i = 0; i < theRow.getKeyValueList().size(); ++i) { // iterate through all kv pairs in a row
                          if (theRow.getKeyValueList()[i].first == eachIndex.first) { // field matches
                              if (theRow.getKeyValueList()[i].second.first == false) { // not null
                                  //eachIndex.second.addKeyValue(theRow.getKeyValueList()[i].second.second, blockNum);
                                  indexCtrl.getIndexes()[tableId][eachIndex.first].addKeyValue(theRow.getKeyValueList()[i].second.second, blockNum);
                                  //std::clog << "inserted into field: " << eachIndex.first << std::endl;
                                  //std::clog << "inserted into index the value: " << std::get<int>(theRow.getKeyValueList()[i].second.second) << std::endl;
                                  //std::clog << "the block num associated with is: " << blockNum << std::endl;
                              }
                              else { // nullable kv
                                  eachIndex.second.addKeyValue(0, blockNum); // default insert 0
                              }
                          }
                      }
                  }
              }
          }

          /*for (auto item : indexCtrl.getIndexes()) {
              std::clog << item.first << std::endl;
              for (auto element : item.second) {
                  std::clog << element.first << std::endl;
                  std::clog << element.second.getFieldName() << std::endl;
                  for (auto each : element.second.getList()) {
                      std::clog << std::get<int>(each.first) << ' ' << each.second << std::endl;
                  }
              }
          }*/

          //std::clog << "reached here" << std::endl;

          /*StorageBlockCluster theCluster(storage);
          return theCluster.store(theData, WriteMethod::WriteNew, BlockType::data_block);*/
      }
      return StatusResult(noError, noError);
  }

  StatusResult Database::select(std::string tableName, Filters& aFilter, RowCollection& aRowCollection) {
      //std::clog << "in database select function" << std::endl;
      //================================================================haoru, second portion returned rowCollection without any rows in it, so I used this first portion
      // after second portion working "where cluse" you can comment out this first portion, Best, David==========================================================
      //if (Schema* theSchema = getSchema(tableName)) {
      //    //std::clog << "found schema obj exist" << std::endl;
      //    std::vector<Row> allRows;
      //    storage.load(allRows, BlockType::data_block);
      //    //std::clog << "displaying: " << std::endl;
      //    /*for (int i = 0; i < allRows.size(); ++i) {
      //        allRows[i].show();
      //    }*/
      //    std::vector<Row> rowsInTable; // put all rows belong to the table
      //    for (int i = 0; i < allRows.size(); ++i) {
      //        if (allRows[i].getTableName() == tableName) {
      //            rowsInTable.push_back(allRows[i]);
      //        }
      //    }
      //    for (int i = 0; i < rowsInTable.size(); ++i) {
      //        aRowCollection.getRowCollections().push_back(new Row(rowsInTable[i])); // make all rows on heap
      //    }
      //    /*std::clog << "collection showing all rows: " << std::endl;
      //    for (int i = 0; i < arowcollection.getrowcollections().size(); ++i) {
      //        arowcollection.getrowcollections()[i]->show();
      //    }*/
      //    return StatusResult(noError, noError);
      //}
      //else return StatusResult{ unknownTable };
      //===========================================================================================================
      if (Schema* theSchema = getSchema(tableName)) {
          storage.loadEach([&aFilter, &aRowCollection, &tableName](StorageBlockCluster& aCluster) {
              if (aCluster.blocks[0].header.type == static_cast<char>(BlockType::data_block)) {
                  Row* theRow = dynamic_cast<Row*>(Row::createRow(aCluster.getData()));
                  if (theRow != nullptr && theRow->getTableName() == tableName && aFilter.rowMatches(theRow)) {
                      theRow->setBlockNum(aCluster.blocks[0].header.id);
                      aRowCollection.getRowCollections().push_back(theRow);
                  }
                  return true;
              }
              else return true;
              });


          //std::clog << "Row collection size is " <<aRowCollection.getRowCollections().size() << std::endl;
      }
      else return StatusResult{ unknownTable };

      return StatusResult(noError, noError);
  }

  bool Database::selectWithIndex(std::string tableName, Filters& aFilter, RowCollection& aRowCollection) {
      //std::clog << "in database select with index function" << std::endl;
      std::vector<std::vector<Token>> expressions = aFilter.getExpressionTokens();
      for (int i = 0; i < expressions.size(); ++i) {
          for (int j = 0; j < expressions[i].size(); ++j) {
              if (expressions[i][j].keyword == Keywords::and_kw || expressions[i][j].keyword == Keywords::or_kw) {
                  //std::clog << "has relation" << std::endl;
                  return false;
              }
          }
      }
      // reached here means no relation
      std::string attrNameAfterWhere;
      for (int i = 0; i < expressions.size(); ++i) {
          for (int j = 0; j < expressions[i].size(); ++j) {
              if (expressions[i][j].keyword == Keywords::where_kw && j + 1 < expressions[i].size()) {
                  attrNameAfterWhere = expressions[i][j+1].data;
              }
          }
      }
      //std::clog << "attr name after where is: " << attrNameAfterWhere << std::endl;
      // now finding the index
      int tableBlockNum = 0;
      if (tables.count(tableName)) {
          tableBlockNum = tables[tableName].first;
      }
      //std::clog << "the block num found is: " << tableBlockNum << std::endl;
      Index* theIndex = nullptr;
      if (indexCtrl.getIndexes().count(tableBlockNum)) {
          if (indexCtrl.getIndexes()[tableBlockNum].count(attrNameAfterWhere)) {
              theIndex = &indexCtrl.getIndexes()[tableBlockNum][attrNameAfterWhere];
          }
      }
      if (theIndex == nullptr) {
          //std::clog << "index is not found" << std::endl;
          return false;
      }
      // index is found
      /*for (auto items : theIndex->getList()) {
          std::clog << std::get<int>(items.first) << ' ' << items.second << std::endl;
          std::clog << aFilter.elementInValueMapMatches(items) << std::endl;
      }*/
      std::vector<std::pair<Entry, uint32_t>> indexPairsMatch; // use this to get blocks
      for (auto items : theIndex->getList()) {
          if (aFilter.elementInValueMapMatches(items)) {
              indexPairsMatch.push_back(items);
          }
      }
      //std::clog << "items ok" << std::endl;
      for (int i = 0; i < indexPairsMatch.size(); ++i) {
          StorageBlock theRowBlock;
          storage.readBlock(theRowBlock, indexPairsMatch[i].second);
          //std::clog << "block read ok" << std::endl;
          std::string theData;
          for (int j = 0; j < kPayloadSize; j++) {
              theData += theRowBlock.data[j];
          }
          Row* theRow = new Row;
          std::stringstream theDataStream(theData);
          theRow->decode(theDataStream);
          //std::clog << "block decode ok" << std::endl;
          theRow->setBlockNum(theRowBlock.header.id);
          aRowCollection.getRowCollections().push_back(theRow);
      }
      //std::clog << "collection ok" << std::endl;
      return true;
  }

  StatusResult Database::update(std::string tableName, Filters& aFilter, RowCollection& aRowCollection) {
      //std::clog << "in database update function" << std::endl;
      ////================================================================haoru, second portion returned rowCollections with nothing or rolls without block indexs
      ////================================================================Swtich to second part later
      //if (Schema* theSchema = getSchema(tableName)) {
      //    //std::clog << "found schema obj exist" << std::endl;
      //    std::vector<Row> allRows;
      //    storage.load(allRows, BlockType::data_block);
      //    //std::clog << "displaying: " << std::endl;
      //    /*for (int i = 0; i < allRows.size(); ++i) {
      //        allRows[i].show();
      //    }*/
      //    std::vector<Row> rowsInTable; // put all rows belong to the table
      //    for (int i = 0; i < allRows.size(); ++i) {
      //        if (allRows[i].getTableName() == tableName) {
      //            rowsInTable.push_back(allRows[i]);
      //        }
      //    }
      //    for (int i = 0; i < rowsInTable.size(); ++i) {
      //        aRowCollection.getRowCollections().push_back(new Row(rowsInTable[i])); // make all rows on heap
      //    }
      //    /*std::clog << "collection showing all rows: " << std::endl;
      //    for (int i = 0; i < aRowCollection.getRowCollections().size(); ++i) {
      //        aRowCollection.getRowCollections()[i]->show();
      //    }*/
      //    //std::clog << "size of expressions vector in filters: " << aFilter.getExpressionTokens().size() << std::endl;
      //    std::vector<std::vector<Token>> expressionTokens = aFilter.getExpressionTokens();
      //    /*for (int i = 0; i < expressionTokens.size(); ++i) {
      //        for (int j = 0; j < expressionTokens[i].size(); ++j) {
      //            std::clog << expressionTokens[i][j].data << ' ';
      //        }
      //        std::clog << std::endl;
      //    }*/
      //    std::string theKey;
      //    DataType theValueType = DataType::no_type;
      //    for (int i = 0; i < theSchema->getAttributes().size(); ++i) {
      //        if (expressionTokens.size() && expressionTokens[0].size() == 4) { // only works for 4, for example: WHERE name='foobar'
      //            if (theSchema->getAttributes()[i].getName() == expressionTokens[0][1].data) { // find the matching attribute name
      //                theKey = theSchema->getAttributes()[i].getName();
      //                theValueType = theSchema->getAttributes()[i].getType(); // find the matching attribute type
      //            }
      //        }
      //    }
      //    Entry theValue;
      //    if (theValueType == DataType::varchar_type) {
      //        if (expressionTokens.size() && expressionTokens[0].size() == 4) {
      //            theValue = expressionTokens[0][3].data;
      //        }
      //    }
      //    else if (theValueType == DataType::int_type) {
      //        if (expressionTokens.size() && expressionTokens[0].size() == 4) {
      //            theValue = stringToInt(expressionTokens[0][3].data);
      //        }
      //    }
      //    else if (theValueType == DataType::float_type) {
      //        if (expressionTokens.size() && expressionTokens[0].size() == 4) {
      //            theValue = stringToDouble(expressionTokens[0][3].data);
      //        }
      //    }
      //    else if (theValueType == DataType::bool_type) {
      //        if (expressionTokens.size() && expressionTokens[0].size() == 4) {
      //            theValue = stringToBool(expressionTokens[0][3].data);
      //        }
      //    }
      //    else if (theValueType == DataType::no_type) {
      //        return StatusResult(syntaxError, syntaxError);
      //    }
      //    //============================filtering=====================================================
      //    for (int i = 0; i < aRowCollection.getRowCollections().size(); ++i) { // iterate through all rows in row collection
      //        if (theValueType == DataType::varchar_type) {
      //            std::string theValueString = std::get<std::string>(theValue);
      //            //std::clog << "theValueString: " << theValueString << std::endl;
      //            for (int j = 0; j < aRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++j) { // iterate through KVPairs
      //                if (aRowCollection.getRowCollections()[i]->getKeyValueList()[j].first == theKey && aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.first == false && std::get<std::string>(aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second) != theValueString) {
      //                    //std::clog << "reached here" << std::endl;
      //                    aRowCollection.getRowCollections().erase(aRowCollection.getRowCollections().begin() + i);
      //                    --i;
      //                    break;
      //                }
      //            }
      //        }
      //        else if (theValueType == DataType::int_type) {
      //            int theValueInt = std::get<int>(theValue);
      //            for (int j = 0; j < aRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++j) { // iterate through KVPairs
      //                if (aRowCollection.getRowCollections()[i]->getKeyValueList()[j].first == theKey && aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.first == false && std::get<int>(aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second) != theValueInt) {
      //                    aRowCollection.getRowCollections().erase(aRowCollection.getRowCollections().begin() + i);
      //                    --i;
      //                    break;
      //                }
      //            }
      //        }
      //        else if (theValueType == DataType::float_type) {
      //            double theValueDouble = std::get<double>(theValue);
      //            for (int j = 0; j < aRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++j) { // iterate through KVPairs
      //                if (aRowCollection.getRowCollections()[i]->getKeyValueList()[j].first == theKey && aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.first == false && std::get<double>(aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second) != theValueDouble) {
      //                    aRowCollection.getRowCollections().erase(aRowCollection.getRowCollections().begin() + i);
      //                    --i;
      //                    break;
      //                }
      //            }
      //        }
      //        else if (theValueType == DataType::bool_type) {
      //            bool theValueBool = std::get<bool>(theValue);
      //            for (int j = 0; j < aRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++j) { // iterate through KVPairs
      //                if (aRowCollection.getRowCollections()[i]->getKeyValueList()[j].first == theKey && aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.first == false && std::get<bool>(aRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second) != theValueBool) {
      //                    aRowCollection.getRowCollections().erase(aRowCollection.getRowCollections().begin() + i);
      //                    --i;
      //                    break;
      //                }
      //            }
      //        }
      //    }
      //    /*std::clog << "collection showing all rows: " << std::endl;
      //    for (int i = 0; i < aRowCollection.getRowCollections().size(); ++i) {
      //        aRowCollection.getRowCollections()[i]->show();
      //    }*/
      //    return StatusResult(noError, noError);
      //}
      //else return StatusResult{ unknownTable, unknownTable };
      //===========================================================================================================
      if (Schema* theSchema = getSchema(tableName)) {
          storage.loadEach([&aFilter, &aRowCollection, &tableName](StorageBlockCluster& aCluster) {// have to user aFilter aRowCollection tableName to match rows and put into rowcollection, the closure function returns true and false to tell each function continue load or not, in the process
              if (aCluster.blocks[0].header.type == static_cast<char>(BlockType::data_block)) {
                  Row* theRow = dynamic_cast<Row*>(Row::createRow(aCluster.getData()));
                  if (theRow != nullptr && theRow->getTableName() == tableName && aFilter.rowMatches(theRow)) {
                      theRow->setBlockNum(aCluster.blocks[0].header.id);
                      aRowCollection.getRowCollections().push_back(theRow);
                  }
                  return true;
              }
              else return true;
              });

          return StatusResult(noError, noError);
      }
  }

  StatusResult Database::updateARow(Row aRow) {
      //std::clog << "updateARow function" << std::endl;
      //aRow.show();
      //StorageBlock theUpdatingBlock;
      //storage.readBlock(theUpdatingBlock, aRow.getBlockNum());
      std::stringstream kloadData;
      //aRow.encode(kloadData);
      ////kloadData.seekg(std::ios::beg);
      //for (int i = 0; i < 1012; ++i) {
      //    theUpdatingBlock.data[i] = kloadData.get();
      //}
      //storage.writeBlock(theUpdatingBlock, aRow.getBlockNum());
      StorageBlockCluster theCluster(storage);
      theCluster.load(aRow.getBlockNum());
      aRow.encode(kloadData);
      theCluster.store(kloadData, WriteMethod::OverwriteExisting, BlockType::data_block);
      return StatusResult(noError, noError);
  }

  StatusResult  Database::encode(std::ostream& aWriter){
      ValueWriter theWriter(aWriter);

      for (auto& theTable : tables) {
          theWriter.operator()(theTable.first);
          theWriter.operator()(theTable.second.first);
          theWriter.operator()(theTable.second.second);
      }

      return StatusResult();
  }
  StatusResult  Database::decode(std::istream& aReader) {
      ValueReader theReader(aReader);
      while (aReader.peek() != '\0') {
          std::string theTableName;
          theReader.binary_read(theTableName);

          TableBlockNum theTableNum = 0;
          theReader.binary_read(theTableNum);

          IndexBlockNum theIndexNum = 0;
          theReader.binary_read(theIndexNum);

          tables[theTableName] = std::make_pair(theTableNum, theIndexNum);
      }

      return StatusResult();
  }

  StatusResult Database::showIndexes() {
      Variants theHeaderContent;
      theHeaderContent.push_back(Entry(std::string("table")));
      theHeaderContent.push_back(Entry(std::string("field")));
      MyRow theHeader(theHeaderContent);
      Sheet theBlocks;

      Variants RowContent; // this is one row

      for (auto eachItem : tables) {
          if (indexCtrl.getIndexes().count(eachItem.second.first)) {
              for (auto eachElement : indexCtrl.getIndexes()[eachItem.second.first]) {
                  RowContent.push_back(eachItem.first);
                  RowContent.push_back(eachElement.first);
                  theBlocks.push_back(RowContent);
                  RowContent.clear();
              }
          }
      }


      MyView theView(theHeader, theBlocks);
      theView.show(std::cout);
      return StatusResult(noError, noError);
  }
}


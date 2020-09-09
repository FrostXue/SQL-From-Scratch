//
//  Storage.cpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include <fstream>
#include <cstdlib>
#include "FolderReader.hpp"
#include "Helpers.hpp"
#include <sstream>
#include <string>

namespace ECE141 {

  // USE: Our main class for managing storage...
  const char* StorageInfo::getDefaultStoragePath1() {
    //STUDENT -- MAKE SURE TO SET AN ENVIRONMENT VAR for DB_PATH! 
    //           This lets us change the storage location during autograder testing

    //WINDOWS USERS:  Use forward slash (/) not backslash (\) to separate paths.
    //                (Windows convert forward slashes for you)
    //return "D:"; // change back to get env later
    //return "D:\\"; // change back to get env later
    const char* thePath = std::getenv("DB_PATH");
    return thePath;
  }

  //----------------------------------------------------------
  //path to the folder where you want to store your DB's...
  std::string getDatabasePath(const std::string &aDBName) {
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


  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName, CreateNewStorage) : name(aName) {
    std::string thePath = getDatabasePath(name);
    //try to create a new db file in known storage location.
    FolderReader theReader(thePath.c_str());
        //throw error if it fails...
    if (theReader.exists(thePath)) 
      throw std::runtime_error("File does not exist");
    else{
      //createMeta(aName,CreateNewStorage{});
    }
  }

  // USE: ctor ---------------------------------------
  Storage::Storage(const std::string aName, OpenExistingStorage) : name(aName) {
    std::string thePath = getDatabasePath(aName);
    //try to OPEN a db file a given storage location
    //if it fails, throw an error
  }

  // USE: dtor ---------------------------------------
  Storage::~Storage() {
    stream.close();
  }

  // USE: validate we're open and ready ---------------------------------------
  bool Storage::isReady() const {
    return stream.is_open();
  }

  // USE: count blocks in file ---------------------------------------
  uint32_t Storage::getTotalBlockCount() {
    //how can we compute the total number of blocks in storage?
      std::ifstream ifs(getDatabasePath(name),std::ostream::ate | std::ostream::binary); // get to file
      uint32_t theSize = ifs.tellg();
      //std::clog << "storage.getTotalBlockCount size: " << theSize << std::endl;
      return theSize / 1024;

      //std::ifstream ifs(getDatabasePath(name)); // get to file
      //std::string content((std::istreambuf_iterator<char>(ifs)),
      //    (std::istreambuf_iterator<char>())); // put into string
      //uint32_t theCount = content.length() / 1024; // calculate how many blocks
      //return theCount;
  }

  // Call this to locate a free block in this storage file.
  // If you can't find a free block, then append a new block and return its blocknumber
  StatusResult Storage::findFreeBlockNum() { //XXX overwrite make next block unrecoginzable, now only appending, caution!!!
      StatusResult toReturn(noError);
      int i = 0;
      StorageBlock theBlock;

      do {
          toReturn = readBlock(theBlock, i++);
      }
      while (toReturn.code != seekError && theBlock.header.type != static_cast<char>(BlockType::free_block));

      if (toReturn.code == noError) {
          toReturn.value = --i;
      }
      else if (toReturn.code == seekError) {
          StorageBlock theFreeBlock(BlockType::free_block);
          theFreeBlock.header.id = --i;
          appendBlock(theFreeBlock);
          //std::clog << "storage.findFreeBlock, after appended a free block, block num: " << i << std::endl;
          return StatusResult(noError, i);
          
      }
      //std::clog << "storage.findFreeBlockNum, free block num found: " << i << std::endl;
      return toReturn;

    return StatusResult{noError}; //return blocknumber in the 'value' field...


    /*StorageBlock theFreeBlock(BlockType::free_block);
    theFreeBlock.header.id = getTotalBlockCount();
    appendBlock(theFreeBlock);
    std::clog << "storage.findFreeBlock, after appended a free block, block num: " << theFreeBlock.header.id << std::endl;
    return StatusResult(noError, theFreeBlock.header.id);*/
  }

  //Claim occupation on the latest freeblock and lock it
  StatusResult Storage::claimFreeBlockUsage(uint32_t aBlockNum) {
      auto theNum = findFreeBlockNum().value;

      if (aBlockNum == theNum) {//Confirm that the block is free
          StorageBlock theBlock;
          readBlock(theBlock, theNum);
          theBlock.header.type = static_cast<char>(BlockType::unknown_block); //Claim usage
          theBlock.header.id = aBlockNum;
          writeBlock(theBlock, aBlockNum);

          return StatusResult{ noError };
      }
      else return StatusResult{ storageFull };
  }

  // USE: for use with "storable API" [NOT REQUIRED -- but very useful]

  //StatusResult Storage::save(Storable &aStorable) {
  //  //High-level IO: save a storable object (like a table row)...
  //    StorageBlockCluster theCluster(*this);
  //    std::stringstream theStream;
  //    aStorable.encode(theStream);
  //    theCluster.store(theStream, WriteMethod::WriteNew, aStorable.getType());
  //   return StatusResult{noError};
  //}

// USE: for use with "storable API" [NOT REQUIRED -- but very useful]

  //StatusResult Storage::load(Storable &aStorable) {
  //  //high-level IO: load a storable object (like a table row)
  //    StorageBlockCluster theCluster(*this);
  //    theCluster.load(aStorable.blockNum);
  //    return aStorable.decode(theCluster.getData());

  //  //return StatusResult{noError};
  //}


  //StatusResult    Storage::load(Storable& aStorable,  std::string aName) {
  //    StorageBlock theBlock;
  //    int i = 0;
  //    while (auto theResult = readBlock(theBlock,i)) {
  //        if (theResult.code != seekError) {
  //            std::string theData;
  //            for (int j = 0; j < kPayloadSize; j++) {
  //                theData += theBlock.data[j];
  //            }
  //            std::stringstream theDataStream(theData);
  //            std::string theName;
  //            theDataStream >> theName;
  //            if (theName == aName) {
  //                theDataStream.str(theData);
  //                aStorable.decode(theDataStream);
  //                break;
  //            }
  //            i++;
  //        }
  //        else break;
  //    }

  //    return StatusResult(noError);
  //}

  //StatusResult Storage::load(std::vector<Storable*>& aStorables, std::function<bool(StorageBlockCluster&)> aPredicate, std::function<Storable*(std::istream&)> aCreator) {
  //    int i = 0;
  //    aStorables.clear();
  //    while (true) {
  //        StorageBlockCluster theCluster(*this);
  //        auto theResult = theCluster.load(i);
  //        if (theResult.code == noError) {
  //            if (aPredicate(theCluster)) {
  //                aStorables.push_back(aCreator(theCluster.getData()));
  //                aStorables.back()->blockNum = theCluster.blocks[0].header.id;
  //            }
  //        }
  //        else if (theResult.code == seekError) return StatusResult{};
  //        i++;
  //    }

  //    return StatusResult{};
  //}

  StatusResult    Storage::load(std::vector<Schema>& aStorables, BlockType aType) {
      StorageBlock theBlock;
      int i = 0;
      //std::clog << "total block count: " << getTotalBlockCount() << std::endl;
      while (auto theResult = readBlock(theBlock, i)) {
          if (theResult.code != seekError) {
              //std::clog << "the header type" << theBlock.header.type << std::endl;
              //std::clog << "showing data in block: " << theBlock.data << std::endl;
              if (theBlock.header.type == static_cast<char>(aType)) {
                  //std::clog << "found schema type" << std::endl;
                  std::string theData;
                  for (int j = 0; j < kPayloadSize; j++) {
                      theData += theBlock.data[j];
                  }
                  aStorables.push_back(Schema("null"));
                  //std::clog << "pushing back schema objects" << std::endl;
                  std::stringstream theDataStream(theData);
                  aStorables.back().decode(theDataStream); // over here set table names, attributes in the schema
                  aStorables.back().setBlockNum(i);
              }
              
          }
          else break;
          i++;
      }


      return StatusResult(noError);
  }

  StatusResult    Storage::load(std::vector<Row>& aStorables, BlockType aType) {
      StorageBlock theBlock;
      int i = 0;
      while (auto theResult = readBlock(theBlock, i)) {
          if (theResult.code != seekError) {
              if (theBlock.header.type == static_cast<char>(aType)) {
                  std::string theData;
                  for (int j = 0; j < kPayloadSize; j++) {
                      theData += theBlock.data[j];
                  }
                  Row theRow;
                  std::stringstream theDataStream(theData);
                  theRow.decode(theDataStream);
                  theRow.setBlockNum(i);
                  aStorables.push_back(theRow);

              }

          }
          else break;
          i++;
      }

      return StatusResult(noError);



  }

  StatusResult    Storage::load(Storable& aStorable, uint32_t aBlockNum) {
          StorageBlockCluster theCluster(*this);
          theCluster.load(aBlockNum);
          aStorable.decode(theCluster.getData());
          aStorable.blockNum = aBlockNum;
          return StatusResult();
  }

  //StatusResult Storage::load(std::vector<Storable*>& aStorables, std::function<Storable* (StorageBlockCluster&)> aCreator) {
  //    int i = 0;
  //    aStorables.clear();
  //    while (true) {
  //        StorageBlockCluster theCluster(*this);
  //        auto theResult = theCluster.load(i);
  //        if (theResult.code == noError) {
  //            if (auto theStorable = aCreator(theCluster)) {
  //                aStorables.push_back(theStorable);
  //                aStorables.back()->blockNum = theCluster.blocks[0].header.id;
  //            }
  //        }
  //        else if (theResult.code == seekError) return StatusResult{};
  //        i++;
  //    }
  //    return StatusResult{};
  //}

  StatusResult Storage::loadEach(std::function <bool(StorageBlockCluster&)> aLoopCondition) {
      int i = 0;
      while (true) {
          StorageBlockCluster theCluster(*this);
          auto theResult = theCluster.load(i);
          if (theResult.code == noError) {
              if (!aLoopCondition(theCluster)) return StatusResult{};
          }

          else if (theResult.code == seekError) return theResult;
          i++;
      }
      //std::clog << i << " blocks are loaded in loadEach\n";
      return StatusResult{};
  }

  StatusResult Storage::update(Storable& aStorable) {
      StorageBlockCluster theCluster(*this);
      std::stringstream theStream;
      aStorable.encode(theStream);
      return theCluster.store(theStream, WriteMethod::OverwriteExisting, aStorable.getType());
      
  }


  // USE: write data a given block (after seek)
  StatusResult Storage::writeBlock(StorageBlock &aBlock, uint32_t aBlockNumber) { 
    //STUDENT: Implement this; this is your low-level block IO...
      std::ofstream MyFile;
      MyFile.open(getDatabasePath(name), std::ofstream::in | std::ofstream::binary); // get the stream for writing block
      
      //Locate the block number
      //std::clog << "in writing function, modifying block: " << aBlockNumber << std::endl;
      uint32_t thePosition = aBlockNumber * 1024;
      MyFile.seekp(thePosition);

      //Write
      aBlock.store(MyFile); // put in data
      MyFile.close(); // close stream

      //std::ifstream MyFileCopy;
      //MyFileCopy.open(getDatabasePath(name));
      //std::string content((std::istreambuf_iterator<char>(MyFileCopy)),
          //(std::istreambuf_iterator<char>())); // put into string
      //std::cout << "after writing, length of the database is: " << content.length() << std::endl;
      //MyFileCopy.close();
    return StatusResult{};
  }

  //append block at the end of storage, return the appended index
  StatusResult    Storage::appendBlock(StorageBlock& aBlock) { // use header.id for block index purpose
      std::ofstream MyFile;
      MyFile.open(getDatabasePath(name), std::ofstream::in | std::ofstream::binary); // get the stream for writing block
      MyFile.seekp((unsigned long long)aBlock.header.id * 1024); // depends on block header id
      StatusResult toReturn{};
      
      //Write
      
      aBlock.store(MyFile); // put in data to the file stream
      MyFile.close(); // close stream
      // check on appending situation
     // std::ifstream MyFileCopy;
      //MyFileCopy.open(getDatabasePath(name));
      //std::string content((std::istreambuf_iterator<char>(MyFileCopy)),
          //(std::istreambuf_iterator<char>())); // put into string
      //std::cout << "after appending, length of the database is: " << content.length() << std::endl;
      //toReturn.value = content.length() / 1024 - 1; //Return the appended index in value
      //MyFileCopy.close();
      return toReturn;
  }

  // USE: read data from a given block (after seek)
  StatusResult Storage::readBlock(StorageBlock &aBlock, uint32_t aBlockNumber) {
    //STUDENT: Implement this; this is your low-level block IO...
    //Open db file
    //std::ifstream theInput;
    //theInput.open(getDatabasePath(name));
    if (aBlockNumber >= getTotalBlockCount())  return StatusResult(Errors::seekError, Errors::seekError); 
    //Locate the block number
    try {
        //std::clog << "reading block num: " << aBlockNumber << std::endl; 

        std::ifstream MyFile;
        MyFile.open(getDatabasePath(name),std::ifstream::binary);
        MyFile.seekg( (unsigned long long) aBlockNumber * (unsigned long long) 1024);
        
        if (MyFile.good()) {
            char theBlockDataBuffer[1024];
            for (int i = 0; i < 1024; i++) { // get 1024 bytes data char array
                theBlockDataBuffer[i] = MyFile.get();
            }

            aBlock.load(theBlockDataBuffer); // put into the block
            //aBlock.header.id = aBlockNumber; // set block num
            //aBlock.header.type = theBlockDataBuffer[0];
            MyFile.close();
        }
        else { 
            MyFile.close();
            return StatusResult(Errors::seekError, Errors::seekError);
        }
    }
    catch (std::exception e) {
        return StatusResult(Errors::seekError, Errors::seekError);
    }

    return StatusResult{};
  }
  //USE: create the meta block on first creation of storage
  StatusResult Storage::createMeta(const std::string aName, CreateNewStorage){      
      StorageBlock theFirstBlock(BlockType::meta_block); // create the first meta block
      theFirstBlock.header.id_char[0]='0';
      appendBlock(theFirstBlock); // put in the first block
      return StatusResult(noError);
  }

  void makeTableSchemaBlockHeader() {

  }

  static std::map<ECE141::BlockType, std::string> blockType = {
    {ECE141::BlockType::meta_block, "Meta"},
    {ECE141::BlockType::data_block, "Data"},
    {ECE141::BlockType::entity_block, "Entity"},
    {ECE141::BlockType::free_block, "Free"},
    {ECE141::BlockType::index_block, "Index"},
    {ECE141::BlockType::unknown_block, "Unknown"}
  };

  //bool         Storage::checkATableExists(std::string tableName) { //XXX-HACK

  //    
  //    StorageInfo Storage;
  //    std::string filePath = Storage.getDefaultStoragePath1();
  //    std::string thePath;
  //    if (filePath[0] == '/') {
  //        thePath = filePath + "/" + name + ".db";
  //    }
  //    else {
  //        thePath = filePath + "\\" + name + ".db";
  //    }
  //    std::ifstream ifs(thePath); // get to file
  //    std::string content((std::istreambuf_iterator<char>(ifs)),
  //        (std::istreambuf_iterator<char>())); // put into string
  //    std::vector<std::string> tableNames;
  //    for (int i = 0; i < getTotalBlockCount(); ++i) {
  //        std::string blockContent = content.substr(1024 * i, 1024);
  //        std::string HeaderString = blockContent.substr(0,12);
  //        std::string DataString = blockContent.substr(12,1012);
  //        std::stringstream DataStream(DataString);
  //        //std::clog << "block type: " << blockType[static_cast<BlockType>(blockContent[0])] << std::endl;
  //        
  //        if (static_cast<BlockType>(blockContent[0]) == BlockType::entity_block) {
  //            std::string theTableName;
  //            DataStream >> theTableName;
  //            tableNames.push_back(theTableName);
  //        }
  //    }
  //    for (int i = 0; i < tableNames.size(); ++i) {
  //        if (tableName == tableNames[i]) { // found a match
  //            return true;
  //        }
  //    }
  //    return false;
  //    
  //}

  StorageBlock Storage::encodeBlockIndexAndType(BlockType aType) {
      StorageBlock theBlock(aType);
      int totalBlockCount = getTotalBlockCount();
      theBlock.header.id = getTotalBlockCount(); // set block num
      //std::clog << "storage.encodeBlockIndexAndType encoded block header id to: " << getTotalBlockCount() << std::endl;
      std::string index = std::to_string(totalBlockCount);
      if (index.length() <= 11) { // make sure the index is less than 11 chars
          for (int i = 0; i < index.length(); ++i) {
              theBlock.header.id_char[i] = index[i];
          }
      }
      return theBlock;
  }

  //bool storeDataStrIntiStorageBlock(std::stringstream& theSSData, StorageBlock& aBlock) {
  //    std::string dataStr = theSSData.str();
  //    //std::cout << theSSData.str().c_str();
  //    if (dataStr.length() <= 1012) {
  //        for (int i = 0; i < dataStr.length(); ++i) {
  //            aBlock.data[i] = dataStr[i];
  //        }
  //        return true;
  //    }
  //    else {
  //        return false; // not enough storage
  //    }
  //    
  //}

  //USE: create table 
  //StatusResult Storage::createTable(Schema* aSchema) {//Not used
      //------------------------------check if the table already exists--------------------------------------------------
      //if (checkATableExists(aSchema->getName())) { /*std::clog << "Error,table exists" << std::endl;*/ return StatusResult(tableExists, tableExists); }
      //------------------------------encode index and block type in to storage block obj------------------------------------
      ////StorageBlock theSchemaBlock = encodeBlockIndexAndType(BlockType::entity_block);
      //-------------------------------------------------------------------------------------------
      //std::stringstream theSSData;
      //aSchema->encode(theSSData);
      //StorageBlockCluster theCluster(*this);
      //theCluster.store(theSSData, WriteMethod::WriteNew, BlockType::entity_block);
      //return StatusResult(noError);
  //}

  //void Storage::writeBlock(char theData[1024], int aBlockIndex) { // pass in a char array lengh = 1024
  //    StorageInfo Storage;
  //    std::ifstream ifs(getDatabasePath(name)); // get to file
  //    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
  //    int k = 0;
  //    for (int j = 1024 * aBlockIndex; j < 1024 * aBlockIndex + 1024; ++j) { content[j] = theData[k]; ++k; }
  //    std::ofstream MyFile;
  //    MyFile.open(getDatabasePath(name)); // get the stream for writing block
  //    MyFile << content;
  //    MyFile.close();
  //}

  void craftFreeBlockDataBuffer(char theData[1024], int blockIndex) {
      std::string index = std::to_string(blockIndex);
      theData[0] = 'F';
      for (int i = 0; i < index.length(); ++i) { if (i <= 10) { theData[i + 1] = index[i]; } }
      for (int i = index.length() + 1; i < 1024; ++i) { theData[i] = 'o'; } // arbitrary char 'o' place holder
      //std::clog << "finished craft free block data buffer function" << std::endl;
  }

  /*Now handled in DB
  StatusResult Storage::dropTable(std::string aName) {
      if (!checkATableExists(aName)) { return StatusResult(unknownTable, unknownTable); }
      else {
          StorageInfo Storage;
          std::ifstream ifs(getDatabasePath(name)); // get to file
          std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())); // put into string
          for (int i = 0; i < getTotalBlockCount(); ++i) { // iterate through all blocks
              std::string blockContent = content.substr(1024 * i, 1024); // 1024 bytes of the block
              std::string HeaderString = blockContent.substr(0, 12);  // 12 bytes of header
              std::string DataString = blockContent.substr(12, 1012); // 1012 bytes of kLoad
              std::stringstream DataStream(DataString); // get a block's kLoad bytes into a string stream
              if (static_cast<BlockType>(blockContent[0]) == BlockType::entity_block || static_cast<BlockType>(blockContent[0]) == BlockType::data_block) { // check if the block is a schema
                  std::string theTableName; DataStream >> theTableName;
                  if (aName == theTableName) { // make sure the name is what we want to drop
                      char theString[1024] = { 0 };
                      craftFreeBlockDataBuffer(theString, i);
                      writeBlock(theString, i);
                      //break;
                  }
              }//return StatusResult(noError, noError);
          }
          return StatusResult(noError);
      }
  }
  */
  //StatusResult Storage::deleteFrom(std::string tableName) {
  //    
  //    if (!checkATableExists(tableName)) { return StatusResult(unknownTable, unknownTable); }
  //    else {

  //        //StorageBlock theBlock;
  //        //int i = 0;
  //        ////std::clog << "total block count: " << getTotalBlockCount() << std::endl;
  //        //while (auto theResult = readBlock(theBlock, i)) {
  //        //    if (theResult.code != seekError) {
  //        //        //std::clog << "the header type" << theBlock.header.type << std::endl;
  //        //        //std::clog << "showing data in block: " << theBlock.data << std::endl;
  //        //        if (theBlock.header.type == static_cast<char>(BlockType::data_block)) {
  //        //            //std::clog << "found schema type" << std::endl;
  //        //            std::string theData;
  //        //            for (int j = 0; j < kPayloadSize; j++) {
  //        //                theData += theBlock.data[j];
  //        //            }
  //        //            Row theRow;
  //        //            std::stringstream theDataStream(theData);
  //        //            theRow.decode(theDataStream);
  //        //            theRow.setBlockNum(i);
  //        //            if (theRow.getTableName() == tableName) { // make sure the name is what we want to drop
  //        //                char theString[1024] = { 0 };
  //        //                craftFreeBlockDataBuffer(theString, i);
  //        //                writeBlock(theString, i);
  //        //                //break;
  //        //            }
  //        //        }

  //        //    }
  //        //    else break;
  //        //    i++;
  //        //}


  //        
  //        StorageInfo Storage;
  //        std::ifstream ifs(getDatabasePath(name)); // get to file
  //        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>())); // put into string
  //        for (int i = 0; i < getTotalBlockCount(); ++i) { // iterate through all blocks             
  //            std::string blockContent = content.substr(1024 * i, 1024); // 1024 bytes of the block
  //            std::string HeaderString = blockContent.substr(0, 12);  // 12 bytes of header
  //            std::string DataString = blockContent.substr(12, 1012); // 1012 bytes of kLoad
  //            std::stringstream DataStream(DataString); // get a block's kLoad bytes into a string stream
  //            std::clog << "storage.deleteFrom function outputting blockContent[0]" << std::endl;
  //            std::clog << blockContent[0] << std::endl;
  //            if (static_cast<BlockType>(blockContent[0]) == BlockType::data_block) { // check if the block is a schema
  //                //std::string theTableName; DataStream >> theTableName;
  //                
  //                Row theRow;
  //                theRow.decode(DataStream);
  //                std::clog << "storage.deletefrom function: showing row: " << std::endl;
  //                theRow.show();
  //                std::clog << "finish showing row" << std::endl;
  //                if (theRow.getTableName() == tableName) { // make sure the name is what we want to drop
  //                    char theString[1024] = { 0 };
  //                    craftFreeBlockDataBuffer(theString, i);
  //                    writeBlock(theString, i);
  //                    //break;
  //                }
  //            }//return StatusResult(noError, noError);
  //        }
  //        return StatusResult(noError);
  //    }
  //}

  //int          Storage::checkFileSize() { // in bytes
  //    std::ifstream ifs(getDatabasePath(name), std::ostream::ate | std::ostream::binary); // get to file
  //    uint32_t theSize = ifs.tellg();
  //    std::clog << "storage.getTotalBlockCount size: " << theSize << std::endl;
  //    return theSize;

  //    std::ifstream MyFileCopy;
  //    MyFileCopy.open(getDatabasePath(name));
  //    std::string content((std::istreambuf_iterator<char>(MyFileCopy)), (std::istreambuf_iterator<char>())); // put into string
  //    std::cout << "length of the database is: " << content.length() << std::endl;
  //    MyFileCopy.close();
  //    return content.length();
  //}

  uint32_t Storage::insertRow(Row* aRow) { // return block num
      //Legacy code
      
      //StorageBlock theDataBlock = encodeBlockIndexAndType(BlockType::data_block); // header area is not binary encoded!!!
      //std::stringstream theSSData;
      //aRow->encode(theSSData);
      //if (storeDataStrIntiStorageBlock(theSSData, theDataBlock)) {
      //    appendBlock(theDataBlock); //std::cout << "Created table: " << tableName << " (ok)" << std::endl;
      //}
      //else { std::clog << "out of space for creating data for table: " << aRow->getTableName() << std::endl; }
      //return StatusResult();
      ////checkFileSize();
      
      std::stringstream theSSData;
      aRow->encode(theSSData);
      StorageBlockCluster theCluster(*this);
      theCluster.store(theSSData, WriteMethod::WriteNew, BlockType::data_block);
      return theCluster.getIndex();
  }

  StatusResult  Storage::freeBlocks(uint32_t aBlockNum) {
      StorageBlockCluster theCluster(*this);
      theCluster.load(aBlockNum);
      return theCluster.freeBlocks();
  }

  void Storage::update(std::function<bool(Row*)> compare) {
      //get clusters
      std::vector<Storable*> findings;
      /*
      load(findings, [&compare](StorageBlockCluster& theCluster) {
          Row* theRow = new Row();
          theRow->decode(theCluster.getData());
          if (compare(theRow)) return theRow;
          else return (Row*)nullptr;
          });
      */
      //Update

      //Writeback
      for (auto& theRow : findings) {
          StorageBlockCluster theCluster(*this);
          theCluster.load(theRow->blockNum);
          std::stringstream theStream;
          theRow->encode(theStream);
          theCluster.store(theStream, WriteMethod::OverwriteExisting, StorableType::Row);
      }


  }
}



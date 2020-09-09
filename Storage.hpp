//
//  Storage.hpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <fstream>
#include <variant>
#include "Errors.hpp"
#include "StorageBlock.hpp"
#include "Schema.hpp"
#include "Row.hpp"
#include "ValueWriter.hpp"
#include "StorageBlockCluster.hpp"
#include <functional>


namespace ECE141 {

    //first, some utility classes...

    class StorageInfo {
    public:
        static const char* getDefaultStoragePath1();
    };

    struct CreateNewStorage {};
    struct OpenExistingStorage {};

    // USE: Our storage manager class...
    class Storage {
    public:

        Storage(const std::string aName, CreateNewStorage);
        Storage(const std::string aName, OpenExistingStorage);
        ~Storage();
        uint32_t        getTotalBlockCount();

        //high-level IO (you're not required to use this, but it may help)...    
        //StatusResult    save(Storable& aStorable); //using a stream api
        //StatusResult    load(Storable& aStorable); //using a stream api
        //StatusResult    load(Storable& aStorable,  std::string aName);
        //StatusResult    load(std::vector<Storable*>& aStorables, std::function<bool(StorageBlockCluster&)> aPredicate, std::function<Storable*(std::istream&)> aCreator);
        //StatusResult    load(std::vector<Storable*>& aStorables, std::function<Storable*(StorageBlockCluster&)> aCreator);
        StatusResult    load(Storable& aStorable, uint32_t aBlockNum); //Load a given block into a storable, assuming it can be decoded properly
        StatusResult    load(std::vector<Schema>& aStorables, BlockType aType);
        StatusResult    load(std::vector<Row>& aStorables, BlockType aType);
        StatusResult    loadEach(std::function <bool(StorageBlockCluster&)> aLoopCondition);
        StatusResult    update(Storable& aStorable); 

        StatusResult createMeta(const std::string aName, CreateNewStorage);
        //StatusResult createTable(Schema* aSchema);//Not used. handled in DB
        //StatusResult dropTable(std::string aName);//Not used. handled in DB
        //bool         checkATableExists(std::string tableName);
        StorageBlock encodeBlockIndexAndType(BlockType aType);
        //int          checkFileSize();
        //void         writeBlock(char theData[1024], int aBlockIndex);
        uint32_t insertRow(Row* aRow);//Not used. handeled in DB
        //StatusResult deleteFrom(std::string tableName);
        //low-level IO...   
        StatusResult    readBlock(StorageBlock& aBlock, uint32_t aBlockNumber);
        StatusResult    writeBlock(StorageBlock& aBlock, uint32_t aBlockNumber);
        StatusResult    appendBlock(StorageBlock& aBlock);
        friend StorageBlockCluster;

    protected:
        bool            isReady() const;

        std::string     name;
        StatusResult    findFreeBlockNum();
        StatusResult    claimFreeBlockUsage(uint32_t aBlockNum);
        StatusResult    freeBlocks(uint32_t aBlockNum);
        std::fstream    stream;

        void update(std::function<bool(Row*)> compare);

        
    };

}

#endif /* Storage_hpp */

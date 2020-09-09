#include "Storage.hpp"
namespace ECE141 {
	StorageBlockCluster::StorageBlockCluster(Storage& aStorage) :storage(&aStorage) { }
	StorageBlockCluster::StorageBlockCluster(const StorageBlockCluster& aCopy) {
		*this = aCopy;
	}
	StorageBlockCluster& StorageBlockCluster::operator=(const StorageBlockCluster& aCopy) {
		storage = aCopy.storage;
		blocks = aCopy.blocks;
		return *this;
	}


	std::stringstream& StorageBlockCluster::getData() {//get the data aggregated from all blocks 
		std::string theResult;
		for (auto theBlock : blocks) {
			for (auto theChar : theBlock.data) {
				theResult += theChar;
			}
		}
		/*for (int i = 0; i < theResult.size(); ++i) {
			data << theResult[i];
		}*/
		data.str(theResult); // set stream
		data.clear(); // stream raise eof, remember read to pos, start over
		return data;
	}

	StatusResult StorageBlockCluster::store(std::istream& aData, WriteMethod aMethod, BlockType aBlockType) { // passed in kloadarea 1012 bytes
		switch (aMethod) {
		case WriteMethod::OverwriteExisting: //overwrite when performing update
			return storeOverwrite(aData, aBlockType);
		case WriteMethod::WriteNew:
			return storeWriteNew(aData, aBlockType);
		}
		return StatusResult{ noError };
	}

	StatusResult StorageBlockCluster::store(std::istream& aData, WriteMethod aMethod, StorableType aStorableType) {
		BlockType theBlockType = StorableTypeToBlockType(aStorableType);
		return store(aData, aMethod, theBlockType);
	}

	StatusResult StorageBlockCluster::load(uint32_t anIndex) {//Load the block at the index and its continuations into the block list
		blocks.clear();
		//Load the first block
		StorageBlock theBlock;
		if (storage->readBlock(theBlock, anIndex).code == seekError) return StatusResult(seekError);
		if (theBlock.header.type == static_cast<char>(BlockType::continue_block)) return StatusResult(continueBlock);
		theBlock.header.id = anIndex; // make sure set the block num here
		blocks.push_back(theBlock);

		while (theBlock.header.hasNext) {//Laod the continuations
			if (storage->readBlock(theBlock, theBlock.header.next).code == seekError) return StatusResult(seekError);
			blocks.push_back(theBlock);
		}
		return StatusResult{};
	}

	BlockType StorageBlockCluster::StorableTypeToBlockType(StorableType aType) {
		switch (aType)
		{
		case ECE141::StorableType::Schema:
			return BlockType::entity_block;
		case ECE141::StorableType::Attribute:
			return BlockType::unknown_block;
		case ECE141::StorableType::MyRow:
			return BlockType::data_block;
		case ECE141::StorableType::Row:
			return BlockType::data_block;
		default:
			return BlockType::unknown_block;
			break;
		}
	}

	bool StorageBlockCluster::hasEnoughSizeFor(int aLength) {
		return aLength <= kPayloadSize;
	}
	StorageBlock StorageBlockCluster::createNewFirstBlock(BlockType aBlockType) {
		StorageBlock theBlock(aBlockType);
		theBlock.header.id = storage->findFreeBlockNum().value; // if there is not a free block, a free block is going to be appended
		//std::clog << "in cluster, trying to insert table into block number: " << storage->findFreeBlockNum().value << std::endl;
		storage->claimFreeBlockUsage(theBlock.header.id);
		return theBlock;
	}

	StorageBlock StorageBlockCluster::createNewContinueBlockAndLink() {
		//Create new continue block and give it a block number in storage
		StorageBlock theBlock(BlockType::continue_block);
		theBlock.header.id = storage->findFreeBlockNum().value;
		storage->claimFreeBlockUsage(theBlock.header.id);

		//Link the last block with the new block
		blocks.back().header.next = theBlock.header.id;
		blocks.back().header.hasNext = true;

		return theBlock;
	}

	StatusResult StorageBlockCluster::storeOverwrite(std::istream& aData, BlockType aBlockType) {
		int theCount = 0;
		int theOriginalLength = blocks.size();
		

		if (theOriginalLength == 0) return storeWriteNew(aData, aBlockType);//If nothing to overwrite, just write new

		while (aData.good()) {
			if (++theCount > theOriginalLength) {//choose to updat existing block or add new, based on if there is enough space
				StorageBlock theBlock = createNewContinueBlockAndLink();
				blocks.push_back(theBlock);
			}
			else {
				blocks[theCount - 1].loadData(aData);
			}
		}

		if (theCount < theOriginalLength) {//free up unused block
			for (int i = theCount - 1; i < theOriginalLength; i++) {
				blocks[i].header.type = static_cast<char>(BlockType::free_block);
			}
		}
		return writeToDisk();
	}

	StatusResult StorageBlockCluster::storeWriteNew(std::istream& aData, BlockType aBlockType) { // pass in just the kloadarea

		blocks.clear();
		blocks.push_back(createNewFirstBlock(aBlockType));
		blocks.front().loadData(aData); // put in kload area

		while (aData.good()) { // this branch prevent continuing blocks, most of the cases can be ignored
			blocks.push_back(createNewContinueBlockAndLink());
			blocks.back().loadData(aData);
		}

		return writeToDisk();
	}

	StatusResult StorageBlockCluster::writeToDisk() {
		for (auto& theBlock : blocks) {
			//std::clog << theBlock.header.id << std::endl;
			//std::clog << theBlock.header.type << std::endl;
			auto theResult = storage->writeBlock(theBlock, theBlock.header.id);		
		}
		return StatusResult{};
	}

	StatusResult StorageBlockCluster::freeBlocks() {
		for (auto& theBlock : blocks) {
			theBlock.header.type = static_cast<char>(BlockType::free_block);
		}

		return writeToDisk();
	}
}
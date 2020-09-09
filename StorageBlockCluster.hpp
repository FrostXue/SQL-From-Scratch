#ifndef StorageBlockCluster_h
#define StorageBlockCluster_h

#include <vector>
#include <iostream>
#include "StorageBlock.hpp"
#include "Errors.hpp"
#include <sstream>
#include "Components.hpp"

namespace ECE141 {
	using Blocks = std::vector <StorageBlock>;
	class Storage;
	class StorageBlockCluster {
	public:
		StorageBlockCluster(Storage& aStorage);
		StorageBlockCluster(const StorageBlockCluster& aCopy);
		StorageBlockCluster& operator=(const StorageBlockCluster& aCopy);

		std::stringstream& getData();// get the data aggregated from all blocks	
		StatusResult store(std::istream& aData, WriteMethod aMethod, BlockType aBlockType);
		StatusResult store(std::istream& aData, WriteMethod aMethod, StorableType aStorableType);
		StatusResult load(uint32_t anIndex); //Load the block at the index and its continuations into the block list
		StatusResult freeBlocks();
		uint32_t getIndex() { 
			if (blocks.size()) { return blocks[0].header.id; }
			else { return -1; }
		}
		char getType() { 
			if (blocks.size()) { return static_cast<char>(blocks[0].header.type); }
			else {return static_cast<char>(BlockType::unknown_block); }
		}
		Blocks blocks;

	protected:
		Storage* storage;
		const int kPayloadSize = 1012;
		std::stringstream data;

		bool hasEnoughSizeFor(int aLength);
		StorageBlock createNewFirstBlock(BlockType aBlockType);
		StorageBlock createNewContinueBlockAndLink();
		StatusResult storeOverwrite(std::istream& aData, BlockType aBlockType);
		StatusResult storeWriteNew(std::istream& aData, BlockType aBlockType);
		StatusResult writeToDisk();
		BlockType StorableTypeToBlockType(StorableType aType);
	};
}
#endif

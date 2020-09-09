#include "IndexController.hpp"

namespace ECE141 {
	IndexController::IndexController(Storage& aStorage) : storage(&aStorage){

	}

	IndexController::IndexController(const IndexController& aCopy) {
		indexes = aCopy.indexes;
		storage = aCopy.storage;
	}

	IndexController& IndexController::operator=(const IndexController& aCopy) {
		indexes = aCopy.indexes;
		storage = aCopy.storage;
		return *this;
	}

	IndexController::~IndexController() {}

	uint32_t IndexController::createIndex(Schema& aSchema) {
		std::clog << "in IndexController createIndex function" << std::endl;
		StorageBlockCluster theCluster(*storage);
		std::stringstream theSStream(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
		theCluster.store(theSStream, WriteMethod::WriteNew, BlockType::index_block); // store nothing into the index block kload area

		if (aSchema.getAttributes().size()) { // has attributes
			const Attribute* theField;

			auto theFieldToIndex = std::find_if(aSchema.getAttributes().begin(), aSchema.getAttributes().end(), [](const Attribute& theAttr) {
				return theAttr.isPrimaryKey();
				}); // get the parimary attribute

			if (theFieldToIndex != aSchema.getAttributes().end()) {
				theField = &(*theFieldToIndex); // index the found primary field
			}
			else theField = &aSchema.getAttributes()[0]; // default index 0th field if no primary
			//std::clog << aSchema.blockNum << std::endl;

			indexes[aSchema.blockNum] = TableIndexes({ std::make_pair(theField->getName(),Index(theField->getName(),theField->getType(),theCluster.getIndex())) });
			//Store the index info
			std::stringstream theIndexStream(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
			indexes[aSchema.blockNum][theField->getName()].encode(theIndexStream);
			theCluster.store(theIndexStream, WriteMethod::OverwriteExisting, BlockType::index_block);
			/*for (auto item : indexes) {
				std::clog << item.first << std::endl;
				for (auto element : item.second) {
					std::clog << element.first << std::endl;
				}
			}*/
		}
		return theCluster.getIndex();
	}


}
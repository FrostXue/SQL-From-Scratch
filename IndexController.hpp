#ifndef IndexController_h
#define IndexController_h
#include <iostream>
#include "Storage.hpp"
#include "Errors.hpp"
#include <map>
#include <string>
#include "Index.hpp"

namespace ECE141 {
	using field = std::string;
	using TableID = uint32_t;
	using TableIndexes = std::map<field, Index>; //map a field name to the index obj
	using DBIndexes = std::map<TableID, TableIndexes>; //map a table block num to the table's indexes

	class IndexController
	{
	public:
		IndexController(Storage& aStorage);
		IndexController(const IndexController& aCopy);
		IndexController& operator=(const IndexController& aCopy);
		~IndexController();

		uint32_t createIndex(Schema& aSchema);
		DBIndexes& getIndexes() { return indexes; };



	protected:
		Storage* storage;
		DBIndexes indexes;
	};

}
#endif

#ifndef RowCollection_h
#define RowCollection_h

#include<iostream>
#include"Row.hpp"

namespace ECE141 {
	class RowCollection {//DO NOT copy the pointers 
	public:
		RowCollection() {}
		RowCollection(const RowCollection& aCopy) {
			*this = aCopy;
		}
		RowCollection& operator=(const RowCollection& aCopy) {
			for (int i = 0; i < listOfRows.size(); ++i) {
				listOfRows.push_back(aCopy.listOfRows[i]);
			}
			return *this;
		}
		~RowCollection() {
			for (auto theRow : listOfRows) {
				if (theRow) delete theRow;
			}
		}
		int getRowsCount() {
			return listOfRows.size();
		}
		std::vector<Row*>& getRowCollections() {
			return listOfRows;
		}
	protected:
		std::vector<Row*> listOfRows;
	};


}


#endif /* RowCollection_h */
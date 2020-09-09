//
//  View.hpp
//  Datatabase4
//
//  Created by rick gessner on 4/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef View_h
#define View_h

#include <iostream>

namespace ECE141 {

  //completely generic view, which you will subclass to show information
  class View {
  public:
    virtual         ~View() {}
    virtual bool    show(std::ostream &aStream)=0;
  };

  struct ColumnDescription {
      std::string name;
      size_t      width;
      bool        sortable;
  };

  //class TableView : public View {
  //public:

  //    //passive or active?
  //    // 1. incremental
  //    // 2. non-incremental (all at once)

  //    //what are the columns ?
  //    bool addColumn(const std::string& aName);

  //    bool addColumns(std::vector<ColumnDescription> aColumnList);

  //    //what are the rows?
  //    bool addRow(const Row& aRow);
  //    bool addRows(const RowCollection& aRow) {
  //        for (auto& theRow : aRows) {
  //            for (auto& theColumn : columns) {
  //                std::string& theValue = aRow[theColumn];

  //                output << theValue;
  //                //how do we do the formatting here?
  //            }
  //        }
  //    }
  //    std::vector<std::string> columns;
  //};

  ////-------------------------------------------------------------
  //"select id, first_name, last_name, email from users limit 6";
  ////-------------------------------------------------------------

  //int SelectStatement::run() {

  //    selectedFields[]; // internal data member of SelectStatement

  //    

  //    TableView theView = new TableView(...);
  //    //Schema theDB.getSchema('users');

  //    ////incremental setup first...
  //    //Row theRow* = theRows[0]; //

  //    //foreach(auto theField : selectedFields) {
  //    //    theView->addColumn(theField);
  //    //}

  //    theView->addColumns(selectedFields);

  //    /*size_t theColCount = theRow->count();
  //    for (int i = 0; i < theColCount; i++) {
  //        theView->addColumn(keyname);
  //    }*/

  //    DBTable theTable(users);
  //    RowCollection theRows; // loaded from a select statement

  //    theView->addRows(theRows);

  //}

}



#endif /* View_h */

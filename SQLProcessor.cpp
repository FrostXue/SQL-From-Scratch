//
//  SQLProcessor.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "SQLProcessor.hpp"
#include "MyView.hpp"

namespace ECE141 {

 //STUDENT: Implement the SQLProcessor class here...
    using StatementFactory = Statement * (*)(SQLProcessor & aProcessor);

    //--------------------------------------------------utility-----------------------------------------------
    int stringToInt2(std::string aStr) {
        int d = stoi(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
        return d;
    }

    // this function converts a string with numbers to a double, only the first appearing number is being converted to double
    double stringToDouble2(std::string aStr) {
        double d = stod(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
        return d;
    }
    //---------------------------------------------------------------------------------------------------------

    SQLProcessor::SQLProcessor(CommandProcessor* aNext) : CommandProcessor(aNext) {
    }
    SQLProcessor::~SQLProcessor() {
    }

    Statement* createStatementFactory(SQLProcessor &aProcessor) {
        return new CreateTableStatement(aProcessor);
    }

    Statement* dropStatementFactory(SQLProcessor& aProcessor) {
        return new DropTableStatement(aProcessor);
    }

    Statement* describeStatementFactory(SQLProcessor& aProcessor) {
        return new DescribeTableStatement(aProcessor);
    }

    Statement* showStatementFactory(SQLProcessor& aProcessor) {
        return new ShowTableStatement(aProcessor);
    }

    Statement* insertStatementFactory(SQLProcessor& aProcessor) {
        return new InsertIntoStatement(aProcessor);
    }

    Statement* deleteFromStatementFactory(SQLProcessor& aProcessor) {
        return new DeleteFromStatement(aProcessor);
    }

    Statement* selectStatementFactory(SQLProcessor& aProcessor) {
        return new SelectStatement(aProcessor);
    }

    Statement* updateStatementFactory(SQLProcessor& aProcessor) {
        return new UpdateStatement(aProcessor);
    }

    Statement* showIndexesStatementFactory(SQLProcessor& aProcessor) {
        return new ShowIndexesStatement(aProcessor);
    }

    Statement* alterStatementFactory(SQLProcessor& aProcessor) {
        return new AlterStatement(aProcessor);
    }

    Statement* SQLProcessor::getStatement(Tokenizer& aTokenizer) {
        
        static std::map<Keywords, StatementFactory> factories = {
            {Keywords::create_kw, createStatementFactory},
            {Keywords::drop_kw, dropStatementFactory},
            {Keywords::show_kw, showStatementFactory},
            {Keywords::describe_kw, describeStatementFactory},
            {Keywords::insert_kw, insertStatementFactory},
            {Keywords::delete_kw, deleteFromStatementFactory},
            {Keywords::select_kw, selectStatementFactory},
            {Keywords::update_kw, updateStatementFactory},
            {Keywords::indexes_kw, showIndexesStatementFactory},
            {Keywords::alter_kw, alterStatementFactory}
        };
        
        if (aTokenizer.size()) {
            aTokenizer.restart();
            std::vector<Token> tokensList;
            while (aTokenizer.more()) {
                tokensList.push_back(aTokenizer.current());
                aTokenizer.next();
            }
            aTokenizer.restart();
            //aTokenizer.dump();
            Token& theToken = aTokenizer.current();
            Token& theNextToken = aTokenizer.peek();
            if (aTokenizer.peek().keyword == Keywords::indexes_kw) {
                aTokenizer.next();
                theToken = aTokenizer.current();
            }
            if (factories.count(theToken.keyword) && (theToken.keyword == Keywords::indexes_kw || theNextToken.keyword == Keywords::table_kw || theNextToken.keyword == Keywords::tables_kw || theNextToken.type == TokenType::identifier || theNextToken.keyword == Keywords::into_kw || theNextToken.keyword == Keywords::from_kw || theToken.keyword == Keywords::select_kw)) {
                if (Statement* theStatement = factories[theToken.keyword](*this)) {
                    //std::clog << "will go in parse" << std::endl;
                    StatusResult theResult = theStatement->parse(aTokenizer);
                    StatusResult noError(noError);
                    //std::clog << "finished parse" << std::endl;
                    //std::clog << "theResult: " << theResult << std::endl;
                    if (theResult) { // careful here
                        //std::clog << theStatement << std::endl;
                        return theStatement;
                    }
                }
            }
        }
        return nullptr;
    }

    StatusResult  SQLProcessor::interpret(const Statement& aStatement) {
        //std::clog << "SQL interpret function" << std::endl;
        StatusResult theResult = (&aStatement)->run(std::cout);
        return theResult;
    }
    
    StatusResult SQLProcessor::createTable(const Schema& aSchema) {
        //std::clog << getActiveDatabase() << std::endl;
        if (Database* theDatabase = getActiveDatabase()) {
            //std::clog << "SQL Processor create table function" << std::endl;
            Schema* thePtr = const_cast<Schema*>(&aSchema);
            StatusResult theResult = theDatabase->createTable(thePtr);
            if (theResult) {
                std::cout << "Created Table \"" << aSchema.getName() << "\"\n";
            }
            return theResult;
        }
        //std::cout << "not suppose to be here" << std::endl;
        return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
    }
    StatusResult SQLProcessor::dropTable(const std::string& aName) {
        if (Database* theDatabase = getActiveDatabase()) {
            //std::clog << "SQL Processor delete table function" << std::endl;
            StatusResult theResult = theDatabase->dropTable(aName);
            if (theResult) {
                std::cout << "Droped Table \"" << aName << "\"\n";
            }
            return theResult;
        }
        return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
    }
    StatusResult SQLProcessor::describeTable(const std::string& aName) const {
        //Get schema
        if (Database* theDatabase = getActiveDatabase()) {

            std::vector<Schema*> theSchemas = theDatabase->getSchemas();
            Schema* theSchema = nullptr;
            auto theSchemaItr = std::find_if(theSchemas.begin(), theSchemas.end(), [&aName](const Schema* aMatch) {return aMatch->getName() == aName; });
            if (theSchemaItr != theSchemas.end()) theSchema = *theSchemaItr;

            if (theSchema) {

                //Format the schema for display
                Variants theHeaderContent{ Entry(std::string("Field")),Entry(std::string("Type")),Entry(std::string("Null")),Entry(std::string("Key")),Entry(std::string("Default")),Entry(std::string("Extra")) };
                MyRow theHeader(theHeaderContent);

                Sheet theAttributes;
                for (auto theAttribute : theSchema->getAttributes()) {
                    auto theAttributeRow = theAttribute.toRow();
                    theAttributes.push_back(theAttributeRow);
                }

                MyView theView(theHeader, theAttributes);
                theView.show(std::cout);

                delete theSchema;
                return StatusResult(noError);
            }
            return StatusResult(unknownTable);
        }
        return StatusResult(noDatabaseSpecified);
    }
    StatusResult SQLProcessor::showTables() const {
        if (Database* theDatabase = getActiveDatabase()) {
            std::vector<Schema*> theSchemas = theDatabase->getSchemas();

            /*for (int i = 0; i < theSchemas.size(); ++i) {
                std::clog << theSchemas[i].getName() << std::endl;
            }*/

            Variants theHeaderContent{ Entry(std::string("Tables_in_") + theDatabase->getName()) };
            MyRow theHeader(theHeaderContent);
            Sheet theTables;
            for (auto theSchema : theSchemas) {
                theTables.push_back(MyRow(Variants({ Entry(theSchema->getName()) })));
                delete theSchema;
            }
            MyView theView(theHeader, theTables);
            theView.show(std::cout);

            return StatusResult(noError);
        }
        return StatusResult(noDatabaseSpecified);
    }

    StatusResult SQLProcessor::insert(std::string tableName, std::vector<std::vector<Token>> rowTokens) {
        /*std::clog << "in processor insert function" << std::endl;*/
        if (Database* theDatabase = getActiveDatabase()) {
            std::vector<Schema> listOfSchemas;
            theDatabase->getStorage().load(listOfSchemas, BlockType::entity_block);
            bool findTable = false;
            int schemaIndex = -1;
            for (int i = 0; i < listOfSchemas.size(); ++i) {
                if (listOfSchemas[i].getName() == tableName) {
                    findTable = true;
                    schemaIndex = i;
                }
            }
            if (findTable) { // schema exists
                //listOfSchemas[schemaIndex].show();
                bool foundAttrInSchema = false;
                for (int i = 0; i < rowTokens[0].size(); ++i) { // iterate through row tokens
                    for (int j = 0; j < listOfSchemas[schemaIndex].getAttributes().size(); ++j) { // iterate through schema
                        if (listOfSchemas[schemaIndex].getAttributes()[j].getName() == rowTokens[0][i].data) {
                            //std::clog << listOfSchemas[schemaIndex].getAttributes()[j].getName() << std::endl;
                            //std::clog << rowTokens[0][i].data << std::endl;
                            foundAttrInSchema = true;
                        }
                    }
                    if (!foundAttrInSchema) { 
                        //for (auto theSchema : listOfSchemas) delete theSchema;
                        return StatusResult(syntaxError, syntaxError); 
                    }
                    else { foundAttrInSchema = false; }
                }
                StatusResult theResult = theDatabase->insert(tableName, rowTokens, listOfSchemas[schemaIndex]);
                if (theResult) {
                    std::cout << "Inserted into \"" << tableName << "\"\n";
                }
                //for (auto theSchema : listOfSchemas) delete theSchema;
                return theResult;
            }
            else { // no schema match
                //for (auto theSchema : listOfSchemas) delete theSchema;
                return StatusResult(unknownTable, unknownTable);
            }
            //for (auto theSchema : listOfSchemas) delete theSchema;
            return StatusResult(noError);
        }
        return StatusResult(noDatabaseSpecified);
    }

    StatusResult SQLProcessor::deleteFrom(std::string tableName, std::vector<std::vector<Token>> expressions){
        if (Database* theDatabase = getActiveDatabase()) {
            //std::clog << "SQL Processor delete table function" << std::endl;
            Filters theFilter(expressions);
            Timer theTimer;
            theTimer.start();
            StatusResult theResult = theDatabase->deleteFrom(tableName, theFilter);
            theTimer.stop();
            if (theResult) {
                std::cout << "(" << theTimer.elapsed() << " ms. )" << std::endl;
            }
            return theResult;
        }
        return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
    }
    
    //-------------------------------------------------------------------------
    //void displayAllFieldsVersion(std::vector<Row>& rowsWillBeDisplayed) {

    //    Variants theHeaderContent;
    //    if (rowsWillBeDisplayed.size()) {
    //        for (int i = 0; i < rowsWillBeDisplayed[0].getKeyValueList().size(); ++i) {
    //            //std::clog << tableSchema->getAttributes()[i].getName() << std::endl;
    //            theHeaderContent.push_back(Entry(rowsWillBeDisplayed[0].getKeyValueList()[i].first));
    //        }
    //        MyRow theHeader(theHeaderContent);
    //        Sheet theBlocks;

    //        Variants RowContent; // this is one row

    //        for (int i = 0; i < rowsWillBeDisplayed.size(); ++i) { // iterate through all rows
    //            for (int j = 0; j < tableSchema->getAttributes().size(); ++j) { // iterate through all attributes in schema
    //                for (int k = 0; k < rowsWillBeDisplayed[i].getKeyValueList().size(); ++k) { // iterate through a rows all kv pairs
    //                    if (rowsWillBeDisplayed[i].getKeyValueList()[k].first == tableSchema->getAttributes()[j].getName()) {
    //                        RowContent.push_back(Entry(rowsWillBeDisplayed[i].getKeyValueList()[k].second.second));
    //                    }
    //                }
    //            }
    //            theBlocks.push_back(RowContent);
    //            RowContent.clear();
    //        }


    //        MyView theView(theHeader, theBlocks);
    //        theView.show(std::cout);
    //    }
    //}

    void displayAllFieldsVersion(std::vector<Row>& rowsWillBeDisplayed, Schema* tableSchema) {
        
        Variants theHeaderContent;
        for (int i = 0; i < tableSchema->getAttributes().size(); ++i) {
            //std::clog << tableSchema->getAttributes()[i].getName() << std::endl;
            theHeaderContent.push_back(Entry(tableSchema->getAttributes()[i].getName()));
        }
        MyRow theHeader(theHeaderContent);
        Sheet theBlocks;

        Variants RowContent; // this is one row

        for (int i = 0; i < rowsWillBeDisplayed.size(); ++i) { // iterate through all rows
            for (int j = 0; j < tableSchema->getAttributes().size(); ++j) { // iterate through all attributes in schema
                for (int k = 0; k < rowsWillBeDisplayed[i].getKeyValueList().size(); ++k) { // iterate through a rows all kv pairs
                    if (rowsWillBeDisplayed[i].getKeyValueList()[k].first == tableSchema->getAttributes()[j].getName()) {
                        RowContent.push_back(Entry(rowsWillBeDisplayed[i].getKeyValueList()[k].second.second));
                    }
                }
            }
            theBlocks.push_back(RowContent);
            RowContent.clear();
        }        

        
        MyView theView(theHeader, theBlocks);
        theView.show(std::cout);
    }

    void displayLimitedFieldsVersion(std::vector<Row>& rowsWillBeDisplayed, std::vector<Token>& fields) {
        Variants theHeaderContent;
        for (int i = 0; i < fields.size(); ++i) {
            //std::clog << tableSchema->getAttributes()[i].getName() << std::endl;
            theHeaderContent.push_back(Entry(fields[i].data));
        }
        MyRow theHeader(theHeaderContent);
        Sheet theBlocks;

        Variants RowContent; // this is one row

        for (int i = 0; i < rowsWillBeDisplayed.size(); ++i) { // iterate through all rows
            for (int j = 0; j < fields.size(); ++j) { // iterate through all attributes in schema
                for (int k = 0; k < rowsWillBeDisplayed[i].getKeyValueList().size(); ++k) { // iterate through a rows all kv pairs
                    if (rowsWillBeDisplayed[i].getKeyValueList()[k].first == fields[j].data) {
                        RowContent.push_back(Entry(rowsWillBeDisplayed[i].getKeyValueList()[k].second.second));
                    }
                }
            }
            theBlocks.push_back(RowContent);
            RowContent.clear();
        }


        MyView theView(theHeader, theBlocks);
        theView.show(std::cout);
    }

    namespace sortFunctions {
        // Driver function to sort the vector elements by  
        // first element of pair in descending order 
        template<typename T>
        bool sortFirstDesc(const std::pair<T, int>& a, const std::pair<T, int>& b) { // used for int and double
            return (a.first > b.first);
        }

        template<>
        bool sortFirstDesc(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) { // used for int and double
            /*char aChar = a.first[0];
            char bChar = b.first[0];
            if (aChar >= 65 && aChar <= 90) {
                aChar += 32;
            }
            if (bChar >= 65 && bChar <= 90) {
                bChar += 32;
            }
            return (aChar > bChar);*/
            return (a.first > b.first);
        }

        // first element of pair in ascending order
        template<typename T>
        bool sortFirstAsc(const std::pair<T, int>& a, const std::pair<T, int>& b) { // used for int and double
            return (a.first < b.first);
        }

        template<>
        bool sortFirstAsc(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) { // used for int and double
            /*char aChar = a.first[0];
            char bChar = b.first[0];
            if (aChar >= 65 && aChar <= 90) {
                aChar += 32;
            }
            if (bChar >= 65 && bChar <= 90) {
                bChar += 32;
            }
            return (aChar < bChar);*/
            return (a.first < b.first);
        }
    }

    std::vector<Row> orderRowsVector(std::vector<Row> rowsWillBeDisplayed, std::string orderByKey, Schema* tableSchema) {
        //std::stringstream getFirstWord; 
        //getFirstWord << orderByKey;
        DataType orderByDataType = DataType::no_type;
        for (int i = 0; i < tableSchema->getAttributes().size(); ++i) {
            if (tableSchema->getAttributes()[i].getName() == orderByKey) {
                orderByDataType = tableSchema->getAttributes()[i].getType();
            }
        }
        if (orderByDataType == DataType::no_type) { return rowsWillBeDisplayed; } // no ordering happening, key is not found in table schema
        std::vector<Row> sortedByOrdering;
        std::vector<std::pair<int, int>> interMediateOrderingByIntVector; // pair.second is index in rowsWillBeDisplayed vector
        std::vector<std::pair<double, int>> interMediateOrderingByDoubleVector;
        std::vector<std::pair<std::string, int>> interMediateOrderingByStringVector;
        if (orderByDataType == DataType::int_type) {
            for (int i = 0; i < rowsWillBeDisplayed.size(); ++i) { // iterate through all rows
                for (int j = 0; j < rowsWillBeDisplayed[i].getKeyValueList().size(); ++j) { // iterate through all kv pairs
                    if (rowsWillBeDisplayed[i].getKeyValueList()[j].first == orderByKey) { // the key is found
                        interMediateOrderingByIntVector.push_back(
                            std::make_pair(std::get<int>(rowsWillBeDisplayed[i].getKeyValueList()[j].second.second), i)); // record int and index
                    }
                }
            }
            std::sort(interMediateOrderingByIntVector.begin(), interMediateOrderingByIntVector.end(), sortFunctions::sortFirstAsc<int>);
            for (int i = 0; i < interMediateOrderingByIntVector.size(); ++i) {
                sortedByOrdering.push_back(rowsWillBeDisplayed[interMediateOrderingByIntVector[i].second]); // pushes rows in ordered vector
            }
            return sortedByOrdering;
        }
        else if (orderByDataType == DataType::float_type) {
            for (int i = 0; i < rowsWillBeDisplayed.size(); ++i) { // iterate through all rows
                for (int j = 0; j < rowsWillBeDisplayed[i].getKeyValueList().size(); ++j) { // iterate through all kv pairs
                    if (rowsWillBeDisplayed[i].getKeyValueList()[j].first == orderByKey) { // the key is found
                        interMediateOrderingByDoubleVector.push_back(
                            std::make_pair(std::get<double>(rowsWillBeDisplayed[i].getKeyValueList()[j].second.second), i)); // record int and index
                    }
                }
            }
            std::sort(interMediateOrderingByDoubleVector.begin(), interMediateOrderingByDoubleVector.end(), sortFunctions::sortFirstAsc<double>);
            for (int i = 0; i < interMediateOrderingByDoubleVector.size(); ++i) {
                sortedByOrdering.push_back(rowsWillBeDisplayed[interMediateOrderingByDoubleVector[i].second]); // pushes rows in ordered vector
            }
            return sortedByOrdering;
        }
        else if (orderByDataType == DataType::varchar_type) {
            for (int i = 0; i < rowsWillBeDisplayed.size(); ++i) { // iterate through all rows
                for (int j = 0; j < rowsWillBeDisplayed[i].getKeyValueList().size(); ++j) { // iterate through all kv pairs
                    if (rowsWillBeDisplayed[i].getKeyValueList()[j].first == orderByKey) { // the key is found
                        auto toPushBack = rowsWillBeDisplayed[i].getKeyValueList()[j].second.second;
                        if (toPushBack.index() == std::variant_npos || toPushBack.index() != 3) toPushBack = std::string("");
                        interMediateOrderingByStringVector.push_back(
                            std::make_pair(std::get<std::string>(toPushBack), i)); // record int and index
                    }
                }
            }
            std::sort(interMediateOrderingByStringVector.begin(), interMediateOrderingByStringVector.end(), sortFunctions::sortFirstAsc<std::string>);
            for (int i = 0; i < interMediateOrderingByStringVector.size(); ++i) {
                sortedByOrdering.push_back(rowsWillBeDisplayed[interMediateOrderingByStringVector[i].second]); // pushes rows in ordered vector
            }
            return sortedByOrdering;
        }
        else {
            return rowsWillBeDisplayed; // no ordering
        }
    }

    StatusResult SQLProcessor::select(std::string tableName, std::vector<Token> fields, std::vector<std::vector<Token>> expressions) {
        if (Database* theDatabase = getActiveDatabase()) {
            Timer theTimer;
            theTimer.start();
            /*for (int i = 0; i < expressions.size(); ++i) {
                std::clog << "size: " << expressions[i].size() << std::endl;
                for (int j = 0; j < expressions[i].size(); ++j) {
                    std::clog << expressions[i][j].data << ' ';
                }
                std::clog << std::endl;
            }*/
            Filters theFilter(expressions);
            RowCollection theRowCollection;
            //if (!theDatabase->selectWithIndex(tableName, theFilter, theRowCollection)) { // RISK: you are speeding up the process at your own risk!!!!!
                theDatabase->select(tableName, theFilter, theRowCollection);
            //}
            //std::clog << "expression size: " << expressions.size() << std::endl;
            //std::clog << "table name: " << tableName << std::endl;
            //std::clog << "theRowCollection size: " << theRowCollection.getRowCollections().size() << std::endl;
            MyView theView;
            std::string orderByKey;
            int limit = -1;
            for (int i = 0; i < expressions.size(); ++i) {
                if (expressions[i].size() == 3) {
                    if (expressions[i][0].keyword == Keywords::order_kw && expressions[i][1].keyword == Keywords::by_kw) { // caution!! won't work for two order by
                        orderByKey = expressions[i][2].data; // order by this key, caution!!! asc and desc is lost FIX THIS later
                    }
                }
                if (expressions[i].size() == 2) {
                    if (expressions[i][0].keyword == Keywords::limit_kw && expressions[i][1].type == TokenType::number) {
                        limit = stringToInt2(expressions[i][1].data);
                    }
                }
            }
            std::vector<Row> rowsWillBeDisplayed;
            std::vector<Row> sortedByOrdering;
            std::vector<Row> orderedAndLimited;
            for (int i = 0; i < theRowCollection.getRowCollections().size(); ++i) {
                rowsWillBeDisplayed.push_back(*theRowCollection.getRowCollections()[i]);
            }
            Schema* tableSchema = theDatabase->getSchema(tableName); // delete it after
            if (tableSchema == nullptr) { return StatusResult(unknownTable, unknownTable); }

            for (int i = 0; i < fields.size(); ++i) { // iterate through all fields
                int appearance = 0;
                for (int j = 0; j < tableSchema->getAttributes().size(); ++j) { // iterate through schema attributes
                    if (fields[i].data == tableSchema->getAttributes()[j].getName()) { ++appearance; }
                }
                if (appearance == 0) { return StatusResult(unknownAttribute, unknownAttribute); }
            }
            std::string attributeAfterWhere;
            for (int i = 0; i < expressions.size(); ++i) {
                for (int j = 0; j < expressions[i].size(); ++j) {
                    if (expressions[i][j].keyword == Keywords::where_kw) {
                        if ((j + 1) < expressions[i].size()) {
                            attributeAfterWhere = expressions[i][j + 1].data;
                        }
                    }
                }
            }
            if (attributeAfterWhere.length() > 0) {
                int appearance = 0;
                for (int j = 0; j < tableSchema->getAttributes().size(); ++j) { // iterate through schema attributes
                    if (attributeAfterWhere == tableSchema->getAttributes()[j].getName()) { ++appearance; }
                }
                if (appearance == 0) { return StatusResult(unknownAttribute, unknownAttribute); }
            }
            sortedByOrdering = orderRowsVector(rowsWillBeDisplayed, orderByKey, tableSchema);
            if (limit != -1 && limit >= 0) { // limit the #
                for (int i = 0; i < limit; ++i) {
                    if (i < sortedByOrdering.size()) {
                        orderedAndLimited.push_back(sortedByOrdering[i]);
                    }
                }
            }
            theTimer.stop();
            if (fields.size() == 0 && limit == -1 && orderByKey.length() == 0) { // select * all fields version
                displayAllFieldsVersion(rowsWillBeDisplayed, tableSchema);
            }
            if (fields.size() != 0 && limit == -1 && orderByKey.length() == 0) {
                displayLimitedFieldsVersion(rowsWillBeDisplayed, fields);
            }
            if (fields.size() == 0 && limit != -1 && orderByKey.length() == 0) { // select * all fields version
                displayAllFieldsVersion(orderedAndLimited, tableSchema);
            }
            if (fields.size() != 0 && limit != -1 && orderByKey.length() == 0) {
                displayLimitedFieldsVersion(orderedAndLimited, fields);
            }
            if (fields.size() == 0 && limit == -1 && orderByKey.length() != 0) { // select * all fields version with order by
                displayAllFieldsVersion(sortedByOrdering, tableSchema);
            }
            if (fields.size() != 0 && limit == -1 && orderByKey.length() != 0) {
                displayLimitedFieldsVersion(sortedByOrdering, fields);
            }
            if (fields.size() == 0 && limit != -1 && orderByKey.length() != 0) { // select * all fields version with order by
                displayAllFieldsVersion(orderedAndLimited, tableSchema);
            }
            if (fields.size() != 0 && limit != -1 && orderByKey.length() != 0) {
                displayLimitedFieldsVersion(orderedAndLimited, fields);
            }
            //std::cout << "select from database " << tableName << " (ok)" << std::endl;
            double elapsedTime = theTimer.elapsed();
            std::cout << "(" << elapsedTime << " ms. )" << std::endl;
            return StatusResult(noError, noError);
        }
        return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
    }

    StatusResult SQLProcessor::join(std::string tableName, std::vector<Token> fields, Join aJoin) {
        std::clog << "in SQLProcessor join function" << std::endl;
        if (Database* theDatabase = getActiveDatabase()) {
            StatusResult theResult(unknownTable, unknownTable);
            Timer theTimer;
            theTimer.start();
            Filters theFilter;

            Schema* theLeftSchema = theDatabase->getSchema(aJoin.onLeft.tableName);
            Schema* theRightSchema = theDatabase->getSchema(aJoin.onRight.tableName);

            RowCollection theLeftRowCollection;
            if (theDatabase->select(aJoin.onLeft.tableName, theFilter, theLeftRowCollection) == theResult) { return theResult; }

            RowCollection theRightRowCollection;
            if (theDatabase->select(aJoin.onRight.tableName, theFilter, theRightRowCollection) == theResult) { return theResult; }

            std::vector<Row> theJoinedRows;

            if (aJoin.joinType == Keywords::left_kw) { // left join
                for (int i = 0; i < theLeftRowCollection.getRowCollections().size(); ++i) { // iterate through all left table's rows
                    for (int j = 0; j < theLeftRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++j) { // iterate through all the kv pairs in the row
                        if (aJoin.onLeft.fieldName == theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[j].first) { // found matching attribute name
                            // theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[j] is the kv pair
                            int foundMatch = 0;
                            for (int k = 0; k < theRightRowCollection.getRowCollections().size(); ++k) { // iterate through right table's rows
                                for (int a = 0; a < theRightRowCollection.getRowCollections()[k]->getKeyValueList().size(); ++a) { // iterate through a row's all kv pairs
                                    if (theRightRowCollection.getRowCollections()[k]->getKeyValueList()[a].first == aJoin.onRight.fieldName) {
                                        if (theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.first == false && theRightRowCollection.getRowCollections()[k]->getKeyValueList()[a].second.first == false) { // both not null
                                            if (theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second.index() == theRightRowCollection.getRowCollections()[k]->getKeyValueList()[a].second.second.index()) { // having same entry index
                                                if (theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second == theRightRowCollection.getRowCollections()[k]->getKeyValueList()[a].second.second) {
                                                    foundMatch = 1;
                                                    Row theJoinedRow;
                                                    for (int b = 0; b < theLeftRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++b) { // iterate through all kv pairs in left table's one row
                                                        theJoinedRow.getKeyValueList().push_back(theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[b]);
                                                    }
                                                    for (int c = 0; c < theRightRowCollection.getRowCollections()[k]->getKeyValueList().size(); ++c) { // iterate through all kv pairs in left table's one row
                                                        theJoinedRow.getKeyValueList().push_back(theRightRowCollection.getRowCollections()[k]->getKeyValueList()[c]);
                                                    }
                                                    theJoinedRows.push_back(theJoinedRow);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if (foundMatch == 0) {
                                Row theJoinedRow;
                                for (int b = 0; b < theLeftRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++b) { // iterate through all kv pairs in left table's one row
                                    theJoinedRow.getKeyValueList().push_back(theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[b]);
                                }
                                Row theRightNullRow;
                                for (int c = 0; c < theRightSchema->getAttributes().size(); ++c) {
                                    theRightNullRow.getKeyValueList().push_back(std::make_pair(theRightSchema->getAttributes()[c].getName(), std::make_pair(true, 0)));
                                }
                                for (int d = 0; d < theRightNullRow.getKeyValueList().size(); ++d) {
                                    theJoinedRow.getKeyValueList().push_back(theRightNullRow.getKeyValueList()[d]);
                                }
                                theJoinedRows.push_back(theJoinedRow);
                            }
                        }
                    }
                }
            }
            else if (aJoin.joinType == Keywords::right_kw) { // right join
                for (int i = 0; i < theRightRowCollection.getRowCollections().size(); ++i) { // iterate through all left table's rows
                    for (int j = 0; j < theRightRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++j) { // iterate through all the kv pairs in the row
                        if (aJoin.onRight.fieldName == theRightRowCollection.getRowCollections()[i]->getKeyValueList()[j].first) { // found matching attribute name
                            // theLeftRowCollection.getRowCollections()[i]->getKeyValueList()[j] is the kv pair
                            int foundMatch = 0;
                            for (int k = 0; k < theLeftRowCollection.getRowCollections().size(); ++k) { // iterate through right table's rows
                                for (int a = 0; a < theLeftRowCollection.getRowCollections()[k]->getKeyValueList().size(); ++a) { // iterate through a row's all kv pairs
                                    if (theLeftRowCollection.getRowCollections()[k]->getKeyValueList()[a].first == aJoin.onLeft.fieldName) {
                                        if (theRightRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.first == false && theLeftRowCollection.getRowCollections()[k]->getKeyValueList()[a].second.first == false) { // both not null
                                            if (theRightRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second.index() == theLeftRowCollection.getRowCollections()[k]->getKeyValueList()[a].second.second.index()) { // having same entry index
                                                if (theRightRowCollection.getRowCollections()[i]->getKeyValueList()[j].second.second == theLeftRowCollection.getRowCollections()[k]->getKeyValueList()[a].second.second) {
                                                    foundMatch = 1;
                                                    Row theJoinedRow;
                                                    for (int b = 0; b < theRightRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++b) { // iterate through all kv pairs in left table's one row
                                                        theJoinedRow.getKeyValueList().push_back(theRightRowCollection.getRowCollections()[i]->getKeyValueList()[b]);
                                                    }
                                                    for (int c = 0; c < theLeftRowCollection.getRowCollections()[k]->getKeyValueList().size(); ++c) { // iterate through all kv pairs in left table's one row
                                                        theJoinedRow.getKeyValueList().push_back(theLeftRowCollection.getRowCollections()[k]->getKeyValueList()[c]);
                                                    }
                                                    theJoinedRows.push_back(theJoinedRow);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            if (foundMatch == 0) {
                                Row theJoinedRow;
                                for (int b = 0; b < theRightRowCollection.getRowCollections()[i]->getKeyValueList().size(); ++b) { // iterate through all kv pairs in left table's one row
                                    theJoinedRow.getKeyValueList().push_back(theRightRowCollection.getRowCollections()[i]->getKeyValueList()[b]);
                                }
                                Row theLeftNullRow;
                                for (int c = 0; c < theLeftSchema->getAttributes().size(); ++c) {
                                    theLeftNullRow.getKeyValueList().push_back(std::make_pair(theLeftSchema->getAttributes()[c].getName(), std::make_pair(true, 0)));
                                }
                                for (int d = 0; d < theLeftNullRow.getKeyValueList().size(); ++d) {
                                    theJoinedRow.getKeyValueList().push_back(theLeftNullRow.getKeyValueList()[d]);
                                }
                                theJoinedRows.push_back(theJoinedRow);
                            }
                        }
                    }
                }
            }

            if (fields.size()) {
                displayLimitedFieldsVersion(theJoinedRows, fields);
            }

            theTimer.stop();
            double elapsedTime = theTimer.elapsed();
            std::cout << "(" << elapsedTime << " ms. )" << std::endl;
            return StatusResult(noError, noError);
        }
        return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
    }

    int stringToIntSQLProcessor(std::string aStr) {
        int d = stoi(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
        return d;
    }

    // this function converts a string with numbers to a double, only the first appearing number is being converted to double
    double stringToDoubleSQLProcessor(std::string aStr) {
        double d = stod(aStr.substr(aStr.find_first_of("0123456789"), aStr.find_last_of("0123456789") + 1));
        return d;
    }

    // this function converts a string with numbers to a bool, only the first appearing number is being converted to bool
    bool stringToBoolSQLProcessor(std::string aStr) {
        bool theBool = (aStr[0] == 't' || aStr[0] == 'T') ? true : false;
        return theBool;
    }

    StatusResult SQLProcessor::update(std::string tableName, std::vector<std::vector<Token>> KVPairs, std::vector<std::vector<Token>> expressions) {
        //std::clog << "in SQLProcessor update function" << std::endl;
        if (Database* theDatabase = getActiveDatabase()) {
            Timer theTimer;
            theTimer.start();
            /*for (int i = 0; i < expressions.size(); ++i) {
                std::clog << "size: " << expressions[i].size() << std::endl;
                for (int j = 0; j < expressions[i].size(); ++j) {
                    std::clog << expressions[i][j].data << ' ';
                }
                std::clog << std::endl;
            }*/
            Filters theFilter(expressions);
            RowCollection theRowCollection;
            theDatabase->update(tableName, theFilter, theRowCollection); // use filter to get rows we want to change
            //std::clog << "table name: " << tableName << std::endl;
            //std::clog << "theRowCollection size: " << theRowCollection.getRowCollections().size() << std::endl;

            //before update showing the row collections that will be changed
            /*for (int i = 0; i < theRowCollection.getRowCollections().size(); ++i) {
                theRowCollection.getRowCollections()[i]->show();
            }*/

            Schema* theSchema = theDatabase->getSchema(tableName);
            if (theSchema == nullptr) { return StatusResult(unknownTable,unknownTable); }
            /*for (int i = 0; i < KVPairs.size(); ++i) {
                for (int j = 0; j < KVPairs[i].size(); ++j) { std::clog << KVPairs[i][j].data << ' '; }
                std::clog << std::endl;
            }*/
            std::string theKey;
            DataType theValueType = DataType::no_type;
            for (int i = 0; i < KVPairs.size(); ++i) {
                bool foundAttrInSchema = false;
                for (int j = 0; j < theSchema->getAttributes().size(); ++j) {
                    if (KVPairs[i].size() == 2) { // make sure pair is one key one value size of 2, otherwise syntax error
                        //std::clog << "in this branch" << std::endl;
                        //std::clog << "attribute name in schema: " << theSchema->getAttributes()[j].getName() << std::endl;
                        if (theSchema->getAttributes()[j].getName() == KVPairs[i][0].data) { /*std::clog << "set to true" << std::endl;*/ theKey = theSchema->getAttributes()[j].getName(); theValueType = theSchema->getAttributes()[j].getType(); foundAttrInSchema = true; }
                    }
                }
                //std::clog << "foundAttrInSchema: " << foundAttrInSchema << std::endl;
                if (!foundAttrInSchema) { /*std::clog << "syntax error here" << std::endl;*/ return StatusResult(syntaxError, syntaxError); }
            }
            //====================================convert value to type with the key name================================================
            for (int i = 0; i < KVPairs.size(); ++i) {
                Entry theValue;
                if (theValueType == DataType::varchar_type) {
                    if (KVPairs[i].size() == 2) {
                        theValue = KVPairs[i][1].data;
                    }
                }
                else if (theValueType == DataType::int_type) {
                    if (KVPairs[i].size() == 2) {
                        theValue = stringToIntSQLProcessor(KVPairs[i][1].data);
                    }
                }
                else if (theValueType == DataType::float_type) {
                    if (KVPairs[i].size() == 2) {
                        theValue = stringToDoubleSQLProcessor(KVPairs[i][1].data);
                    }
                }
                else if (theValueType == DataType::bool_type) {
                    if (KVPairs[i].size() == 2) {
                        theValue = stringToBoolSQLProcessor(KVPairs[i][1].data);
                    }
                }
                else if (theValueType == DataType::no_type) {
                    return StatusResult(syntaxError, syntaxError);
                }
                for (int a = 0; a < theRowCollection.getRowCollections().size(); ++a) { // iterate through all rows in clooection
                    for (int b = 0; b < theRowCollection.getRowCollections()[a]->getKeyValueList().size(); ++b) { // iterate through all kv pairs in a row
                        if (theKey == theRowCollection.getRowCollections()[a]->getKeyValueList()[b].first) { // found the attribut
                            theRowCollection.getRowCollections()[a]->getKeyValueList()[b].second.first = false;
                            theRowCollection.getRowCollections()[a]->getKeyValueList()[b].second.second = theValue;
                        }
                    }
                }
            }
            //=========================================================================================
            /*std::clog << "after updating" << std::endl;
            for (int i = 0; i < theRowCollection.getRowCollections().size(); ++i) {
                theRowCollection.getRowCollections()[i]->show();
            }*/
            for (int i = 0; i < theRowCollection.getRowCollections().size(); ++i) {
                theDatabase->updateARow(*theRowCollection.getRowCollections()[i]);
            }
            theTimer.stop();
            std::cout << theRowCollection.getRowsCount() << " rows affected ";
            std::cout << "(" << theTimer.elapsed() << " ms. )" << std::endl;
            return StatusResult(noError, noError);
        }
        return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
    }

    StatusResult SQLProcessor::showIndexes() {
        if (Database* theDatabase = getActiveDatabase()) {
            theDatabase->showIndexes();
            return StatusResult(noError, noError);
        }
        else {
            return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
        }
    }

    StatusResult SQLProcessor::alterTable(const std::string tableName, const Attribute& toAlter) {
        if (Database* theDatabase = getActiveDatabase()) {
            if (Schema* theSchema = theDatabase->getSchema(tableName)) {
                if (std::nullopt == theSchema->getAttribute(toAlter.getName())) {
                    theSchema->addAttribute(toAlter);

                    std::stringstream theData(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
                    theSchema->encode(theData);
                    StorageBlockCluster theCluster(theDatabase->getStorage());
                    theCluster.load(theSchema->blockNum);
                    theCluster.store(theData, WriteMethod::OverwriteExisting, BlockType::entity_block);
                    
                    //add null attr to all rows
                    theDatabase->getStorage().loadEach([&tableName, &toAlter](StorageBlockCluster& aCluster) {
                        if (aCluster.getType() == static_cast<char>(BlockType::data_block)) {
                            Row theRow;
                            theRow.decode(aCluster.getData());
                            theRow.getKeyValueList().push_back(NullableKVPair(toAlter.getName(), std::make_pair(false, Entry(-1))));
                            std::stringstream theRowData(std::stringstream::binary | std::stringstream::in | std::stringstream::out);
                            theRow.encode(theRowData);
                            aCluster.store(theRowData, WriteMethod::OverwriteExisting, BlockType::data_block);
                        }
                        return true;
                        });

                    std::cout << "New attribute added.";
                    return StatusResult();
                }
                else return StatusResult(invalidAttribute, invalidAttribute);
            }
            else return StatusResult(unknownTable, unknownTable);
        }
        else {
            return StatusResult(noDatabaseSpecified, noDatabaseSpecified);
        }
    }
}

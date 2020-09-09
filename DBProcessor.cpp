#include <iostream>
#include "DBProcessor.hpp"
#include "Tokenizer.hpp"
#include "Storage.hpp"
#include "Database.hpp"
#include <memory>
#include <vector>
#include <fstream> 
#include <string>

namespace ECE141 {
    class CreateDatabaseStatement : public Statement {
    public:
        CreateDatabaseStatement() : Statement(Keywords::createDB_kw) {}
        std::vector<ECE141::Token>& getTokensInCreateDatabaseStatement() { return tokens; }
    protected:
        std::vector<ECE141::Token>    tokens;
    };

    class DropDatabaseStatement : public Statement {
    public:
        DropDatabaseStatement() : Statement(Keywords::dropDB_kw) {}
        std::vector<ECE141::Token>& getTokensInDropDatabaseStatement() { return tokens; }
    protected:
        std::vector<ECE141::Token>    tokens;
    };

    class UseDatabaseStatement : public Statement {
    public:
        UseDatabaseStatement() : Statement(Keywords::useDB_kw) {}
        std::vector<ECE141::Token>& getTokensInUseDatabaseStatement() { return tokens; }
    protected:
        std::vector<ECE141::Token>    tokens;
    };

    class ShowDatabaseStatement : public Statement {
    public:
        ShowDatabaseStatement() : Statement(Keywords::showDB_kw) {}
        std::vector<ECE141::Token>& getTokensInShowDatabaseStatement() { return tokens; }
    protected:
        std::vector<ECE141::Token>    tokens;
    };

    class DescribeDatabaseStatement : public Statement {
    public:
        DescribeDatabaseStatement() : Statement(Keywords::describeDB_kw) {}
        std::vector<ECE141::Token>& getTokensInDescribeDatabaseStatement() { return tokens; }
    protected:
        std::vector<ECE141::Token>    tokens;
    };

    //.....................................

    DBCmdProcessor::DBCmdProcessor(CommandProcessor* aNext, Database* anActiveDatabase) : CommandProcessor(aNext) {
        activeDB = anActiveDatabase;
    }

    DBCmdProcessor::~DBCmdProcessor() {
        releaseDB();
    }

    void writeMetaBolckForCreating(std::string DBName) { // this is called when creating a database, use in-memory database object do write the meta block into the file
        //OpenExistingStorage existTag; // tag struct
        Database theDatabase(DBName, CreateNewStorage{}); // create in memory database

        //Already called in database constructor
        //theDatabase.createDatabase(theDatabase.getName()); // call in memory database create to put meta block into the file
    }

    std::vector<std::string> iterateThroughAFolderAndGetFileNamesForBothWindowsAndIOSAndLinux(std::string filePath) {
        std::vector<std::string> txtFilesNames;
        if (filePath[0] != '/') {
            for (const auto& entry : std::filesystem::directory_iterator(filePath + "\\")) { // push in all the txt file names found in file to the vector
                auto theFileName = entry.path();
                if (theFileName.has_extension() && theFileName.extension().string() == ".db") {
                    //std::clog << entry.path().filename().string() << '\n';
                    txtFilesNames.push_back(entry.path().filename().string());
                }
            }
        }
        if (filePath[0] == '/') { // for ios/linux devices
            for (const auto& entry : std::filesystem::directory_iterator(filePath + "/")) { // push in all the txt file names found in file to the vector
                auto theFileName = entry.path();
                if (theFileName.has_extension() && theFileName.extension().string() == ".db") {
                    //std::clog << entry.path().filename().string() << '\n';
                    txtFilesNames.push_back(entry.path().filename().string());
                }
            }
        }
        return txtFilesNames;
    }

    Database* DBCmdProcessor::loadDatabase(const std::string& aName) const { // incomplete member function
        return nullptr;
    }

    void DBCmdProcessor::updateAvtiveDB(std::string aDBName) { // release previous using database, then create new active db with the found database name
        releaseDB();
        activeDB = new Database(aDBName, OpenExistingStorage{});
        //std::clog << "db pointer: " << activeDB << std::endl;
        //activeDB->describeDatabase(activeDB->getName()); // for testing really the pointer is updated by calling describe
    }

    StatusResult doCreateDBCommand(const Statement& aStatement) {
        Statement& unConstStatement = const_cast<Statement&>(aStatement); // remove constness
        CreateDatabaseStatement* temp = static_cast<CreateDatabaseStatement*>(&unConstStatement); // convert back 

        StorageInfo Storage; // create storage object
        std::string filePath = Storage.getDefaultStoragePath1();
        std::vector<std::string> txtFilesNames;

        txtFilesNames = iterateThroughAFolderAndGetFileNamesForBothWindowsAndIOSAndLinux(filePath);

        if (temp->getTokensInCreateDatabaseStatement().size() == 3 && temp->getTokensInCreateDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInCreateDatabaseStatement()[1].data == "database") {
            // Create and open a text file
            //std::clog << "in this branch" << std::endl;
            for (int i = 0; i < txtFilesNames.size(); ++i) {
                //std::clog << "comparison" << (temp->getTokensInCreateDatabaseStatement()[2].data + ".txt" == txtFilesNames[i]) << std::endl;
                if (temp->getTokensInCreateDatabaseStatement()[2].data + ".db" == txtFilesNames[i]) {
                    return StatusResult(databaseExists, databaseExists);
                }
            }

            // std::ofstream MyFile;
            // if (filePath[0] == '/') {
            //     MyFile.open(filePath + "/" + temp->getTokensInCreateDatabaseStatement()[2].data + ".txt");
            // }
            // else {
            //     MyFile.open(filePath + "\\" + temp->getTokensInCreateDatabaseStatement()[2].data + ".txt");
            // }
            // // Close the file
            // MyFile.close();
            writeMetaBolckForCreating(temp->getTokensInCreateDatabaseStatement()[2].data); // pass in the database name
            std::cout << "created database " + temp->getTokensInCreateDatabaseStatement()[2].data + " (ok)" << std::endl;
            return StatusResult();
        }
        if (temp->getTokensInCreateDatabaseStatement().size() != 3 && temp->getTokensInCreateDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInCreateDatabaseStatement()[1].data == "database") {
            return StatusResult(illegalIdentifier, illegalIdentifier);
        }
        return StatusResult(unknownCommand, unknownCommand);
    }

    StatusResult doDropDBCommand(const Statement& aStatement) {
        Statement& unConstStatement = const_cast<Statement&>(aStatement); // remove constness
        DropDatabaseStatement* temp = static_cast<DropDatabaseStatement*>(&unConstStatement); // convert back 
        StorageInfo Storage;
        std::string filePath = Storage.getDefaultStoragePath1();
        std::vector<std::string> txtFilesNames;

        txtFilesNames = iterateThroughAFolderAndGetFileNamesForBothWindowsAndIOSAndLinux(filePath);

        if (temp->getTokensInDropDatabaseStatement().size() == 3 && temp->getTokensInDropDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInDropDatabaseStatement()[1].data == "database") {
            // Create and open a text file
            //std::clog << "in this branch" << std::endl;
            for (int i = 0; i < txtFilesNames.size(); ++i) {
                //std::clog << "comparison" << (temp->getTokensInCreateDatabaseStatement()[2].data + ".txt" == txtFilesNames[i]) << std::endl;
                if (temp->getTokensInDropDatabaseStatement()[2].data + ".db" == txtFilesNames[i]) {
                    std::string pathAndFile;
                    if (filePath[0] == '/') {
                        pathAndFile = filePath + "/" + temp->getTokensInDropDatabaseStatement()[2].data + ".db";
                    }
                    else {
                        pathAndFile = filePath + "\\" + temp->getTokensInDropDatabaseStatement()[2].data + ".db";
                    }
                    remove(pathAndFile.c_str());
                    std::cout << "drop database " + temp->getTokensInDropDatabaseStatement()[2].data + " (ok)" << std::endl;
                    return StatusResult();
                }
            }
            return StatusResult(unknownDatabase, unknownDatabase);
        }
        if (temp->getTokensInDropDatabaseStatement().size() != 3 && temp->getTokensInDropDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInDropDatabaseStatement()[1].data == "database") {
            return StatusResult(illegalIdentifier, illegalIdentifier);
        }
        return StatusResult(unknownCommand, unknownCommand);
    }

    StatusResult doUseDBCommand(const Statement& aStatement, DBCmdProcessor* aDBProcessor) {
        Statement& unConstStatement = const_cast<Statement&>(aStatement); // remove constness
        UseDatabaseStatement* temp = static_cast<UseDatabaseStatement*>(&unConstStatement); // convert back 

        StorageInfo Storage;
        std::string filePath = Storage.getDefaultStoragePath1();
        std::vector<std::string> txtFilesNames;

        txtFilesNames = iterateThroughAFolderAndGetFileNamesForBothWindowsAndIOSAndLinux(filePath);

        if (temp->getTokensInUseDatabaseStatement().size() == 3 && temp->getTokensInUseDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInUseDatabaseStatement()[1].data == "database") {
            // Create and open a text file
            //std::clog << "in this branch" << std::endl;
            for (int i = 0; i < txtFilesNames.size(); ++i) {
                //std::clog << "comparison" << (temp->getTokensInCreateDatabaseStatement()[2].data + ".txt" == txtFilesNames[i]) << std::endl;
                if (temp->getTokensInUseDatabaseStatement()[2].data + ".db" == txtFilesNames[i]) {
                    // use databse code put here
                    aDBProcessor->updateAvtiveDB(temp->getTokensInUseDatabaseStatement()[2].data);

                    std::cout << "using database " + temp->getTokensInUseDatabaseStatement()[2].data << std::endl;
                    return StatusResult();
                }
            }
            return StatusResult(unknownDatabase, unknownDatabase);
        }
        if (temp->getTokensInUseDatabaseStatement().size() != 3 && temp->getTokensInUseDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInUseDatabaseStatement()[1].data == "database") {
            return StatusResult(illegalIdentifier, illegalIdentifier);
        }
        return StatusResult(unknownCommand, unknownCommand);
    }

    StatusResult doShowDBCommand(const Statement& aStatement) {
        Statement& unConstStatement = const_cast<Statement&>(aStatement); // remove constness
        ShowDatabaseStatement* temp = static_cast<ShowDatabaseStatement*>(&unConstStatement); // convert back 

        StorageInfo Storage;
        std::string filePath = Storage.getDefaultStoragePath1();
        std::vector<std::string> txtFilesNames;

        txtFilesNames = iterateThroughAFolderAndGetFileNamesForBothWindowsAndIOSAndLinux(filePath);

        for (int i = 0; i < txtFilesNames.size(); ++i) {
            for (int j = 0; j < 3; ++j) {
                txtFilesNames[i].pop_back();
            }
        }

        if (temp->getTokensInShowDatabaseStatement().size() == 2 && temp->getTokensInShowDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInShowDatabaseStatement()[1].data == "databases") {
            // Create and open a text file
            //std::clog << "in this branch" << std::endl;
            if (txtFilesNames.size()) {
                for (int i = 0; i < txtFilesNames.size(); ++i) {
                    std::cout << txtFilesNames[i] << std::endl;
                }
                return StatusResult();
            }
            return StatusResult(seekError, seekError);
        }
        return StatusResult(unknownCommand, unknownCommand);
    }

    StatusResult doDescribeDBCommand(const Statement& aStatement) {
        Statement& unConstStatement = const_cast<Statement&>(aStatement); // remove constness
        DescribeDatabaseStatement* temp = static_cast<DescribeDatabaseStatement*>(&unConstStatement); // convert back 

        StorageInfo Storage;
        std::string filePath = Storage.getDefaultStoragePath1();
        std::vector<std::string> txtFilesNames;

        txtFilesNames = iterateThroughAFolderAndGetFileNamesForBothWindowsAndIOSAndLinux(filePath);

        if (temp->getTokensInDescribeDatabaseStatement().size() == 3 && temp->getTokensInDescribeDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInDescribeDatabaseStatement()[1].data == "database") {
            // Create and open a text file
            //std::clog << "in this branch" << std::endl;
            for (int i = 0; i < txtFilesNames.size(); ++i) {
                //std::clog << "comparison" << " " << (temp->getTokensInDescribeDatabaseStatement()[2].data + ".txt" == txtFilesNames[i]) << std::endl;
                if (temp->getTokensInDescribeDatabaseStatement()[2].data + ".db" == txtFilesNames[i]) { // if the database is found
                    OpenExistingStorage existTag; // tag struct
                    Database theDatabase(temp->getTokensInDescribeDatabaseStatement()[2].data, existTag); // create in memory database
                    theDatabase.describeDatabase(temp->getTokensInDescribeDatabaseStatement()[2].data);
                    return StatusResult();
                }
            }
            return StatusResult(unknownDatabase, unknownDatabase);
        }
        if (temp->getTokensInDescribeDatabaseStatement().size() != 3 && temp->getTokensInDescribeDatabaseStatement()[0].type == TokenType::keyword && temp->getTokensInDescribeDatabaseStatement()[1].data == "database") {
            return StatusResult(illegalIdentifier, illegalIdentifier);
        }
        return StatusResult(unknownCommand, unknownCommand);
    }

    // USE: -----------------------------------------------------
    StatusResult DBCmdProcessor::interpret(const Statement& aStatement) {
        //STUDENT: write code related to given statement
        switch (aStatement.getType()) {
        case Keywords::createDB_kw:  return doCreateDBCommand(aStatement);
        case Keywords::dropDB_kw: {
            releaseDB(); // set to nullptr
            //std::clog << "activeDB is set to: " << getActiveDatabase() << std::endl;
            return doDropDBCommand(aStatement);
        }
        case Keywords::useDB_kw: return doUseDBCommand(aStatement, this);
        case Keywords::showDB_kw: return doShowDBCommand(aStatement);
        case Keywords::describeDB_kw: return doDescribeDBCommand(aStatement);
        }
        return StatusResult();
    }

    // create statements---------------------------------------------
    CreateDatabaseStatement* makeCreateDBStatement(Tokenizer& aTokenizer) {
        CreateDatabaseStatement* temp = new CreateDatabaseStatement;
        while (aTokenizer.more()) { // consume all tokens followed by help keyword
            temp->getTokensInCreateDatabaseStatement().push_back(aTokenizer.current()); // push in all the tokens after help
            aTokenizer.next(); // consumed that token, good to go
        }
        if (temp->getTokensInCreateDatabaseStatement()[1].keyword != ECE141::Keywords::database_kw) {
            delete temp;
            return nullptr;
        }
        return temp;
    }

    DropDatabaseStatement* makeDropDBStatement(Tokenizer& aTokenizer) {
        DropDatabaseStatement* temp = new DropDatabaseStatement;
        while (aTokenizer.more()) { // consume all tokens followed by help keyword
            temp->getTokensInDropDatabaseStatement().push_back(aTokenizer.current()); // push in all the tokens after help
            aTokenizer.next(); // consumed that token, good to go
        }
        if (temp->getTokensInDropDatabaseStatement()[1].keyword != ECE141::Keywords::database_kw) {
            delete temp;
            return nullptr;
        }
        return temp;
    }

    UseDatabaseStatement* makeUseDBStatement(Tokenizer& aTokenizer) {
        UseDatabaseStatement* temp = new UseDatabaseStatement;
        while (aTokenizer.more()) { // consume all tokens followed by help keyword
            temp->getTokensInUseDatabaseStatement().push_back(aTokenizer.current()); // push in all the tokens after help
            aTokenizer.next(); // consumed that token, good to go
        }
        if (temp->getTokensInUseDatabaseStatement()[1].keyword != ECE141::Keywords::database_kw) {
            delete temp;
            return nullptr;
        }
        return temp;
    }

    ShowDatabaseStatement* makeShowDBStatement(Tokenizer& aTokenizer) {
        ShowDatabaseStatement* temp = new ShowDatabaseStatement;
        while (aTokenizer.more()) { // consume all tokens followed by help keyword
            temp->getTokensInShowDatabaseStatement().push_back(aTokenizer.current()); // push in all the tokens after help
            aTokenizer.next(); // consumed that token, good to go
        }
        if (temp->getTokensInShowDatabaseStatement()[1].keyword != ECE141::Keywords::databases_kw) {
            delete temp;
            return nullptr;
        }
        return temp;
    }

    DescribeDatabaseStatement* makeDescribeDBStatement(Tokenizer& aTokenizer) {
        DescribeDatabaseStatement* temp = new DescribeDatabaseStatement;
        while (aTokenizer.more()) { // consume all tokens followed by help keyword
            temp->getTokensInDescribeDatabaseStatement().push_back(aTokenizer.current()); // push in all the tokens after help
            aTokenizer.next(); // consumed that token, good to go
        }
        if (temp->getTokensInDescribeDatabaseStatement()[1].keyword != ECE141::Keywords::database_kw) {
            delete temp;
            return nullptr;
        }
        return temp;
    }

    // USE: factory to create statement based on given tokens...
    Statement* DBCmdProcessor::getStatement(Tokenizer& aTokenizer) {
        //aTokenizer.dump();
        Token& theToken = aTokenizer.current();
        if (theToken.keyword == ECE141::Keywords::create_kw) {
            return makeCreateDBStatement(aTokenizer); // you get the idea, but 50 lines in this case is enough
        }
        else if (theToken.keyword == ECE141::Keywords::drop_kw) {
            return makeDropDBStatement(aTokenizer);
        }
        else if (theToken.keyword == ECE141::Keywords::use_kw) {
            return makeUseDBStatement(aTokenizer);
        }
        else if (theToken.keyword == ECE141::Keywords::show_kw) {
            return makeShowDBStatement(aTokenizer);
        }
        else if (theToken.keyword == ECE141::Keywords::describe_kw) {
            return makeDescribeDBStatement(aTokenizer);
        }

        return nullptr;
    }

    DBCmdProcessor& DBCmdProcessor::releaseDB() {
        delete activeDB;
        activeDB = nullptr; // set to nullptr
        return *this;
    }

    Database* DBCmdProcessor::getActiveDatabase() const {
        //std::clog << "DB Processor returning database: " << activeDB << std::endl;
        return activeDB;
    }
}
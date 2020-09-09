#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include <string>
#include <iostream>
#include <filesystem>

namespace ECE141 {


    class DBCmdProcessor : public CommandProcessor {
    public:

        DBCmdProcessor(CommandProcessor* aNext = nullptr, Database* anActiveDatabase = nullptr);
        virtual ~DBCmdProcessor();

        virtual Statement* getStatement(Tokenizer& aTokenizer);
        virtual StatusResult  interpret(const Statement& aStatement);
        virtual Database* getActiveDatabase() const;

        void updateAvtiveDB(std::string aDBName);

    protected:
        Database* loadDatabase(const std::string& aName) const;
        DBCmdProcessor& releaseDB();
        Database        *activeDB;
    };

}

#endif /* DBProcessor_hpp */
#include "Test.hpp"
#include <iostream>

namespace ECE141{
    void Test::run(){
        //Test FolderReader
        reader = testFolderReader();
        if (!logTestResult(reader,"FileReader")) return;

    }

    Test::Test(){

    }

    Test::~Test(){
        if (reader) delete reader;

    }

    FolderReader* Test::testFolderReader(){
        FolderReader* theReader = new FolderReader("G:\\Repos\\ECE141B\\ece141-sp20-assignment2-namingpanic");

        //Test exists()
        bool testExist = theReader->exists("main.cpp");
        if (!assertTrue(testExist,"exists()")){
            delete theReader;
            return nullptr;
        }

        //Test each()
        class TestFolderListerner: public FolderListener{ //FL is a generic class and thus requires a test subclass
        public:
            bool operator()(const std::string &aName){
                success= aName == std::string("G:\\Repos\\ECE141B\\ece141-sp20-assignment2-namingpanic\\autograder.exe");
                return success;
            }
            bool success;
        };
        TestFolderListerner theListerner;
        theReader->each(theListerner, ".exe"); //Test to find the .exe files
        if (!assertTrue(theListerner.success, "each()")){
            delete theReader;
            return nullptr;
        }

        //Test success
        return theReader;
    }

    bool Test::assertTrue(bool aStatement, std::string anInfo){
        std::clog << "Assert True \"" << anInfo << "\": ";
        std::clog << (aStatement? "PASS" : "FAIL") << "\n";
        return aStatement;
    }

    bool Test::assertFalse(bool aStatement, std::string anInfo){
        std::clog << "Assert True \"" << anInfo << "\" ";
        std::clog << (aStatement? "FAIL" : "PASS") << "\n";
        return !aStatement;
    }

    bool Test::logTestResult(bool aResult, std::string aTestInfo){
        std::clog << "Test \"" << aTestInfo << "\" ";
        std::clog <<(aResult? "PASS" : "FAIL") << "\n";
        return aResult;
    }
}
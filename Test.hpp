#ifndef Test_hpp
#define Test_hpp
#include "FolderReader.hpp"

namespace ECE141{
    class Test{
    public:
        Test();
        ~Test();
        void run();

        //Factory for constructing a FolderReader and testing its methods. Null is returned if any test fails.
        FolderReader* testFolderReader();

    protected:
        //Determine and log a true assertion.
        bool assertTrue(bool aStatement, std::string anInfo="");

        //Determine and log a false assertion.
        bool assertFalse(bool aStatement, std::string anInfo="");
        
        //Log a test result and return the result.
        bool logTestResult(bool aResult, std::string aTestInfo="");
        FolderReader* reader;
    };
}
#endif /* Test_hpp */
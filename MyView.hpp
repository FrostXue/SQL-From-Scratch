#ifndef MyView_h
#define MyView_h

#include <iostream>
#include "View.hpp"
#include "MyRow.hpp"
#include <vector>

namespace ECE141{
    using Sheet = std::vector<MyRow>;//A list of rows form a sheet
    class MyView: public View{
    public:
        MyView();
        MyView(const MyRow &aHeader,const Sheet &aData);
        virtual bool show(std::ostream &aStream);

        bool updateHeader(const MyRow &aHeader);
        bool updateHeader(const std::vector<std::string> & aHeader);
        bool updateData(const Sheet &aData);
    protected:
        MyRow header;
        Sheet data;
        bool output(std::ostream &aStream, const std::vector<std::vector<std::string>>&aSheet, const std::vector<size_t> &aColWidths);
        void printLine(std::ostream &aStream, const std::vector<size_t> &aColWidths);
        void printRow(std::ostream &aStream, const std::vector<size_t> &aColWidths, const std::vector<std::string> &aContent);
    };

    

} // namespace ECE141










#endif /* MyView_h */
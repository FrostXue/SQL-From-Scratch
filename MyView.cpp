#include "MyView.hpp"
#include <iomanip>
#include "Helpers.hpp"

namespace ECE141{
    MyView::MyView(){
        
    }

    MyView:: MyView(const MyRow &aHeader,const Sheet &aData): header(aHeader),data(aData){

    }

    bool MyView::show(std::ostream &aStream){
        try{
            if ((header.size() && data.size() && header.size() == data[0].size()) || data.size() == 0){
                //Transform every header and data to string
                std::vector<std::vector<std::string>> theSheet;
                //Enter the header
                theSheet.push_back(Helpers::toString(header.row));
                //Enter the data
                for (auto theRow : data){
                    theSheet.push_back(Helpers::toString(theRow.row));
                }

                //Determine the width of each column to accomodate the longest data in the column
                const int kMaxWidth = 30;
                std::vector<size_t> theColWidths;
                //Iterate to find the longest entry in each column
                for (int i = 0; i < header.size(); i++) theColWidths.push_back(0);
                for (auto theRow : theSheet){
                    for (int i = 0; i < theRow.size(); i++){
                        if (theRow[i].size() > theColWidths[i]){
                            theColWidths[i] = theRow[i].size() < kMaxWidth ? theRow[i].size() : kMaxWidth;
                        }
                    }
                }
                //Output
                return output(aStream, theSheet, theColWidths);
            }
        }
        catch (std::exception e) {
            std::clog << e.what() << "\n";
            return false;
        }
    }

    bool MyView::updateHeader(const MyRow &aHeader){
        header = aHeader;
        return true;
    }

    bool  MyView::updateHeader(const std::vector<std::string> & aHeader){
        header = aHeader;
        return true;
    }
    bool MyView::updateData(const Sheet &aData){
        data = aData;
        return true;
    }

    bool MyView::output(std::ostream &aStream, const std::vector<std::vector<std::string>>& aSheet, const std::vector<size_t>& aColWidths){
        //Print header
        printLine(aStream, aColWidths);
        printRow(aStream, aColWidths, aSheet[0]);
        printLine(aStream, aColWidths);
        //Print data
        for (auto theRow = aSheet.begin() + 1; theRow < aSheet.end(); theRow++){
            printRow(aStream,aColWidths,*theRow);
        }
        printLine(aStream,aColWidths);
        aStream << aSheet.size() - 1 << " rows in set\n";
        return true;
    }

    void MyView::printLine(std::ostream &aStream, const std::vector<size_t> &aColWidths){
        for(auto theWidth : aColWidths){
            aStream << "+-";
            for (size_t i = 0; i < theWidth; i++)
                aStream << "-";
            aStream << "-";
        }
        aStream << "+\n";
    }

    void MyView::printRow(std::ostream &aStream, const std::vector<size_t> &aColWidths, const std::vector<std::string> &aContent){
        if (aContent.size()) {
            for (int i = 0; i < aColWidths.size(); i++) {
                aStream << "| ";
                aStream << std::setw(aColWidths[i]) << aContent[i];
                aStream << " ";
            }
            aStream << "|\n";
        }
    }
}
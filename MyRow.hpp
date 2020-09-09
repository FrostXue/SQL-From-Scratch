#ifndef MyRow_h
#define MyRow_h

#include <variant>
#include <vector>
#include <string>
#include "Helpers.hpp"
#include "Value.hpp"
#include "Components.hpp"
//#include "Storage.hpp"

namespace ECE141{
    
    //using Entry = std::variant<int,double,char,std::string,bool,float>; //An entry in a row could be any data...
    //using Variants = std::vector<Entry>; //A list of entries form a row

    class MyRow : public Storable{
        public:
        MyRow(){}
        MyRow(const Variants &aRow):row(aRow){}
        MyRow(const MyRow &aCopy):row(aCopy.row){}
        


        MyRow& operator=(const MyRow&aCopy){row = aCopy.row; return *this;}
        MyRow& operator=(const std::vector<std::string> &aCopy){
            row.clear();
            for(auto theStr: aCopy)
                row.push_back(theStr);
            return *this;
        }

        virtual StatusResult  encode(std::ostream &aWriter){
            for (auto theEntry : row){

            }
            return StatusResult();
        }
        virtual StatusResult  decode(std::istream &aReader){

            return StatusResult();
        }
        virtual StorableType getType() {
            return StorableType::MyRow;
        }
        //const Variants::iterator& begin(){return row.begin();}
        //const Variants::iterator& end(){return row.end();}
        void clear(){row.clear();}

        int size(){return row.size();}
        Entry& operator [](int anIndex){return row[anIndex];}

        Variants row; // a list of varients
    };
}


#endif /* MyRow_h */
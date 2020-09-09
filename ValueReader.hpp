#ifndef ValueReader_hpp
#define ValueReader_hpp
#include <iostream>
#include "Errors.hpp"
#include "Components.hpp"
namespace ECE141 {
    enum class ReaderPrefix{Included, Naked};
    class ValueReader {
    public:
        ~ValueReader() {}
        std::istream& stream;

        ValueReader(std::istream& aStream) : stream{ aStream }{}

        StatusResult binary_read(int & anOutput, ReaderPrefix aPrefix = ReaderPrefix::Included){
            if (aPrefix == ReaderPrefix::Included) {
                char theType; char* theIntChars; char theBoolChar; char* theDoubleChars; char theChar;
                std::string theString;
                stream >> theType;
                switch (theType) {
                case 'I':
                    theIntChars = new char[4];
                    for (int i = 0; i < 4; i++) theIntChars[i] = stream.get();
                    anOutput = *reinterpret_cast<int*>(theIntChars);
                    delete[] theIntChars;
                    break;
                case 'B':
                    stream.get(theBoolChar);
                    anOutput = *reinterpret_cast<bool*>(&theBoolChar);
                    break;
                case 'F':
                    theDoubleChars = new char[8];
                    for (int i = 0; i < 8; i++) theDoubleChars[i] = stream.get();
                    anOutput = *reinterpret_cast<double*>(theDoubleChars);
                    delete[] theDoubleChars;
                    break;
                case 'V':
                    stream.get(theChar);
                    while (theChar != '\037') {
                        theString += theChar;
                        stream.get(theChar);
                    }
                    anOutput = static_cast<int>(*reinterpret_cast<int*>(&theString));
                    break;
                case 'C':
                    char theChar;
                    stream.get(theChar);
                    anOutput = static_cast<int>(*reinterpret_cast<int*>(&theChar));
                    break;

                }
                return StatusResult{};
            }
            else return binary_read_naked(anOutput);
        }

        StatusResult binary_read(double& anOutput, ReaderPrefix aPrefix = ReaderPrefix::Included) {
            if (aPrefix == ReaderPrefix::Included) {
                char theType; char* theIntChars; char theBoolChar; char* theDoubleChars; char theChar;
                std::string theString;
                stream >> theType;
                switch (theType) {
                case 'I':
                    theIntChars = new char[4];
                    for (int i = 0; i < 4; i++) theIntChars[i] = stream.get();
                    anOutput = *reinterpret_cast<int*>(theIntChars);
                    delete[] theIntChars;
                    break;
                case 'B':
                    stream.get(theBoolChar);
                    anOutput = *reinterpret_cast<bool*>(&theBoolChar);
                    break;
                case 'F':
                    theDoubleChars = new char[8];
                    for (int i = 0; i < 8; i++) theDoubleChars[i] = stream.get();
                    anOutput = *reinterpret_cast<double*>(theDoubleChars);
                    delete[] theDoubleChars;
                    break;
                case 'V':
                    stream.get(theChar);
                    while (theChar != '\037') {
                        theString += theChar;
                        stream.get(theChar);
                    }
                    anOutput = static_cast<double>(*reinterpret_cast<double*>(&theString));
                    break;
                case 'C':
                    char theChar;
                    stream.get(theChar);
                    anOutput = static_cast<double>(*reinterpret_cast<double*>(&theChar));
                    break;

                }
                return StatusResult{};
            }
            else return binary_read_naked(anOutput);
        }

        StatusResult binary_read(bool& anOutput, ReaderPrefix aPrefix = ReaderPrefix::Included) {
            if (aPrefix == ReaderPrefix::Included) {
                char theType; char* theIntChars; char theBoolChar; char* theDoubleChars; char theChar;
                std::string theString;
                stream >> theType;
                switch (theType) {
                case 'I':
                    theIntChars = new char[4];
                    for (int i = 0; i < 4; i++) theIntChars[i] = stream.get();
                    anOutput = *reinterpret_cast<int*>(theIntChars);
                    delete[] theIntChars;
                    break;
                case 'B':
                    stream.get(theBoolChar);
                    anOutput = *reinterpret_cast<bool*>(&theBoolChar);
                    break;
                case 'F':
                    theDoubleChars = new char[8];
                    for (int i = 0; i < 8; i++) theDoubleChars[i] = stream.get();
                    anOutput = *reinterpret_cast<double*>(theDoubleChars);
                    delete[] theDoubleChars;
                    break;
                case 'V':
                    stream.get(theChar);
                    while (theChar != '\037') {
                        theString += theChar;
                        stream.get(theChar);
                    }
                    anOutput = static_cast<bool>(*reinterpret_cast<bool*>(&theString));
                    break;
                case 'C':
                    char theChar;
                    stream.get(theChar);
                    anOutput = static_cast<bool>(*reinterpret_cast<bool*>(&theChar));
                    break;

                }
                return StatusResult{};
            }
            else return binary_read_naked(anOutput);
        }

        StatusResult binary_read(std::string& anOutput, ReaderPrefix aPrefix = ReaderPrefix::Included) {
            if (aPrefix == ReaderPrefix::Included) {
                char theType; char* theIntChars; char theBoolChar; char* theDoubleChars; char theChar;
                std::string theString;
                stream >> theType;
                switch (theType) {
                case 'I':
                    theIntChars = new char[4];
                    for (int i = 0; i < 4; i++) theIntChars[i] = stream.get();
                    anOutput = *reinterpret_cast<int*>(theIntChars);
                    delete[] theIntChars;
                    break;
                case 'B':
                    stream.get(theBoolChar);
                    anOutput = *reinterpret_cast<bool*>(&theBoolChar);
                    break;
                case 'F':
                    theDoubleChars = new char[8];
                    for (int i = 0; i < 8; i++) theDoubleChars[i] = stream.get();
                    anOutput = *reinterpret_cast<double*>(theDoubleChars);
                    delete[] theDoubleChars;
                    break;
                case 'V':
                    stream.get(theChar);
                    while (theChar != '\037') {
                        theString += theChar;
                        stream.get(theChar);
                    }
                    anOutput = static_cast<std::string>(*reinterpret_cast<std::string*>(&theString));
                    break;
                case 'C':
                    char theChar;
                    stream.get(theChar);
                    anOutput = static_cast<std::string>(*reinterpret_cast<std::string*>(&theChar));
                    break;

                }
                return StatusResult{};
            }
            else return binary_read_naked(anOutput);
        }

        StatusResult binary_read(char& anOutput, ReaderPrefix aPrefix = ReaderPrefix::Included) {
            if (aPrefix == ReaderPrefix::Included) {
                char theType; char* theIntChars; char theBoolChar; char* theDoubleChars; char theChar;
                std::string theString;
                stream >> theType;
                switch (theType) {
                case 'I':
                    theIntChars = new char[4];
                    for (int i = 0; i < 4; i++) theIntChars[i] = stream.get();
                    anOutput = *reinterpret_cast<int*>(theIntChars);
                    delete[] theIntChars;
                    break;
                case 'B':
                    stream.get(theBoolChar);
                    anOutput = *reinterpret_cast<bool*>(&theBoolChar);
                    break;
                case 'F':
                    theDoubleChars = new char[8];
                    for (int i = 0; i < 8; i++) theDoubleChars[i] = stream.get();
                    anOutput = *reinterpret_cast<double*>(theDoubleChars);
                    delete[] theDoubleChars;
                    break;
                case 'V':
                    stream.get(theChar);
                    while (theChar != '\037') {
                        theString += theChar;
                        stream.get(theChar);
                    }
                    anOutput = static_cast<char>(*reinterpret_cast<char*>(&theString));
                    break;
                case 'C':
                    char theChar;
                    stream.get(theChar);
                    anOutput = static_cast<char>(*reinterpret_cast<char*>(&theChar));
                    break;

                }
                return StatusResult{};
            }
            else return binary_read_naked(anOutput);
        }

        StatusResult binary_read(uint32_t& anOutput, ReaderPrefix aPrefix = ReaderPrefix::Included) {
            if (aPrefix == ReaderPrefix::Included) {
                char theType; char* theIntChars; char theBoolChar; char* theDoubleChars; char theChar;
                std::string theString;
                stream >> theType;
                switch (theType) {
                case 'I':
                    theIntChars = new char[4];
                    for (int i = 0; i < 4; i++) theIntChars[i] = stream.get();
                    anOutput = *reinterpret_cast<int*>(theIntChars);
                    delete[] theIntChars;
                    break;
                case 'B':
                    stream.get(theBoolChar);
                    anOutput = *reinterpret_cast<bool*>(&theBoolChar);
                    break;
                case 'F':
                    theDoubleChars = new char[8];
                    for (int i = 0; i < 8; i++) theDoubleChars[i] = stream.get();
                    anOutput = *reinterpret_cast<double*>(theDoubleChars);
                    delete[] theDoubleChars;
                    break;
                case 'V':
                    stream.get(theChar);
                    while (theChar != '\037') {
                        theString += theChar;
                        stream.get(theChar);
                    }
                    anOutput = static_cast<uint32_t>(*reinterpret_cast<uint32_t*>(&theString));
                    break;
                case 'C':
                    char theChar;
                    stream.get(theChar);
                    anOutput = static_cast<uint32_t>(*reinterpret_cast<uint32_t*>(&theChar));
                    break;

                }
                return StatusResult{};
            }
            else return binary_read_naked(anOutput);
        }

        StatusResult binary_read(Entry& anOutput, ReaderPrefix aPrefix = ReaderPrefix::Included) {
            if (aPrefix == ReaderPrefix::Included) {
                char theType; char* theIntChars; char theBoolChar; char* theDoubleChars; char theChar;
                std::string theString;
                stream >> theType;
                switch (theType) {
                case 'I':
                    theIntChars = new char[4];
                    for (int i = 0; i < 4; i++) theIntChars[i] = stream.get();
                    anOutput = *reinterpret_cast<int*>(theIntChars);
                    delete[] theIntChars;
                    break;
                case 'B':
                    stream.get(theBoolChar);
                    anOutput = *reinterpret_cast<bool*>(&theBoolChar);
                    break;
                case 'F':
                    theDoubleChars = new char[8];
                    for (int i = 0; i < 8; i++) theDoubleChars[i] = stream.get();
                    anOutput = *reinterpret_cast<double*>(theDoubleChars);
                    delete[] theDoubleChars;
                    break;
                case 'V':
                    stream.get(theChar);
                    while (theChar != '\037') {
                        theString += theChar;
                        stream.get(theChar);
                    }
                    anOutput = theString;
                    break;
                case 'C':
                    char theChar;
                    stream.get(theChar);
                    anOutput = theChar;
                    break;

                }
                return StatusResult{};
            }
            else return binary_read_naked(anOutput);
        }

    protected:
        template <typename U>
        StatusResult binary_read_naked(U& anOutput) {
            char theChars[sizeof(U)];
            for (int i = 0; i < sizeof(U); i++)
                stream >> theChars[i];
            //std::clog << "Size of U is " << sizeof(U) << "\n";
            anOutput = static_cast<U>(*reinterpret_cast<U*>(&theChars));
            //delete[] theChars;
            return StatusResult{};
        }

    };
}

//does anyone how to use template specialization in a class, and resolve the explicit specialization in non-namespace scope error? Normally, if not putting in a class we can wrap the template function and specialization in to a namespace, but cannot put a namespace in a class.


#endif
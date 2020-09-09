#ifndef ValueWriter_hpp
#define ValueWriter_hpp
#include"Tokenizer.hpp"
#include <vector>

namespace ECE141 {
    enum class ValuePrefix{Included, Naked}; //Determine whether or not to writer an identifier before the data

    struct ValueWriter {
        ~ValueWriter() {}
        std::ostream& stream;
        ValuePrefix prefix;

        ValueWriter(std::ostream& aStream, ValuePrefix aPrefix = ValuePrefix::Included) : stream{ aStream }, prefix{ aPrefix } {}

        template<typename T>
        StatusResult binary_write(T const& aValue, char theTypeChar) const {
            if (prefix == ValuePrefix::Included)
                stream.write(reinterpret_cast<const char*>(&theTypeChar), sizeof(char));

            stream.write(reinterpret_cast<const char*>(&aValue), sizeof(T));          

            return StatusResult{};
        }

        StatusResult operator()(bool const& aValue) {
            //const bool theCopy = *reinterpret_cast<const bool*>(reinterpret_cast<const char*>(&aValue));
            //std::clog << "The value passed in: " << aValue << ", while the copy is " << theCopy << "\n";

            return binary_write(aValue, 'B');

        }

        StatusResult operator()(int const& aValue) {
            return binary_write(aValue, 'I');
        }

        StatusResult operator()(uint32_t const& aValue) {
            return binary_write(aValue, 'I');
        }

        StatusResult operator()(double const& aValue) {
            return binary_write(aValue, 'F');
        }
        StatusResult operator()(std::string const& aValue) {
            //stream << aValue;
            //stream << 'V';
            if (prefix == ValuePrefix::Included)
                stream << 'V';
            stream << aValue;
            stream << '\037';//unit seperator
            return StatusResult{};
        }

        StatusResult operator()(char const& aValue) {
            return binary_write(aValue, 'C');
        }

        //StatusResult operator()(char aValue) { // for putting in spaces
        //    stream << aValue;
        //    //return binary_write(aValue, 'F');
        //    return StatusResult{};
        //}
    };

}
#endif /* ValueWriter_hpp */

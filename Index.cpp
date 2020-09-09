#include "Index.hpp"
namespace ECE141 {

	StatusResult Index::encode(std::ostream& aWriter) {
		ValueWriter theWriter(aWriter);
		theWriter.operator()(field); //field name
		theWriter.operator()(static_cast<char>(type)); //data type
		theWriter.operator()(schemaId); //schema id
		theWriter.operator()(changed); //dirty
		for (auto thePair : list) {
			std::visit(theWriter, thePair.first); //indexes
			theWriter.operator()(thePair.second);
		}

		return StatusResult();
	}

	StatusResult Index::decode(std::istream& aReader) {
		ValueReader theReader(aReader);
		theReader.binary_read(field);

		char theType;
		theReader.binary_read(theType);
		type = static_cast<DataType>(theType);

		theReader.binary_read(schemaId);
		theReader.binary_read(changed);
		while (aReader.good() && aReader.peek() != '\0') {
			Entry theKey = 0;
			theReader.binary_read(theKey);

			uint32_t theVal = 0;
			theReader.binary_read(theVal);

			list[theKey] = theVal;
		}

		return StatusResult();
	}
}
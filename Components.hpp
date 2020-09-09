#ifndef Components_h
#define Components_h

#include <variant>
#include <vector>
namespace ECE141 {
	using Entry = std::variant<int, double, char, std::string, bool>; //An entry in a row could be any data...
	using Variants = std::vector<Entry>; //A list of entries form a row
	enum class WriteMethod { WriteNew, OverwriteExisting };

	struct TableField {
		TableField(const std::string& aTableName = "", const std::string& aFieldName = "")
			: tableName(aTableName), fieldName(aFieldName) {}

		TableField(const TableField& aCopy) {
			tableName = aCopy.tableName;
			fieldName = aCopy.fieldName;
		}

		std::string tableName;
		std::string fieldName;
	};
	struct Join {
		Join(const std::string& aTable, ECE141::Keywords aType, const std::string& aLHS, const std::string& aRHS)
			: table(aTable), joinType(aType), onLeft(aLHS), onRight(aRHS) {}

		Join(const std::string& aTable, ECE141::Keywords aType, TableField aLHS, TableField aRHS)
			: table(aTable), joinType(aType), onLeft(aLHS), onRight(aRHS) {}

		Join(const Join& aCopy) {
			joinType = aCopy.joinType;
			table = aCopy.table;
			onLeft = aCopy.onLeft;
			onRight = aCopy.onRight;
		}

		Join operator=(const Join& aCopy) {
			joinType = aCopy.joinType;
			table = aCopy.table;
			onLeft = aCopy.onLeft;
			onRight = aCopy.onRight;
			return *this;
		}

		~Join() {}

		void show() {
			std::clog << "join type: ";
			if (joinType == ECE141::Keywords::left_kw) { std::clog << "left join" << std::endl; }
			else if (joinType == ECE141::Keywords::right_kw) { std::clog << "right join" << std::endl; }
			else { std::clog << std::endl; }
			std::clog << "table name: " << table << std::endl;
			std::clog << "on left: " << std::endl;
			std::clog << "table name: " << onLeft.tableName << std::endl;
			std::clog << "field name: " << onLeft.fieldName << std::endl;
			std::clog << "on right: " << std::endl;
			std::clog << "table name: " << onRight.tableName << std::endl;
			std::clog << "field name: " << onRight.fieldName << std::endl;
		}

		ECE141::Keywords    joinType;
		std::string table;
		TableField  onLeft;
		TableField  onRight;
	};

}

#endif Components_h

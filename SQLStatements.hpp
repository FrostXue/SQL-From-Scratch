#ifndef SQLStatement_hpp
#define SQLStatement_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <optional>
#include "Statement.hpp"
#include "Attribute.hpp"
#include "Schema.hpp"
#include "Tokenizer.hpp"
#include "TokenSequence.hpp"
#include "Row.hpp"
#include "SQLProcessor.hpp"
namespace ECE141 {

	class SQLProcessor;
	struct TableName {
		TableName(const std::string& aTableName = "", const std::string& anAlias = "")
			: tableName(aTableName), alias(anAlias) {}

		TableName& operator=(const std::string& aName) {
			tableName = aName;
			return *this;
		}

		operator const std::string() { return tableName; }

		std::string tableName;
		std::string alias;
	};

	
	//-----------------------------------------------------------------------

	class SQLStatement : public Statement {
	public:
		SQLStatement(SQLProcessor& aProc, Keywords aStatementType = Keywords::unknown_kw);
		virtual StatusResult parse(Tokenizer& aTokenizer);

		//StatusResult parseValueList(StringList& aList, Tokenizer& aTokenizer);
		//StatusResult parseIdentifierList(StringList& aList, Tokenizer& aTokenizer);
		//StatusResult parseIdValueList(KeyValues& aList, Tokenizer& aTokenizer);

		virtual StatusResult run(std::ostream &anOutput);

		TableName tableName;
		SQLProcessor& processor;
	};

	//-----------------------------------------------------------------------

	class CreateTableStatement: public SQLStatement{
	public:
		CreateTableStatement(SQLProcessor& aProc);

		StatusResult parseAttributes(Tokenizer& aTokenizer);
		StatusResult parse(Tokenizer& aTokenizer);

		StatusResult run(std::ostream& aStream) const;

		std::string name;
		std::vector<Attribute> attributes;
	};

	class DescribeTableStatement : public SQLStatement {
	public:
		DescribeTableStatement(SQLProcessor& aProc);

		StatusResult parse(Tokenizer& aTokenizer);
		StatusResult run(std::ostream& aStream) const;

		std::string name;
	};

	class ShowTableStatement : public SQLStatement {
	public:
		ShowTableStatement(SQLProcessor& aProc);

		StatusResult parse(Tokenizer& aTokenizer);
		StatusResult run(std::ostream& aStream) const;
	};

	class DropTableStatement : public SQLStatement {
	public:
		DropTableStatement(SQLProcessor& aProc);

		StatusResult parse(Tokenizer& aTokenizer);
		StatusResult run(std::ostream& aStream) const;

		std::string name;
	};

	class InsertIntoStatement : public SQLStatement {
	public:
		InsertIntoStatement(SQLProcessor& aProc);

		StatusResult parseRows(Tokenizer& aTokenizer);
		StatusResult parse(Tokenizer& aTokenizer) override;

		StatusResult run(std::ostream& aStream) const;

		std::string name;
		std::vector<Row> attributes;
		std::vector<std::vector<Token>> rowTokens;
	};

	class DeleteFromStatement : public SQLStatement {
	public:
		DeleteFromStatement(SQLProcessor& aProc);

		StatusResult parse(Tokenizer& aTokenizer);
		StatusResult run(std::ostream& aStream) const;
		StatusResult parseExpressions(Tokenizer& aTokenizer);

		std::string name;
		std::vector<std::vector<Token>> expressions; // expression conditions, can be empty
	};

	class SelectStatement : public SQLStatement {
	public:
		SelectStatement(SQLProcessor& aProc);

		StatusResult parseFields(Tokenizer& aTokenizer);
		StatusResult parseExpressions(Tokenizer& aTokenizer);
		StatusResult parseJoin(Tokenizer& aTokenizer);
		StatusResult parse(Tokenizer& aTokenizer) override;

		StatusResult run(std::ostream& aStream) const;

		std::string tableName; // select from a table
		std::vector<Token> fields; // what fields? can be *
		std::vector<std::vector<Token>> expressions; // expression conditions, can be empty
		std::vector<Join> joins;
	};

	class UpdateStatement : public SQLStatement {
	public:
		UpdateStatement(SQLProcessor& aProc);

		StatusResult parseKVPairs(Tokenizer& aTokenizer);
		StatusResult parseExpressions(Tokenizer& aTokenizer);
		StatusResult parse(Tokenizer& aTokenizer) override;

		StatusResult run(std::ostream& aStream) const;

		std::string tableName; // select from a table
		std::vector<std::vector<Token>> KVPairs; // what KV pairs that are going to be updated?
		std::vector<std::vector<Token>> expressions; // expression conditions, can be empty
	};

	class ShowIndexesStatement : public SQLStatement {
	public:
		ShowIndexesStatement(SQLProcessor& aProc);

		StatusResult parse(Tokenizer& aTokenizer) override;

		StatusResult run(std::ostream& aStream) const;
	};

	class AlterStatement : public SQLStatement {
	public:
		AlterStatement(SQLProcessor& aProc);
		StatusResult parse(Tokenizer& aTokenizer) override;
		StatusResult run(std::ostream& aStream) const;
	protected:
		Attribute toModify;
	};
}

#endif /* SQLStatement */
#include "SQLStatements.hpp"


namespace ECE141 {

	SQLStatement::SQLStatement(SQLProcessor& aProc, Keywords aStatementType) : Statement(aStatementType) , processor(aProc) {

	}

	StatusResult SQLStatement::parse(Tokenizer& aTokenizer) {
		return StatusResult{};
	}

	StatusResult SQLStatement::run(std::ostream& anOutput) {
		//std::clog << "here" << std::endl;
		return StatusResult{};
	}

	CreateTableStatement::CreateTableStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::createTB_kw) {
		name = "";
	}

	StatusResult CreateTableStatement::parse(Tokenizer& aTokenizer) {
		//std::clog << "in create table statement parse function" << std::endl;
		const int kMinLength = 8; // smallest # of tokens in create statement
		StatusResult theResult(syntaxError, syntaxError);
		//aTokenizer.dump();
		if (kMinLength <= aTokenizer.remaining()) {
			
			if (Keywords::table_kw == aTokenizer.peek(1).keyword) {
				TokenSequence getTokens(aTokenizer);
				std::vector<Token> tokens;
				tokens = getTokens.getTokens();
				aTokenizer.restart();
				/*for (int i = 0; i < tokens.size(); ++i) {
					std::clog << tokens[i].data << std::endl;
				}*/
				if (tokens[0].keyword == Keywords::create_kw && tokens[1].keyword == Keywords::table_kw && tokens[2].type == TokenType::identifier && tokens[3].data.length() >= 1 && tokens[3].data[0] == '(') { // check the statement is ok
					
					return parseAttributes(aTokenizer);
				}	
				else {
					return StatusResult(unknownCommand, unknownCommand);
				}
			}
		}
		return StatusResult(unknownCommand,unknownCommand);
	}

	//========================================================================utility functions========================================================================

	bool createTableStatementSyntaxCheckCase1(std::vector<std::vector<Token>> listOfPotentialAttributes) {
		for (int i = 0; i < listOfPotentialAttributes.size(); ++i) {
			if (listOfPotentialAttributes[i].size() == 0) {
				return false;
			}
			else if (listOfPotentialAttributes[i][0].type != TokenType::identifier) {
				return false;
			}
		}
		return true;
	}

	bool createTableStatementSyntaxCheckCase2(std::vector<std::vector<Token>> listOfPotentialAttributes) {
		for (int i = 0; i < listOfPotentialAttributes.size(); ++i) {
			for (int j = 0; j < listOfPotentialAttributes[i].size(); ++j) {
				if (listOfPotentialAttributes[i][j].type == TokenType::keyword && listOfPotentialAttributes[i][j].data.length() >= 7 && listOfPotentialAttributes[i][j].data[0] == 'v' && listOfPotentialAttributes[i][j].data[1] == 'a' && listOfPotentialAttributes[i][j].data[1] == 'r') {
					if ((j + 3) >= listOfPotentialAttributes[i].size() || listOfPotentialAttributes[i][j + 1].data.length() == 0 || listOfPotentialAttributes[i][j + 2].data.length() == 0 || listOfPotentialAttributes[i][j + 3].data.length() == 0 || listOfPotentialAttributes[i][j + 1].data[0] != '(' || listOfPotentialAttributes[i][j + 2].type != TokenType::number || listOfPotentialAttributes[i][j + 3].data[0] != ')') {
						return false;
					}
				}
			}
		}
		return true;
	}

	bool createTableStatementSyntaxCheckCase3(std::vector<std::vector<Token>> listOfPotentialAttributes) {
		for (int i = 0; i < listOfPotentialAttributes.size(); ++i) {
			for (int j = 0; j < listOfPotentialAttributes[i].size(); ++j) {
				if (listOfPotentialAttributes[i][j].type == TokenType::identifier && listOfPotentialAttributes[i][j].data.length() >= 7 && listOfPotentialAttributes[i][j].data[0] == 'D' && listOfPotentialAttributes[i][j].data[1] == 'E' && listOfPotentialAttributes[i][j].data[2] == 'F' && listOfPotentialAttributes[i][j].data[3] == 'A' && listOfPotentialAttributes[i][j].data[4] == 'U' && listOfPotentialAttributes[i][j].data[4] == 'L' && listOfPotentialAttributes[i][j].data[5] == 'T') {
					if (j + 1 <= listOfPotentialAttributes[i].size()) {
						return false;
					}
				}
			}
		}
		return true;
	}

	bool createTableStatementSyntaxCheckCase4(std::vector<std::vector<Token>> listOfPotentialAttributes) {
		for (int i = 0; i < listOfPotentialAttributes.size(); ++i) {
			for (int j = 0; j < listOfPotentialAttributes[i].size(); ++j) {
				if (listOfPotentialAttributes[i][j].type == TokenType::keyword && listOfPotentialAttributes[i][j].data.length() >= 7 && listOfPotentialAttributes[i][j].data[0] == 'p' && listOfPotentialAttributes[i][j].data[1] == 'r' && listOfPotentialAttributes[i][j].data[2] == 'i' && listOfPotentialAttributes[i][j].data[3] == 'm' && listOfPotentialAttributes[i][j].data[4] == 'a' && listOfPotentialAttributes[i][j].data[4] == 'r' && listOfPotentialAttributes[i][j].data[5] == 'y') {
					if (j + 1 <= listOfPotentialAttributes[i].size() || listOfPotentialAttributes[i][j + 1].type != TokenType::keyword || listOfPotentialAttributes[i][j + 1].data[0] != 'k') {
						return false;
					}
				}
			}
		}
		return true;
	}

	bool createTableStatementSyntaxCheck(std::vector<std::vector<Token>> listOfPotentialAttributes) { //syntax check, will go through 4 cases
		if (createTableStatementSyntaxCheckCase1(listOfPotentialAttributes)) {
			if (createTableStatementSyntaxCheckCase2(listOfPotentialAttributes)) {
				if (createTableStatementSyntaxCheckCase3(listOfPotentialAttributes)) {
					if (createTableStatementSyntaxCheckCase4(listOfPotentialAttributes)) {
						return true;
					}
				}
			}
		}
		return false;
	}

	void setAttributeType(Attribute& theAttribute, std::vector<std::vector<Token>> listOfPotentialAttributes, int i) { // brout force break out separate function, try to understand it with the following function
		if (listOfPotentialAttributes[i][1].type == TokenType::keyword && listOfPotentialAttributes[i][1].data[0] == 'i') {
			theAttribute.setType(DataType::int_type);
			//std::clog << "attribute type is: " << static_cast<char>(theAttribute.getType()) << std::endl;
		}
		else if (listOfPotentialAttributes[i][1].type == TokenType::keyword && listOfPotentialAttributes[i][1].data[0] == 'v') {
			theAttribute.setType(DataType::varchar_type);
			theAttribute.setSize(std::stoi(listOfPotentialAttributes[i][3].data));
			//std::clog << "attribute type is: " << static_cast<char>(theAttribute.getType()) << std::endl;
		}
		else if (listOfPotentialAttributes[i][1].type == TokenType::keyword && listOfPotentialAttributes[i][1].data[0] == 'f') {
			theAttribute.setType(DataType::float_type);
			//std::clog << "attribute type is: " << static_cast<char>(theAttribute.getType()) << std::endl;
		}
		else if (listOfPotentialAttributes[i][1].type == TokenType::keyword && listOfPotentialAttributes[i][1].data[0] == 'T') {
			theAttribute.setType(DataType::datetime_type);
			//std::clog << "attribute type is: " << static_cast<char>(theAttribute.getType()) << std::endl;
		}
		else if (listOfPotentialAttributes[i][1].type == TokenType::keyword && (listOfPotentialAttributes[i][1].data[0] == 'b' || listOfPotentialAttributes[i][1].data[0] == 'B')) {
			theAttribute.setType(DataType::bool_type);
			//std::clog << "attribute type is: " << static_cast<char>(theAttribute.getType()) << std::endl;
		}
		else {
			theAttribute.setType(DataType::no_type);
			//std::clog << "attribute type is: " << static_cast<char>(theAttribute.getType()) << std::endl;
		}
	}

	std::vector<Attribute> makeAttributesList(std::vector<std::vector<Token>> listOfPotentialAttributes) {
		std::vector<Attribute> attributes;
		for (int i = 0; i < listOfPotentialAttributes.size(); ++i) {
			Attribute theAttribute;
			theAttribute.setName(listOfPotentialAttributes[i][0].data);
			setAttributeType(theAttribute, listOfPotentialAttributes, i);
			//std::clog << "here" << std::endl;
			for (int j = 2; j < listOfPotentialAttributes[i].size(); ++j) {
				if (listOfPotentialAttributes[i][j].type == TokenType::keyword && listOfPotentialAttributes[i][j].keyword == Keywords::auto_increment_kw) { theAttribute.setAutoIncrement(true); }
				if (j + 1 < listOfPotentialAttributes[i].size() && listOfPotentialAttributes[i][j].type == TokenType::keyword && listOfPotentialAttributes[i][j].keyword == Keywords::primary_kw && listOfPotentialAttributes[i][j+1].keyword == Keywords::key_kw) { theAttribute.setPrimaryKey(true); ++j; }
				if (j + 1 < listOfPotentialAttributes[i].size() && listOfPotentialAttributes[i][j].type == TokenType::keyword && listOfPotentialAttributes[i][j].keyword == Keywords::not_kw && listOfPotentialAttributes[i][j + 1].keyword == Keywords::null_kw) { theAttribute.setNullable(false); ++j; }
				
				
				if (j + 1 < listOfPotentialAttributes[i].size() && listOfPotentialAttributes[i][j].type == TokenType::keyword && listOfPotentialAttributes[i][j].keyword == Keywords::default_kw) {
					if (listOfPotentialAttributes[i][j+1].type == TokenType::number) {
						theAttribute.defaultValue = std::stod(listOfPotentialAttributes[i][j + 1].data);
					}
					else if(listOfPotentialAttributes[i][j + 1].type == TokenType::identifier){
						if (listOfPotentialAttributes[i][j + 1].data[0] == 'F' || listOfPotentialAttributes[i][j + 1].data[0] == 'f') {
							theAttribute.defaultValue = false;
						}
						if (listOfPotentialAttributes[i][j + 1].data[0] == 'T' || listOfPotentialAttributes[i][j + 1].data[0] == 't') {
							theAttribute.defaultValue = true;
						}
					}
					++j;
				}
			}
			attributes.push_back(theAttribute);
			//std::clog << "here" << std::endl;
		}
		return attributes;
	}

	bool insertIntoSyntaxCheck(std::vector<Token> tokens) {
		if (tokens[0].keyword == Keywords::insert_kw && tokens[1].keyword == Keywords::into_kw && tokens[2].type == TokenType::identifier && tokens[3].data.length() >= 1 && tokens[3].data[0] == '(') { // check the statement is ok
			int valueCount = 0;
			int leftParaCount = 0;
			int rightParaCount = 0;
			for (int i = 0; i < tokens.size(); ++i) {
				if (tokens[i].keyword == Keywords::values_kw) {
					valueCount += 1;
				}
				if (tokens[i].data[0] == '(' && tokens[i].type == TokenType::punctuation) {
					leftParaCount += 1;
				}
				if (tokens[i].data[0] == ')' && tokens[i].type == TokenType::punctuation) {
					rightParaCount += 1;
				}
			}
			if (valueCount == 1 && leftParaCount == rightParaCount) {
				TokenSequence getTokens(tokens);
				std::vector<std::vector<Token>> tokensBetweenPara = getTokens.parseTokensBetweenTwoPara();
				for (int i = 1; i < tokensBetweenPara.size(); ++i) {
					if (tokensBetweenPara[i].size() != tokensBetweenPara[i-1].size()) {
						return false;
					}
				}
				return true;
			}
		}
		return false;
	}

	//=====================================================================================================================================

	StatusResult CreateTableStatement::parseAttributes(Tokenizer& aTokenizer) {
		StatusResult theResult(syntaxError, syntaxError);
		TokenSequence getList(aTokenizer);
		std::vector<Token> tokens = getList.getTokens();
		
		name = tokens[2].data; // store name in statement
		if (tokens[tokens.size() - 1].data[0] != ')') { return theResult; } // syntax error
		tokens.pop_back();
		for (int i = 0; i < 4; ++i) { // pop front 4 times
			tokens.erase(tokens.begin());
		}

		// starting parsing
		TokenSequence theTokenSequence(tokens);
		std::vector<std::vector<Token>> listOfPotentialAttributes = theTokenSequence.tokensParseBy(TokenType::punctuation, ',');

		if (!createTableStatementSyntaxCheck(listOfPotentialAttributes)) { return theResult; }
		//std::clog << "passed syntax check" << std::endl;
		attributes = makeAttributesList(listOfPotentialAttributes);
		return StatusResult(noError);
	}

	

	StatusResult CreateTableStatement::run(std::ostream& aStream) const {
		//convert stored statement.attributes to an entity...
		Schema* theSchema = new Schema(this->name);
		for (auto theAttr : attributes) {
			//std::clog << "the Attr: " << theAttr.getName() << std::endl;
			theSchema->addAttribute(theAttr);
		}
		//std::clog << "create table statement running" << std::endl;
		return processor.createTable(*theSchema); //DB will "own" the entity...
	}

	InsertIntoStatement::InsertIntoStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::insert_kw) {
		name = "";
	}

	StatusResult InsertIntoStatement::parseRows(Tokenizer& aTokenizer) {
		/*std::clog << "in insert statement parse row function" << std::endl;*/
		std::vector<Token> tokens;
		TokenSequence getTokens(aTokenizer);
		tokens = getTokens.getTokens();
		std::vector<std::vector<Token>> tokensBetweenPara = getTokens.parseTokensBetweenTwoPara(); // at this step inside the paraen same amount of tokens for sure
		std::vector<std::vector<Token>> tokensBetweenParaWithOutCommas; // first item is a list of attributes name
		for (int i = 0; i < tokensBetweenPara.size(); ++i) {
			TokenSequence theSequence(tokensBetweenPara[i]);
			tokensBetweenParaWithOutCommas.push_back(theSequence.tokensSequenceErase(TokenType::punctuation, ','));
		}
		/*for (int i = 0; i < tokensBetweenParaWithOutCommas.size(); ++i) {
			for (int j = 0; j < tokensBetweenParaWithOutCommas[i].size(); ++j) {
				std::clog << tokensBetweenParaWithOutCommas[i][j].data << ' ';
			}
			std::clog << std::endl;
		}*/
		rowTokens = tokensBetweenParaWithOutCommas;
		return StatusResult{};
	}

	StatusResult InsertIntoStatement::parse(Tokenizer& aTokenizer) {
		std::clog << "in insert statement parse function" << std::endl;
		const int kMinLength = 4; // smallest # of tokens in create statement
		StatusResult theResult(syntaxError, syntaxError);
		//aTokenizer.dump();
		if (kMinLength <= aTokenizer.remaining()) {

			if (Keywords::into_kw == aTokenizer.peek(1).keyword) {
				std::vector<Token> tokens;
				TokenSequence getTokens(aTokenizer);
				tokens = getTokens.getTokens();
				name = tokens[2].data; // put in table name
				if (insertIntoSyntaxCheck(tokens)) {
					return parseRows(aTokenizer);
				}
				else {
					return StatusResult(unknownCommand, unknownCommand);
				}
			}
		}
		return StatusResult(unknownCommand, unknownCommand);
	}

	StatusResult InsertIntoStatement::run(std::ostream& aStream) const {
		//std::clog << "in insert statement run function" << std::endl;
		return processor.insert(name, rowTokens);
		return StatusResult{};
	}

	DropTableStatement::DropTableStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::dropTB_kw) {
		name = "";
	}

	StatusResult DropTableStatement::parse(Tokenizer& aTokenizer) {
		//std::clog << "in create table statement parse function" << std::endl;
		const int kMinAndMaxLength = 3; // smallest # of tokens in create statement
		StatusResult theResult(syntaxError, syntaxError);
		//aTokenizer.dump();
		if (kMinAndMaxLength == aTokenizer.remaining()) {

			if (Keywords::table_kw == aTokenizer.peek(1).keyword) {

				Keywords theKeywords[] = { Keywords::drop_kw, Keywords::table_kw };
				std::vector<Token> tokens;

				while (aTokenizer.more()) { // consume all tokens 
					tokens.push_back(aTokenizer.current()); // push in all the tokens after help
					aTokenizer.next(); // consumed that token, good to go
				}
				//aTokenizer.restart();
				/*for (int i = 0; i < tokens.size(); ++i) {
					std::clog << tokens[i].data << std::endl;
				}*/
				if (tokens[0].keyword == Keywords::drop_kw && tokens[1].keyword == Keywords::table_kw && tokens[2].type == TokenType::identifier && tokens[2].data.length() >= 1) { // check the statement is ok
					name = tokens[2].data;
					return StatusResult(noError);
				}
				else {
					return StatusResult(unknownCommand, unknownCommand);
				}
			}
		}
		return StatusResult(unknownCommand, unknownCommand);
	}

	StatusResult DropTableStatement::run(std::ostream& aStream) const {
		
		return processor.dropTable(name); //DB will "own" the entity...
	}

	DescribeTableStatement::DescribeTableStatement(SQLProcessor& aProc) :SQLStatement(aProc,Keywords::describeTB_kw), name("") {

	}

	StatusResult DescribeTableStatement::parse(Tokenizer& aTokenizer) {
		const int kLength = 2; // must have 2 tokens
		if (kLength == aTokenizer.remaining()) {
			if (Keywords::describe_kw == aTokenizer.current().keyword && TokenType::identifier == aTokenizer.peek(1).type) {
				name = aTokenizer.peek(1).data;
				aTokenizer.next();
				aTokenizer.next();
				return StatusResult(noError);
			}
		}
		return StatusResult(unknownCommand, unknownCommand);
	}
	StatusResult DescribeTableStatement::run(std::ostream& aStream) const {
		return processor.describeTable(name);
	}


	ShowTableStatement::ShowTableStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::describe_kw) { // why is describe key word here?

	}

	StatusResult ShowTableStatement::parse(Tokenizer& aTokenizer){
		const int kLength = 2; // must have 2 tokens
		if (kLength == aTokenizer.remaining()) {
			//aTokenizer.dump();
			if (Keywords::show_kw == aTokenizer.current().keyword && Keywords::tables_kw == aTokenizer.peek(1).keyword) {
				aTokenizer.next();
				aTokenizer.next();
				return StatusResult(noError);
			}
		}
		return StatusResult(unknownCommand, unknownCommand);
	}
	StatusResult ShowTableStatement::run(std::ostream& aStream) const {
		return processor.showTables();
	}

	DeleteFromStatement::DeleteFromStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::dropTB_kw) {
		name = "";
	}

	StatusResult DeleteFromStatement::parse(Tokenizer& aTokenizer) {
		//std::clog << "in create table statement parse function" << std::endl;
		const int kMinAndMaxLength = 3; // smallest # of tokens in create statement
		StatusResult theResult(syntaxError, syntaxError);
		//aTokenizer.dump();
		if (aTokenizer.remaining() >= kMinAndMaxLength) {

			if (Keywords::from_kw == aTokenizer.peek(1).keyword) {

				std::vector<Token> tokens;

				while (aTokenizer.more()) { // consume all tokens 
					tokens.push_back(aTokenizer.current()); // push in all the tokens after help
					aTokenizer.next(); // consumed that token, good to go
				}
				//aTokenizer.restart();
				/*for (int i = 0; i < tokens.size(); ++i) {
					std::clog << tokens[i].data << std::endl;
				}*/
				if (tokens[0].keyword == Keywords::delete_kw && tokens[1].keyword == Keywords::from_kw && tokens[2].type == TokenType::identifier && tokens[2].data.length() >= 1) { // check the statement is ok
					name = tokens[2].data;
					parseExpressions(aTokenizer);
					return StatusResult(noError);
				}
				else {
					return StatusResult(unknownCommand, unknownCommand);
				}
			}
		}
		return StatusResult(unknownCommand, unknownCommand);
	}

	StatusResult DeleteFromStatement::parseExpressions(Tokenizer& aTokenizer) {
		//std::clog << "in delete from statement parse expressions function" << std::endl;
		std::vector<Token> tokens;
		TokenSequence getTokens(aTokenizer);
		tokens = getTokens.getTokens();
		int whereIndex = -1;
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i].keyword == Keywords::where_kw) { whereIndex = i; break; }
		}
		if (whereIndex != -1) { // where does exist
			tokens.erase(tokens.begin(), tokens.begin() + whereIndex); // leave tokens after where keyword including where key word
			/*for (int i = 0; i < tokens.size(); ++i) {
				std::clog << tokens[i].data << std::endl;
			}*/
		}
		expressions.push_back(tokens);
		/*for (int i = 0; i < expressions.size(); ++i) {
			for (int j = 0; j < expressions[i].size(); ++j) {
				std::clog << expressions[i][j].data << ' ';
			}
			std::clog << std::endl;
		}*/
		//std::clog << "finished delete from statement parse expressions function" << std::endl;
		return StatusResult(noError, noError);
	}

	StatusResult DeleteFromStatement::run(std::ostream& aStream) const {

		return processor.deleteFrom(name, expressions); //DB will "own" the entity...
	}

	SelectStatement::SelectStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::createTB_kw) {
		tableName = "";
	}

	StatusResult SelectStatement::parseFields(Tokenizer& aTokenizer) {
		//std::clog << "in select statement parse fields function" << std::endl;
		TokenSequence getTokens(aTokenizer);
		std::vector<Token> tokens = getTokens.getTokens();		
		/*for (int i = 0; i < tokens.size(); ++i) {
			std::clog << tokens[i].data << std::endl;
		}*/
		if (tokens.size() >= 4 && tokens[1].data[0] == '*') {			
			return StatusResult(noError, noError);
		}
		int fromIndex = -1;
		for (int i = 0; i < tokens.size(); ++i) { // iterate through tokens
			if (tokens[i].keyword == Keywords::from_kw) {
				fromIndex = i;
				break;
			}
		}
		//for (int i = 0; i < tokens.size(); ++i) { // iterate through tokens
		//	std::clog << tokens[i].data << std::endl;
		//}
		if (fromIndex == -1) {
			return StatusResult(syntaxError, syntaxError);
		}
		std::vector<Token> fieldTokens;
		for (int i = 1; i < fromIndex; ++i) {
			fieldTokens.push_back(tokens[i]);
		}
		/*for (int i = 0; i < fieldTokens.size(); ++i) {
			std::clog << fieldTokens[i].data << std::endl;
		}*/
		TokenSequence theSequence(fieldTokens);
		std::vector<std::vector<Token>> temp = theSequence.tokensParseBy(TokenType::punctuation, ',');
		std::vector<Token> resultFieldsVector; // caution!!! fields are single token, so a single vector will do, vector of vector is for utilizing token sequence function
		for (int i = 0; i < temp.size(); ++i) {
			for (int j = 0; j < temp[i].size(); ++j) {
				resultFieldsVector.push_back(temp[i][j]);
				//std::clog << fields[i][j].data << std::endl;
			}
		}
		/*for (int i = 0; i < resultFieldsVector.size(); ++i) {
			std::clog << resultFieldsVector[i].data << std::endl;
		}*/
		fields = resultFieldsVector;
 		return StatusResult(noError, noError);
	}

	StatusResult SelectStatement::parseExpressions(Tokenizer& aTokenizer) { // right now only can handle one condition, fix this later
		//std::clog << "in select statement parse expressions function" << std::endl;
		TokenSequence getTokens(aTokenizer);
		std::vector<Token> tokens = getTokens.getTokens();
		if (tokens.back().data == ";") { tokens.pop_back(); }

		//Delete up to and including the table name
		auto theTableName = std::find_if(tokens.begin(), tokens.end(),
			[*this](Token& aToken) {return aToken.data == tableName; });

		if (theTableName != tokens.end()) { // found the table name

			//find where, order by, or limit
			for (int i = std::distance(tokens.begin(),theTableName) + 1; i < tokens.size(); i++) {
				if ((tokens[i].keyword == Keywords::order_kw)
					|| tokens[i].keyword == Keywords::where_kw
					|| tokens[i].keyword == Keywords::limit_kw) {
					expressions.push_back(std::vector<Token>{tokens[i]});
				}
				else {
					if (0 == expressions.size()) { return StatusResult(syntaxError); }
					expressions.back().push_back(tokens[i]);
				}
			}

			//check that "order" is followed by "by"
			auto theOrderBy = std::find_if(expressions.begin(), expressions.end(),
				[](std::vector<Token>& aTokens) {return aTokens.size() && aTokens[0].keyword == Keywords::order_kw; });
			if (theOrderBy != expressions.end())
				if (theOrderBy->size() < 2 || (*theOrderBy)[1].keyword != Keywords::by_kw) return StatusResult(syntaxError);

			/*for (int i = 0; i < expressions.size(); ++i) {
				for (int j = 0; j < expressions[i].size(); ++j) {
					std::clog << expressions[i][j].data << ' ';
				}
				std::clog << std::endl;
			}*/
			return StatusResult{};
		}

		return StatusResult(syntaxError);
	}

	//jointype JOIN tablename ON table1.field=table2.field
	StatusResult SelectStatement::parseJoin(Tokenizer& aTokenizer) {
		//std::clog << "in select statement parse join function" << std::endl;
		StatusResult theResult{ joinTypeExpected , joinTypeExpected}; //add joinTypeExpected to your errors file if missing...
		TokenSequence getTokens(aTokenizer);
		std::vector<Token> tokens = getTokens.getTokens();
		if (tokens.back().data == ";") { tokens.pop_back(); }

		//aTokenizer.dump();

		Keywords aType = Keywords::unknown_kw;

		for (int i = 0; i < tokens.size(); ++i) { // get join type, left or right
			if (tokens[i].keyword == Keywords::join_kw) {
				if (i - 1 >= 0 && i - 1 < tokens.size()) {
					aType = tokens[i - 1].keyword;
					break;
				}
			}
		}

		// reached here means join key word is found
		if (aType == Keywords::unknown_kw) { return StatusResult(notJoinSelect, notJoinSelect); }

		TableField theLHS("","");
		TableField theRHS("","");

		int indexOfOn = -1;
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i].keyword == Keywords::on_kw) {
				indexOfOn = i;
			}
		}

		if (indexOfOn == -1) { return StatusResult(syntaxError, syntaxError); }

		//std::clog << indexOfOn << std::endl;

		Join theJoin(tableName, aType, std::string(""), std::string(""));

		tokens.erase(tokens.begin(), tokens.begin() + indexOfOn + 1);

		/*for (int i = 0; i < tokens.size(); ++i) {
			std::clog << tokens[i].data << std::endl;
		}*/

		TokenSequence theParser(tokens);

		std::vector<std::vector<Token>> separatedByEq = theParser.tokensParseBy(TokenType::operators, '=');

		/*for (int i = 0; i < separatedByEq.size(); ++i) {
			for (int j = 0; j < separatedByEq[i].size(); ++j) {
				std::clog << separatedByEq[i][j].data << ' ';
			}
			std::clog << std::endl;
		}*/

		if (separatedByEq.size() != 2) { return StatusResult(syntaxError, syntaxError); }
		else { // in this branch means there are 2 elements
			TokenSequence theParser1(separatedByEq[0]);
			TokenSequence theParser2(separatedByEq[1]);
			std::vector<std::vector<Token>> separatedByDot1 = theParser1.tokensParseBy(TokenType::operators, '.');
			std::vector<std::vector<Token>> separatedByDot2 = theParser2.tokensParseBy(TokenType::operators, '.');
			/*for (int i = 0; i < separatedByDot1.size(); ++i) {
				for (int j = 0; j < separatedByDot1[i].size(); ++j) {
					std::clog << separatedByDot1[i][j].data << ' ';
				}
				std::clog << std::endl;
			}
			for (int i = 0; i < separatedByDot2.size(); ++i) {
				for (int j = 0; j < separatedByDot2[i].size(); ++j) {
					std::clog << separatedByDot2[i][j].data << ' ';
				}
				std::clog << std::endl;
			}*/
			if (separatedByDot1.size() != 2 || separatedByDot1[0].size() != 1 || separatedByDot1[1].size() != 1) { return StatusResult(syntaxError, syntaxError); }
			if (separatedByDot2.size() != 2 || separatedByDot2[0].size() != 1 || separatedByDot2[1].size() != 1) { return StatusResult(syntaxError, syntaxError); }
			theLHS.tableName = separatedByDot1[0][0].data;
			//std::clog << separatedByDot1[0][0].data << std::endl;
			theLHS.fieldName = separatedByDot1[1][0].data;
			//std::clog << separatedByDot1[1][0].data << std::endl;
			theRHS.tableName = separatedByDot2[0][0].data;
			//std::clog << separatedByDot2[0][0].data << std::endl;
			theRHS.fieldName = separatedByDot2[1][0].data;
			//std::clog << separatedByDot2[1][0].data << std::endl;
 		}

		theJoin.onLeft = theLHS;
		theJoin.onRight = theRHS;

		//theJoin.show();
		joins.push_back(theJoin);
		
		return theResult;
	}

	StatusResult SelectStatement::parse(Tokenizer& aTokenizer) {
		//std::clog << "in select statement parse function" << std::endl;
		aTokenizer.restart();
		//aTokenizer.dump();
		const int kMinLength = 4; // smallest # of tokens in create statement
		StatusResult theResult(syntaxError, syntaxError);
		aTokenizer.restart();
		if (kMinLength <= aTokenizer.remaining()) {
			TokenSequence getTokens(aTokenizer);
			std::vector<Token> tokens = getTokens.getTokens();
			for (int i = 0; i < tokens.size(); ++i) {
				if (tokens[i].keyword == Keywords::from_kw && i + 1 < tokens.size() && tokens[i + 1].type == TokenType::identifier) {
					tableName = tokens[i + 1].data; // update select from a table
				}
			}
			if (parseFields(aTokenizer) == theResult) { return theResult; }
			StatusResult joinResult{ joinTypeExpected , joinTypeExpected };
			auto theResult = parseJoin(aTokenizer);
			if (theResult.value == joinResult.value) { 
				return StatusResult(noError, noError); 
			} // this is a join type select
			//std::clog << "here" << std::endl;
			if (parseExpressions(aTokenizer) == theResult) { return theResult; }
		}
		
		return StatusResult(noError, noError);
	}

	StatusResult SelectStatement::run(std::ostream& aStream) const {
		if (joins.size() == 0) {
			return processor.select(tableName, fields, expressions);
		}
		else {
			return processor.join(tableName, fields, joins[0]); // pass in join
		}
	}

	UpdateStatement::UpdateStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::update_kw) {
		tableName = "";
	}

	StatusResult UpdateStatement::parseKVPairs(Tokenizer& aTokenizer) {
		//std::clog << "in update statement parse KVPairs function" << std::endl;
		std::vector<Token> tokens;
		TokenSequence getTokens(aTokenizer);
		tokens = getTokens.getTokens();
		tokens.erase(tokens.begin(), tokens.begin() + 3); // erase first 3 tokens
		int whereIndex = -1;
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i].keyword == Keywords::where_kw) { whereIndex = i; break; }
		}
		if (whereIndex == -1) { // where does not exist
			TokenSequence theParser(tokens);
			KVPairs = theParser.tokensParseBy(TokenType::punctuation, ','); // vector of vector of tokens
		}
		else {
			tokens.erase(tokens.begin() + whereIndex, tokens.end()); // erase where parts
			//for (int i = 0; i < tokens.size(); ++i) { std::clog << tokens[i].data << std::endl; }
			TokenSequence theParser(tokens);
			KVPairs = theParser.tokensParseBy(TokenType::punctuation, ',');
			for (int i = 0; i < KVPairs.size(); ++i) {
				if (KVPairs[i].size() != 3) { return StatusResult(syntaxError, syntaxError); } // must be 3 ... = ...
				else { KVPairs[i].erase(KVPairs[i].begin()+1); } // erase '='
				/*for (int j = 0; j < KVPairs[i].size(); ++j) {
					std::clog << KVPairs[i][j].data << ' ';
				}
				std::clog << std::endl;*/
			}
		}
		return StatusResult(noError, noError);
	}

	StatusResult UpdateStatement::parseExpressions(Tokenizer& aTokenizer) { // parsing all things after where including where
		//std::clog << "in update statement parse expressions function" << std::endl;
		std::vector<Token> tokens;
		TokenSequence getTokens(aTokenizer);
		tokens = getTokens.getTokens();
		int whereIndex = -1;
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i].keyword == Keywords::where_kw) { whereIndex = i; break; }
		}
		if (whereIndex != -1) { // where does exist
			tokens.erase(tokens.begin(), tokens.begin() + whereIndex); // leave tokens after where keyword including where key word
			/*for (int i = 0; i < tokens.size(); ++i) {
				std::clog << tokens[i].data << std::endl;
			}*/
		}
		expressions.push_back(tokens);
		/*for (int i = 0; i < expressions.size(); ++i) {
			for (int j = 0; j < expressions[i].size(); ++j) {
				std::clog << expressions[i][j].data << ' ';
			}
			std::clog << std::endl;
		}*/
		return StatusResult(noError, noError);
	}

	StatusResult UpdateStatement::parse(Tokenizer& aTokenizer) {
		//std::clog << "in update statement parse function" << std::endl;
		//aTokenizer.dump();
		std::vector<Token> tokens;
		TokenSequence getTokens(aTokenizer);
		tokens = getTokens.getTokens();
		if (tokens.size() >= 2) { tableName = tokens[1].data; } // put in table name
		//std::clog << "update statement table name set to: " << tableName << std::endl;
		bool hasSet = false;
		bool hasWhere = false;
		for (int i = 0; i < tokens.size(); ++i) {
			if (tokens[i].keyword == Keywords::set_kw) { hasSet = true; }
			if (tokens[i].keyword == Keywords::where_kw) { hasWhere = true; }
		}
		if (!hasSet) { return StatusResult(invalidCommand, invalidCommand); } // no set means invalid command
		if (!parseKVPairs(aTokenizer)) { return StatusResult(syntaxError, syntaxError); }
		if(hasWhere){ parseExpressions(aTokenizer); } // parse expressions only if where exists
		return StatusResult(noError, noError);
	}

	StatusResult UpdateStatement::run(std::ostream& aStream) const {
		//std::clog << "in update statement run function" << std::endl;
		//std::clog << "update statement run function table name is: " << tableName << std::endl;
		return processor.update(tableName, KVPairs, expressions);
		return StatusResult(noError, noError);
	}

	ShowIndexesStatement::ShowIndexesStatement(SQLProcessor& aProc) : SQLStatement(aProc, Keywords::update_kw) {
	}

	StatusResult ShowIndexesStatement::parse(Tokenizer& aTokenizer) {
		//std::clog << "in show indexes statement parse function" << std::endl;
		while (aTokenizer.more()) {
			aTokenizer.next();
		}
		return StatusResult(noError, noError);
	}

	StatusResult ShowIndexesStatement::run(std::ostream& aStream) const {
		//std::clog << "in show indexes statement run function" << std::endl;
		return processor.showIndexes();
		//return StatusResult(noError, noError);
	}

	AlterStatement::AlterStatement(SQLProcessor& aProc) :SQLStatement(aProc, Keywords::alter_kw) {
		toModify.setNullable(true);

	}

	void getDataType(std::vector<Token>& aTokens, uint32_t aPos, Attribute& anAttr) {
		if (aPos < aTokens.size()) {
			DataType toInsert = DataType::no_type;
			switch (aTokens[aPos].keyword) {
			case Keywords::boolean_kw:
				toInsert = DataType::bool_type;
				break;
			case Keywords::integer_kw:
				toInsert = DataType::int_type;
				break;
			case Keywords::float_kw:
				toInsert = DataType::float_type;
				break;
			case Keywords::varchar_kw:
				toInsert = DataType::varchar_type;
				if (aPos < aTokens.size() - 3 && aTokens[aPos + 1].type == TokenType::punctuation && aTokens[aPos + 3].type == TokenType::punctuation && aTokens[aPos + 2].type == TokenType::number) {
					anAttr.setSize(std::stoi(aTokens[aPos + 2].data));
				}
				else toInsert = DataType::no_type;
				break;
			}
			anAttr.setType(toInsert);
		}
	}

	StatusResult AlterStatement::parse(Tokenizer& aTokenizer) {
		std::vector<Token> tokens;
		TokenSequence getTokens(aTokenizer);
		tokens = getTokens.getTokens();

		if (tokens.size() > 6) {
			if (tokens[0].keyword == Keywords::alter_kw) {
				if (tokens[1].keyword == Keywords::table_kw) {
					if (tokens[2].type == TokenType::identifier) {
						tableName = tokens[2].data;
						if (tokens[3].keyword == Keywords::add_kw) {
							if (tokens[4].type == TokenType::identifier) {
								toModify.setName(tokens[4].data);
								if (tokens[5].type == TokenType::keyword) {
									getDataType(tokens, 5, toModify);
									if (toModify.getType() != DataType::no_type) {
										return StatusResult();
									}
								}
							}
						}
					}
				}
			}
		}


		return StatusResult(syntaxError, syntaxError);
	}
	StatusResult AlterStatement::run(std::ostream& aStream) const {
		return processor.alterTable(tableName.tableName, toModify);

		return StatusResult();
	}
}

//
//  Filters.hpp
//  Assignement6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//


#include <stdio.h>
#include "Filters.hpp"
#include "Row.hpp"
#include "Schema.hpp"
#include "Helpers.hpp"
#include "Components.hpp"

namespace ECE141 {
	bool Expression::operator()(NullableKVList& aList) {
		return true;
	}

	Filters::Filters(TokenTable& aTable) {//grab the first 3 tokens of the where clause and put them into the expression list
		expressionTokens = aTable;
		flag = StatusResult();
		// theWhereClause is an iterator to a vector
		auto theWhereClause = std::find_if(aTable.begin(), aTable.end(), [](Tokens& aTokens) {return aTokens.size() && aTokens[0].keyword == Keywords::where_kw; });
		if (theWhereClause != aTable.end()) {

			const static int kMinTokens = 3; //something operator something
			if (theWhereClause->size() >= kMinTokens) {
				int i = 1;
				while (i < theWhereClause->size() && theWhereClause->size() - i >= kMinTokens) {
					auto theLeftToken = (*theWhereClause)[i++];
					auto theOpToken = (*theWhereClause)[i++];
					auto theRightToken = (*theWhereClause)[i++];

					Expression* theExpression = new Expression(Operand(theLeftToken), theOpToken.op, Operand(theRightToken)); // construct an expression on heap, operand auto convert token to type data
					add(theExpression); // push in one expression
					//relation = LogicRelation::Nothing;

					if (i < theWhereClause->size() - 1 && (*theWhereClause)[i].type == TokenType::keyword) {//check the logic operator behind
						auto theLogic = (*theWhereClause)[i++].keyword;
						if (theLogic == Keywords::and_kw) relation = LogicRelation::And;
						else if (theLogic == Keywords::or_kw) relation = LogicRelation::Or;
						else flag = StatusResult(syntaxError, syntaxError); // flag not used later
					}
					else flag = StatusResult(syntaxError, syntaxError); // flag not used later
				}
			}
			else flag = StatusResult(syntaxError, syntaxError); // flag not used later
		}
	}

	Filters::Filters(Join& aJoin, RowCollection& aCollection) {
		flag = StatusResult();
		join = new Join(aJoin);
		joinCandidates = aCollection;
	}


	Filters::Filters(const Filters& aFilters) {
		for (int i = 0; i < aFilters.expressions.size(); ++i) {
			expressions.push_back(aFilters.expressions[i]);
		}
		flag = aFilters.flag;
		relation = aFilters.relation;
	}

	Filters::~Filters() {
		for (int i = 0; i < expressions.size(); ++i) {
			if (expressions[i]) delete expressions[i]; // release memory
		}
		if (join) delete join;
	}

	bool Filters::matches(NullableKVList& aList) const {
		return true;
	}

	Filters& Filters::add(Expression* anExpression) {
		expressions.push_back(anExpression);
		return *this;
	}

	bool Filters::rowMatches(Row* aRow) {
		if (0 == expressions.size()) return true;
		bool theLastResult = false; 
		for (auto& theExpression : expressions) { // iterate through all expression
			Entry theLeftValue = operandToEntryIfNameOfTheAttributeReturnRowsCorrespondingValueElseReturnStringOrBoolOrNum(theExpression->lhs, aRow);
			Entry theRightValue = operandToEntryIfNameOfTheAttributeReturnRowsCorrespondingValueElseReturnStringOrBoolOrNum(theExpression->rhs, aRow);

			if (theLeftValue.index() != std::variant_npos 
				&& theRightValue.index() != std::variant_npos 
				&& theLeftValue.index() == theRightValue.index()) {
				theLastResult = compareEntries(theLeftValue, theRightValue, theExpression->op);
			}
			else theLastResult = false;

			if (!shouldContinueCompare(theLastResult)) { // short circuiting
				break;
			}
		}
		return theLastResult;
	}

	bool Filters::elementInValueMapMatches(std::pair<Entry, uint32_t> anElement) {
		bool theLastResult = false;
		Entry theLeftValue = anElement.first;
		Entry theRightValue = expressions[0]->rhs.value;
		theLastResult = compareEntries(theLeftValue, theRightValue, expressions[0]->op);
		return theLastResult;
	}

	Entry Filters::operandToEntryIfNameOfTheAttributeReturnRowsCorrespondingValueElseReturnStringOrBoolOrNum(Operand& anOp, Row* aRow) {
		if (anOp.type == TokenType::identifier) {
			std::string theNameCopy = Helpers::toLower(anOp.name);
			if (theNameCopy == "true" || theNameCopy == "false"){ // string might be bool
				return Entry(theNameCopy == "true" ? true : false);
			}
			else { // string attribute name
				auto* theKVList = &(aRow->getKeyValueList()); // get kv pairs
				auto toReturn = std::find_if(theKVList->begin(), theKVList->end(), [&anOp](NullableKVPair& aPair) {return aPair.first == anOp.name; }); // find the iterator has the kvpair matching the operand name
				if (toReturn != theKVList->end()) { // found in row
					return toReturn->second.second; // return the value
				}
				else return Entry(); // std::variant_npos
			}
		}
		else if (anOp.type == TokenType::number || anOp.type == TokenType::string) {
			return anOp.value;
		}
		else return Entry(); // std::variant_npos
	}

	bool Filters::compareEntries(Entry& aLhs, Entry& aRhs, Operators& anOp) {
		if (aRhs.index() == 1) {
			double temp = std::get<double>(aLhs);
			aLhs = temp;
		}
		switch (anOp) {
		case Operators::equal_op:
			return aLhs == aRhs;
		case Operators::lt_op:
			return aLhs < aRhs;
		case Operators::lte_op:
			return aLhs <= aRhs;
		case Operators::gt_op:
			return aLhs > aRhs;
		case Operators::gte_op:
			return aLhs >= aRhs;
		default:
			return false;
		}
	}

	bool Filters::shouldContinueCompare(const bool& aLast) {
		if (relation == LogicRelation::And) return aLast;
		else if (relation == LogicRelation::Or) return !aLast;
		return true;
	}

	bool getCompareResult(const bool& aLast) {
		return aLast;
	}

	int Filters::rowMatchesJoin(Row* aRow) {
		theFieldNameToFind = (aRow->getTableName() == join->onLeft.tableName) ? join->onLeft.fieldName : join->onRight.fieldName;
		theCandidateFieldName = (aRow->getTableName() == join->onLeft.tableName) ? join->onRight.fieldName : join->onLeft.fieldName;

		auto theFields = &(aRow->getKeyValueList());
		auto theField = findFieldByName(theFieldNameToFind, *aRow);
		if (theField != theFields->end()) {

			for (int i = 0; i < joinCandidates.getRowsCount(); i++) {
				Row* theRow = joinCandidates.getRowCollections()[i];
				auto theCandidateField = findFieldByName(theCandidateFieldName, *theRow);

				if (theCandidateField != theRow->getKeyValueList().end()) {
					if (theField->second.second == theCandidateField->second.second) return i;
				}
			}
		}
		return -1;
	}

	NullableKVList::iterator Filters::findFieldByName(std::string& aFieldName, Row& aRow) {
		return std::find_if(aRow.getKeyValueList().begin(), aRow.getKeyValueList().end(), [&aFieldName](NullableKVPair& aField) {return aField.first == aFieldName && !aField.second.first; });
	}
}




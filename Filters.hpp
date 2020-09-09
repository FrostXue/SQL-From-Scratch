//
//  Filters.hpp
//  RGAssignment6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Filters_h
#define Filters_h

#include <stdio.h>
#include <vector>
#include <string>
#include "Errors.hpp"
#include "Value.hpp"
#include "Tokenizer.hpp"
#include "Components.hpp"
#include "Index.hpp"
#include "RowCollection.hpp"

namespace ECE141 {
  
  class Row;
  class Schema;
  enum class LogicRelation{And,Or,Nothing};
  
  struct Operand {
    Operand() {}
    Operand(std::string &aName, TokenType aType, Entry &aValue, uint32_t anId=0)
      : name(aName), type(aType), entityId(anId), value(aValue) {}

    Operand(Token& aToken) 
    :name(""),type(aToken.type),value(false),entityId(0){
        if (aToken.type == TokenType::number) {
            size_t lengthOfNumber = 0;
            value = (int)std::stoi(aToken.data, &lengthOfNumber);
            //Check if the number is int or double
            if (lengthOfNumber != aToken.data.length()) { // not an int
                value = (double)std::stod(aToken.data);
            }
        }
        else if (aToken.type == TokenType::string) {
            value = aToken.data;
        }
        else if (aToken.type == TokenType::identifier) {
            name = aToken.data;
        }
    }
    
    TokenType   type; //so we know if it's a field, a const (number, string)...
    std::string name; //for named attr. in schema
    Entry   value;
    uint32_t    entityId;
  };
  
  //---------------------------------------------------

  struct Expression {
    Operand     lhs;
    Operand     rhs;
    Operators   op;
    
    Expression(Operand aLHSOperand, Operators anOp, Operand aRHSOperand)
      : lhs(aLHSOperand), op(anOp), rhs(aRHSOperand) {}
    
    bool operator()(NullableKVList&aList);
  };
  
  //---------------------------------------------------

  using Expressions = std::vector<Expression*>;
  using Tokens = std::vector<Token>;
  using TokenTable = std::vector<Tokens>;

  //---------------------------------------------------

  class Filters {
  public:
    
    Filters(){}
    Filters(TokenTable& aTable);
    Filters(Join& aJoin, RowCollection& aCollection);
    Filters(const Filters &aFilters);
    ~Filters();
    
    size_t        getCount() const {return expressions.size();}
    bool          matches(NullableKVList&aList) const;
    Filters&      add(Expression *anExpression);
    std::vector<std::vector<Token>> getExpressionTokens() { return expressionTokens; }
    StatusResult getFlag() { return flag; }
    LogicRelation getRelation() { return relation; }
    bool rowMatches(Row* aRow);
    bool elementInValueMapMatches(std::pair<Entry, uint32_t> anElement);

    int rowMatchesJoin(Row* aRow);

    friend class Tokenizer;
    
  protected:
      std::vector<std::vector<Token>> expressionTokens;
    Expressions  expressions;
    Entry operandToEntryIfNameOfTheAttributeReturnRowsCorrespondingValueElseReturnStringOrBoolOrNum(Operand& anOp, Row* aRow);
    bool compareEntries(Entry& aLhs, Entry& aRhs, Operators& anOp);
    LogicRelation relation;
    StatusResult flag;
    bool shouldContinueCompare(const bool& aLast);//Should the logical comparison continue or short-circuited
    bool getCompareResult(const bool& aLast);
    NullableKVList::iterator findFieldByName(std::string& aFieldName, Row& aRow);

    Join* join = nullptr;
    RowCollection joinCandidates;
    std::string theFieldNameToFind;
    std::string theCandidateFieldName;
  };
   
}

#endif /* Filters_h */


#include "TokenSequence.hpp"
#include <iostream>


namespace ECE141{
    TokenSequence::TokenSequence(Tokenizer& aTokenizer) {
        aTokenizer.restart();
        while (aTokenizer.more()) { // consume all tokens
            listOfTokens.push_back(aTokenizer.current()); // push in all the tokens after help
            aTokenizer.next(); // consumed that token, good to go
        }
    }

    TokenSequence::TokenSequence(std::vector<Token> aListOfTokens) {
        for (int i = 0; i < aListOfTokens.size(); ++i) {
            listOfTokens.push_back(aListOfTokens[i]);
        }
    }

    TokenSequence::TokenSequence(const TokenSequence& aCopy) {
        for (int i = 0; i < aCopy.listOfTokens.size(); ++i) {
            listOfTokens.push_back(aCopy.listOfTokens[i]);
        }
    }

    TokenSequence& TokenSequence::operator=(const TokenSequence& aCopy) {
        for (int i = 0; i < aCopy.listOfTokens.size(); ++i) {
            listOfTokens.push_back(aCopy.listOfTokens[i]);
        }
        return *this;
    }

    TokenSequence::~TokenSequence() {

    }

    std::vector<std::vector<Token>> TokenSequence::tokensParseBy(TokenType aType, char aChar) {
        std::vector<std::vector<Token>> listOfPotentialAttributes;
        std::vector<Token> potentialAttributeProperties;
        for (int i = 0; i < listOfTokens.size(); ++i) {
            if (listOfTokens[i].type == aType && listOfTokens[i].data[0] == aChar) { listOfPotentialAttributes.push_back(potentialAttributeProperties);  potentialAttributeProperties.clear(); }
            else { potentialAttributeProperties.push_back(listOfTokens[i]); }
        }
        listOfPotentialAttributes.push_back(potentialAttributeProperties); // push-in the last one vector
        return listOfPotentialAttributes;
    }

    std::vector<std::vector<Token>> TokenSequence::parseTokensBetweenTwoPara() { // have to have correct format like: () () ()
        //std::clog << "TokenSequence Parse between two para function" << std::endl;
        std::vector<std::vector<Token>> listOfListOfTokens;
        std::vector<Token> ListOfTokens;
        int leftParaPos = -1;
        int rightParaPos = -1;
    again:
        leftParaPos = -1;
        rightParaPos = -1;
        for (int i = 0; i < listOfTokens.size(); ++i) {
            if (listOfTokens[i].data[0] == '(' && listOfTokens[i].type == TokenType::punctuation) {
                leftParaPos = i;
                //std::clog << i << std::endl;
            }
            if (listOfTokens[i].data[0] == ')' && listOfTokens[i].type == TokenType::punctuation) {
                rightParaPos = i;
                //std::clog << i << std::endl;
                break;
            }
        }
        if (leftParaPos != -1 && rightParaPos != -1) {
            for (int i = leftParaPos + 1; i < rightParaPos; ++i) {
                ListOfTokens.push_back(listOfTokens[i]);
            }
            //for (int i = 0; i < ListOfTokens.size(); ++i) {
            //    std::clog << ListOfTokens[i].data << std::endl;
            //}
            listOfListOfTokens.push_back(ListOfTokens);
            ListOfTokens.clear();
            for (int i = 0; i < rightParaPos + 1; ++i) {
                listOfTokens.erase(listOfTokens.begin());
            }
            goto again; // goto statement really bad practice but oh well
        }
        return listOfListOfTokens;
    }

    std::vector<Token> TokenSequence::tokensSequenceErase(TokenType aType, char aChar) { // will change underlying sequence
        for (int i = 0; i < listOfTokens.size(); ++i) {
            if (listOfTokens[i].type == aType && listOfTokens[i].data[0] == aChar) {
                listOfTokens.erase(listOfTokens.begin() + i);
                --i;
            }
        }
        return listOfTokens;
    }

    std::vector<Token> TokenSequence::getTokens() {
        return listOfTokens;
    }
}
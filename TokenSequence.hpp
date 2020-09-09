#ifndef TokenSequence_hpp
#define TokenSequence_hpp
#include"Tokenizer.hpp"
#include <vector>

namespace ECE141 {
    class TokenSequence {
    public:
        TokenSequence(Tokenizer& aTokenizer);
        TokenSequence(std::vector<Token> aListOfTokens);
        TokenSequence(const TokenSequence& aCopy);
        TokenSequence& operator=(const TokenSequence& aCopy);
        ~TokenSequence();
        std::vector<std::vector<Token>> tokensParseBy(TokenType aType, char aChar);
        std::vector<Token> tokensSequenceErase(TokenType aType, char aChar);
        std::vector<std::vector<Token>> parseTokensBetweenTwoPara();
        std::vector<Token> getTokens();
    protected:
        std::vector<Token> listOfTokens;
    };
    
}
#endif /* TokenSequence_hpp */
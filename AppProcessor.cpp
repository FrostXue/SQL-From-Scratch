//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "AppProcessor.hpp"
#include "Tokenizer.hpp"
#include <memory>
#include <vector>

namespace ECE141 {
  
  class VersionStatement : public Statement {
  public:
    VersionStatement() :  Statement(Keywords::version_kw) {}
  };

  class HelpStatement : public Statement {
  public:
      HelpStatement() : Statement(Keywords::help_kw) {}
      std::vector<ECE141::Token>& getTokensInHelpStatement() { return tokens; }
  protected:
      std::vector<ECE141::Token>    tokens;
  };

  class ExitStatement : public Statement {
  public:
      ExitStatement() : Statement(Keywords::quit_kw) {}
  };

  //.....................................

  AppCmdProcessor::AppCmdProcessor(CommandProcessor *aNext) : CommandProcessor(aNext) {
  }
  
  AppCmdProcessor::~AppCmdProcessor() {}

  static std::map<ECE141::Keywords, std::string> availableCommandsSoFar = {
  {ECE141::Keywords::help_kw, "     -- help - shows this list of commands"},
  {ECE141::Keywords::version_kw, "     -- version -- shows the current version of this application"},
  {ECE141::Keywords::quit_kw, "     -- quit  -- terminates the execution of this DB application"},
  {ECE141::Keywords::create_kw, "     -- create database <name> -- creates a new database"},
  {ECE141::Keywords::drop_kw, "     -- drop database <name> -- drops the given database"},
  {ECE141::Keywords::use_kw, "     -- use database <name>  -- uses the given database"},
  {ECE141::Keywords::describe_kw, "     -- describe database <name>  -- describes the given database"},
  {ECE141::Keywords::show_kw, "     -- show databases   -- shows the list of databases available"}
  };
  
  StatusResult doHelpCommand(const Statement& aStatement) {
      Statement& unConstStatement = const_cast<Statement&>(aStatement); // remove constness
      HelpStatement* temp = static_cast<HelpStatement*>(&unConstStatement); // convert back to HelpStatement
      /*for (int i = 0; i < temp->getTokensInHelpStatement().size(); ++i) {
          std::clog << temp->getTokensInHelpStatement()[i].data << "\n";
      }*/
      //std::clog << "in do help command function" << std::endl;
      if (temp->getTokensInHelpStatement().size() == 1 && temp->getTokensInHelpStatement()[0].keyword == ECE141::Keywords::help_kw) { // just one help
          std::cout << "help -- the available list of commands shown below:" << std::endl; // showing all list of available commands
          auto it = availableCommandsSoFar.begin();
          while (it != availableCommandsSoFar.end()) { // iterate theough
              std::cout << it->second << std::endl;
              ++it;
          }
          return StatusResult();
      }
      if (temp->getTokensInHelpStatement().size() == 2 && temp->getTokensInHelpStatement()[1].type == TokenType::keyword) {
          auto it = availableCommandsSoFar.begin();
          while (it != availableCommandsSoFar.end()) { // iterate theough
              if (it->first == temp->getTokensInHelpStatement()[1].keyword) {
                  std::cout << it->second << std::endl;
                  break;
              }
              ++it;
          }
          return StatusResult();
      }
      if (temp->getTokensInHelpStatement().size() == 3 && temp->getTokensInHelpStatement()[1].data == "show" && temp->getTokensInHelpStatement()[2].data == "databases") {
          auto it = availableCommandsSoFar.begin();
          while (it != availableCommandsSoFar.end()) { // iterate theough
              if (it->first == temp->getTokensInHelpStatement()[1].keyword) {
                  std::cout << it->second << std::endl;
                  break;
              }
              ++it;
          }
          return StatusResult();
      }
      if ((temp->getTokensInHelpStatement().size() == 3 || temp->getTokensInHelpStatement().size() == 4) && (temp->getTokensInHelpStatement()[1].data == "create" || temp->getTokensInHelpStatement()[1].data == "drop" || temp->getTokensInHelpStatement()[1].data == "use" || temp->getTokensInHelpStatement()[1].data == "describe") && temp->getTokensInHelpStatement()[2].data == "database") {
          auto it = availableCommandsSoFar.begin();
          while (it != availableCommandsSoFar.end()) { // iterate theough
              if (it->first == temp->getTokensInHelpStatement()[1].keyword) {
                  std::cout << it->second << std::endl;
                  break;
              }
              ++it;
          }
          return StatusResult();
      }
      std::cout << "     Nothing found\n";
      std::cout << "     Please try to run 'help' for a list of all accessible topics\n";
      return StatusResult();
  }

  StatusResult doVersionCommand(const Statement& aStatement) {
      std::cout << "version ECE141b-1\n";
      return StatusResult();
  }

  StatusResult doExitCommand(const Statement& aStatement) {
      std::cout << "Bye\n";
      return StatusResult(userTerminated, userTerminated);
  }

  // USE: -----------------------------------------------------
  StatusResult AppCmdProcessor::interpret(const Statement &aStatement) {
    //STUDENT: write code related to given statement
      switch (aStatement.getType()) {
          case Keywords::help_kw:  return doHelpCommand(aStatement);
          case Keywords::version_kw: return doVersionCommand(aStatement);
          case Keywords::quit_kw: return doExitCommand(aStatement);
      }
    return StatusResult();
  }
  
  // USE: factory to create statement based on given tokens...
  Statement* AppCmdProcessor::getStatement(Tokenizer &aTokenizer) {
      //STUDENT: Analyze tokens in tokenizer, see if they match one of the
      //         statements you are supposed to handle. If so, create a
      //         statement object of that type on heap and return it.

      //         If you recognize the tokens, consider using a factory
      //         to construct a custom statement object subclass.

      //std::vector<ECE141::Keywords> theTerms{
      //    Keywords::help_kw, Keywords::version_kw, Keywords::quit_kw
      //};
      //aTokenizer.dump();
      //Token& theToken = aTokenizer.current();
      //auto it = std::find(theTerms.begin(), theTerms.end(), theToken.keyword);
      //if (it != theTerms.end()) {
      //    // I'm ready to make a statement...
      //    aTokenizer.next(); // consumed that token, good to go
      //    return new Statement(theToken.keyword);
      //}

      //aTokenizer.dump();
      Token& theToken = aTokenizer.current();
      switch (theToken.keyword) {
      case Keywords::version_kw: 
          aTokenizer.next(); // consumed that token, good to go
          return new VersionStatement;
      case Keywords::quit_kw: 
          aTokenizer.next(); // consumed that token, good to go
          return new ExitStatement;
      case Keywords::help_kw:
          HelpStatement* temp = new HelpStatement;
          while (aTokenizer.more()) { // consume all tokens followed by help keyword
              temp->getTokensInHelpStatement().push_back(aTokenizer.current()); // push in all the tokens after help
              aTokenizer.next(); // consumed that token, good to go
          }
          return temp;
      }
    
    return nullptr;
  }
  
}

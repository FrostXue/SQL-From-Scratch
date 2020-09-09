//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <filesystem>
#include <sstream>

namespace ECE141 {
  
  class FolderListener {
  public:
    virtual bool operator()(const std::string &aName)=0;
  };
  
  class FolderReader {
  public:
        
                  FolderReader(const char *aPath) : path(aPath) {}
    virtual       ~FolderReader() {}
    
    virtual bool  exists(const std::string &aPath) {
                    //STUDENT: add logic to see if FOLDER at given path exists.
                    //return std::filesystem::exists(path + "\\" + aPath);
					return std::filesystem::exists(aPath);
                  }
    
    virtual void  each(FolderListener &aListener, const std::string &anExtension) const {
                    //STUDENT: iterate db's, pass the name of each to listener
                    for (const auto& entry: std::filesystem::directory_iterator(path)){ //Iterate all entries in the dir specified by 'path'
                      auto theFileName = entry.path();
                      if (!anExtension.size() || (theFileName.has_extension() && theFileName.extension().string() == anExtension)) //Check extension
                        aListener.operator()(entry.path().filename().string()); //Tell listerner every matching file it finds
                    }
                    
                  };
      
    std::string path;
  };
  
}

#endif /* FolderReader_h */

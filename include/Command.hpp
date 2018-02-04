#ifndef COMMAND_HPP
#define COMMAND_HPP

// Throws Command::error, and occasionally std::out_of_range
// both should be caught by Handler
// This class's main job is to work with strings, to later be used by
// handlers
#include <iostream>
#include <string>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> //runtime_error

class Command {
    // The entire str.
    std::string str;
    // what to have toString return, maybe not used.
    std::string toStringstr;
    std::vector<std::vector<std::string> > args;

    // If true, disallow conversion to non Directive types
    void setCommandAndType();

    void processSS(); // process spaces
    void processPP(); // process pipes
    void processPS(); // process pipe spaces
    void processDD(); // process Directive
    void processD2(); // process 2d directive.
    
    // Set of bools to set
    bool hasNewlines = false;
    bool hasBackSlash = false;
    // No arguments, just command
    bool commandOnly = false;
    bool hasSpacesInArgs = false;
    bool hasPipesInArgs = false;
    bool uses2d = false;
    // END BOOLS

    // Should always get set
    // Where the argument part of the string is
    std::size_t argStart = 0;

    std::string type;

    // Maybe will be used.
    unsigned int internalIndex1 = 0; // vector<vector..> index
    unsigned int internalIndex2 = 0; //        vector<string> index
    bool letUserAddArgs = false;
    std::string _cmd;

    // Internal only escape.
    // Will update things such as hasNewlines, etc.
    std::string escape(std::string& s,bool);

    void recalculateType();
    void toUpper(std::string& s);
  public:
    const std::string& cmd = _cmd;
    std::string toString();
    Command(std::string everything,bool);
    Command(std::string command);
    static std::vector<std::string> split(std::string s);
    std::vector<std::string> directiveSplit(std::string directive,
					    std::string data);
    class error : public std::runtime_error {
      public:
	error(std::string what) :
	    runtime_error(what) { }
    };
    std::string get(int x,int y);

    std::string arg(int i);
    std::string arg();
    void setIndex(int x,int y);

    // Everything left over in this row.
    std::string rest(unsigned int i);
    std::string rest();

    std::size_t rows();
    std::size_t numArgsInRow(int row);

    void add(std::string s);
    void addRow();
    bool next();
    bool nextRow();

    // Just handle '\n' and '\'
    static std::string escape(std::string& s);
    static std::string unescape(std::string& s);

    void forceTypeAsD2();
    // For testing
    void printArgs() {
	for(unsigned int k = 0;k < args.size();++k) {
	    for(unsigned int i = 0;i < args[k].size();++i) {
		std::cout<<"["<<k<<"]["<<i<<"]: '"
			 <<unescape(args[k][i])<<"'"<<std::endl;
	    }
	}

    }
};

#endif

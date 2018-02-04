#include "Command.hpp"
#include <cctype> // For toUpper
#include <algorithm> // For transform
// Should parse everything, more or less.
Command::Command(std::string everything,bool)
    : str(everything) {
    using namespace std;
    if(str.size() < 3) {
	throw error("Unrecognized Command, Empty or too small");
    }
    setCommandAndType();
    // cout << "cmd: '"<<_cmd<<"'"<<endl;
    // cout << "type: '"<<type<<"'"<<endl;
    // cout << "rest: '"<<str.substr(argStart)<<"'"<<endl;

    if("SS" == type) {
	processSS();
    } else if ("||" == type) {
	processPP();
    } else if ("|S" == type) {
	processPS();
    } else if ("DD" == type) {
	processDD();
    } else if ("D2" == type) {
	processD2();
    }
}    
void Command::setCommandAndType() {
    auto foundAt = str.find(' ');

    if(foundAt == std::string::npos) {
	_cmd = str;
	commandOnly = true;
	argStart = str.size();
    } else {
	_cmd = str.substr(0,foundAt);
	argStart = foundAt+1;
    }

    if(_cmd.size() < 3) {
	throw error("Unrecognized Command, Command too small");
    }
    type = _cmd.substr(_cmd.size()-2);
    if("SS" != type &&
       "||" != type &&
       "|S" != type &&
       "DD" != type &&
       "D2" != type) {
	std::cout << "Auto deriving type, SS" << std::endl;
	type = "SS";
    } else  {
	_cmd = _cmd.substr(0,_cmd.size()-2);
    }
    toUpper(_cmd);
    // Now the correct command and type should be set.
    // And argStart
}
// process spaces
void Command::processSS() {
    if(commandOnly) {
	return;
    }
    // make our container for arguments
    args.push_back(split(str.substr(argStart)));
}
std::vector<std::string> Command::split(std::string s) {
    using namespace std;

    vector<string> ret;
    // Maybe make into an argument
    auto x = 0;
    auto y = s.find(' ',x);
    while(y != std::string::npos) {
	ret.push_back(s.substr(x,y-x));
	x=y+1;
	y = s.find(' ',x);
    }
    string last = s.substr(x);
    if(last.size() == 0) {
	return ret;
    }
    ret.push_back(last);
    return ret;
}
    
// process pipes
void Command::processPP() {
    using namespace std;
    if(commandOnly) {
	throw error("Improper command format, must have arguments");
    }
    
    args.push_back(std::vector<std::string>());
    auto x = argStart;
    auto y = str.find('|',x);
    while(y != std::string::npos) {
	args[0].push_back(str.substr(x,y-x));
	x=y+1;
	y = str.find('|',x);
    }
    string last = str.substr(x);
    if(last.size() == 0) {
	return;
    }
    args[0].push_back(last);
}
// process pipe spaces
void Command::processPS() {
    using namespace std;
    if(commandOnly) {
	throw error("Improper command format, must have arguments");
    }
    auto x = argStart;
    auto y = str.find('|',x);
    while(y != std::string::npos) {
	args.push_back(split(str.substr(x,y-x)));
	x=y+1;
	y = str.find('|',x);
    }
    string last = str.substr(x);
    if(last.size() == 0) {
	return;
    }
    args.push_back(split(last));
}
// Both DD and D2 should unescape newlines when processing.
std::vector<std::string> Command::directiveSplit(std::string directive,
						 std::string data) {
    using namespace std;
    if(directive.size() < 5) {
	throw error("Incorrect Directive, too small");
    }
    if(directive[0] != '{' ||
       directive[directive.size()-1] != '}') {
	throw error("Incorrect Directive, bad format");
    }
    
    std::size_t x = 1; // one past the directive.
    std::size_t y;
    vector<string> ret;
    unsigned int start; // for each part
    unsigned int end;

    /**
     * Needed explnation to the below. Hopefully the above makes sense.
     * Basically, we try to read the start and end ints of the directive, one
     * at a time, and add them to the vector.
     * we end if we either meet a '}' (which we shoulden't end this way.)
     * or if we find our '}' as the marker of an end int.
     * (it should end this way.
     **/
    bool exitThisLoop = false;
    while(directive[x] != '}' &&
	  !exitThisLoop) {
	y = directive.find(':',x);
	if(y == std::string::npos) {
	    throw error("Incorrect Directive, bad numbers");
	}
	try {
	    start = std::stoul(directive.substr(x,y-x));
	    x = y+1;
	    y = directive.find(',',x);
	    if(y == std::string::npos) {
		y = directive.find('}',x);
		exitThisLoop = true;
	    }
	    if(y == std::string::npos) {
		// Should be caught, then thrown again.
		// which is weird but works
		throw error("Incorrect Directive, bad numbers");
	    }
	    end = std::stoul(directive.substr(x,y-x));
	    string toPut = data.substr(start, end - start);
	    // Unescape done here
	    toPut = unescape(toPut);
	    ret.push_back(toPut);
	    x = y+1;
	} catch (std::exception e) {
	    throw error("Incorrect Directive, bad numbers");
	}
    }
    return ret;
}
// process Directive
void Command::processDD() {
    using namespace std;
    if(commandOnly) {
	throw error("Improper command format, must have arguments");
    }
    auto x = argStart;
    auto y = str.find(' ',x);
    if( y == std::string::npos) {
	throw error("Incorrect Directive");
    }
    string directive = str.substr(x,y-x);
    auto dataStart = y+1;
    args.push_back(directiveSplit(directive,str.substr(dataStart)));
}
// process 2d directive.
void Command::processD2() {
    using namespace std;
    if(commandOnly) {
	throw error("Improper command format, must have arguments");
    }
    auto x = argStart;
    auto y = str.find(' ',x);
    if( y == std::string::npos) {
	throw error("Incorrect Directive");
    }
    string full_directive = str.substr(x,y-x);
    auto dataStart = y+1;

    string data = str.substr(dataStart);
    y = str.find('}',x);
    while(y != string::npos) {
	// We want the }, so we do (y-x)+1
	args.push_back(directiveSplit(str.substr(x,(y-x)+1),
				      data));
	x  = y+1;
	y = str.find('}',x);
    }
}
// Really sorry for this guys, they really should go in their own function
// Also technically SS and || should return an error if they go past args[0]
// Also, directive toStrings should escape newlines before sending things out.
std::string Command::toString() {
    using namespace std;
    if(commandOnly) {
	return _cmd; // Command without the type, ignore the type.
    }
    string ret = "";
    ret += _cmd+type+" ";
    if("SS" == type) {
	for(auto svec : args) {
	    for(unsigned int i = 0;i < svec.size();++i) {
		// If not on the last element
		if(i != svec.size()-1) {
		    ret += svec[i]+" ";
		} else { // If we are don't add a space
		    ret += svec[i];
		}
	    }
	}
    } else if ("||" == type) {
	for(auto svec : args) {
	    for(unsigned int i = 0;i < svec.size();++i) {
		// If not on the last element
		if(i != svec.size()-1) {
		    ret += svec[i]+"|";
		} else { // If we are don't add a pipe
		    ret += svec[i];
		}
	    }
	}
    } else if ("|S" == type) {
	for(unsigned int k = 0;k < args.size();++k) {
	    for(unsigned int i = 0;i < args[k].size();++i) {
		if(i != args[k].size()-1) {
		    ret += args[k][i]+" ";
		} else {
		    ret += args[k][i];
		}
	    }
	    if(k != args.size()-1) {
		ret += "|";
	    }
	}
    } else if ("D2" == type || "DD" == type) {
	string full_directive = "";
	string rest = "";
	unsigned int pos = 0;
	for(unsigned int k = 0;k < args.size();++k) {
	    string directive = "{";
	    for(unsigned int i = 0;i < args[k].size();++i) {
		// Escape done here
		string to_add = escape(args[k][i]);
		rest += to_add;
		directive +=
		    std::to_string(pos)+
		    ":"+
		    std::to_string(pos+to_add.size());
		if(i != args[k].size()-1) {
		    directive += ",";
		}
		pos += to_add.size();
	    }
	    directive += "}";
	    full_directive += directive;
	}
	full_directive += " ";
	ret += full_directive+rest;
    }
    return ret;
}
std::string Command::escape(std::string& s) {
    std::string ret = "";
    for(auto c : s) {
	if(c == '\\') {
	    ret += "\\\\";
	} else if (c == '\n') {
	    ret += "\\n";
	} else {
	    ret += c;
	}
    }
    return ret;
}
std::string Command::escape(std::string& s,bool) {
    std::string ret = "";
    for(auto c : s) {
	if(c == '\\') {
	    ret += "\\\\";
	    hasBackSlash = true;
	} else if (c == '\n') {
	    hasNewlines = true;
	    ret += "\\n";
	} else if (c == ' ') {
	    hasSpacesInArgs = true;
	    ret += c;
	} else if (c == '|') {
	    hasPipesInArgs = true;
	    ret += c;
	} else {
	    ret += c;
	}
    }
    return ret;
}
std::string Command::unescape(std::string& s) {
    std::string ret = "";
    bool backslash = false;
    for(auto c : s) {
	if(backslash) {
	    if(c == '\\') {
		ret += '\\';
	    } else if (c == 'n') {
		ret += '\n';
	    } else {
		ret += c;
	    }
	    backslash = false;
	} else {
	    if(c == '\\') {
		backslash = true;
	    } else {
		ret += c;
	    }
	}
    }
    return ret;
}
void Command::recalculateType() {
    // D2 fits anything
    if("D2" == type) {
	return;
    }
    if(uses2d) {
	if(hasNewlines || hasBackSlash ||
	   hasPipesInArgs || hasSpacesInArgs) {
	    type = "D2";
	} else {
	    // If we have no newlines, backslashes or pipes, we can get
	    // away with this.
	    // We also have to have no spaces in args, strangly.
	    type = "|S";
	}
    } else if(hasNewlines || hasBackSlash ||
	      (hasPipesInArgs && hasSpacesInArgs)) {
	type = "DD";
    } else if(hasSpacesInArgs && !hasPipesInArgs) {
	type = "||";
    } else if(hasPipesInArgs && !hasSpacesInArgs) {
	type = "SS";
    } else {
	type = "SS"; // no pipes no spaces, no nothing.
    }
}
void Command::forceTypeAsD2() {
    type = "D2";
}

// // Everything left over in this row.
// std::string rest(int i);
// std::string rest();
// bool next();
// bool nextRow();
    
// int internalIndex1 = 0; // vector<vector..> index
// int internalIndex2 = 0; //        vector<string> index
// bool letUserAddArgs = false;

// Don't give a type, let the object figure it out for you!
Command::Command(std::string command) {
    _cmd = command;
    toUpper(_cmd);
    letUserAddArgs = true;
    commandOnly = true;
}
// True if there is a next element
bool Command::next() {
    if((args.at(internalIndex1).size()-1) <= internalIndex2)
	return false;
    ++internalIndex2;
    return true;
}
bool Command::nextRow() {
    if((args.size()-1) <= internalIndex1)
	return false;
    ++internalIndex1;
    return true;
}
std::size_t Command::rows() {
    return args.size();
}
std::size_t Command::numArgsInRow(int row) {
    return args.at(row).size();
}
std::string Command::arg() {
    return get(internalIndex1,internalIndex2);
}
std::string Command::arg(int i) {
    return get(internalIndex1,i);
}
std::string Command::get(int x,int y) {
    return unescape(args.at(x).at(y));
}
std::string Command::rest(unsigned int i) {
    std::string ret = "";
    if(args.size() == 0) { // No commands at all
	return "";
    }
    for(;i<args.at(internalIndex1).size();++i) {
	// Maybe make this configureable in the future
	// What the splitter is
	if(i >= args.at(internalIndex1).size() - 1) {
	    ret += get(internalIndex1,i);
	} else {
	    ret += get(internalIndex1,i)+" ";
	}

    }
    return ret;
}    
std::string Command::rest() {
    return rest(internalIndex2);
}

// It's dangerous to not bounds check but w/e
void Command::setIndex(int x,int y) {
    internalIndex1 = x;
    internalIndex2 = y;
}
void Command::add(std::string s) {
    if(!letUserAddArgs) {
	return;
    }
    commandOnly = false;
    // May not be the right way
    if(args.size() == 0) {
	args.push_back(std::vector<std::string>());
    }
    args.at(internalIndex1).push_back(escape(s,true));
    recalculateType();
    ++internalIndex2;
}
void Command::addRow() {
    if(!letUserAddArgs) {
	return;
    }
    ++internalIndex1;
    args.push_back(std::vector<std::string>());
    uses2d = true;
    recalculateType();
}

void Command::toUpper(std::string& s) {
    std::transform(s.begin(),s.end(),s.begin(),
		   [](unsigned char c){ return std::toupper(c);});
}

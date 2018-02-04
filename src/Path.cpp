/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided to
    University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course without
    prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included 
    that were extracted from original Warcraft II by Blizzard Entertainment 
    were found freely available via internet sources and have been labeld as 
    abandonware. They have been included in this distribution for educational 
    purposes only and this copyright notice does not attempt to claim any 
    ownership of this material.
*/
#include "Path.h"
#include <unistd.h>

const char CPath::DDelimiter = '/';
const std::string CPath::DDelimiterString = "/";


/**
* Set/Initialize variables DIsValid and DIsRelative when no pathname is provided
*
* @author Wonhee Park
*
* @version 9.0
*
* @date 12/3/17
*
* Contact: wonpark@ucdavis.edu 
*
*/

CPath::CPath(){
    DIsRelative = true;
    DIsValid = false;
}

/**
* Set/Initialize variables DIsValid and DIsRelative and DDecomposedPath when pathname is provided
*
* @param[in] path reference from CPath
*
*/

CPath::CPath(const CPath &path){
    DDecomposedPath = path.DDecomposedPath;
    DIsRelative = path.DIsRelative;
    DIsValid = path.DIsValid;
}

/**
* Set/Initialize variables DIsValid and DIsRelative when pathname is provided
*
* @param[in] path Pathname
*
*/

CPath::CPath(const std::string &path){
    DIsValid = DecomposePath(DDecomposedPath, path);
    DIsRelative = true;
    if(path.length()){
        if(path[0] == DDelimiter){
            DIsRelative = false;   
        }
    }
}

/**
* Defining = operator, assignment
*
* @param[in] path from CPath
*
* @return instance of Cpath
*/

CPath &CPath::operator=(const CPath &path){
    if(&path != this){
        DDecomposedPath = path.DDecomposedPath;
        DIsRelative = path.DIsRelative;
        DIsValid = path.DIsValid;
    }
    return *this;
}

/**
* Defining Containing call, returns contents
*
* @return ReturnPath 
*/

CPath CPath::Containing() const{
    CPath ReturnPath(*this);
    
    if(ReturnPath.DDecomposedPath.size()){
        ReturnPath.DDecomposedPath.pop_back();
    }
    else if(ReturnPath.DIsRelative){
        ReturnPath.DDecomposedPath.push_back("..");
    }
    else{
        ReturnPath.DIsValid = false;   
    }
    
    return ReturnPath;
}

/**
* Defining ToString call, converts input to a string
*
* @return converted string stored in variable TempString
*/

std::string CPath::ToString() const{
    std::string TempString;
    size_t StartIndex = 0;
    
    if(!DIsValid){
        return "";    
    }
    if(DIsRelative){
        if(DDecomposedPath.size()){
            if(DDecomposedPath[0] != ".."){
                TempString = ".";   
            }
            else{
                TempString = "..";
                StartIndex = 1;
            }
        }
    }
    while(StartIndex < DDecomposedPath.size()){
        TempString += DDelimiterString + DDecomposedPath[StartIndex];
        StartIndex++;
    }
    return TempString;
}

CPath::operator std::string() const{
    return this->ToString();
}

CPath CPath::Simplify(const CPath &destpath) const{
    return SimplifyPath(*this, destpath);
}

CPath CPath::Relative(const CPath &destpath) const{
    return RelativePath(*this, destpath);
}

/**
* Check that path is decomposed
*
* @param[in] decomp 
* @param[in] path  Pathname
*
* @return True if path decomposed
* @return False if path doesn't exist (length = 0)
*/

bool CPath::DecomposePath(std::vector< std::string > &decomp, const std::string &path){
    size_t Anchor, DelimiterLocation;
    bool PathIsRelative = true;
    size_t Index = 0;
    
    if(0 == path.length()){
        return false;
    }
    if(path[0] == DDelimiter){
        PathIsRelative = false;   
    }
    decomp.clear();
    Anchor = 0;
    while((Anchor + 1 < path.length()) && (-1 != (DelimiterLocation = path.find(DDelimiterString, Anchor)))){
        decomp.push_back(path.substr(Anchor, DelimiterLocation - Anchor));
        Anchor = DelimiterLocation + 1;
    }
    if(Anchor < path.length()){
        std::string TempString = path.substr(Anchor, path.length() - Anchor);
        if(TempString != DDelimiterString){
            decomp.push_back(TempString);
        }
    }
    if(decomp.size()){
        if(!decomp[0].length()){
            decomp.erase(decomp.begin());
        }
    }
    while(Index  < decomp.size()){
        if(decomp[Index] == "."){
            decomp.erase(decomp.begin() + Index);
        }
        else if(decomp[Index] == ".."){
            if(Index){
                Index--;
                decomp.erase(decomp.begin() + Index);
                decomp.erase(decomp.begin() + Index);
            }
            else if(!PathIsRelative){
                return false;
            }
            else{
                Index++;
            }
        }
        else{
            Index++;
        }
    }    

    return true;
}

/**
* Gets the simplified path that goes from source to destination
*
* @param[in] srcpath from CPath
* @param[in] destpath from CPath
*
* @return enum CPath type of ReturnPath
*/

CPath CPath::SimplifyPath(const CPath &srcpath, const CPath &destpath){
    CPath ReturnPath;
    std::vector< std::string > BasePath;
    std::vector< std::string > RelPath;
    size_t Index = 0;

    ReturnPath.DIsValid = true;
    if(destpath.IsAbsolute()){
       ReturnPath.DIsRelative = false;
    }
    else{
        ReturnPath.DIsRelative = srcpath.DIsRelative;
        ReturnPath.DDecomposedPath = srcpath.DDecomposedPath;
    }
    RelPath = destpath.DDecomposedPath;
    while(Index  < RelPath.size()){
        if(RelPath[Index] == "."){
            RelPath.erase(RelPath.begin());
        }
        else if(RelPath[Index] == ".."){
            if(Index){
                Index--;
                RelPath.erase(RelPath.begin() + Index);
                RelPath.erase(RelPath.begin() + Index);
            }
            else if(ReturnPath.DDecomposedPath.size()){
                ReturnPath.DDecomposedPath.erase(ReturnPath.DDecomposedPath.end() - 1);
                RelPath.erase(RelPath.begin());
            }
            else{
                ReturnPath.DIsValid = false;
                return ReturnPath;
            }
        }
        else{
            Index++;
        }
    }    
    ReturnPath.DDecomposedPath.insert(ReturnPath.DDecomposedPath.end(), RelPath.begin(), RelPath.end());
    return ReturnPath;
}

/**
* Gets the relative path that goes from source to destination
*
* @param[in] srcpath from CPath
* @param[in] destpath from CPath
*
* @return enum CPath type of ReturnPath
*/

CPath CPath::RelativePath(const CPath &srcpath, const CPath &destpath){
    CPath ReturnPath(destpath);
    std::vector< std::string > BasePath;

    ReturnPath.DIsRelative = true;
    if(srcpath.DIsRelative){
        if(destpath.DIsRelative){
            return SimplifyPath(srcpath,destpath);
        }
        else{
            return RelativePath(CurrentPath().Simplify(srcpath), destpath);    
        }
    }
    else{
        BasePath = srcpath.DDecomposedPath;    
        while(ReturnPath.DDecomposedPath.size() && BasePath.size()){
            if(ReturnPath.DDecomposedPath[0] == BasePath[0]){
                ReturnPath.DDecomposedPath.erase(ReturnPath.DDecomposedPath.begin());
                BasePath.erase(BasePath.begin());
            }
            else{
                break;
            }
        }
        while(BasePath.size()){
            ReturnPath.DDecomposedPath.insert(ReturnPath.DDecomposedPath.begin(), "..");
            BasePath.erase(BasePath.begin());
        }  
    }
    return ReturnPath;
}

/**
* Defining currentpath call when pathname not provided, returns current path 
*
* @return enum CPath type of current path data 
*/

CPath CPath::CurrentPath(){
    std::vector< char > CurrentPathName;

    CurrentPathName.resize(1024);
    while(NULL == getcwd(CurrentPathName.data(), CurrentPathName.size())){
        CurrentPathName.resize(CurrentPathName.size() * 2);
    }
    return CPath(CurrentPathName.data());
}

/**
* Defining currentpath call when pathname provided, returns current path 
*
* @param[in] path Reference from CPath
*
* @return enum CPath type of CurrentPath constructor
*/

CPath CPath::CurrentPath(const CPath &path){
    CPath NewPath = SimplifyPath(CurrentPath(), path);
    
    chdir(NewPath.ToString().c_str());
    return CurrentPath();
}


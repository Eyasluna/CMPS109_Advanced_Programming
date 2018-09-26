// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_pair = xpair<const string,string>;
using str_str_map = listmap<string, string, xless<string>>;
typedef listmap<string, string, xless<string>>::iterator lmap_str_itor;

void trim(string &str){
    size_t startpos = str.find_first_not_of(" \t");
    size_t endpos = str.find_last_not_of(" \t");

    //check for empty string
    if(string::npos == startpos || string::npos == endpos) str = "";
    else str = str.substr(startpos,endpos-startpos+1);
}

    listmap<string, string, xless<string>> test;
void parsefile(const string &infilename, istream &infile){
    for(int linenr = 1;; ++linenr) {
        try {
            //get the line
            string line;
            getline (infile, line);
            if(infile.eof()) break; //if end of file, stop loop
            trim(line);
            //print command
            cout << infilename << ": " << linenr << ": " << line <<endl;
            
            //blank line
            if (line.size() == 0){
                //do nothing
            }
            
            //#
            else if (line.at(0) == '#'){
                //do nothing
            }
            
            //=
            else if (line.compare("=") == 0){ 
                DEBUGF('a', "print everything");
                lmap_str_itor itor = test.end();
                while(itor != test.begin()){
                    --itor;
                    cout << itor->first << " = "
                         << (*itor).second << endl;
                }
            }
            
            //key
            else if(line.find_first_of("=", 0) == string::npos){
                lmap_str_itor lkup = test.find(line);
                if(lkup == test.end())
                    cout << line <<": key not found" << endl;
                else
                    cout << (*lkup).first << " = "
                         << (*lkup).second << endl;
            }
            
            //key =
            else if(line.find_first_of("=", 0) == line.size() - 1){
                DEBUGF('a',"key =");
                string key = line.substr(0, line.size() - 1);
                trim(key);
                lmap_str_itor lkup = test.find(key);
                if(lkup != test.end()) lkup.erase();
            }
            
            //= value
            else if(line.at(0) == '='){
                DEBUGF('a',"= value");
                string value = line.substr(1, line.size());
                lmap_str_itor itor = test.end();
                while(itor!=test.begin()){
                    --itor;
                    if(value.compare(itor->second)==0)
                        cout << itor->first << " = "
                             << itor->second << endl;
                }
            }
            
            //key = value
            else{
                DEBUGF('a',"key = value");
                string key = line.substr(0, line.find_first_of("=", 0));
                string value = line.substr(
                           line.find_first_of("=", 0) + 1, line.size());
                trim(key); trim(value);
                str_str_pair pr(key, value);
                test.insert(pr); //will replace if necessary
                cout << key << " = " << value <<endl;
            }
            DEBUGF ('m', line);

        } catch(runtime_error error){
            complain() << infilename << ":" << linenr
                       << ": " << error.what() <<endl;
        }
    }
    DEBUGF ('m', infilename << "EOF");
}

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

int main (int argc, char** argv) {
    sys_info::execname (argv[0]);
    scan_options (argc, argv);
    
    bool ran = false;
    for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
        ran = true;
        string filename = *argp;
        if (filename.compare("-") == 0){
            parsefile("-", cin);
            continue;
        }
        ifstream in(filename);
        if (in.fail()) {
            syscall_error(filename);
            continue;
        }
        DEBUGF ('m', filename << " =>");
        parsefile(filename, in);
        in.close();
    }
    if(!ran) parsefile("-", cin);
    
    return EXIT_SUCCESS;
}

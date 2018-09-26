// $Id: main.cpp,v 1.9 2016-01-14 16:16:52-08 - - $

#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <unistd.h>

using namespace std;

#include "commands.h"
#include "debug.h"
#include "file_sys.h"
#include "util.h"

// scan_options
//    Options analysis:  The only option is -Dflags. 

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
            complain() << "-" << static_cast<char> (option)
                       << ": invalid option" << endl;
            break;
      }
   }
   if (optind < argc) {
      complain() << "operands not permitted" << endl;
   }
}


// main -
//    Main program which loops reading commands until end of file.

int main (int argc, char** argv) {
   execname (argv[0]);
   cout << boolalpha;  // Print false or true instead of 0 or 1.
   cerr << boolalpha;
   cout << argv[0] << " build " << __DATE__ << " " << __TIME__ << endl;
   scan_options (argc, argv);
   bool need_echo = want_echo();
   // Only 1 inode_state, use constructor to create root
   inode_state state; 
   
   try {
      for (;;) {
         try {
            // Read a line, break at EOF, and echo print the prompt
            // if one is needed.
            cout << state.prompt();
            string line;
            getline (cin, line);
            if (cin.eof()) {
               if (need_echo) cout << "^D";
               cout << endl;
               DEBUGF ('y', "EOF");
               break;
            }
            if (need_echo) cout << line << endl;
            // Keeps from segfaulting since can't split empty string
            if (line.empty()) continue;
            // Split the line into words and lookup the appropriate
            // function.  Complain or call it
            wordvec words = split (line, " \t");
            DEBUGF ('y', "words = " << words);
            // Check if comment
            if (words[0] == "#"){
               // Return to start of for loop
               continue;
            }
            // Get command from wordvec at "arg[0]"
            // set corresponding function to be pointed at by fn
            command_fn fn = find_command_fn (words.at(0));
            // fn is void* fucntion pointer
            // since all functions are void
            fn (state, words);
         }catch (command_error& error) {
            // If there is a problem discovered in any function, an
            // exn is thrown and printed here.
            complain() << error.what() << endl;
         }
      }
   } catch (ysh_exit&) {
      // This catch intentionally left blank.
   }

   return exit_status_message();
}


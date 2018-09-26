// $Id: commands.cpp,v 1.16 2016-01-14 16:10:40-08 - - $

#include <iomanip>
#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_ls    },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   // If not found returns end()
   const auto result = cmd_hash.find (cmd);
   // .end() points past the last element
   if (result == cmd_hash.end()) {  
      throw command_error (cmd + ": no such function");
   }
   //If found return appropriate command_fn
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

// Splits up pathname into wordvec 
wordvec decode_path (const string& line, bool& start_from_root) {
   wordvec pathvec;
   size_t end = 0;
   for(;;){
      if (line.find_first_of("/") == 0){start_from_root = true;}
      if (line == "/"){
         pathvec.push_back(line);
         return pathvec;
      }
      size_t start = line.find_first_not_of ("/", end); 
      if (start == string::npos) break;
      end = line.find_first_of ("/", start);
      pathvec.push_back (line.substr (start, end - start));
   }
   return pathvec;
}

// Readfile print wordvec
void fn_cat (inode_state& state, const wordvec& words){ 
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
      cout << "cat: need file name" << endl;
      return;
   }
   // If cat /
   else if (words[1] == "/"){
      cout << "cat: /: is a directory" << endl;
      return;
   }

   // Used to keep track of where we are in pathname
   int itercheck {0};
   // Needed to check if we start at root or cwd
   bool root_decode {false};
   // Needed to help stop checking pathname if we can't cat
   // Otherwise we will continue printing error messages
   bool cant_cat {false};
   // Store the cwd so that we can return on error
   inode_ptr return_dir = state.get_cwd(); 
   for (size_t i = 1; i < words.size(); i++){
      cant_cat = false;
      itercheck = 0;
      wordvec decode = decode_path(words[i], root_decode);
      int path_end = decode.size();
      if (root_decode) {state.set_cwd(state.get_root());}

      for (auto sub_path: decode){
         ++itercheck;
         for (auto elem: state.get_cwd()->
            get_contents()->get_dirents()){
               // Iterator for end
               auto end = state.get_cwd()->
               get_contents()->get_dirents().rbegin();
            // If filename and end of pathname, readfile
            if (elem.first == sub_path and elem.second->is_file){
               if (itercheck == path_end){
                  cout << elem.second->
                     get_contents()->readfile() << endl;
                  state.set_cwd(return_dir);

               }
               else{
                  // Throw error if pathname invalid
                  cout << "cat: not a valid pathname" << endl;
                  state.set_cwd(return_dir);
               }
            cant_cat = true;
            break;
            }
            // If directory and end of pathname, throw error
            else if (elem.first == sub_path and itercheck == path_end){
               cout << "cat: " << sub_path 
                  << ": is a directory" << endl;
               state.set_cwd(return_dir);
               cant_cat = true;
               break;
            }
            // If directory and not end of pathname
            // set cwd to the directory
            else if (elem.first == sub_path){
               state.set_cwd(elem.second);
               break;
            }
            // If filename not found, throw error
            else if (elem.first == end->first){
               cout << "cat: " << sub_path 
                  << ": No such file or directory" << endl;
               state.set_cwd(return_dir);
               cant_cat = true;
               break;
            }
         }
         // If can't cat, stop checking path
         if (cant_cat) break;
      }
   }  
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // If cd then take user back to root
   if (words.size() == 1 or words[1] == "/"){
      state.set_cwd(state.get_root());
      return;
   }
   else if (words.size() > 2){
      cout << "cd: more than one operand specified" << endl;
      return;
   }
   bool root_decode {false};
   // Gets each individual directory in pathname
   wordvec decode = decode_path(words[1], root_decode);
   // Need a return directory if we mess up
   inode_ptr return_dir = state.get_cwd();
   // Check if we need to start at root
   if (root_decode) {state.set_cwd(state.get_root());}
   for (auto sub_path: decode){
      for (auto elem: state.get_cwd()->
         get_contents()->get_dirents()){
         // Iterator for end
         auto end = state.get_cwd()->
            get_contents()->get_dirents().rbegin();
         //need to get substring and check conditions
         if (elem.first == sub_path and elem.second->is_file){
            cout << "cd: " << sub_path << ": is a file" << endl;
            state.set_cwd(return_dir);
            return;
         }
         else if (elem.first == sub_path){
            state.set_cwd(elem.second);
            break;
         }
         else if (elem.first == end->first){
            cout << "cd: " << sub_path 
               << ": No such directory" << endl;
            state.set_cwd(return_dir);
            return;
         }
      }
   }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Echos out words after "echo" cmd.
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Exits the program on command
   exit_status exit_;
   // If no status or 0 exit withs status 0
   if (words.size() == 1 or words[1] == "0"){
      exit_.set(0);
   }
   // If status given exit with given status
   else if (atoi(words[1].c_str())){
      exit_.set(atoi(words[1].c_str()));
   }
   else
      // If non numeric argument given exit 127
      exit_.set(127);

   throw ysh_exit();
}

void ls_print(inode_state& state, auto sub_path){
   if (state.get_cwd() == state.get_root()){
      cout << state.get_cwd()->get_path() + ":" << endl;
   }
   else
      cout << sub_path + ":" << endl;
   for (auto elem: state.get_cwd()->get_contents()->get_dirents()){
      cout << setw(6);
      cout << elem.second->get_inode_nr();
      cout << setw(6);
      cout << elem.second->get_contents()->size() << + " ";
      // Special printing based on file or directory
      if (elem.second->is_file){
         cout << elem.first << endl;
      }
      // Dont add "/" to "." or ".."
      else if (elem.first == "." or elem.first == ".."){
         cout << elem.first << endl;
      }
      else
         // Add "/" to denote directory
         cout << elem.first + "/" << endl;
   }
   return;
}

void fn_lsr (inode_state& state, inode_ptr return_dir, 
     inode_ptr start_dir){
   for(;;){
      for (auto elem: state.get_cwd()->
         get_contents()->get_dirents()){
         // Iterator to know where the end is
         auto end = state.get_cwd()->
         get_contents()->get_dirents().rbegin();
         // If empty directory, at end of fn_lsr
         if (state.get_cwd()->
            get_contents()->get_dirents().size() == 2){
            // Print out directory contents
            ls_print(state, state.get_cwd()->get_path());
            // If we lsr in root with no directories
            // start_dir == cwd and we can stop
            if (state.get_cwd() == start_dir){
               state.set_cwd(return_dir);
               return;
            }
            else{
               // Mark directory as checked so can skip over
               state.get_cwd()->checked = true;
               // We are at the end of the pathway since 
               // there are no more directories to enter
               // so cd .. and start over, skipping this directory. 
               state.set_cwd(state.get_cwd()->
               get_contents()->get_dirents().find("..")->second);
               break;
            }
         }
         else if (elem.first == "." or elem.first == "..") 
         { /*Do nothing*/ } 

         // Has this directory been checked yet?
         else if (elem.second->checked) { 
            // Are we at the end of the directory?
            if (elem.first == end->first){
               // Need to return values to false or else
               // will stay true outside of function.
               for (auto setfalse: state.get_cwd()->
                  get_contents()->get_dirents()){
                  setfalse.second->checked = false;
               }
               // Reset print state back to true so that
               // it doesn't stay true after we return.
               state.get_cwd()->not_printed = true;
               // If we are at root directory and have
               // checked everything then end function.
               if (state.get_cwd() == start_dir){
                  state.set_cwd(return_dir);
                  return;
               }
               // If directory in cwd has has been checked
               // and at end of directory, mark cwd checked.
               else{
                  state.get_cwd()->checked = true;
                  state.set_cwd(state.get_cwd()->
                  get_contents()->get_dirents().find("..")->second);
                  break;
               }
            }
         }
         // Are we looking at a file?
         else if (elem.second->is_file){
            // Are we at the end of the directory? 
            if (elem.first == end->first){
               // If in root directory, end function.
               if (state.get_cwd() == start_dir){
                  state.set_cwd(return_dir);
                  return;
               }
               else{
                  // Print the directory.
                  ls_print(state, state.get_cwd()->get_path());
                  // Need to return values to false or else
                  // will stay true outside of function.
                  for (auto setfalse: state.get_cwd()->
                     get_contents()->get_dirents()){
                     setfalse.second->checked = false;
                  }
                  // If at end of directory mark checked.
                  state.get_cwd()->checked = true;
                  // Move to parent directory
                  state.set_cwd(state.get_cwd()->
                  get_contents()->get_dirents().find("..")->second);
                  break;
               }
            }
         }
         // Must be looking at an unchecked directory, 
         // cd into said directory
         else{
            // Don't print if directory has been printed previously
            if (state.get_cwd()->not_printed){
               ls_print(state, state.get_cwd()->get_path());
               state.get_cwd()->not_printed = false;
            }
            state.set_cwd(elem.second);
            break;
         }
      }
   }
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   int itercheck {0};
   bool lsr {false};
   bool root_decode {false};
   inode_ptr return_dir = state.get_cwd();
   inode_ptr start_dir;

   // Check if recursive ls
   if (words[0] == "lsr") {lsr = true;}
   // If ls with no arguments
   if (words.size() == 1){
      if (lsr) {fn_lsr(state, return_dir, return_dir);}
      else{
         ls_print(state, state.get_cwd()->get_path());
      }
      return;
   }
   // Loops through each pathname 
   for (size_t i = 1; i < words.size(); i++){ 
      itercheck = 0;
      wordvec decode = decode_path(words[i], root_decode);
      int path_end = decode.size();
      if (root_decode) {state.set_cwd(state.get_root());}
      // Loops through each individual directory in pathname 
      for (auto sub_path: decode){
      ++itercheck;
         // Loops through elements in cwd to compare
         for (auto elem: state.get_cwd()->
            get_contents()->get_dirents()){
            // Iterator for end
            auto end = state.get_cwd()->
            get_contents()->get_dirents().rbegin();
            // If ls root 
            if (sub_path == "/"){
               state.set_cwd(state.get_root());
               if (lsr){
                  fn_lsr(state, return_dir, state.get_root());
                  state.set_cwd(return_dir);
                  break;
               }
               ls_print(state, sub_path);
               state.set_cwd(return_dir);
               break;
            }
            // If ls "filename", just print out filename
            else if (elem.first == sub_path and elem.second->is_file){
               if (itercheck == path_end){
                  cout << sub_path << endl;
                  state.set_cwd(return_dir);
                  break;
               }
               else{
                  cout << words[0] << ": not a valid pathname" << endl;
                  state.set_cwd(return_dir);
                  break;
               }
            }
            // If find directory and at end of pathname 
            // print dir contents   
            else if (elem.first == sub_path 
                  and itercheck == path_end){
               state.set_cwd(elem.second);
               if (lsr){
                  fn_lsr(state, return_dir, elem.second);
                  state.set_cwd(return_dir);
                  break;
               }
               ls_print(state, state.get_cwd()->get_path());
               state.set_cwd(return_dir);
               break;
            }
            // If find directory but not at end of pathname
            // set found directory to cwd
            else if (elem.first == sub_path){
               state.set_cwd(elem.second);
               break;
            }
            // If find nothing throw error
            else if (elem.first == end->first){
               cout << words[0] << ": cannot access " << sub_path
               << ": No such file or directory" << endl;
               state.set_cwd(return_dir);
               break;
            }
         }
      }
   }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
      cout << "make: need file name" << endl;
      return;
   }
   // If make /
   else if (words[1] == "/"){
      cout << "make: /: is a directory" << endl;
      return;
   }
   // Use itercheck to check if we're 
   // at the end of the pathname
   int itercheck {0};
   bool root_decode {false};
   wordvec decode = decode_path(words[1], root_decode);
   int path_end = decode.size();
   inode_ptr return_dir = state.get_cwd();

   if (root_decode) {state.set_cwd(state.get_root());}
   for (auto sub_path: decode){
      // Decode will always be at least size 1
      ++itercheck;
      for (auto elem: state.get_cwd()->get_contents()->get_dirents()){
         auto end = state.get_cwd()->
         get_contents()->get_dirents().rbegin();
         // If file already exists, just skip to writefile
         // Needs to be end of pathname
         if (elem.first == sub_path and elem.second->is_file){
            if (itercheck == path_end){
               elem.second->get_contents()->writefile(words);
               state.set_cwd(return_dir);
            }
            else{
               // Error if pathname invalid at certain point
               cout << "make: not a valid pathname" << endl;
               state.set_cwd(return_dir);
            }
            return;
         } 
         // If directory name already exists throw error
         else if (elem.first == sub_path and itercheck == path_end){
            cout << "make: " << sub_path << ": is a directory" << endl;
            state.set_cwd(return_dir);
            return;
         }
         // If directory but not end of pathname, update cwd
         else if (elem.first == sub_path){
            state.set_cwd(elem.second);
            break;
         }
         // Condition for reaching end of path (end of decode)
         // If we are at last element of directory and the end
         // of the pathname, then create file because it doesn't exist
         else if (elem.first == end->first and itercheck == path_end){
            // Need to make file first then fill it with writefile
            inode_ptr file = state.get_cwd()->
               get_contents()->mkfile(decode.back());
            file->get_contents()->writefile(words);
            state.set_cwd(return_dir);
            return;
         }
         // Don't find anything and not end of pathname
         else if (elem.first == end->first){
            cout << "make: not a valid pathname" << endl;
            state.set_cwd(return_dir);
            return;
         }
      }
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
      cout << "mkdir: need directory name" << endl;
      return;
   }
   else if (words[1] == "/"){
      cout << "mkdir: file or directory name already exists" << endl;
      return;
   }
   else if (words.size() > 2){
      cout << "mkdir: more than one operand specified" << endl;
      return;
   }
   int itercheck {0};
   bool root_decode {false};
   wordvec decode = decode_path(words[1], root_decode);
   int path_end = decode.size();
   inode_ptr return_dir = state.get_cwd();

   if (root_decode) {state.set_cwd(state.get_root());}
   for (auto sub_path: decode){
      ++itercheck;
      // If filename, throw error since filename already exists
      for (auto elem: state.get_cwd()->get_contents()->get_dirents()){
      auto end = state.get_cwd()->
      get_contents()->get_dirents().rbegin();
         if (elem.first == sub_path and elem.second->is_file){
            cout << "mkdir: file or directory "
                 << "name already exists" << endl;
            state.set_cwd(return_dir);
            return;
         }
         // If directory and end of pathname, 
         // error directory already exists
         else if (elem.first == sub_path and itercheck == path_end){
            cout << "mkdir: file or directory "
                 << "name already exists" << endl;
            state.set_cwd(return_dir);
            return;
         }
         // If directory, but not end of pathname set directory to cwd
         else if (elem.first == sub_path){
            state.set_cwd(elem.second);
            break;
         }
         // If end of directory and pathname, create new directory
         else if (elem.first == end->first and itercheck == path_end){
            // Calls mkdir in file_sys passing in the directory name,
            // puts new directory in cwd's dirents, 
            // and returns an inode_ptr 
            inode_ptr dir = state.get_cwd()->
               get_contents()->mkdir(sub_path);
            // Put (.) and (..) inside of the new directory.
            dir->get_contents()->dir_init(state.get_cwd(), dir);
            if (state.get_cwd()->is_root){
               dir->set_path(state.get_cwd()->get_path() + sub_path);
            }
            else
               dir->set_path(state.get_cwd()->
                  get_path() + "/" + sub_path);
            state.set_cwd(return_dir);
            return;
         }
         // If end of directoy and didn't find anything error
         else if (elem.first == end->first){
            cout << "mkdir: not a valid pathname" << endl;
            state.set_cwd(return_dir);
            return;
         }
      }  
   }
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string new_prompt;
   // Create new prompt from words from cmd line
   for (size_t i = 1; i < words.size(); i++){
    new_prompt += words[i] + " ";
   }
   // Make sure there is a space after new prompt
   state.prompt_ = new_prompt;
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() > 1){
      cout << "pwd: ignoring non-option arguments" << endl;
   }
   cout << state.get_cwd()->get_path() << endl;
}

void fn_rmr (inode_state& state, inode_ptr return_dir, auto elem){
   for(;;){
      for (auto sub_elem: state.get_cwd()->
         get_contents()->get_dirents()){
         //while not . or .. cd into, if is_file, remove. 
         inode_ptr parent = state.get_cwd()->
         get_contents()->get_dirents().find("..")->second;
         
         if (sub_elem.second->is_file){
            state.get_cwd()->get_contents()->remove(sub_elem.first);
         }
         else if (sub_elem.first == "." or sub_elem.first == "..") 
            { /*Do Nothing*/ }
         else{
            // Check size = 2 since empty directories 
            // only contain . and ..
            if (sub_elem.second->get_contents()->
                  get_dirents().size() == 2){
               state.get_cwd()->get_contents()->remove(sub_elem.first);
            }
            else{
               state.set_cwd(sub_elem.second);
            }
         }
         //if cwd == 2 set cwd to parent 
         if (state.get_cwd()->get_contents()->
            get_dirents().size() == 2){
            if (state.get_cwd() == elem.second){
               state.set_cwd(parent);
               state.get_cwd()->get_contents()->remove(elem.first);
               state.set_cwd(return_dir);
               return;
            }
            else
               state.set_cwd(parent);
         }
      }
   }
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.size() == 1){
      cout << words[0] << ": need file or directory name" << endl;
      return;
   }
   else if (words[1] == "/"){
      cout << words[0] << ": cannot remove /" << endl;
      return;
   }
   else if (words.size() > 2){
      cout << words[0] << ": more than one operand specified" << endl;
      return;
   }
   int itercheck {0};
   bool root_decode {false};
   wordvec decode = decode_path(words[1], root_decode);
   int path_end = decode.size();
   inode_ptr return_dir = state.get_cwd();
   
   if (root_decode) {state.set_cwd(state.get_root());}
   for (auto sub_path: decode){
      ++itercheck;
      // Check if directory name already exists in cwd
      for (auto elem: state.get_cwd()->get_contents()->get_dirents()){
      auto end = state.get_cwd()->get_contents()->
         get_dirents().rbegin();
         if (elem.first == sub_path and elem.second->is_file){
            if (itercheck == path_end){
               state.get_cwd()->get_contents()->remove(elem.first);
               state.set_cwd(return_dir);
               return;
            }
            else{
               cout << words[0] << ": not a valid pathname" << endl;
               state.set_cwd(return_dir);
               return;
            }
         }
         else if (elem.first == sub_path and itercheck == path_end){
            if (sub_path == "." or sub_path == ".."){
               cout << words[0] << ": cannot remove " 
                  << sub_path << endl;
               state.set_cwd(return_dir);
               return;
               }
            // Call rmr function if rmr
            else if (words[0] == "rmr"){
               state.set_cwd(elem.second);
               fn_rmr(state, return_dir, elem);
               return;
            }
            else if (elem.second->get_contents()->
               get_dirents().size() == 2){
               state.get_cwd()->get_contents()->remove(elem.first);
               state.set_cwd(return_dir);
               return;
            }
            else{
               cout << words[0] << ": " << sub_path 
                  << ": is not empty" << endl;
               state.set_cwd(return_dir);
               return;
            }
         }
         else if (elem.first == sub_path){
            state.set_cwd(elem.second);
            break;
         }
         else if (elem.first == end->first){
            cout << words[0] << ": " << sub_path 
               << ": No such file or directory" << endl;
            state.set_cwd(return_dir);
            return;
         }
      }
   }
}

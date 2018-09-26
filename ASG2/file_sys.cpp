// $Id: file_sys.cpp,v 1.5 2016-01-14 16:16:52-08 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
  size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
  }
};

// Prints out file type
ostream& operator<< (ostream& out, file_type type) {
  static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
  };
  return out << hash[type];
}

void plain_file::dir_init(inode_ptr, inode_ptr){
  throw file_error ("is a plain file");
}

// Sets up initial directory with (.) and (..)
void directory::dir_init(inode_ptr parent, inode_ptr current){
  dirents.emplace(".",  current);
  dirents.emplace("..", parent);
  for (auto elem: dirents){
  }
}

// inode_state constructor
inode_state::inode_state() {
  root = make_shared<inode>(file_type::DIRECTORY_TYPE);
  root->contents->dir_init(root,root);
  root->is_root = true;
  root->path = "/";
  cwd = root;
  DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

const string& inode_state::prompt() { return prompt_; }

// Overload operator<< in order to print inode_state objects
ostream& operator<< (ostream& out, const inode_state& state) {
  out << "inode_state: root = " << state.root
      << ", cwd = " << state.cwd;
  return out;
}

// inode constructor
inode::inode(file_type type): inode_nr (next_inode_nr++) {
  switch (type) {
    case file_type::PLAIN_TYPE:
          //Depending on type make base_ptr contents
          //point to plain_type or directory_type 
          contents = make_shared<plain_file>();
          break;
    case file_type::DIRECTORY_TYPE:
          //Calls directory() constructor and 
          //creates object of directory type
          //pointed to by shared pointer contents
          contents = make_shared<directory>();
          break;
   }
  DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
  DEBUGF ('i', "inode = " << inode_nr);
  return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

// loop through wordvec then assign size
size_t plain_file::size() const {
  size_t size {0};
  string concat;

  if (data.size() == 0){
    return 0;
  }
  for (auto elem: data) {
    concat += elem;
    size++;
  }
  // Need to subtract off last space
  size += concat.length() - 1;
  DEBUGF ('i', "size = " << size);
  return size;
}

const wordvec& plain_file::readfile() const {
  DEBUGF ('i', data);
  return data;
}

void plain_file::writefile (const wordvec& words) {
  DEBUGF ('i', words);
  // Clear vector to be rewritten
  data.clear();
  // Check that words.size() > 2 (bigger than cmd + filename)
  if (words.size() > 2){
    for (size_t i = 2; i < words.size(); i++){
      data.push_back(words[i]);
    }
  }
}

void plain_file::remove (const string&) {
  // "directory" so removed with directory remove?
  throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (const string&) {
  throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
  throw file_error ("is a plain file");
}

map<string,inode_ptr> plain_file::get_dirents() const {
  throw file_error ("is a plain_file");
}

wordvec directory::get_data() const {
   throw file_error ("is a directory");
}

size_t directory::size() const {
  size_t size {0};
  for (auto elem: dirents) ++size;
  DEBUGF ('i', "size = " << size);
  return size;
}

const wordvec& directory::readfile() const {
  throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
  throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
  DEBUGF ('i', filename);
  dirents.erase(filename);
}

inode_ptr directory::mkdir (const string& dirname) {
  DEBUGF ('i', dirname);
  // Create new inode_ptr to inode directory type
  inode_ptr dir = make_shared<inode>(file_type::DIRECTORY_TYPE);
  // Add new directory entry into dirents for cwd.
  dirents.emplace(dirname, dir);
  return dir;
}

inode_ptr directory::mkfile (const string& filename) {
  DEBUGF ('i', filename);
  // Create new inode_ptr to inode file type
  inode_ptr file = make_shared<inode>(file_type::PLAIN_TYPE);
  file->is_file = true;
  // Add new file entry into dirents
  dirents.emplace(filename, file);
  return file;
}


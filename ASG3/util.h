// $Id: util.h,v 1.6 2018-01-25 14:18:43-08 - - $

//
// util -
//    A utility class to provide various services not conveniently
//    associated with other modules.
//

#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <list>
#include <stdexcept>
#include <string>

using namespace std;

//
// sys_info -
//    Keep track of execname and exit status.  Must be initialized
//    as the first thing done inside main.  Main should call:
//       sys_info::set_execname (argv[0]);
//    before anything else.
//

class sys_info {
   private:
      static string execname_;
      static int exit_status_;
      static void execname (const string& argv0);
      friend int main (int argc, char** argv);
   public:
      static const string& execname ();
      static void exit_status (int status);
      static int exit_status ();
};

//
// datestring -
//    Return the current date, as printed by date(1).
//

const string datestring ();

//
// split -
//    Split a string into a list<string>..  Any sequence
//    of chars in the delimiter string is used as a separator.  To
//    Split a pathname, use "/".  To split a shell command, use " ".
//

list<string> split (const string& line, const string& delimiter);


//
// complain -
//    Used for starting error messages.  Sets the exit status to
//    EXIT_FAILURE, writes the program name to cerr, and then
//    returns the cerr ostream.  Example:
//       complain() << filename << ": some problem" << endl;
//

ostream& complain();

//
// syscall_error -
//    Complain about a failed system call.  Argument is the name
//    of the object causing trouble.  The extern errno must contain
//    the reason for the problem.
//

void syscall_error (const string&);

//
// operator<< (list) -
//    An overloaded template operator which allows lists to be
//    printed out as a single operator, each element separated from
//    the next with spaces.  The item_t must have an output operator
//    defined for it.
//

template <typename item_t>
ostream& operator<< (ostream& out, const list<item_t>& vec);

//
// string to_string (thing) -
//    Convert anything into a string if it has an ostream<< operator.
//

template <typename item_t>
string to_string (const item_t&);

//
// thing from_string (cons string&) -
//    Scan a string for something if it has an istream>> operator.
//

template <typename item_t>
item_t from_string (const string&);

//
// Put the RCS Id string in the object file.
//

#include "util.tcc"
#endif


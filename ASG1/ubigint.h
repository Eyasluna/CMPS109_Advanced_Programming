#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"

class ubigint {
   friend ostream& operator<< (ostream&, const ubigint&);
   private:          
      using quo_rem = pair<ubigint, ubigint>;      
      using udigit_t = unsigned char;
      using ubigvalue_t = vector<udigit_t>; //Create a vector of char 
      using number = unsigned long;
      number uvalue {};
      ubigvalue_t ubig_value;               //to represent input.
      quo_rem udivide (const ubigint&) const;


   public:

      void multiply_by_2();
      void divide_by_2();
      void remove_0s(ubigint&) const;
      void checksize(const ubigint&, const ubigint&, 
                        ubigint&, ubigint&) const; 

      ubigint() = default;     // Need default ctor as well.
      ubigint (unsigned long); // using an integer constant directly 
      ubigint (const string&); // in code that requires a bigint.

      ubigint operator+ (const ubigint&) const;
      ubigint operator- (const ubigint&) const;
      ubigint operator* (const ubigint&) const;
      ubigint operator/ (const ubigint&) const;
      ubigint operator% (const ubigint&) const;

      bool operator== (const ubigint&) const;
      bool operator<  (const ubigint&) const;
};

#endif

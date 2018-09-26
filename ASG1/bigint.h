// $Id: bigint.h,v 1.29 2016-03-24 19:30:57-07 - - $

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

class bigint {
   friend ostream& operator<< (ostream&, const bigint&);
   private:
      //Creates uvalue ubigint 
      ubigint uvalue;  
      bool is_negative {false};
   public:
      // Needed or will be suppressed.
      bigint() = default; 
      bigint (long); 
      //Passing a ubigint back to bigint      
      bigint (const ubigint&, bool is_negative = false); 
      //Created when pushed onto operand_stack in main().
      explicit bigint (const string&);  

      bigint operator+() const;
      bigint operator-() const;

      bigint operator+ (const bigint&) const;
      bigint operator- (const bigint&) const;
      bigint operator* (const bigint&) const;
      bigint operator/ (const bigint&) const;
      bigint operator% (const bigint&) const;

      bool operator== (const bigint&) const;
      bool operator<  (const bigint&) const;
};

#endif


//CMPS 109 ASG1 
//Yibo Fu
//Zhufeng Chen 

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
  DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}


bigint::bigint (const string& that) {
    is_negative = that.size() > 0 && that[0] == '_';
    uvalue = ubigint (that.substr (is_negative ? 1 : 0));  
    
}   


bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}


bigint bigint::operator+ (const bigint& that) const {     
  if (that.is_negative && is_negative )
  {                  
    return {that.uvalue + uvalue, true};                   
    }                                                     
  else if (is_negative || that.is_negative){              
    if (is_negative)
    {
      if (uvalue == that.uvalue || uvalue < that.uvalue)
        {
          return uvalue - that.uvalue;
        }             
    }
    else if (that.is_negative)
    {                            
      if (that.uvalue < uvalue || that.uvalue == uvalue)   
        {
          return uvalue - that.uvalue;
        }                   
    }
    return {uvalue - that.uvalue, true}; 
  }
  return that.uvalue + uvalue;                 
}                                              



bigint bigint::operator- (const bigint& that) const {
  if (is_negative && that.is_negative){
    if (uvalue < that.uvalue || uvalue == that.uvalue)
    {  
      return uvalue - that.uvalue;                       
    }
    return {uvalue - that.uvalue, true};                 
  }
  else if (is_negative || that.is_negative)
  {
    if (is_negative)
    {
      return {uvalue + that.uvalue, true};
    }
    else if (that.is_negative)

      return uvalue + that.uvalue;
  }
  else{
    if (that.uvalue > uvalue)
      return {uvalue - that.uvalue, true};
  }
  return uvalue - that.uvalue;
}


bigint bigint::operator* (const bigint& that) const 
{
  if (that.is_negative && is_negative)
  { 
    return uvalue * that.uvalue;
  }
  else if (that.is_negative || is_negative)
  {
    return {uvalue * that.uvalue, true};
  }
  return uvalue * that.uvalue; 
}

bigint bigint::operator% (const bigint& that) const 
{
  if (that.is_negative && is_negative)
  { 
    return uvalue % that.uvalue;
  }
  else if (that.is_negative && is_negative)
  { 
    return {uvalue % that.uvalue, true};
  }
  return uvalue % that.uvalue;
}

bigint bigint::operator/ (const bigint& that) const 
{
  if (that.is_negative && is_negative){ 
    return uvalue / that.uvalue;
  }
  else if (that.is_negative || is_negative){ 
    return {uvalue / that.uvalue, true};
  }
  return uvalue / that.uvalue;
}


bool bigint::operator== (const bigint& that) const 
{
  return is_negative == that.is_negative && uvalue == that.uvalue;
}


bool bigint::operator< (const bigint& that) const 
{
  if (is_negative != that.is_negative) return is_negative;    
  return is_negative ? that.uvalue < uvalue                   
                     : uvalue < that.uvalue;                  
}

ostream& operator<< (ostream& out, const bigint& that) 
{      
  return out << (that.is_negative ? "-" : "") 
              << that.uvalue;
}


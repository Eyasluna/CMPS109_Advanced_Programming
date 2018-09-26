//CMPS109 ASG1
//Yibo Fu
//Zhufeng Chen

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <algorithm>
#include <typeinfo> 
#include <cmath>
#include <climits>
using namespace std;

#include "ubigint.h"
#include "debug.h"
#include "util.h"

ubigint::ubigint (unsigned long that) : uvalue (that)
{        
   DEBUGF ('~', this << " -> " << uvalue)           
   if (that > ULONG_MAX)
   {
      throw ydc_exn ("exceed maxium");
   }  
   while (that > 0) {                                    
      ubig_value.push_back(that%10);                     
      that/= 10;
   }
}

ostream& operator<< (ostream& out, const ubigint& that) 
{ 
   unsigned int nline = 69, size;
   vector<char> vnum;
   string num;

   if (that.ubig_value.empty())
      return out << "0";
   for (int i : that.ubig_value)
   {
      char c = i + '0';                     
      vnum.insert(vnum.begin(), c);         
   }
   for (char j : vnum){
      num += j;
   }
   size = vnum.size();
   while (nline < size)
   {
      num.insert(nline, "\\");
      num.insert(nline+1, "\n");
      nline += 71;
      size += 2;
   }
   return out << num;   
}


ubigint::ubigint (const string& that): uvalue(0)
{
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {                              
      if (not isdigit (digit)) {                         
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }

      int idigit = digit - '0';                          
      ubig_value.insert(ubig_value.begin(), idigit);     
   }
   ubigint::remove_0s(*this);                            
}
ubigint ubigint::operator+ (const ubigint& that) const 
{ 
   ubigint sum, resize, fixed;                           
   int isum    {0};
   size_t iter {0}; 
   bool carry {false};
   
   ubigint::checksize(*this, that, resize, fixed);  
   for (int left: resize.ubig_value){
      int right = fixed.ubig_value[iter]; 
      if (carry){++isum;} 
      isum += left + right;
      if (isum >= 10 and resize.ubig_value.size() == iter + 1){
         isum -= 10;
         sum.ubig_value.push_back(isum);
         isum = 1;
      }  
      else if (isum >= 10)
      {
         isum -= 10;
         carry = true;
      }
      else carry = false;
      sum.ubig_value.push_back(isum);
      isum = 0;
      iter = iter + 1;
   } 
   ubigint::remove_0s(sum); 
   return sum; 
}
   


ubigint ubigint::operator* (const ubigint& that) const 
{
   ubigint mult;
   int carry = 0, digit = 0;
   int usize = ubig_value.size();
   int tsize = that.ubig_value.size();
   mult.ubig_value.assign (ubig_value.size() + 
      that.ubig_value.size(), 0);

   for (int i = 0; i < usize; i++){         
      carry = 0;
      for (int j = 0; j < tsize; j++){
         digit = mult.ubig_value[i+j] + 
                  (ubig_value[i] * that.ubig_value[j]) + carry;
         mult.ubig_value[i+j] = digit%10;
         carry = digit/10;
         mult.ubig_value[i+tsize] = carry;
      }
   }
   ubigint::remove_0s(mult);   
   return mult;
}

void ubigint::multiply_by_2() 
   {(*this) = (*this) + (*this);}     

void ubigint::divide_by_2() 
{
   int curr = ubig_value.size();
   pair<int,int> div_2 {0,0};
   bool carry {false};

   while (curr > 0)
   {
      --curr;
      int num = ubig_value[curr];
      if (carry){
         num += div_2.second;
      }
      div_2 = make_pair(num/2,num%2);
      if (div_2.second == 0){
         carry = false;
      }
      else{
         div_2.second *= 10;
         carry = true;
      }
      ubig_value[curr] = div_2.first;
   }
   while (ubig_value.size() > 0 and ubig_value.back() == 0)  
      ubig_value.pop_back(); 
}

void ubigint::remove_0s(ubigint& that) const
{
   while (that.ubig_value.size() > 0 and that.ubig_value.back() == 0)   
      that.ubig_value.pop_back(); 
}

ubigint ubigint::operator- (const ubigint& that) const 
{
   ubigint diff, resize, fixed;     
   int idiff    {0};                
   size_t iter  {0}; 
   bool carry {false};

   ubigint::checksize(*this, that, resize, fixed);
     
   for (int left: fixed.ubig_value){               
      int right = resize.ubig_value[iter];          
      if (carry)
      {
         --idiff;
      }  
      idiff += left - right;  
      if (idiff < 0)
      {
         idiff += 10;
         carry = true;
      }
      else
         carry = false; 
      diff.ubig_value.push_back(idiff);
      idiff = 0;
      iter = iter + 1;
   }
   ubigint::remove_0s(diff);   
   return diff;
}

void ubigint::checksize(const ubigint& left, const ubigint& right, 
                           ubigint& resize, ubigint& fixed) const 
{
   size_t Lsize = left.ubig_value.size();
   size_t Rsize = right.ubig_value.size(); 
   if (Lsize > Rsize){                                   
      resize.ubig_value = right.ubig_value;              
      fixed.ubig_value = left.ubig_value;                
      while (Lsize > resize.ubig_value.size())
         resize.ubig_value.push_back(0);                  
   }
   else if (Lsize < Rsize)
   {
      resize.ubig_value = left.ubig_value;
      fixed.ubig_value = right.ubig_value;

      while (resize.ubig_value.size() < Rsize)
         resize.ubig_value.push_back(0);
   }
   else{
     
      if (left < right){                                 
         fixed.ubig_value = right.ubig_value;            
         resize.ubig_value = left.ubig_value;
      }
      else{
         fixed.ubig_value = left.ubig_value;
         resize.ubig_value = right.ubig_value;
      }  
   }
}



ubigint ubigint::operator/ (const ubigint& that) const {
   return (udivide (that)).first;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return (udivide (that)).second;
}

bool ubigint::operator== (const ubigint& that) const {
   size_t iter {0};
   if (ubig_value.size() == that.ubig_value.size()){                 
      for (int left : ubig_value){                         
         int right = that.ubig_value[iter];
         if (left == right){                                        
            if (left == right and ubig_value.size() == iter + 1){   
               return true;
            }
         }
         else
            break;
      iter = iter + 1;
      }
   }
   return false;
}

ubigint::quo_rem ubigint::udivide (const ubigint& that) const {
   ubigint zero {"0"};
   if (that == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {"1"};
   ubigint quotient {"0"};
   ubigint divisor = that;
   ubigint remainder = *this;
   while (divisor < remainder) {
      divisor.multiply_by_2();         
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {quotient, remainder};
}

bool ubigint::operator< (const ubigint& that) const {  
   int Lsize = ubig_value.size();
   int Rsize = that.ubig_value.size();
   if (Lsize < Rsize){                                               
      return true;
   }
   else if (Rsize == Lsize){                                         
      --Lsize;
      while (Lsize > -1)
      {
         if (ubig_value[Lsize] < that.ubig_value[Lsize])
         {            
            return true; 
         }
         else if (ubig_value[Lsize] == that.ubig_value[Lsize])
         {      
            --Lsize;
         }
         else
            break;                                                   
      }
   }
   return false;
}


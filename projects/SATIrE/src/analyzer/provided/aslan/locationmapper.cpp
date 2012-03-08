#include "locationmapper.h"


bool operator<(LocalPosition lhs,LocalPosition rhs){
   bool result=false;
   
   if(lhs.first<rhs.first){
      result=true;
   }
   else{
      if(lhs.first>rhs.first){
         result=false;
      }
      else{ // is equal
         if(lhs.second<rhs.second){
            result=true;
         }
         else{
            result=false;
         }
      }
   }
   
   return result;
}


bool operator<(LocalSite lhs,LocalSite rhs){
   bool result=false;
   
   result=(lhs.first<rhs.first);
   
   return result;
}



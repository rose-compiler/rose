
int y_GlobalScope_TopOfIncludeRegion; 
int y_FileScope_TopOfIncludeRegion; 
int y_FunctionScope_TopOfIncludeRegion; 
int y_NestedConditionalScope_TopOfIncludeRegion; 
int y_NestedLoopScope_TopOfIncludeRegion; 
int y_ParentScope_TopOfIncludeRegion; 
int y_LocalScope_TopOfIncludeRegion;

#include<stdio.h>

int y_LocalScope_BottomOfIncludeRegion;
int y_ParentScope_BottomOfIncludeRegion;
int y_NestedLoopScope_BottomOfIncludeRegion;
int y_NestedConditionalScope_BottomOfIncludeRegion;
int y_FunctionScope_BottomOfIncludeRegion;
int y_FileScope_TopOfScope;
int y_FileScope_BottomOfIncludeRegion;
int y_FileScope_BeforeCurrentPosition;
int y_GlobalScope_TopOfScope;
int y_GlobalScope_BottomOfIncludeRegion;
int y_GlobalScope_BeforeCurrentPosition; 

int main()
   { 
     int y_FunctionScope_TopOfScope; 
     int y_NestedConditionalScope_TopOfScope; 
     int y_NestedLoopScope_TopOfScope; 
     int y_ParentScope_TopOfScope;
     for (int i = 0; i < 1; i++)
        { 
          int y_LocalScope_TopOfScope;
          int y_LocalScope_BeforeCurrentPosition; 

          int x;

          int y_LocalScope_AfterCurrentPosition; 
          int y_LocalScope_BottomOfScope; 
        } 
     int y_ParentScope_BottomOfScope; 
     int y_NestedLoopScope_BottomOfScope; 
     int y_NestedConditionalScope_BottomOfScope; 
     int y_FunctionScope_BottomOfScope; 

     return 0;
   } 

int y_FileScope_AfterCurrentPosition; 
int y_FileScope_BottomOfScope; 
int y_GlobalScope_AfterCurrentPosition; 
int y_GlobalScope_BottomOfScope; 

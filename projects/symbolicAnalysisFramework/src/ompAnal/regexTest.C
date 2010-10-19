#include <boost/regex.hpp>
#include <iostream>
#include <map>

using namespace boost;
using namespace std;

void print_captures(const std::string& regx, const std::string& text)
{
   boost::regex e(regx);
   boost::smatch what;
   std::cout << "Expression:  \"" << regx << "\"\n";
   std::cout << "Text:        \"" << text << "\"\n";
   	
	sregex_iterator m1(text.begin(), text.end(), e, match_extra);
	sregex_iterator m2;
	// This is a workaround for a boost bug where the captures from the previous match
	// get preserved for the next match. These stale captures always get placed at the end
	// of the next match, so if we know how many of them there are, we can ignore them.
	map<int, int> lastNumCaptures;
	bool firstMatch=true;
	
	while(m1 != m2)
	{
		smatch what = *m1;
		cout << "ompReductionsSet: " << what[0] << " | " << what[1] << "\n";

		unsigned i, j;
      std::cout << "** Match found **\n   Sub-Expressions:\n";
		for(i = 0; i < what.size(); ++i)
		{
         std::cout << "      $" << i << " = \"" << what[i] << "\"\n";
        	if(firstMatch)
        	{
        		lastNumCaptures[0] = 0;
      	}
      }
      std::cout << "   Captures:\n";
      for(i = 0; i < what.size(); ++i)
      {	
         std::cout << "      $" << i << " = {";
         //cout << " lastNumCaptures.at("<<i<<") = "<<lastNumCaptures[i]<<", what.captures("<<i<<").size()="<<what.captures(i).size()<<"\n";
         for(j = 0; j < (what.captures(i).size() - lastNumCaptures[i]); ++j)
         {
            if(j) std::cout << ", ";
            else std::cout << " ";
            std::cout << "\"" << what.captures(i)[j] << "\"";
         }
         std::cout << " }\n";
         	
         if(!firstMatch) lastNumCaptures[i] = what.captures(i).size();
      }
      firstMatch = false;

		m1++;
	}
   	
/*   if(boost::regex_match(text, what, e, boost::match_extra))
   {
      unsigned i, j;
      std::cout << "** Match found **\n   Sub-Expressions:\n";
      for(i = 0; i < what.size(); ++i)
         std::cout << "      $" << i << " = \"" << what[i] << "\"\n";
      std::cout << "   Captures:\n";
      for(i = 0; i < what.size(); ++i)
      {
         std::cout << "      $" << i << " = {";
         for(j = 0; j < what.captures(i).size(); ++j)
         {
            if(j)
               std::cout << ", ";
            else
               std::cout << " ";
            std::cout << "\"" << what.captures(i)[j] << "\"";
         }
         std::cout << " }\n";
      }
   }
   else
   {
      std::cout << "** No Match found **\n";
   }*/
}

int main(int , char* [])
{
   print_captures("(([[:lower:]]+)|([[:upper:]]+))+", "aBBcccDDDDDeeeeeeee aBc dEf gHi");
   print_captures("(.*)bar|(.*)bah", "abcbar");
   print_captures("(.*)bar|(.*)bah", "abcbah");
   print_captures("^(?:(\\w+)|(?>\\W+))*$", "now is the time for all good men to come to the aid of the party");
   return 0;
}

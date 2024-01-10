#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <cassert>
#include <vector>
#include <regex>
/*
 * See readme.md for details.
 * Liao, 1/9/2024
 * */
//--------------parsing the text AST content-----------
struct Record {
    std::string sgClassName;
    std::string fileName;
    int line;
    int column;
};

std::unordered_map<std::string, Record> parseFile(const std::string& fileName) {
    std::unordered_map<std::string, Record> resultMap;

    std::ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return resultMap;
    }

    std::string line;
    int line_count=1;
    while (std::getline(inputFile, line)) {
        size_t arrowPos = line.find("->@");
        if (arrowPos != std::string::npos) {
//            std::string memoryAddress = line.substr(arrowPos + 3);
            std::istringstream iss(line.substr(arrowPos + 3));
            std::string memoryAddress, sgClassName, fileName, lineColumn;
            int lineNum, columnNum;


            //first two fields are mandatory
            if (iss >> memoryAddress >> sgClassName) {
                if (iss >> fileName >> lineColumn){
                    // Split the "line:column" using ':' as the delimiter
                    size_t colonPos = lineColumn.find(":");
                    if (colonPos != std::string::npos) {
                        std::string lineStr = lineColumn.substr(0, colonPos);
                        std::string columnStr = lineColumn.substr(colonPos + 1);

                        // Convert the extracted strings to integers
                        try {
                            // Convert the extracted strings to integers with error handling
                            lineNum = std::stoi(lineStr);
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Failed to convert to integer: " << e.what() << std::endl;
                            continue; // Skip this record and move to the next line
                        } catch (const std::out_of_range& e) {
                            std::cerr << "Out of range error: " << e.what() << std::endl;
                            continue; // Skip this record and move to the next line
                        }

                        try {
                            // Convert the extracted strings to integers with error handling
                            columnNum = std::stoi(columnStr);
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Failed to convert to integer: " << e.what() << std::endl;
                            continue; // Skip this record and move to the next line
                        } catch (const std::out_of_range& e) {
                            std::cerr << "Out of range error: " << e.what() << std::endl;
                            continue; // Skip this record and move to the next line
                        }

                    }
                    else
                    {

                        std::cerr<<"Failed to parse line:colum:" <<lineColumn <<std::endl;
                        assert (false);
                    }
                }
                else // only SgProject and SgFileList , SgSourceFile do not have file line:column
                {
                    if (sgClassName != "SgProject" && sgClassName != "SgFileList"  && sgClassName != "SgSourceFile" && sgClassName !="SgPragma" )
                    {
                       std::regex pattern1("SgType.*");
                       std::regex pattern2("Sg.*Type");
                       if (!((std::regex_match(sgClassName,pattern1) || (std::regex_match(sgClassName,pattern2)))))
                       {
                           std::cerr<<"All node class names need file and line:column fields, excpet for project, filelist, source file, pragma, Types :" << sgClassName <<std::endl;
                           assert (false);
                       }
                    }
                }
                
                Record record;
                record.sgClassName = sgClassName;
                record.fileName = fileName;
                record.line = lineNum;
                record.column = columnNum;
                resultMap[memoryAddress] = record;
            } else {
                std::cerr << "Failed to parse line: " << line << std::endl;
                assert (false);
            }
        } else {
            // this is allowed, some lines are for comments
            size_t comment_pos = line.find ("relative pos=");
            size_t type_section_pos =line.find ("Types encountered ....");
            if  (comment_pos == std::string::npos && type_section_pos  ==std::string::npos)
            {
                std::cerr << "Line does not contain '->@' or 'relative pos=' or 'Types encountered ...': " << line << "@" <<line_count << std::endl;
                assert (false);
            }
        }

      line_count++;
    }

    inputFile.close();
    return resultMap;
}

//------------parsing the gdb backtrace output

struct StackRecord {
    std::string memoryAddress;
    std::string functionName;
    std::vector<std::string> parameters;
    std::string fileName;
    int lineNumber;
};

/*

To handle entries that span multiple lines, we first need to preprocess the input file.
The goal is to remove newline characters (\n) that are not followed by a pattern indicating the start of a new record (like # followed by an integer).
After preprocessing the input to ensure each record is on a single line, we can then process each line as a complete entry.
*/
std::string preprocessFile(const std::string& fileName) {
    std::ifstream file(fileName);
    std::ofstream tempFile(fileName + ".preprocessed.txt");
    std::string line, processedContent;
    std::string currentRecord;

    while (std::getline(file, line)) {
        // Check if line starts with a new record
        if (!line.empty() && line[0] == '#') {
            // Append previous record to processedContent
            if (!currentRecord.empty()) {
                processedContent += currentRecord + '\n';
                tempFile << currentRecord + '\n';  // Write to temp file
                currentRecord.clear();
            }
        }
        // Remove newlines if line doesn't start with a new record
        if (!currentRecord.empty()) {
            currentRecord += " ";
        }
        currentRecord += line;
    }
    // Append the last record
    if (!currentRecord.empty()) {
        processedContent += currentRecord + '\n';
        tempFile << currentRecord + '\n';  // Write to temp file
    }
    tempFile.close();
    return processedContent;
}

std::vector<StackRecord> parseGDBBacktrace(const std::string& content) {
    std::vector<StackRecord> stackRecords;
    std::istringstream stream(content);
    std::string line;

  // one or more space between #ID memory_address
    std::regex entryRegex(R"(#(\d+)\s+0x([0-9a-f]+) in ([^\(]+)\((.*?)\).* at (.*):(\d+))");
    std::smatch matches;

   int line_offset=0; // the index to the result vector
    while (std::getline(stream, line)) {
        if (std::regex_search(line, matches, entryRegex) && matches.size() == 7) {
            StackRecord record;
            record.memoryAddress = matches[2];
            record.functionName = matches[3];

            std::string params = matches[4];
            std::istringstream paramStream(params);
            std::string param;
            while (std::getline(paramStream, param, ',')) {
                // Trim leading and trailing whitespaces if necessary
                param.erase(0, param.find_first_not_of(" \n\r\t"));
                param.erase(param.find_last_not_of(" \n\r\t") + 1);
                record.parameters.push_back(param);
            }

            record.fileName = matches[5];
            record.lineNumber = std::stoi(matches[6]);
            if ( (int) (stackRecords.size())!=line_offset)
            {
               std::cerr << "mismatched line offset and resulting vector size: line_offset="  << line_offset
                   << "\nCurrent vector size before inserting the line at this offset="<< stackRecords.size() <<std::endl;
               std::cerr<<" Relevant line is:"<< line <<std::endl;
               assert (false);
            }
            stackRecords.push_back(record);
        }
        else
        {
          std::cerr<< "Error: cannot parse preprocessed line "<<"@" << (line_offset+1) <<std::endl;
          std::cerr<< line <<std::endl;
          assert (false);
        }
        line_offset++;
    }
    return stackRecords;
}


//---------- main function
// read into text AST file
// read into gdb backtraqce file
// Do the analysis: connecting function parameters with values matching text AST dictionary
int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " text.ast.txt gdb.backtrace.txt" << std::endl;
        return 1;
    }

    const std::string fileName = argv[1];



//    const std::string fileName = "your_input_file.txt";
    std::unordered_map<std::string, Record> resultMap = parseFile(fileName);
#if DEBUG
    for (const auto& pair : resultMap) {
        std::cout << "Memory Address: " << pair.first << std::endl;
        std::cout << "Sg_class_name: " << pair.second.sgClassName << std::endl;
        std::cout << "File Name: " << pair.second.fileName << std::endl;
        std::cout << "Line: " << pair.second.line << std::endl;
        std::cout << "Column: " << pair.second.column << std::endl;
        std::cout << std::endl;
    }
#endif

  //TODO: sanity check of the AST entries


    std::string gdb_fileName = argv[2];
    std::string content = preprocessFile(gdb_fileName);
    auto stackRecords = parseGDBBacktrace(content);

    // Output the parsed data (for demonstration)
    // write to a result file, possibly jason??
    std::ofstream outFile(gdb_fileName + ".analyzed.txt");
    int stack_id=0;
//    for (const auto& record : stackRecords) {
//    We iterate backwards, the stack level with biggest ID is the main entry
    for (auto it = stackRecords.rbegin(); it != stackRecords.rend(); ++it) {
        const auto& record = *it;
        outFile << stack_id <<std::endl;
        outFile << "Memory Address: " << record.memoryAddress << std::endl;
        outFile << "Function: " << record.functionName << std::endl;
        for (const auto& param : record.parameters) {
            outFile << "\t Param: " << param << std::endl;
            size_t equalPos = param.find('=');
            std::string rhs;

			// Check if '=' is found in the string
			if (equalPos != std::string::npos) {
				// Extract substring after '='
				rhs = param.substr(equalPos + 1);
			} else {
				// In case there is no '=', rhs will be the entire param
				//rhs = param;
               std::cerr <<"Error: cannot extract rhs from parameter:"<< param<<std::endl;
			}
           
           if (resultMap.count(rhs))
		   {
              auto second = resultMap[rhs]; 
			   outFile << "\t\t Memory Address: " << rhs; // this should be mem address
			   outFile << " Sg_class_name: " << second.sgClassName ;
			   outFile << " File Name: " << second.fileName ;
			   outFile << " Line: " << second.line;
			   outFile << " Column: " << second.column;
			   outFile << std::endl;
		   }            
        }
        outFile << "File: " << record.fileName << ", Line: " << record.lineNumber << std::endl << std::endl;
        stack_id++;
    }

    outFile.close();

    return 0;
}


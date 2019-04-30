
#include "rose.h"
#include "BinaryConcolic.h"

namespace concolic = Rose::BinaryAnalysis::Concolic;

concolic::SpecimenId 
copyBinaryToDB( concolic::Database::Ptr db, 
                const boost::filesystem::path& executableName, 
                concolic::Update::Flag update = concolic::Update::YES
              )
{
  try 
  {
    concolic::Specimen::Ptr binary = concolic::Specimen::instance(executableName);  
    concolic::SpecimenId    id = db->id(binary, update);
    std::cout << "conc: copying over " << executableName 
              << " new id: " << id.orElse(-1)
              << std::endl;
    return id;
  }
  catch (const SqlDatabase::Exception& e)
  {
    std::cout << "conc: copying over " << executableName 
              << " failed with: " << e.what()
              << std::endl;
  }    
  catch (const std::runtime_error& e)
  {
    std::cout << "conc: copying over " << executableName 
              << " failed with: " << e.what()
              << std::endl;    
  }
  
  return concolic::SpecimenId();
}

int main()
{
  std::string             dburi = "sqlite3://tmp/test.db";
  std::string             dburl = SqlDatabase::Connection::connectionSpecification(dburi, SqlDatabase::SQLITE3);
  
  concolic::Database::Ptr db  = concolic::Database::instance(dburi);
  
  // add new file
  copyBinaryToDB(db, "testBinaryConcolic", concolic::Update::YES);
  copyBinaryToDB(db, "/usr/bin/ls",        concolic::Update::YES);
  copyBinaryToDB(db, "/usr/bin/grep",      concolic::Update::NO );    
  copyBinaryToDB(db, "/usr/bin/grep",      concolic::Update::YES);
  copyBinaryToDB(db, "/usr/bin/xyz",       concolic::Update::YES);
}


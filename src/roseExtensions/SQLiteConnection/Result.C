#include "sage3basic.h"
#include <Result.h>

using namespace std;

// copy constructor
Result::Result(const Result& rS)
{
  this->nRows = rS.nRows;
  this->nCols = rS.nCols;
  this->dataRows.reserve(nRows);
  for (int i = 0; i < nRows; i++)
  {
    dataRows[i].reserve(nCols);
    for(int j = 0; j < nCols; j++)
    {
      Row r = rS.dataRows[i];
      dataRows[i].push_back(r[j]);
    }
  }
}

// constructor
// build an object with the resulting data of a query
Result::Result (char **d, int nRow, int nCol, vector <string> *types)
{
  assert(!(nRow < 0) || !(nCol < 0));
  // dimensions of the result table
  nRows = nRow;
  nCols = nCol;
  // types of the columns, not enforced by sqlite
  if (types)
    colTypes = *types;
  else
    colTypes.clear();
  // first row has the table schema
  headerRow.reserve(nCols);
  for (int i = 0; i < nCols; i++)
  {
    if (d[i])
    {
      string tempS(d[i]);
      headerRow.push_back(tempS);
    }
    else
      headerRow.push_back("");
  }
  // saving the actual data
  dataRows.reserve(nRows);
  for (int i = 0; i < nRows; i++)
  {
    Row tmpv;
    tmpv.reserve(nCols);
    for (int j = 0; j < nCols; j++)
    {
      if (d[(i + 1) * nCols + j])
      {
	string tempS(d[(i + 1) * nCols + j]);
	tmpv.push_back(tempS);
      }
      else
	tmpv.push_back("");
    }
    dataRows.push_back(tmpv);
  }
}

// output the data of the object
void Result::showResult ()
{
  cout << "Header row:\n";
  for (int i = 0; i < nCols; i++)
    cout << "\t" << headerRow[i];
  cout << "\nData:";
  for (int i = 0; i < nRows; i++)
  {
    cout << "\n";
    for (int j = 0; j < nCols; j++)
      cout << "\t" << dataRows[i][j];
  }
  cout << "\n";
}


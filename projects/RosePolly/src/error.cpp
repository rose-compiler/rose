#include <rose.h>

using namespace std;

#include <rosepoly/error.h>


static string errorBuf;
static int errorCount = 1;

void front_error( SgNode * aNode, frontStage stage , const string message, bool rec )
{
	
	if ( errorCount == 1 && !rec )
		return;
	
	Sg_File_Info * info = (aNode != NULL) ? aNode->get_file_info() : NULL;
	string temp;
	
	switch (stage)
	{
		case DATA:
			temp = "In gathering data";
			break;
		case GRAPH:
			temp = "In graph construction";
			break;
		case ACCESSPAT:
			temp = "In identifying access patterns";
			break;
		case EVAL:
			temp = "In evaluating kernel against polyhedral constraints";
			break;
		default:
			temp = "In unknown stage";
	}
	
	if ( info != NULL && info->isTransformation() )
	{
		cout<<endl<<endl<<"ERROR (in tranformation: "<<temp<<") : "<<message<<":"<<errorBuf<<endl<<endl;
	} else {
		cout<<endl<<endl<<"ERROR (frontEnd: "<<temp<<") "<<info->get_filenameString()<<":[line "<<info->get_line()<<" ] : "<<message<<":"<<errorBuf<<endl<<endl;
	}
	
	ROSE_ASSERT(false);
	
}

void report_error( const string message )
{
	stringstream temp;
	
	temp<<endl<<"["<<errorCount<<"]:"<<"[position unavailable]"<<message;
	errorCount++;
	
	errorBuf.insert( 0, temp.str() );
}


void report_error( const string message , SgNode * aNode )
{
	Sg_File_Info * info = (aNode != NULL) ? aNode->get_file_info() : NULL;
	stringstream temp;
	
	if ( info != NULL && info->isTransformation() ){
		temp<<endl<<"["<<errorCount<<"]:"<<"[position unavailable]"<<message;
	} else {
		temp<<endl<<"["<<errorCount<<"]:"<<info->get_filenameString()<<":[line "<<info->get_line()<<" ]"<<message;
	}
	errorCount++;
	
	errorBuf.insert( 0, temp.str() );
	
}

void simple_error( string message )
{
	
	cout<<endl<<endl<<"ERROR: "<<message<<":"<<errorBuf<<endl<<endl;
	
	ROSE_ASSERT(false);
	
}





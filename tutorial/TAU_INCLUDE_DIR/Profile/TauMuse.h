#ifndef _TAU_MUSE_H_
#define _TAU_MUSE_H_

/* The TAU MAGNET/MUSE API */
#if (defined(TAU_MUSE) || defined(TAU_MUSE_EVENT) || defined(TAU_MUSE_MULTIPLE))

#include <Profile/Profiler.h>

#define MAX_ARGLEN 		255
#define MAX_REPLY_LENGTH 	1024 
#define MAXNUMOFCPU		2 
#define MAXNUMOF_HANDLERS	4	
#define MAXNUMOF_FILTERS	4	
#define MAXNUMOF_ADDFILTERS	4	
#define MAX_HANDLER_NAME_LEN	30	
#define MAX_ADDFILTERS_ARGS_LEN	100	
#define MAXNUMOF_COUNTERS	10
#define MAXNUMOF_PACKAGES	10
#define MAXNUMOF_METRICS	100
#define MAX_METRIC_LEN		100

#define AF_UNIX_MODE
#define VAR_LOCK_DIRECTORY 	"/tmp"
//#define VAR_LOCK_DIRECTORY 	"/var/lock"


struct filter_info{
	int filter_argc;		//Number of addfilter command
	char args[MAXNUMOF_ADDFILTERS][MAX_ADDFILTERS_ARGS_LEN];		
					//Actual addfilter command
};

struct metric_info{
	char info[MAX_METRIC_LEN];
};

struct handler_info{
	int handlerID;					//handlerID
	char handler_name[MAX_HANDLER_NAME_LEN];	//handler names
	int numoffilters;				//Number of filters.
	struct filter_info filters[MAXNUMOF_FILTERS];	//filters	
	struct metric_info metrics[MAXNUMOF_METRICS];
	int numofcounters;
};

struct package_info{
	int numofhandlers;			//Number of handlers.
	int totalcounters;
	char package_name[MAX_HANDLER_NAME_LEN];	//handler names
	struct handler_info handlers[MAXNUMOF_HANDLERS];	//handlers
};

struct package_list_info{
	int numofpackages;
	int initialized;
	struct package_info packages[MAXNUMOF_PACKAGES];
};

/* TheMuseSockId() is a global variable now */
int& TheMuseSockId(void);

#if (defined(TAU_MUSE) || defined(TAU_MUSE_MULTIPLE))
struct package_list_info& Mono_PkgList(void);
#endif // (defined(TAU_MUSE) || defined(TAU_MUSE_MULTIPLE))

#ifdef TAU_MUSE_EVENT
struct package_list_info& NonMono_PkgList(void);
#endif //TAU_MUSE_EVENT

/***************************************************************************************
* This is for handlers stuff
* *************************************************************************************/
//=====================================
// TAU_count handler
//=====================================
/*********************
 * Description	: Struct for input data for count.
 * From		: handler/count/count.h
 *********************/
struct count_handler_input_data
{
        int fsize;
        int fname_size;
};

/*********************
 * Description	: Struct for return data for count.
 * From		: handler/count/count.h
 *********************/
struct count_handler_return_data
{
        int count;
};

int CreateTauCountEncode(char *ascii_command, int size, char *binary_command);
double QueryTauCountDecode(const char *binary_command, 
		const char *binary_reply, int size, char *ascii_reply,double data[]);

//=====================================
// process_scheduling handler
//=====================================

struct process_scheduling_handler_input_data{
};

struct cpu_stat{
	double numofcontextswitch;
        unsigned long long time_busy;
        unsigned long long time_sched;
};

struct process_scheduling_handler_return_data{
  unsigned long long total_time;
  unsigned long long cpu_speed;
  unsigned int numofcpu;
  struct cpu_stat stat[MAXNUMOFCPU];
};

int CreateProcessSchedulingEncode(char *ascii_command, int size, char *binary_command);
double QueryProcessSchedulingDecode(const char *binary_command, 
		const char *binary_reply, int size, char *ascii_reply,double data[]);
//=====================================
// bandwidth handler
//=====================================
struct bandwidth_handler_input_data
{
        /* All of these values are assumed to be Big Endian */
        double delta_time;	/*length of time to collect events for each block*/
        int fsize;		/*maximum size for tracefile.  Set to 0 for default size*/
        int fname_size;		/*Number of bytes following this struct in the data stream.
                                  These bytes are a null terminated ASCII stream representing
				  the tracefile name.  Set to zero for no tracefile*/

};

struct bandwidth_handler_return_data
{
        /* All of these values are assumed to be Big Endian*/
        double send_average_bandwidth;
        double recv_average_bandwidth;
        double send_recent_bandwidth;
        double recv_recent_bandwidth;
        double block_start_time;
        int block_id;
        int lost_events;

};

int CreateBandwidthEncode(char *ascii_command, int size, char *binary_command);
double QueryBandwidthDecode(const char *binary_command, 
		const char *binary_reply, int size, char *ascii_reply,double data[]);

//=====================================
// tcpbandwidth handler
//=====================================
struct tcpbandwidth_handler_input_data
{
        /* All of these values are assumed to be Big Endian */
        double delta_time;	/*length of time to collect events for each block*/
        int fsize;		/*maximum size for tracefile.  Set to 0 for default size*/
        int fname_size;		/*Number of bytes following this struct in the data stream.
                                  These bytes are a null terminated ASCII stream representing
				  the tracefile name.  Set to zero for no tracefile*/

};

struct tcpbandwidth_handler_return_data
{
        /* All of these values are assumed to be Big Endian*/
        double send_average_bandwidth;
        double recv_average_bandwidth;
        double send_recent_bandwidth;
        double recv_recent_bandwidth;
        double block_start_time;
        int block_id;
        int lost_events;

};

int CreateTcpBandwidthEncode(char *ascii_command, int size, char *binary_command);
double QueryTcpBandwidthDecode(const char *binary_command, 
		const char *binary_reply, int size, char *ascii_reply,double data[]);


//=====================================
// accumulator handler
//=====================================
struct accumulator_handler_input_data
{
};

struct accumulator_handler_return_data
{
        unsigned long long sum;
};

int CreateAccumulatorEncode(char *ascii_command, int size, char *binary_command);
double QueryAccumulatorDecode(const char *binary_command, 
		const char *binary_reply, int size, char *ascii_reply,double data[]);

/***************************************************************************************
* This is for filters stuff
* *************************************************************************************/
int AddFilterProcessFilterEncode(char *ascii_command, int size, char *binary_command);
int AddFilterSocketFilterEncode(char *ascii_command, int size, char *binary_command);

/***************************************************************************************
* This is for Packages stuff
* *************************************************************************************/
int create_encode_selector(char *handler_name,char *ascii_command,int size, char *binary_command);
double query_decode_selector(char *handler_name, const char *binary_command, 
		const char *binary_reply, int size, char *ascii_reply, double data[]);
int addfilter_encode_selector(char *filter_name,char *ascii_command,int size, char *binary_command);
int monotonic_package_selector(struct package_info *pkg);
int nonmonotonic_package_selector(struct package_info *pkg);
//int report_user_defined_events(double data[]);

/***************************************************************************************
* This is for TauMuse stuff
* *************************************************************************************/
int TauMuseInit(void);
int TauMuseCreate(struct package_info *pkg);

#ifdef TAU_MUSE
double TauMuseQuery(void);
#endif //TAU_MUSE

#ifdef TAU_MUSE_EVENT
int TauMuseEventQuery(double data[],int size);
int TauMuseGetMetricsNonMono(char *data[],int size);
//int TauMuseGetSizeNonMono(void);
#endif //TAU_MUSE_EVENT

#ifdef TAU_MUSE_MULTIPLE
int TauMuseMultipleQuery(double data[],int size);
int TauMuseGetMetricsMono(char *data[],int size);
//int TauMuseGetSizeMono(void);
#endif //TAU_MUSE_MULTIPLE

void TauMuseDestroy(void);
#endif /* (defined(TAU_MUSE) || defined(TAU_MUSE_EVENT) || defined(TAU_MUSE_MULTIPLE))*/ 

#endif /* _TAU_MUSE_H_ */

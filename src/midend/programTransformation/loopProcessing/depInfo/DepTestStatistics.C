#include <sstream>
#include <fstream>
#include <iostream>
#include <string.h>
#include <CommandOptions.h>
#include <DepTestStatistics.h>
#include <PlatoOmegaInterface.h>

DepTestStatistics DepStats;

using namespace PlatoOmegaInterface;

int DepTestStatistics::AddProblem(int p)
{
	_total_problems += p;
	return _total_problems;
}

int DepTestStatistics::AddProcessedAdhoc(int p)
{
	_num_processed_adhoc++;
	return _num_processed_adhoc;
}

int DepTestStatistics::AddProcessedPlato(int p)
{
	_num_processed_plato++;
	return _num_processed_plato;
}

int DepTestStatistics::AddProcessedOmega(int p)
{
	_num_processed_omega++;
	return _num_processed_omega;
}

void DepTestStatistics::GetProcessed(int *a, int *p, int *o)
{
	*a = _num_processed_adhoc;
	*p = _num_processed_plato;
	*o = _num_processed_omega;
}

void DepTestStatistics::SetFileName(const std::string fname)
{
	_filename = fname;
}

bool DepTestStatistics::CompareDepTests(unsigned int c, DepInfo a, DepInfo p, DepInfo o)
{
	bool isDiff = false;
	int last;

	AddProblem(1);
	if (a == 0 || p == 0 || o == 0)  {
		return false;
	}
	//std::cerr << "rows " << a.rows() << " cols " << a.cols() << std::endl;

	switch (c)
	{
		case ADHOC | PLATO :
		{
			for (int i = 0; i < a.rows() && i < p.rows(); i++)
			{
				for (int j = 0; j < a.cols() && j < p.cols(); j++)
				{
					if (a.Entry( i, j) != p.Entry( i, j))
					{
						isDiff = true;
					}
				}
			}
		}
		break;
		case ADHOC | OMEGA :
		{
			for (int i = 0; i < a.rows() && i < o.rows(); i++)
			{
				for (int j = 0; j < a.cols() && j < o.cols(); j++)
				{
					if (a.Entry( i, j) != o.Entry( i, j))
					{
						isDiff = true;
					}
				}
			}
		}
		break;
		case PLATO | OMEGA :
		{
			for (int i = 0; i < p.rows() && i < o.rows(); i++)
			{
				for (int j = 0; j < p.cols() && j < o.cols(); j++)
				{
					if (p.Entry( i, j) != o.Entry( i, j))
					{
						isDiff = true;
					}
				}
			}
		}
		break;
		case ADHOC | PLATO | OMEGA :
		{
			int tempDim = (a.rows() >= a.cols()) ? a.rows() : a.cols();
			int commLevel = (a.rows() <= a.cols()) ? a.rows() : a.cols();
			for (int i = 0; i < commLevel; i++)
			{				
				AddAdhocDV(RoseToPlatoDV(a.Entry( i, i)));
				AddPlatoDV(RoseToPlatoDV(p.Entry( i, i)));
				AddOmegaDV(RoseToPlatoDV(o.Entry( i, i)));
				if (a.Entry( i, i) != p.Entry( i, i) && a.Entry( i, i) != o.Entry( i, i))
					isDiff = true;				
			}
		}
		break;
	}
	return isDiff;
}

unsigned int DepTestStatistics::RoseToPlatoDV(const DepRel dr)
{
	unsigned int dv = 0;
	DepDirType _dv = dr.GetDirType();
	switch (_dv)
	{
		case DEPDIR_EQ :
		{
			if (dr.GetMinAlign() < 0) {
				dv = DDLES;
			}
			else if (dr.GetMaxAlign() > 0)
			{
				dv = DDGRT;
			}
			else
			{
				dv = DDEQU;
			}
			//std::cerr << "eq " << dr.GetMinAlign() << " " << dr.GetMaxAlign() << " " << dr.toString() << std::endl;
		}
		break;
		case DEPDIR_LE :
		{
			dv = DDLES;
		}
		break;
		case DEPDIR_GE :
		{
			dv = DDGRT;
		}
		break;
		case DEPDIR_ALL :
		{
			dv = DDALL;
		}
		break;
		case DEPDIR_NONE :
		{
			dv = 0;
		}
		break;
	}
	return dv;
}

DepDirType DepTestStatistics::PlatoToRoseDV(unsigned int _dv)
{
	DepDirType dv;
	
	switch (_dv)
	{
		case DDEQU :
		{
			dv = DEPDIR_EQ;
		}
		break;
		case DDLES :
		{
			dv = DEPDIR_LE;
		}
		break;
		case DDGRT :
		{
			dv = DEPDIR_GE;
		}
		break;
		case DDALL :
		{
			dv = DEPDIR_ALL;
		}
		break;
		case 0 :
		{
			dv = DEPDIR_NONE;
		}
		break;
	}
	return dv;
}

void DepTestStatistics::GetDiffs(int *ap, int *ao, int *po)
{
	*ap = _num_diffs_adhoc_plato;
	*ao = _num_diffs_adhoc_omega;
	*po = _num_diffs_plato_omega;
}

void DepTestStatistics::GetTimes(double *a, double *p, double *o)
{
	*a = _total_time_adhoc;
	*p = _total_time_plato;
	*o = _total_time_omega;
}

double DepTestStatistics::AddAdhocTime(double t0, double t1)
{
	_total_time_adhoc += (t1 - t0);
	return _total_time_adhoc;
}

double DepTestStatistics::AddPlatoTime(double t0, double t1)
{
	_total_time_plato += (t1 - t0);
	return _total_time_plato;
}

double DepTestStatistics::AddOmegaTime(double t0, double t1)
{
	_total_time_omega += (t1 - t0);
	return _total_time_omega;
}

int DepTestStatistics::AddAdhocDV(unsigned int dv)
{
	switch (dv)
	{
		case DDLES :
		{
			_num_less_than_dvs_adhoc++;
		}
		break;
		case DDLES | DDEQU :
		{
			_num_less_than_dvs_adhoc++;
			_num_equal_dvs_adhoc++;
		}
		break;
		case DDEQU :
		{
			_num_equal_dvs_adhoc++;
		}
		break;
		case DDGRT :
		{
			_num_greater_than_dvs_adhoc++;
		}
		break;
		case DDGRT | DDEQU :
		{
			_num_greater_than_dvs_adhoc++;
			_num_equal_dvs_adhoc++;
		}
		break;
		case DDLES | DDGRT : //Don't know if this can happen
		{
			_num_less_than_dvs_adhoc++;
			_num_greater_than_dvs_adhoc++;
		}
		break;
		case DDALL :
		{
			_num_star_dvs_adhoc++;
		}
		break;
		case 0 :
		{
			_num_no_dep_adhoc++;
		}
		break;
	}
}

int DepTestStatistics::AddOmegaDV(unsigned int dv)
{
	switch (dv)
	{
		case DDLES :
		{
			_num_less_than_dvs_omega++;
		}
		break;
		case DDLES | DDEQU :
		{
			_num_less_than_dvs_omega++;
			_num_equal_dvs_omega++;
		}
		break;
		case DDEQU :
		{
			_num_equal_dvs_omega++;
		}
		break;
		case DDGRT :
		{
			_num_greater_than_dvs_omega++;
		}
		break;
		case DDGRT | DDEQU :
		{
			_num_greater_than_dvs_omega++;
			_num_equal_dvs_omega++;
		}
		break;
		case DDLES | DDGRT : //Don't know if this can happen
		{
			_num_less_than_dvs_omega++;
			_num_greater_than_dvs_omega++;
		}
		break;
		case DDALL :
		{
			_num_star_dvs_omega++;
		}
		break;
		case 0 :
		{
			_num_no_dep_omega++;
		}
		break;
	}
}

int DepTestStatistics::AddPlatoDV(unsigned int dv)
{
	switch (dv)
	{
		case DDLES :
		{
			_num_less_than_dvs_plato++;
		}
		break;
		case DDLES | DDEQU :
		{
			_num_less_than_dvs_plato++;
			_num_equal_dvs_plato++;
		}
		break;
		case DDEQU :
		{
			_num_equal_dvs_plato++;
		}
		break;
		case DDGRT :
		{
			_num_greater_than_dvs_plato++;
		}
		break;
		case DDGRT | DDEQU :
		{
			_num_greater_than_dvs_plato++;
			_num_equal_dvs_plato++;
		}
		break;
		case DDLES | DDGRT : //Don't know if this can happen
		{
			_num_less_than_dvs_plato++;
			_num_greater_than_dvs_plato++;
		}
		break;
		case DDALL :
		{
			_num_star_dvs_plato++;
		}
		break;
		case 0 :
		{
			_num_no_dep_plato++;
		}
		break;
	}
}

void DepTestStatistics::GetEqualDVs(unsigned int *a, unsigned int *p, unsigned int *o)
{
	*a = _num_equal_dvs_adhoc;
	*p = _num_equal_dvs_plato;
	*o = _num_equal_dvs_omega;
}

void DepTestStatistics::GetLessThanDVs(unsigned int *a, unsigned int *p, unsigned int *o)
{
	*a = _num_less_than_dvs_adhoc;
	*p = _num_less_than_dvs_plato;
	*o = _num_less_than_dvs_omega;
}

void DepTestStatistics::GetGreaterThanDVs(unsigned int *a, unsigned int *p, unsigned int *o)
{
	*a = _num_greater_than_dvs_adhoc;
	*p = _num_greater_than_dvs_plato;
	*o = _num_greater_than_dvs_omega;
}

void DepTestStatistics::GetStarDVs(unsigned int *a, unsigned int *p, unsigned int *o)
{
	*a = _num_star_dvs_adhoc;
	*p = _num_star_dvs_plato;
	*o = _num_star_dvs_omega;
}

void DepTestStatistics::InitAdhocTime(void)
{
	_adhoc_t0 = GetTime();
}

void DepTestStatistics::InitPlatoTime(void)
{
	_plato_t0 = GetTime();
}

void DepTestStatistics::InitOmegaTime(void)
{
	_omega_t0 = GetTime();
}

double DepTestStatistics::SetAdhocTime(void)
{
	double diff;
	diff = (GetTime() - _adhoc_t0);
	_total_time_adhoc += diff;
	return diff;
}

double DepTestStatistics::SetPlatoTime(void)
{
	double diff;
	diff = (GetTime() - _plato_t0);
	_total_time_plato += diff;
	return diff;
}

double DepTestStatistics::SetOmegaTime(void)
{
	double diff;
	diff = (GetTime() - _omega_t0);
	_total_time_omega += diff;
	return diff;
}

double DepTestStatistics::GetTime(void)
{
	struct rusage ruse;
	getrusage(RUSAGE_SELF, &ruse);
	return( (double)(ruse.ru_utime.tv_sec+ruse.ru_utime.tv_usec*1.0e-6) );
}

void DepTestStatistics::SetDepChoice(unsigned int dep_test_choice)
{
	_dep_test_choice = dep_test_choice;
}

void DepTestStatistics::PrintResults(void)
{
	std::string fname;
	fname = "roseResults";
	std::stringstream buffer;
	switch(_dep_test_choice)
	{
		case ADHOC | PLATO | OMEGA :
		{
			//buffer << "\n#Filename: " << _filename << std::endl;
			//buffer << "#\t======\t";
			//buffer << "<<<<<<\t";
			//buffer << ">>>>>>\t";
			//buffer << "******\t";
			//buffer << "--Total Probs--\t";
			//buffer << "--Total Time--" << std::endl;
			buffer << "Adhoc\t";
			buffer << _num_equal_dvs_adhoc;
			buffer << "\t" << _num_less_than_dvs_adhoc;
			buffer << "\t" << _num_greater_than_dvs_adhoc;
			buffer << "\t" << _num_star_dvs_adhoc;
			buffer << "\t" << _total_problems;
			buffer << "\t\t" << _total_time_adhoc << std::endl;
			buffer << "Plato\t";
			buffer << _num_equal_dvs_plato;
			buffer << "\t" << _num_less_than_dvs_plato;
			buffer << "\t" << _num_greater_than_dvs_plato;
			buffer << "\t" << _num_star_dvs_plato;
			buffer << "\t" << _total_problems;
			buffer << "\t\t" << _total_time_plato << std::endl;
			buffer << "Omega\t";
			buffer << _num_equal_dvs_omega;
			buffer << "\t" << _num_less_than_dvs_omega;
			buffer << "\t" << _num_greater_than_dvs_omega;
			buffer << "\t" << _num_star_dvs_omega;
			buffer << "\t" << _total_problems;
			buffer << "\t\t" << _total_time_omega << std::endl;
		}
		break;
		default :
			std::cerr << "defaulted" << std::endl;
			break;
	}
   if (CmdOptions::GetInstance()->HasOption("-depAnalOnlyPrintF"))
   {
      std::fstream outFile;
      outFile.open(fname.c_str(),std::fstream::out | std::fstream::app);
      if (outFile) {
         outFile << buffer.str();
         outFile.close();
      }
   }
   else if (CmdOptions::GetInstance()->HasOption("-depAnalOnlyPrintS"))
   {
      std::cerr << buffer.str();
   }
}

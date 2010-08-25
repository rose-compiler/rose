#ifndef DEP_TEST_STATISTICS_H
#define DEP_TEST_STATISTICS_H

#include <DepRel.h>
#include <DDTypes.h>
#include <DepInfo.h>
#include <sys/time.h>
#include <sys/resource.h>

class DepTestStatistics
{
        private:
                int _total_problems;
                int _num_diffs_adhoc_plato;
                int _num_diffs_adhoc_omega;
                int _num_diffs_plato_omega;
                int _num_processed_adhoc;
                int _num_processed_plato;
                int _num_processed_omega;
                int _num_no_dep_adhoc;
                int _num_equal_dvs_adhoc;
                int _num_less_than_dvs_adhoc;
                int _num_greater_than_dvs_adhoc;
                int _num_star_dvs_adhoc;
                int _num_no_dep_plato;
                int _num_equal_dvs_plato;
                int _num_less_than_dvs_plato;
                int _num_greater_than_dvs_plato;
                int _num_star_dvs_plato;
                int _num_no_dep_omega;
                int _num_equal_dvs_omega;
                int _num_less_than_dvs_omega;
                int _num_greater_than_dvs_omega;
                int _num_star_dvs_omega;
                double _total_time_adhoc;
                double _total_time_plato;
                double _total_time_omega;
                double _adhoc_t0;
                double _plato_t0;
                double _omega_t0;
                std::string _filename;
                unsigned int _dep_test_choice;

        public:
                DepTestStatistics() : _total_problems(0),
                                      _num_diffs_adhoc_plato(0),
                                      _num_diffs_adhoc_omega(0),
                                      _num_diffs_plato_omega(0),
                                      _num_processed_adhoc(0),
                                      _num_processed_plato(0),
                                      _num_processed_omega(0),
                                      _num_no_dep_adhoc(0),
                                      _num_equal_dvs_adhoc(0),
                                      _num_less_than_dvs_adhoc(0),
                                      _num_greater_than_dvs_adhoc(0),
                                      _num_star_dvs_adhoc(0),
                                      _num_no_dep_plato(0),
                                      _num_equal_dvs_plato(0),
                                      _num_less_than_dvs_plato(0),
                                      _num_greater_than_dvs_plato(0),
                                      _num_star_dvs_plato(0),
                                      _num_no_dep_omega(0),
                                      _num_equal_dvs_omega(0),
                                      _num_less_than_dvs_omega(0),
                                      _num_greater_than_dvs_omega(0),
                                      _num_star_dvs_omega(0),
                                      _total_time_adhoc(0),
                                      _total_time_plato(0),
                                      _total_time_omega(0),
                                      _dep_test_choice(0) {};


                int AddProblem(int p);
                int AddProcessedAdhoc(int p);
                int AddProcessedPlato(int p);
                int AddProcessedOmega(int p);
                void GetProcessed(int *a, int *p, int *o);
                void SetFileName(const std::string fname);
                bool CompareDepTests(unsigned int c, DepInfo a, DepInfo p, DepInfo o);
                unsigned int RoseToPlatoDV(const DepRel dr);
                DepDirType PlatoToRoseDV(unsigned int _dv);
                void GetDiffs(int *ap, int *ao, int *po);
                void GetTimes(double *a, double *p, double *o);
                double AddAdhocTime(double t0, double t1);
                double AddPlatoTime(double t0, double t1);
                double AddOmegaTime(double t0, double t1);
                int AddAdhocDV(unsigned int dv);
                int AddOmegaDV(unsigned int dv);
                int AddPlatoDV(unsigned int dv);
                void GetEqualDVs(unsigned int *a, unsigned int *p, unsigned int *o);
                void GetLessThanDVs(unsigned int *a, unsigned int *p, unsigned int *o);
                void GetGreaterThanDVs(unsigned int *a, unsigned int *p, unsigned int *o);
                void GetStarDVs(unsigned int *a, unsigned int *p, unsigned int *o);
                void InitAdhocTime(void);
                void InitPlatoTime(void);
                void InitOmegaTime(void);
                double SetAdhocTime(void);
                double SetPlatoTime(void);
                double SetOmegaTime(void);
                double GetTime(void);
                void SetDepChoice(unsigned int dep_test_choice);
                void PrintResults(void);
};

#endif

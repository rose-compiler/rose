// An example using member access within if-clause
// Extracted from QuickSilver
//

struct SimulationParameters
{
  int mpiThreadMultiple;
};

struct Parameters
{
   SimulationParameters simulationParams;
};

class MC_Processor_Info
{
public:
    int num_tasks;
};

class MonteCarlo
{
public:

   MonteCarlo(const Parameters& params);
   ~MonteCarlo();

public:
    Parameters _params;
    MC_Processor_Info *processor_info;
};

extern int Receive_Particle_Buffer(int particle_vault_task_num); 

void foo (MonteCarlo * mcco)
{
  int num_received_buffers[mcco->processor_info->num_tasks];
#pragma omp parallel for schedule (static) if (mcco->_params.simulationParams.mpiThreadMultiple == 1)
  for ( int task_index = 0; task_index < mcco->processor_info->num_tasks; task_index++ )
  {
    num_received_buffers[task_index] = Receive_Particle_Buffer(task_index);
  }
}


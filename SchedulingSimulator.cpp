#include <stdio.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <list>
#include <atomic>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <random>

using namespace std;

#pragma pack(1) //1 byte packing
typedef struct _PCB {
    char priority;
    char name[32];
    int processID;
    char activity_status;
    int cpu_burst_time;
    int base_register; // lowest address
    uint64_t limit_register; // number of bytes allocated
    char process_type;
    int number_of_files;
}PCB;
typedef struct _Core {
    int num_core_procs;
    int num_finished_procs;
    int* process_index_list; // this contains the ordering

}Core;
class Processors {
    int num_cores;
    Core* cores;
    int num_procs;
    int num_procs_remaining;
    PCB* processes;
    bool* completed_procs;
    bool processes_remaining;
    int* algorithms_used;
public:
    Processors(int* algos, double* percent_allocated, int num_cores, PCB* processes, int num_procs)
    {
        this->num_cores = num_cores;
        this->num_procs = num_procs;
        this->num_procs_remaining = num_procs;
        this->processes = processes;
        this->completed_procs = (bool*)malloc(sizeof(bool) * num_procs);
        this->processes_remaining = true;
        this->algorithms_used = algos;
        distribute_cores(percent_allocated);
    }

    PCB& get_next_process(int core_index)
    {
        //int algo = algorithms_used[core_index];
        // get the corresponding index from the core's listing
        int core_list_index = cores[core_index].num_finished_procs;
        if (core_list_index >= cores[core_index].num_core_procs) // if the core has run out of processes
        {
            return nullptr; // return nullptr;
        }
        int proc_index = cores[core_index].process_index_list[core_list_index];
        cores[core_index].num_finished_procs++;
        this->num_procs_remaining--;
        completed_procs[proc_index] = true;
        return processes[proc_index];

    }
    void sort_core(int coreindex)
    {
        int algo = algorithms_used[coreindex];
        if (algo == 1)
        {
            sort_FCFS(coreindex);
        }
        else if (algo == 2)
        {
            sort_round_robin(coreindex);
        }
        else if (algo == 3)
        {
            sort_shortest(coreindex);
        }
        else if (algo == 4)
        {
            sort_priority(coreindex);
        }
    }
    void sort_priority(int coreindex)
    {

    }
    
    void sort_shortest(int coreindex)
    {

    }

    void sort_round_robin(int coreindex)
    {

    }

    void sort_FCFS(int coreindex)
    {

    }
    //order is chosen on core allocation
    void distribute_cores(double* percents)
    {
        // get distributions for each core
        int dist_total = 0;
        for (int i = 0; i < this->num_cores; i++)
        {
            cores[i].num_finished_procs = 0;
            cores[i].num_core_procs = percents[i] * num_procs_remaining;
            dist_total += cores[i].num_core_procs;
        }
        while (dist_total < num_procs_remaining)
        {
            cores[dist_total % num_cores].num_core_procs++;
            dist_total++;
        }
        //now distribute processes accordingly
        int proc_pointer = 0;
        for (int i = 0; i < num_cores; i++)
        {
            cores[i].process_index_list = (int*)malloc(sizeof(int) * cores[i].num_core_procs);
            for (int j = 0; j < cores[i].num_core_procs && proc_pointer < this->num_procs; j++, proc_pointer++)
            {
                if (!completed_procs[proc_pointer])
                {
                    cores[i].process_index_list[j] = proc_pointer;
                }
                else 
                {
                    j--;
                }
            }
            // sort core according to algorithm
            sort_core(i);
        }
    }
    void distribute_cores()
    {
        double percents[num_cores];
        double equal_percent = (1.0 / ((double)num_cores));
        memset(percents, equal_percent, sizeof(double) * num_cores); // this should be correct
        distribute_cores(percents);
    }

};

void* manage_cores(void* arg)
{
    return NULL;
}

void* run_processor(void* arg)
{
   
    return NULL;
}

int main(int argc, char** argv)
{
   
   
}
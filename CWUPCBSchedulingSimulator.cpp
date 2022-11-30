//Milo McCarty
// 11/30/2022
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
#include <cstring>
#include <stdexcept>
#include <tuple>

#define PRIORITY_TIME_QUANTUM 5.0

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
        this->cores = (Core*)malloc(sizeof(Core)* num_cores);
        this->num_procs = num_procs;
        this->num_procs_remaining = num_procs;
        this->processes = processes;
        this->completed_procs = (bool*)malloc(sizeof(bool) * num_procs);
        this->processes_remaining = true;
        this->algorithms_used = algos;
        distribute_cores(percent_allocated);
    }
    Processors()
    {

    }
    bool proc_has_remaining_processes()
    {
        return this->processes_remaining;
    }
    bool has_remaining_processes(int core_index)
    {
        bool result = cores[core_index].num_core_procs > cores[core_index].num_finished_procs;
        
        return result;
    }
    // gets the number of processes total remaining
    int get_num_process()
    {
        return this->num_procs_remaining;
    }
    int get_num_cores()
    {
        return this->num_cores;
    }
    PCB& get_next_process(int core_index)
    {
        
        //int algo = algorithms_used[core_index];
        // get the corresponding index from the core's listing
        int core_list_index = cores[core_index].num_finished_procs;
        int proc_index = cores[core_index].process_index_list[core_list_index];
        cores[core_index].num_finished_procs++;
        this->num_procs_remaining--;
        this->processes_remaining = num_procs_remaining > 0;
        completed_procs[proc_index] = true;
        processes_remaining = this->num_procs_remaining > 0;
        return processes[proc_index];

    }
    // sortation by algorithm selected for the core coreindex
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
        else
        {
            throw std::exception();
        }
    }
    // bubble sort by priority
    void sort_priority(int coreindex)
    {
        for (int i = 0; i < cores[coreindex].num_core_procs ; i++)
        {
            for (int j = 0; j < cores[coreindex].num_core_procs ; j++)
            {
                char priorityl = processes[cores[coreindex].process_index_list[i]].priority;
                char priorityr = processes[cores[coreindex].process_index_list[j]].priority;
                if (priorityr > priorityl)
                {
                    swap(cores[coreindex].process_index_list[i], cores[coreindex].process_index_list[j]);
                    //i--;
                    j;
                }
            }
        }
    }
    // bubble sort by cpu burst time
    void sort_shortest(int coreindex)
    {
        for (int i = 0; i < cores[coreindex].num_core_procs; i++)
        {
            for (int j = 0; j < cores[coreindex].num_core_procs; j++)
            {
                int priorityl = processes[cores[coreindex].process_index_list[i]].cpu_burst_time;
                int priorityr = processes[cores[coreindex].process_index_list[j]].cpu_burst_time;
                if (priorityr > priorityl)
                {
                    swap(cores[coreindex].process_index_list[i], cores[coreindex].process_index_list[j]);
                }
            }
        }
    }
    //random "sort"
    void sort_round_robin(int coreindex)
    {
        //ever process is equal. cpu time is shared among all processes.
        for (int i = 0; i < cores[coreindex].num_core_procs; i++)
        {
            int index = rand() % cores[coreindex].num_core_procs;
            swap(cores[coreindex].process_index_list[i], cores[coreindex].process_index_list[index]);
        }
    }

    void sort_FCFS(int coreindex)
    {
        // no sorting necessary
    }
    //update priority of all active processes
    void time_quantum()
    {
        for (int i = 0; i < num_procs; i++)
        {
            // if the process is not already completed or is not already at minimum
            if (!this->completed_procs[i] && processes[i].priority!=0)
            {
                //decrement priority
                processes[i].priority--;
            }
        }
    }
    //order is chosen on core allocation
    void distribute_cores(double* percents)
    {
        cout << "distributing processes among all the cores" << endl;
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

//global variables 
Processors proc;
pthread_mutex_t lock;

void* manage_cores(void* arg)
{
    int num_cores = (int)(long)(arg);

    clock_t begin_time_quantum = clock();
    while (proc.proc_has_remaining_processes())
    {
        float seconds_elapsed = (float) (clock() - begin_time_quantum ) / CLOCKS_PER_SEC;
        if (seconds_elapsed > PRIORITY_TIME_QUANTUM)
        {
            proc.time_quantum();
            begin_time_quantum = clock();
            cout << "time quantum reached" << endl;
        }
        
        //proc.distribute_cores();
    }
    return (void*)NULL;
}

void* run_processor(void* arg)
{  
    int core_id = (int)(long)(arg);
    cout << "launching core " << core_id << endl;
    
    while (proc.proc_has_remaining_processes())
    {
        int waittime = 0;
        if (proc.has_remaining_processes(core_id))
        {
            PCB pcb_task = proc.get_next_process(core_id);
            waittime = pcb_task.cpu_burst_time;
            pcb_task.cpu_burst_time = 0;
            cout << "running " << pcb_task.name << " on core " << core_id << " with priority " << (int)pcb_task.priority << endl;
            this_thread::sleep_for(chrono::milliseconds(waittime));
            cout << "task finished after " << waittime << "ms" << endl;
        }
        else if (proc.get_num_process() >= proc.get_num_cores())
        {
            proc.distribute_cores();
        }
        
        
    }
    return (long)NULL;
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    if (argc < 4 || argc % 2 == 1)
    {
        cout << "Improper minimum arguments met for core allocation." << endl;
        return -1;
    }
    string process_file = argv[1];
    cout << process_file << endl;
    int num_cores = (argc / 2) - 1;
    double total_percentage = 0.0;
    double* percentages = (double*)malloc(sizeof(double) * num_cores);
    int* selection_algorithms = (int*)malloc(sizeof(int) * num_cores);
    for (int i = 2; i < argc; i += 2)
    {
        cout << argv[i] << " " << argv[i + 1] << endl;
        int selection = stoi(argv[i]);
        double percentage = stod(argv[i + 1]);
        total_percentage += percentage;
        int core_index = (i / 2) - 1;
        percentages[core_index] = percentage;
        selection_algorithms[core_index] = selection;
    }

    if (total_percentage != 1.0)
    {
        cout << "Incorrect percentages given" << endl;
        return -1;
    }

    ifstream readfile(process_file);

    if (!readfile)
    {
        cout << "an error has occured reading " << process_file << endl;
        return -1;
    }

    readfile.seekg(0, ios::end);
    int file_size = readfile.tellg();
    int total_num_processes = file_size / sizeof(PCB);
    readfile.seekg(0, ios::beg);
    if (file_size % sizeof(PCB))
    {
        cout << "Binary file size mismatch " << process_file << endl;
        return -1;
    }
    PCB processes[total_num_processes];
    int total_files_process = 0;
    uint64_t total_mem_alloc = 0;
    for (int i = 0; i < total_num_processes; i++)
    {
        readfile.read((char*)&processes[i], sizeof(processes));
        total_files_process += processes[i].number_of_files;
        total_mem_alloc += processes[i].limit_register;
    }
    cout << "Total amount of memory allocated by all processes: " << total_mem_alloc << endl;
    cout << "Total number of processes: " << total_num_processes << endl;
    cout << "Total number of files over all processes: " << total_files_process << endl;
    proc = Processors(selection_algorithms, percentages, num_cores, processes, total_num_processes);

    pthread_t core_tid[num_cores];
    pthread_t supervisor_tid;
    for (int i = 0; i < num_cores; i++)
    {
        void* arg = (void*)(long)(i);
        pthread_create(&core_tid[i], NULL, run_processor, arg);
    }
    pthread_create(&supervisor_tid, NULL, manage_cores, &proc);
    //pthread_join(supervisor_tid, NULL);
    //flush out remaining processes still underway
    for (int i = 0; i < num_cores; i++)
    {
        pthread_join(core_tid[i], NULL);
    }
}
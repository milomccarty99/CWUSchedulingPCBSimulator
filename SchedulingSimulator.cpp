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

int num_cores;
bool processes_remain;
atomic_int semaphore_counter;

#pragma pack(1) //1 byte packing
typedef struct _PCB {
    char priority;
    char name[32];
    int processID;
    char activity_status;
    int cpu_burst_time;
    int base_register;
    uint64_t limit_register;
    char process_type;
    int number_of_files;
}PCB;

class Processor {
    list<PCB> processes;
    int selection = 0;
    public:
    Processor(int selection)
    {
        this->selection = selection;
        
    }
    void add_process(PCB process)
    {
        if(selection == 1)
        {
            processes.push_back(process);
        }
        else
        {
            processes.push_back(process);
        }
    }
    PCB* get_next_process()
    {
        PCB* result = new PCB(processes.front());
        //if(result != nullptr)
        {
            processes.pop_front();
        }
        return result;
    }

};
void distribute_processes(Processor* cores, double* percentages, PCB* processes, int num_processes)
{
    int process_counter = 0;
    for(int i = 0; i < num_cores; i++)
    {
        int amount_distribute = percentages[i]*num_processes;
        for(int j = 0; j < amount_distribute; j++, process_counter++)
        {
            cores[i].add_process(processes[process_counter]);
        }
    }
    while (process_counter < num_processes)
    {
        cores[process_counter%num_cores].add_process(processes[process_counter]);
        process_counter++;
    }

    //return NULL;
}
void* manage_cores(void* arg)
{
    Processor* cores = (Processor*) arg;
    while(processes_remain)
    {
        
    }
    

    return NULL;
}

void* run_processor(void* arg)
{
    Processor core = ((Processor*)arg)[0];
    

    while(processes_remain)
    {
        while(semaphore_counter>0)
            ;
        PCB* working_proc = core.get_next_process();
        &working_proc->activity_status;
    }
    return NULL;
}

int main(int argc, char** argv)
{
    processes_remain = true;
    if(argc < 4 || argc %2 == 1)
    {
        // if there is less than 1 core or if there is an incomplete core
        cout << "Improper minimum arguments met for core." << endl;
        return -1;
    }
    string process_file = argv[1];
    //cout << process_file << endl;

    num_cores = (argc / 2) - 1;
    double total_percentage = 0.0;
    double* percentages = (double*) malloc(sizeof(double) * num_cores);
    int* selections = (int*) malloc(sizeof(int)*num_cores);
    for(int i = 2; i < argc; i+=2)
    {
        //cout << argv[i] << " " << argv[i+1] << endl;
        int selection = stoi(argv[i]);
        selections[i] = selection;
        double percentage = stod(argv[i+1]);
        total_percentage += percentage;
        percentages[(i/2)-1] = percentage;
    }

    if(total_percentage != 1.0)
    {
        cout << "Incorrect percentages given" << endl;
        return -1;
    }
    
    //cout << "size of pcb: " << sizeof(PCB) << endl;
    ifstream readFile (process_file);

    if(!readFile)
    {
        cout<<"an error has occurred in reading the file" << endl;
        return -1;
    }
    
    readFile.seekg(0,ios::end);
    int file_size = readFile.tellg();
    int total_processes = file_size/sizeof(PCB);
    readFile.seekg(0,ios::beg);
    if(file_size%sizeof(PCB))
    {
        cout<< "binary file size mismatch" << endl;
        return -1;
    }
    PCB processes[total_processes];
    Processor* cores = (Processor*)malloc(sizeof(Processor) * num_cores);
    for(int i = 0; i < num_cores; i++)
    {
        cores[i] = Processor(selections[i]);
    }
    int total_files = 0;
    for(int i = 0; i < total_processes; i++)
    {
        //todo: add up total memory allocated
        readFile.read((char*)&processes[i], sizeof(processes));
        total_files += processes[i].number_of_files;
    }  
    cout << "total amount of memory allocated by all processes: " << endl; //TO-DO
    cout << "total number of processes: " << total_processes << endl;
    cout << "total number of files over all processes: " << total_files << endl;
    distribute_processes(cores,percentages,processes,total_processes);
    list<PCB> blah;
    cout << blah.front().name << endl;
    blah.pop_front();
}
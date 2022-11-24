#include <stdio.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <atomic>
#include <chrono>
#include <thread>
#include <pthread.h>

using namespace std;

//global variables:
atomic_int semaphore_counter;
bool processes_remain;
int num_cores;

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
    queue<PCB> ready_queue;
    char selection;

    public:

    Processor(char selection, PCB * processes, int size)
    {
        this->selection = selection;
        for(int i = 0; i < size; i++)
        {
            ready_queue.push(processes[i]);
        }
        // push PCB to ready queue
        //start runnning thread
        
    }

    int get_total_processes_remaining()
    {
        return ready_queue.size();
    }
    // call every 5 seconds from super thread to increase the priority
    void priority_quantum() 
    {
        //TODO
    }
    PCB get_next_PCB()
    {
        PCB pcb;
        if(selection == '1')
        {
            pcb = ready_queue.front();
            ready_queue.pop();
        }
        
        return pcb;
    }
    PCB* scrape_processes(int threshold)
    {
        //TO-DO
        return NULL;
    }
    void fill_process_queue(PCB* procs)
    {
        //TO-DO
    }

// priority scheduling
//      has a queue ordered by priority
//shortest job first
//      has a queue ordered by CPU burst time
// round robin
//      linked list or an array randomly selected from
// first come first serve
//      linked list that is pulled from

};

void display_cores(Processor* cores)
{
    system("clear");
    for(int i = 0; i < num_cores; i++)
    {

    }
}

static void* run_processor(void* arg)
{
    Processor* proc = (Processor*) arg;
    cout << "starting processor "  << endl;
    while (processes_remain)
        { 
            while(semaphore_counter > 0)
                cout << "waiting for super thread to redistribute" << endl;
                
            if(proc->get_total_processes_remaining()>0)
            {
                //remove process from ready queue if available
                PCB running_process = (*proc).get_next_PCB();
            
                cout << "running process: "<< running_process.name << endl;
                //" process " wait millisecond cpu burst time           
                cout << running_process.cpu_burst_time << endl;
                this_thread::sleep_for(chrono::milliseconds(running_process.cpu_burst_time));
            }
            
        }
        
        return NULL;
}

static void* processor_management(void* args)
{
    Processor* processors = (Processor*)args;
    
    clock_t timer = clock();
    while(processes_remain)
    {
        // check to see if 5 seconds has passed to update quantum
        float seconds_elapsed = (float)timer/CLOCKS_PER_SEC;
        if(seconds_elapsed > 5.0)
        {
            timer = clock();
            for(int i = 0; i < num_cores; i++)
            {
                processors[i].priority_quantum();
            }
        }
        // check if there are cores of 0 size
        bool process_remain_flag = false;
        bool needs_redistribute = false;
        for(int i = 0; i < num_cores; i++)
        {
            if(processors[i].get_total_processes_remaining() == 0)
            {
                needs_redistribute = true;
                cout << "core " << i << " has run out of processes" << endl;
            }
            else
            {
                process_remain_flag = true;
            }
        }
        if(!process_remain_flag) 
            processes_remain = false;
            // if it is every core, there are no processes_remain
            // else 
                // redistribute
        
    }

    return NULL;
}

int main(int argc, char** argv)
{
    //init global variables
    semaphore_counter = 0;
    processes_remain = true;

    if(argc < 4 || argc %2 == 1)
    {
        // if there is less than 1 core or if there is an incomplete core
        cout << "Improper minimum arguments met for core." << endl;
        return -1;
    }
    string process_file = argv[1];
    cout << process_file << endl;

    double total_percentage = 0.0;
    for(int i = 2; i < argc; i+=2)
    {
        cout << argv[i] << " " << argv[i+1] << endl;
        int selection = stoi(argv[i]);
        total_percentage += stod(argv[i+1]);
        num_cores++;
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
    int total_files = 0;
    for(int i = 0; i < total_processes; i++)
    {
        readFile.read((char*)&processes[i], sizeof(processes));
        total_files += processes[i].number_of_files;
    }
    cout << "total amount of memory allocated by all processes: " << endl; //TO-DO
    cout << "total number of processes: " << total_processes << endl;
    cout << "total number of files over all processes: " << total_files << endl;
    Processor* cores = (Processor*)malloc(sizeof(Processor) * num_cores);
    pthread_t coreIDs[num_cores];
    //assume one super core for now
    for(int i = 0; i < num_cores; i++)
    {
        char selection = argv[(i+ 1)*2][0];
        cores[i] = Processor(selection,processes,total_processes);
        pthread_create(&coreIDs[i],NULL,run_processor, (void*) &cores[i]); 
        cout << "creating core thread " << endl;       
        //pthread_join(coreIDs[i], NULL); // join the thread so that supervisor can manage
    }

    //create supervisor thread
    pthread_t super_thread_id;
    cout << "creating supervisor thread " << endl;
    pthread_create(&super_thread_id, NULL,processor_management,(void*) cores);
    pthread_join(super_thread_id,NULL);
    for(int i = 0; i < num_cores; i++)
    {
        pthread_join(coreIDs[i],NULL);
    }
    cout << "exiting main" << endl;
}

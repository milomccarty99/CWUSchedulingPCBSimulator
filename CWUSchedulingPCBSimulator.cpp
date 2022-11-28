#include <stdio.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <list>
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
    // needs semaphore flag from super
    void priority_quantum() 
    {
        for(int i = 0 ; i < ready_queue.size(); i++)
        {
            list<PCB> queue;
            
        }
    }
    PCB get_next_PCB()
    {
        PCB pcb;
        if(selection == '1') //FCFS
        {
            pcb = ready_queue.front();
            ready_queue.pop();
        }
        else if (selection == '2') // Round Robin TODO
        {
            pcb = ready_queue.front();
            ready_queue.pop();
        }
        else if (selection == '3') // Shortest Job First TODO
        {
            pcb = ready_queue.front();
            ready_queue.pop();
        }
        else if (selection == '4') // Priority Scheduling TODO
        {
            pcb = ready_queue.front();
            ready_queue.pop();
        }
        
        return pcb;
    }

    list<PCB> scrape_processes(int threshold)
    {
        list<PCB> result;
        while(ready_queue.size() > threshold)
        {
            result.push_front(ready_queue.front());
            ready_queue.pop();
        }
        return result;
    }
    
    void add_process(PCB proc)
    {
        ready_queue.push(proc);
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
                //cout << "core " << i << " has run out of processes" << endl;
            }
            else
            {
                process_remain_flag = true;
            }
        }
        
        processes_remain = process_remain_flag; 
                // redistribute
        if(needs_redistribute)
        {
            semaphore_counter++;
            int total_processes =0;
            for(int i = 0; i < num_cores; i++)
            {
                total_processes+= processors[i].get_total_processes_remaining();
            }
            int average = total_processes/num_cores;
            int remainder = total_processes%num_cores;
            list<PCB> span_extras;

            for(int i = 0; i< num_cores;i++)
            {
                int num_procs = processors[i].get_total_processes_remaining();
                if(average >= num_procs)
                {
                    int diff = average - num_procs;
                    list<PCB> extras = processors[i].scrape_processes(diff);
                    for(int j = 0; j < diff; j++)
                    {
                        span_extras.push_front(extras.front());
                        extras.pop_front();
                    }
                }
            }
            for(int i = 0; i < num_cores; i++)
            {
                int num_procs = processors[i].get_total_processes_remaining();
                int diff = average - num_procs;
                for(int j = 0; j < diff; j++)
                {
                    processors[i].add_process(span_extras.back());
                    span_extras.pop_back();
                }
            }
            semaphore_counter--;
        }
    }

    return NULL;
}
int* distribution_partition(int total_processes, double* percentages)
{
    int* partitions = (int*)malloc(sizeof(int) * num_cores);
    double running_total;
    for(int i = 0; i < num_cores; i++)
    {
        running_total += percentages[i];
        partitions[i] = (total_processes -1) * running_total;
    }
    return partitions;
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

    num_cores = (argc / 2) - 1;
    double total_percentage = 0.0;
    double* percentages = (double*) malloc(sizeof(double) * num_cores);

    for(int i = 2; i < argc; i+=2)
    {
        cout << argv[i] << " " << argv[i+1] << endl;
        int selection = stoi(argv[i]);
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
    //assign PCB processes into the different cores
    int* partitions =  distribution_partition(total_processes,percentages);
    int fencepost = 0;
    for(int i = 0; i < num_cores; i++)
    {

        PCB* processes_to_run[partitions[i] - fencepost];
        fencepost = partitions[i];
        char selection = argv[(i+ 1)*2][0];
        cores[i] = Processor(selection,processes,total_processes);
        pthread_create(&coreIDs[i],NULL,run_processor, (void*) &cores[i]); 
        cout << "creating core thread " << endl;       
    }

    //create supervisor thread
    pthread_t super_thread_id;
    cout << "creating supervisor thread " << endl;
    pthread_create(&super_thread_id, NULL,processor_management,(void*) cores);
    pthread_join(super_thread_id,NULL);
    for(int i = 0; i < num_cores; i++)
    {
        pthread_join(coreIDs[i],NULL); // join the thread so that supervisor can manage
    }
    cout << "exiting main" << endl;
}

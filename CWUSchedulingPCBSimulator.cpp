#include <stdio.h>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

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

int main(int argc, char** argv)
{
    if(argc < 4 || argc %2 == 1)
    {
        // if there is less than 1 core or if there is an imcomplete core
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
    }
    if(total_percentage != 1.0)
    {
        cout << "Incorrect percentages given" << endl;
        return -1;
    }
    

    cout << "size of pcb: " << sizeof(PCB) << endl;
    ifstream readFile (process_file);

    if(!readFile)
    {
        cout<<"an error has occurred" << endl;
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

}

class Processor {
    queue<PCB> ready_queue;

    public:
    Processor(int selection, PCB * processes, int size)
    {
        for(int i = 0; i < size; i++)
        {
            ready_queue.push(processes[i]);
        }
        // push PCB to ready queue
        //start runnning thread
            // pull process from the ready queue (priority scheduling, shortest job first, Round robin, FCFS)
            // "run" the process
            // check for semaphore flag
        
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
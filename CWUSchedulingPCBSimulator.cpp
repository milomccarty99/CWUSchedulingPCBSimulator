#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    if(argc < 3 || argc %2 != 1)
    {
        // if there is less than 1 core or if there is an imcomplete core
        cout << "Improper core arguments." << endl;
        return -1;
    }

    double total_percentage = 0.0;
    for(int i = 1; i < argc; i+=2)
    {
        cout << argv[i] << " " << argv[i+1] << endl;
        int selection = stoi(argv[i]);
        total_percentage += stod(argv[i+1]);
    }
    if(total_percentage != 1.0)
    {
        cout << "Incorrect percentages given" << endl;
    }

}
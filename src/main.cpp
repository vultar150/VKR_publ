#include <iostream>
#include <vector>
#include <cstdio>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <map>

#include "classes.h"
#include "functions.h"


int main(int argc, char **argv)
{
    clock_t time;
    time = clock();

    XMLError eResult;
    std::vector<TaskGraph> graphs;
    std::unordered_map<int,Task*> tasks;
    std::vector<Task*> sortedQueue;
    Processors processors;
    XMLDocument xmlDocument;

    eResult = xmlDocument.LoadFile(argv[1]); // load XML file
    XMLCheckResult(eResult);

    int targetTask = strtol(argv[2], 0, 10);

//// XML parsing//////////////////////////////////////////
    XMLNode * xmlNode = xmlDocument.FirstChildElement(); // root tag
    if (xmlNode == nullptr) return XML_ERROR_FILE_READ_ERROR;

    std::unordered_map<int, bool> usd;
    int maxId = -1;
    setTasks(xmlNode, tasks, usd, processors, maxId);
    setLinks(xmlNode, tasks, usd, maxId);
    tasks[targetTask]->_isTarget = true;


    setInfoTasks(tasks, usd, graphs, processors);

    int major_frame = tasks[targetTask]->_major_frame;
    expandGraphs(graphs, major_frame, processors);
    sortTasks(graphs, sortedQueue);
    assignHigherPrioritySet(graphs, processors);
    computeTimeBounds(sortedQueue);
    bool schedulability = setWCRTs(graphs, tasks);

    time = clock() - time;

    std::ofstream fout("output.txt", std::ios::app);
    fout << "WCRT = " << tasks[targetTask]->_WCRT << std::endl;
    fout << "Period = " << tasks[targetTask]->_period << std::endl;
    fout << "Task num = " << targetTask << std::endl;
    fout << "Time = " << static_cast<float>(time)/CLOCKS_PER_SEC << std::endl;
    fout << "Schedulability of all configure: " << schedulability << std::endl;
    fout << std::endl;
    fout.close();

    int averageWCRT = 0;
    int maxWCRT = 0;
    int count = 0;
    std::map<int, int> wcrts;
    for (auto & graph : graphs) {
        if (graph._instanceNum > 0) break;
        for (auto & task : graph) {
            if (not task.second->_isMessage) {
                // task.second->outInfo();
                if (maxWCRT < task.second->_WCRT) {
                    maxWCRT = task.second->_WCRT;
                }
                averageWCRT += task.second->_WCRT;
                count++;
                wcrts[task.second->_id] = task.second->_WCRT;
            }
        }
        // std::cout << std::endl;
    }
    
    for (auto task : wcrts) {
        std::cout << "t_" << task.first << "  R = " << task.second << std::endl;
    }

    // std::cout << std::endl;
    // std::cout << "WCRT = " << tasks[targetTask]->_WCRT << std::endl;
    // std::cout << "Period = " << tasks[targetTask]->_period << std::endl;
    // std::cout << "Task num = " << targetTask << std::endl;
    // std::cout << "Time = " << static_cast<float>(time)/CLOCKS_PER_SEC << std::endl;
    // std::cout << "Schedulability of all configure: " << schedulability << std::endl;
    // std::cout << "Number of tasks: " << count << std::endl;
    std::cout << "Average WCRT: " << (double)averageWCRT / count << std::endl;
    std::cout << "max WCRT: " << maxWCRT << std::endl;
    /*
    for (const auto & q : sortedQueue)
    {
        q->outInfo();
        cout << endl;
    }
    cout << endl << endl;
    */
    /*
    for (const auto & p : graphs)
    {
        p.outInfo();
    }
    printf("\n/////////////////////////////\n");
    processors.outInfo();
    */
    for (auto& task : sortedQueue) {
        delete task;
    }

    return 0;
}

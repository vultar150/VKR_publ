#include <iostream>
#include <vector>
#include <cstdio>
#include <map>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <ctime>

#include "classes.h"
#include "functions.h"


int main(int argc, char **argv)
{
    clock_t time;
    time = clock();

    XMLError eResult;
    std::vector<TaskGraph> graphs;
    std::map<int,Task*> tasks;
    std::vector<Task*> sortedQueue;
    Processors processors;
    XMLDocument xmlDocument;

    eResult = xmlDocument.LoadFile(argv[1]); // load XML file
    XMLCheckResult(eResult);

    int targetTask = strtol(argv[2], 0, 10);

//// XML parsing//////////////////////////////////////////
    XMLNode * xmlNode = xmlDocument.FirstChildElement(); // root tag
    if (xmlNode == nullptr) return XML_ERROR_FILE_READ_ERROR;

    std::map<int, bool> usd;
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
    setWCRTs(graphs, tasks);

    time = clock() - time;

    std::ofstream fout("output.txt", std::ios::app);
    fout << "WCRT = " << tasks[targetTask]->_WCRT << std::endl;
    fout << "Period = " << tasks[targetTask]->_period << std::endl;
    fout << "Task num = " << targetTask << std::endl;
    fout << "Time = " << static_cast<float>(time)/CLOCKS_PER_SEC << std::endl;
    fout << std::endl;
    fout.close();

    for (auto & graph : graphs) {
        if (graph._instanceNum > 0) break;
        for (auto & task : graph) {
            if (not task.second->_isMessage) {
                task.second->outInfo();
            }
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "WCRT = " << tasks[targetTask]->_WCRT << std::endl;
    std::cout << "Period = " << tasks[targetTask]->_period << std::endl;
    std::cout << "Task num = " << targetTask << std::endl;
    std::cout << "Time = " << static_cast<float>(time)/CLOCKS_PER_SEC << std::endl;
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

#include <iostream>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <cmath>


#include "tinyxml2.h"

using namespace tinyxml2;


#ifndef XMLCheckResult
    #define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); exit(a_eResult); }
#endif




class Task {
public:
    int id, prio, bcet, wcet, period, deadline;

    Task(int id=-1, int prio=-1, int bcet=-1, int wcet=-1, 
         int period=-1, int deadline=-1):
       id(id), prio(prio), bcet(bcet), wcet(wcet), period(period),
       deadline(deadline) {}

    void print() {
        printf("task id=%d, prio=%d, bcet=%d, wcet=%d, period=%d, deadline=%d\n", 
                id, prio, bcet, wcet, period, deadline);
    }
};

// <tlink src="20" dist="18" bctt="64" wctt="74"/>

class Tlink {
public:
    int src, dist, bctt, wctt;

    Tlink(int src=-1, int dist=-1, int bctt=-1, int wctt=-1):
       src(src), dist(dist), bctt(bctt), wctt(wctt) {}

    void print() {
        printf("tlink src=%d, dist=%d, bctt=%d, wctt=%d\n", src, dist, bctt, wctt);
    }
};



class Window {
public:
    int start, stop;

    Window(int start=-1, int stop=-1): start(start), stop(stop) {}

    void print() {
        printf("window start=%d, stop=%d\n", start, stop);
    }
};


class Partition {
public:
    int id, proc, maj_fr;
    std::vector<Task*> tasks;
    std::vector<Window*> windows;

    Partition(int id=-1, int proc=-1, int maj_fr=-1):
        id(id), proc(proc), maj_fr(maj_fr) {}

    void print() {
        printf("partition id=%d, proc=%d, maj_fr=%d\n", id, proc, maj_fr);
        for (auto & task : tasks) {
            printf("\t"); task->print();
        }
        for (auto & window : windows) {
            printf("\t"); window->print();
        }
    }

    ~Partition() {
        for (auto & task: tasks) { delete(task); }
        for (auto & win : windows) { delete(win); }
    }
};


void setInfo(XMLNode * xmlNode, std::vector<Partition*> & partitions, std::vector<Tlink*> & tlinks) {
    XMLError eResult;
    // <partition id="0" proc="4" maj_fr="10000">
    XMLElement * pListPartitions = xmlNode->FirstChildElement("partition");
    while (pListPartitions != nullptr) {
        int id, proc, maj_fr;
        eResult = pListPartitions->QueryIntAttribute("id", &id);
        XMLCheckResult(eResult);
        eResult = pListPartitions->QueryIntAttribute("proc", &proc);
        XMLCheckResult(eResult);
        eResult = pListPartitions->QueryIntAttribute("maj_fr", &maj_fr);
        XMLCheckResult(eResult);

        Partition* partition = new Partition(id, proc, maj_fr);

        // <task id="33" prio="33" bcet="84" wcet="169" period="10000" deadline="10000"/>
        XMLElement * pListTasks = pListPartitions->FirstChildElement("task");
        while (pListTasks != nullptr) {
            int id, prio, bcet, wcet, period, deadline;
            // int id, wcet, prio, offset = 0, period, deadline;
            eResult = pListTasks->QueryIntAttribute("id", &id);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("prio", &prio);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("wcet", &wcet);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("period", &period);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("deadline", &deadline);
            XMLCheckResult(eResult);
//////////////////////////////////////////////////////////////////////////////
            int newBCET = 0;
//////////////////////////////////////////////////////////////////////////////
            Task* task = new Task(id, prio, newBCET, wcet, period, deadline);
            partition->tasks.push_back(task);
            pListTasks = pListTasks->NextSiblingElement("task");
        }

        XMLElement * pListWindows = pListPartitions->FirstChildElement("window");

        while (pListWindows != nullptr) {
            int start, stop;
            eResult = pListWindows->QueryIntAttribute("start", &start);
            XMLCheckResult(eResult);
            eResult = pListWindows->QueryIntAttribute("stop", &stop);
            XMLCheckResult(eResult);
            Window* win = new Window(start, stop);
            partition->windows.push_back(win);
            pListWindows = pListWindows->NextSiblingElement("window");
        }
        partitions.push_back(partition);
        pListPartitions = pListPartitions->NextSiblingElement("partition");
    }

    XMLElement * pListLinks = xmlNode->FirstChildElement("tlink");
    // <tlink src="21" dist="22" bctt="1" wctt="4"/>

    while(pListLinks != nullptr) {
        int src, dist, bctt, wctt;
        eResult = pListLinks->QueryIntAttribute("src", &src);
        XMLCheckResult(eResult);
        eResult = pListLinks->QueryIntAttribute("dist", &dist);
        XMLCheckResult(eResult);
        eResult = pListLinks->QueryIntAttribute("bctt", &bctt);
        XMLCheckResult(eResult);
        eResult = pListLinks->QueryIntAttribute("wctt", &wctt);
        XMLCheckResult(eResult);
        tlinks.push_back(new Tlink(src, dist, bctt, wctt));
        pListLinks = pListLinks->NextSiblingElement("tlink");
    }
}


void createXmlInputData(std::vector<Partition*> & partitions, 
                        std::vector<Tlink*> & tlinks, 
                        char* fileName) {
    XMLDocument xmlDoc;
    XMLNode * pRoot = xmlDoc.NewElement("system");
    xmlDoc.InsertFirstChild(pRoot);
    XMLElement * partElement = nullptr;
    for (int i = 0; i < partitions.size(); i++) {
        partElement = xmlDoc.NewElement("partition");
        int id = partitions[i]->id;
        int proc = partitions[i]->proc;
        int maj_fr = partitions[i]->maj_fr;
        partElement->SetAttribute("id", id);
        partElement->SetAttribute("proc", proc);
        partElement->SetAttribute("maj_fr", maj_fr);
        XMLElement * taskElement = nullptr;
        for (const auto task : partitions[i]->tasks) {
            // <task id="33" prio="33" bcet="84" wcet="169" period="10000" deadline="10000"/>
            taskElement = xmlDoc.NewElement("task");
            taskElement->SetAttribute("id", task->id);
            taskElement->SetAttribute("prio", task->prio);
            taskElement->SetAttribute("bcet", task->bcet);
            taskElement->SetAttribute("wcet", task->wcet);
            taskElement->SetAttribute("period", task->period);
            taskElement->SetAttribute("deadline", task->deadline);
            partElement->InsertEndChild(taskElement);
        }

        XMLElement * winElement = nullptr;
        for (const auto win : partitions[i]->windows) {
            winElement = xmlDoc.NewElement("window");
            winElement->SetAttribute("start", win->start);
            winElement->SetAttribute("stop", win->stop);
            partElement->InsertEndChild(winElement);
        }

        pRoot->InsertEndChild(partElement);
    }

    XMLElement * linkElement = nullptr;
    // <tlink src="0" dist="1" bctt="0" wctt="0"/>
    for (const auto tlink : tlinks) {
        linkElement = xmlDoc.NewElement("tlink");
        linkElement->SetAttribute("src", tlink->src);
        linkElement->SetAttribute("dist", tlink->dist);
        linkElement->SetAttribute("bctt", tlink->bctt);
        linkElement->SetAttribute("wctt", tlink->wctt);
        pRoot->InsertEndChild(linkElement);
    }
    XMLError eResult = xmlDoc.SaveFile(fileName);
    XMLCheckResult(eResult);
}


int main(int argc, char **argv) {
    // std::unordered_map<int, Task*> tasks;
    std::vector<Partition*> partitions;
    std::vector<Tlink*> tlinks;
    // std::unordered_map<int, std::vector<int>> processors; 

    XMLError eResult;
    XMLDocument xmlDocument;
    eResult = xmlDocument.LoadFile(argv[1]);
    XMLCheckResult(eResult);
    XMLNode * xmlNode = xmlDocument.FirstChildElement(); // root tag
    if (xmlNode == nullptr) return XML_ERROR_FILE_READ_ERROR;
    setInfo(xmlNode, partitions, tlinks);

    createXmlInputData(partitions, tlinks, argv[2]);

    for (auto & part : partitions) { delete(part); }

    for (auto & link : tlinks) { delete(link); }

    return 0;
}
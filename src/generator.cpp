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



const int MAX_NUM_PART_ON_PROC = 5;
const int AVG_SIZE_WINDOW_COEF = 100; // min period on proc divide AVG_SIZE_...
const char* FILE_NAME = "data.xml";

int maj_fr = 0;



class Task {
public:
    int id, prio, proc, bcet, wcet, period, deadline;

    Task(int id=-1, int prio=-1, int proc=-1, int bcet=-1, int wcet=-1, 
         int period=-1, int deadline=-1):
       id(id), prio(prio), proc(proc), bcet(bcet), wcet(wcet), period(period),
       deadline(deadline) {}

    void print() {
        printf("task id=%d, prio=%d, proc=%d, bcet=%d, wcet=%d, period=%d, deadline=%d\n", 
                id, prio, proc, bcet, wcet, period, deadline);
    }
};

class Tlink {
public:
    int src, dist, delay;

    Tlink(int src=-1, int dist=-1, int delay=-1):
       src(src), dist(dist), delay(delay) {}

    void print() {
        printf("tlink src=%d, dist=%d, delay=%d\n", src, dist, delay);
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
        for (auto & win : windows) { delete(win); }
    }
};


/*
    <partition id="0" proc="1" maj_fr="80">
        <task id="0" prio="5" bcet="20" wcet="20" period="80" deadline="80"/>


    <task id="0" maj_fr="2870" prio="0" bcet="4" wcet="98" period="2870" deadline="2870" proc="12" 
    Dep="" Inf="101,69,37,13,45,77,29,93,21,53,85,61" />
*/


void setTasksInfo(XMLNode * xmlNode, std::unordered_map<int, Task*> & tasks, 
                  std::unordered_map<int, std::vector<int>> & processors, 
                  std::vector<Tlink*> & tlinks) {
    XMLError eResult;

    XMLElement * pListElement = xmlNode->FirstChildElement("task");

    eResult = pListElement->QueryIntAttribute("maj_fr", &maj_fr);
    XMLCheckResult(eResult);

    while (pListElement != nullptr) {
        int id, prio, proc, bcet, wcet, period, deadline;
        eResult = pListElement->QueryIntAttribute("id", &id);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("prio", &prio);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("proc", &proc);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("bcet", &bcet);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("wcet", &wcet);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("period", &period);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("deadline", &deadline);
        XMLCheckResult(eResult);

        tasks[id] = new Task(id, prio, proc, bcet, wcet, period, deadline);
        processors[proc].push_back(id);

        pListElement = pListElement->NextSiblingElement("task");
    }

    pListElement = xmlNode->FirstChildElement("tlink");

    while(pListElement != nullptr) {
        int src, dist, delay;
        eResult = pListElement->QueryIntAttribute("src", &src);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("dist", &dist);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("delay", &delay);
        XMLCheckResult(eResult);
        tlinks.push_back(new Tlink(src, dist, delay));
        pListElement = pListElement->NextSiblingElement("tlink");
    }
}


void generateWindowsForProcPartitions(std::vector<int> & processor, 
                                      std::vector<Partition*> & partitions, int start,
                                      std::unordered_map<int, Task*> & tasks) {
    int numOfPartitions = partitions.size() - start;
    if (numOfPartitions == 1) {
        partitions[start]->windows.push_back(new Window(0, maj_fr));
        return;
    } 
    int minPeriodOnProc = tasks[processor[0]]->period;
    for (auto & id : processor) {
        if (minPeriodOnProc > tasks[id]->period) {
            minPeriodOnProc = tasks[id]->period;
        }
    }
    int averageSizeOfWindow = minPeriodOnProc / AVG_SIZE_WINDOW_COEF;
    int right = 0, left = 0;
    int partNum = 0;
    while (right < maj_fr) {
        left = right;
        right += arc4random_uniform(averageSizeOfWindow + 1) + averageSizeOfWindow / 2;
        if (right > maj_fr) right = maj_fr;
        partitions[start + partNum]->windows.push_back(new Window(left, right));
        partNum = (partNum + 1) % numOfPartitions;
    }
}


void generatePartitionsForProcessor(std::vector<int> & processor, int procNum,
                                    std::vector<Partition*> & partitions, 
                                    std::unordered_map<int, Task*> & tasks) {
    int sizeOfProc = processor.size();
    int numOfPartitions = arc4random_uniform(
                            std::min(sizeOfProc, MAX_NUM_PART_ON_PROC)) + 1;
    int part_id = partitions.size();
    int start_part_id = part_id;
    // create partitions
    for (int i = 0; i < numOfPartitions; i++) {
        partitions.push_back(new Partition(part_id, procNum, maj_fr));
        part_id++;
    }
    // add at least one task on each partition
    std::vector<int> copyProcessor = processor;
    for (int i = start_part_id; i < partitions.size(); i++) {
        int index = arc4random_uniform(copyProcessor.size());
        partitions[i]->tasks.push_back(tasks[copyProcessor[index]]);
        copyProcessor.erase(copyProcessor.begin() + index);
    }
    // add other tasks on partitions
    for (int i = 0; i < copyProcessor.size(); i++) {
        int on_part_id = arc4random_uniform(numOfPartitions) + start_part_id;
        partitions[on_part_id]->tasks.push_back(tasks[copyProcessor[i]]);
    }
    copyProcessor.clear();
    generateWindowsForProcPartitions(processor, partitions, start_part_id, tasks);
}


void generatePartitions(std::unordered_map<int, std::vector<int>> & processors, 
                        std::vector<Partition*> & partitions, 
                        std::unordered_map<int, Task*> & tasks) {
    for (auto & processor : processors) {
        generatePartitionsForProcessor(processor.second, processor.first, 
                                       partitions, tasks);
    }
}


void createXmlInputData(std::vector<Partition*> & partitions, 
                        std::vector<Tlink*> & tlinks) {
    XMLDocument xmlDoc;
    XMLNode * pRoot = xmlDoc.NewElement("system");
    xmlDoc.InsertFirstChild(pRoot);
    XMLElement * partElement = nullptr;
    for (int i = 0; i < partitions.size(); i++) {
        partElement = xmlDoc.NewElement("partition");
        int id = partitions[i]->id;
        int proc = partitions[i]->proc;
        partElement->SetAttribute("id", id);
        partElement->SetAttribute("proc", proc);
        partElement->SetAttribute("maj_fr", maj_fr);
        XMLElement * taskElement = nullptr;
        for (const auto task : partitions[i]->tasks) {
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
        int bctt = arc4random_uniform(tlink->delay + 1);
        int wctt = tlink->delay;
        linkElement->SetAttribute("bctt", bctt);
        linkElement->SetAttribute("wctt", wctt);
        pRoot->InsertEndChild(linkElement);
    }
    XMLError eResult = xmlDoc.SaveFile(FILE_NAME);
    XMLCheckResult(eResult);
}


int main(int argc, char **argv) {
    std::unordered_map<int, Task*> tasks;
    std::vector<Tlink*> tlinks;
    std::unordered_map<int, std::vector<int>> processors; 
    std::vector<Partition*> partitions;

    XMLError eResult;
    XMLDocument xmlDocument;
    eResult = xmlDocument.LoadFile(argv[1]);
    XMLCheckResult(eResult);
    XMLNode * xmlNode = xmlDocument.FirstChildElement(); // root tag
    if (xmlNode == nullptr) return XML_ERROR_FILE_READ_ERROR;
    setTasksInfo(xmlNode, tasks, processors, tlinks);


    generatePartitions(processors, partitions, tasks);

    createXmlInputData(partitions, tlinks);

// print info

    // for (auto & task : tasks) {
    //     task.second->print();
    // }
    // std::cout << std::endl;

    // for (auto & proc : processors) {
    //     std::cout << "proc num = " << proc.first << std::endl;
    //     std::copy(proc.second.begin(), proc.second.end(), std::ostream_iterator<int>(std::cout, " "));
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;
    // std::cout << std::endl;

    // for (auto & link : tlinks) {
    //     link->print();
    // }

    // for (auto & part : partitions) {
    //     part->print();
    // }
// end print info




    for (auto & task : tasks) { delete(task.second); }

    for (auto & part : partitions) { delete(part); }

    for (auto & link : tlinks) { delete(link); }

    return 0;
}
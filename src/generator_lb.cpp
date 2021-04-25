#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <map>
#include <algorithm>


#include "tinyxml2.h"

using namespace tinyxml2;


#ifndef XMLCheckResult
    #define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); exit(a_eResult); }
#endif



const int MAX_NUM_PART_ON_PROC = 4;
const int AVG_SIZE_WINDOW_COEF = 10; // min period on proc divide AVG_SIZE_...
const char* FILE_NAME = "data.xml";

// int maj_fr = 0;



// <partition id="0" proc="0" maj_fr="40">
//     <task id="1" prio="4" bcet="2" wcet="2" period="40" deadline="40"/>
//     <task id="3" prio="3" bcet="5" wcet="5" period="40" deadline="40"/>
//     <window start="0" stop="10"/>
//     <window start="20" stop="30"/>


// <task id="1" name="kgs" wcet="2284" prio="622" offset="0" period="80000" deadline="80000"/>
class Task {
public:
    int id, wcet, prio, offset, period, deadline;
    std::string name;

    Task(int id=-1, int wcet=-1, int prio=-1, int offset=0,
         int period=-1, int deadline=-1):
       id(id), name(std::string("Task") + std::to_string(id)), 
       wcet(wcet), prio(prio), offset(offset), period(period),
       deadline(deadline) {}

    void print() {
        printf("\t\ttask id=%d, name=%s, wcet=%d, prio=%d, offset=%d, period=%d, deadline=%d\n", 
                id, name.c_str(), wcet, prio, offset, period, deadline);
    }
};

// <tlink src="1" dist="3" bctt="0" wctt="0"/>

class Tlink {
public:
    int src, dst, delay;

    Tlink(int src=-1, int dst=-1, int delay=-1):
       src(src), dst(dst), delay(delay) {}

    void print() {
        printf("link src=%d, dst=%d, delay=%d\n", src, dst, delay);
    }
};


class Window {
public:
    int start, stop, partition;

    Window(int start=-1, int stop=-1, int partition=-1): 
        start(start), stop(stop), partition(partition) {}

    void print() {
        printf("\twindow start=%d, stop=%d, partition=%d\n", start, stop, partition);
    }
};


class Partition {
public:
    int id, proc, maj_fr;
    std::string name, scheduler;
    std::vector<Task*> tasks;
    // std::vector<Window*> windows;

    Partition(int id=-1, int proc=-1, int maj_fr=-1):
        id(id), proc(proc), maj_fr(maj_fr), name(std::string("Partition") + std::to_string(id)), 
        scheduler("FPPS") {}

    void print() {
        printf("\tpartition id=%d, name=%s, scheduler=%s\n", id, name.c_str(), scheduler.c_str());
        for (auto & task : tasks) {
            task->print();
        }
        // for (auto & window : windows) {
        //     printf("\t"); window->print();
        // }
    }

    ~Partition() {
        for (auto & task : tasks) { delete(task); }
    }
};

// <module major_frame="2000000" name="1@Module1">
//     <partition id="0" name="kernel4" scheduler="FPPS">
//         <task id="0" name="UniversalTime" wcet="3000" prio="640" offset="0" period="20000" deadline="20000"/>
//     </partition>
//     <partition id="1" name="proc3_part1" scheduler="FPPS">
//         <task id="1" name="kgs" wcet="2284" prio="622" offset="0" period="80000" deadline="80000"/>
//         <task id="2" name="vurk100_dbv" wcet="19" prio="640" offset="0" period="10000" deadline="10000"/>
//         <task id="3" name="vrp100" wcet="4" prio="632" offset="0" period="10000" deadline="10000"/>
//         ...
//     </partition>
//     <window start="20" stop="2950" partition="1"/>
//     <window start="2970" stop="5900" partition="0"/>

class Module {
public:
    int major_frame;
    std::string name;
    std::vector<Partition*> partitions;
    std::vector<Window*> windows;

    Module(int major_frame=-1, std::string name=""): 
        major_frame(major_frame), name(name) {}

    void print() {
        printf("module major_frame=%d, name=%s\n", major_frame, name.c_str());
        for (auto & part : partitions) {
            part->print();
        }
        for (auto & window : windows) {
            window->print();
        }
    }

    ~Module() {
        for (auto & part : partitions) { delete(part); }
        for (auto & win : windows) { delete(win); }
    }
};



// void renamePartitions(Module* module) {
//     for (int i = 0; i < module->partitions.size(); i++) {
//         module->partitions[i]->id = i;
//         // module->partitions[i]->name = std::string("Partition") + std::to_string(i);
//     }
// }


// <partition id="0" proc="0" maj_fr="40">

void setModulesInfo(XMLNode * xmlNode, 
                    std::map<int, Module*> & modules, 
                    std::vector<Tlink*> & tlinks) {
    XMLError eResult;

    XMLElement * pListPartitions = xmlNode->FirstChildElement("partition");
    while (pListPartitions != nullptr) {
        int id, proc, maj_fr;
        eResult = pListPartitions->QueryIntAttribute("id", &id);
        XMLCheckResult(eResult);
        eResult = pListPartitions->QueryIntAttribute("proc", &proc);
        XMLCheckResult(eResult);
        eResult = pListPartitions->QueryIntAttribute("maj_fr", &maj_fr);
        XMLCheckResult(eResult);
        // printf("id = %d\n", id);

        Partition* partition = new Partition(id, proc, maj_fr);
        // <partition id="0" name="kernel4" scheduler="FPPS">

        XMLElement * pListTasks = pListPartitions->FirstChildElement("task");
        // <task id="1" name="kgs" wcet="2284" prio="622" offset="0" period="80000" deadline="80000"/>
        while (pListTasks != nullptr) {
            int id, wcet, prio, offset = 0, period, deadline;
            eResult = pListTasks->QueryIntAttribute("id", &id);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("wcet", &wcet);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("prio", &prio);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("period", &period);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("deadline", &deadline);
            XMLCheckResult(eResult);
            Task* task = new Task(id, wcet, prio, offset, period, deadline);
            partition->tasks.push_back(task);
            pListTasks = pListTasks->NextSiblingElement("task");
        }
        // <module major_frame="2000000" name="1@Module1">
        auto it = modules.find(proc);
        if (it == modules.end()) {
            modules[proc] = new Module(maj_fr, std::string("Module") + std::to_string(proc));
        }
        id = partition->id = modules[proc]->partitions.size();
        modules[proc]->partitions.push_back(partition);

        XMLElement * pListWindows = pListPartitions->FirstChildElement("window");

        while (pListWindows != nullptr) {
            int start, stop;
            eResult = pListWindows->QueryIntAttribute("start", &start);
            XMLCheckResult(eResult);
            eResult = pListWindows->QueryIntAttribute("stop", &stop);
            XMLCheckResult(eResult);
            Window* win = new Window(start, stop, id);
            modules[proc]->windows.push_back(win);
            pListWindows = pListWindows->NextSiblingElement("window");
        }
        pListPartitions = pListPartitions->NextSiblingElement("partition");
    }

    for (auto & module : modules) {
        std::sort(module.second->windows.begin(), 
                  module.second->windows.end(),
                  [] (Window* win1, Window* win2) {
                        return win1->start < win2->start;
                  });
        // renamePartitions(module.second);
    }

    XMLElement * pListLinks = xmlNode->FirstChildElement("tlink");
    // <link src="134" dst="62" delay="26"/>

    while(pListLinks != nullptr) {
        int src, dst, delay;
        eResult = pListLinks->QueryIntAttribute("src", &src);
        XMLCheckResult(eResult);
        eResult = pListLinks->QueryIntAttribute("dist", &dst);
        XMLCheckResult(eResult);
        eResult = pListLinks->QueryIntAttribute("wctt", &delay);
        XMLCheckResult(eResult);
        tlinks.push_back(new Tlink(src, dst, delay));
        pListLinks = pListLinks->NextSiblingElement("tlink");
    }
}



void createXmlInputData(std::map<int, Module*> & modules, 
                        std::vector<Tlink*> & tlinks, 
                        char* fileName) {
    XMLDocument xmlDoc;
    XMLNode * pRoot = xmlDoc.NewElement("system");
    xmlDoc.InsertFirstChild(pRoot);
    XMLElement * moduleElement = nullptr;
    for (auto & module : modules) {
        moduleElement = xmlDoc.NewElement("module");
        int major_frame = module.second->major_frame;
        std::string name = module.second->name;
        moduleElement->SetAttribute("major_frame", major_frame);
        moduleElement->SetAttribute("name", name.c_str());
        XMLElement * partElement = nullptr;
        for (auto & part : module.second->partitions) {
            partElement = xmlDoc.NewElement("partition");
            partElement->SetAttribute("id", part->id);
            partElement->SetAttribute("name", part->name.c_str());
            partElement->SetAttribute("scheduler", part->scheduler.c_str());
            XMLElement * taskElement = nullptr;
            for (auto & task : part->tasks) {
                taskElement = xmlDoc.NewElement("task");
                taskElement->SetAttribute("id", task->id);
                taskElement->SetAttribute("name", task->name.c_str());
                taskElement->SetAttribute("wcet", task->wcet);
                taskElement->SetAttribute("prio", task->prio);
                taskElement->SetAttribute("offset", task->offset);
                taskElement->SetAttribute("period", task->period);
                taskElement->SetAttribute("deadline", task->deadline);
                partElement->InsertEndChild(taskElement);
            }
            moduleElement->InsertEndChild(partElement);
        }
        XMLElement * winElement = nullptr;
        for (const auto win : module.second->windows) {
            winElement = xmlDoc.NewElement("window");
            winElement->SetAttribute("start", win->start);
            winElement->SetAttribute("stop", win->stop);
            winElement->SetAttribute("partition", win->partition);
            moduleElement->InsertEndChild(winElement);
        }
        pRoot->InsertEndChild(moduleElement);
    }

    XMLElement * linkElement = nullptr;
    // <tlink src="0" dist="1" bctt="0" wctt="0"/>
    for (const auto link : tlinks) {
        linkElement = xmlDoc.NewElement("link");
        linkElement->SetAttribute("src", link->src);
        linkElement->SetAttribute("dst", link->dst);
        linkElement->SetAttribute("delay", link->delay);
        pRoot->InsertEndChild(linkElement);
    }
    XMLError eResult = xmlDoc.SaveFile(fileName);
    XMLCheckResult(eResult);
}


int main(int argc, char** argv) {
    std::map<int, Module*> modules;
    std::vector<Tlink*> tlinks;

    XMLError eResult;
    XMLDocument xmlDocument;
    eResult = xmlDocument.LoadFile(argv[1]);
    XMLCheckResult(eResult);
    XMLNode * xmlNode = xmlDocument.FirstChildElement(); // root tag
    if (xmlNode == nullptr) return XML_ERROR_FILE_READ_ERROR;
    setModulesInfo(xmlNode, modules, tlinks);

    createXmlInputData(modules, tlinks, argv[2]);

    // for (auto & module : modules) {
    //     module.second->print();
    // }

    // for (auto & link : tlinks) {
    //     link->print();
    // }

    for (auto & module : modules) {
        delete(module.second);
    }

    for (auto & link : tlinks) {
        delete(link);
    }
    std::cout << std::endl;
}


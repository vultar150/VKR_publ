#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

#include "functions.h"


void setTasks(XMLNode *xmlNode,
              std::unordered_map<int,Task*> & tasks,
              std::unordered_map<int, bool> & usd,
              Processors & processors,
              int & maxId) {
    XMLError eResult;

    XMLElement * pListElement = xmlNode->FirstChildElement("partition");

    if (pListElement == nullptr) exit(XML_ERROR_PARSING_ELEMENT);

    while (pListElement != nullptr) {
        int partitionId, processorNum, major_frame;
        eResult = pListElement->QueryIntAttribute("id", &partitionId);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("proc", &processorNum);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("maj_fr", &major_frame);
        XMLCheckResult(eResult);
        std::vector<TaskPosition> taskPos;
        XMLElement * pListTasks = pListElement->FirstChildElement("task");
        std::vector<int> ids;
        while (pListTasks != nullptr) {
            int id, priority, period, BCET, WCET;
            eResult = pListTasks->QueryIntAttribute("id", &id);
            XMLCheckResult(eResult);
            if (maxId < id) maxId = id;
            eResult = pListTasks->QueryIntAttribute("prio", &priority);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("period", &period);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("bcet", &BCET);
            XMLCheckResult(eResult);
            eResult = pListTasks->QueryIntAttribute("wcet", &WCET);
            XMLCheckResult(eResult);
            usd[id] = false;
            tasks[id] = new Task(id, major_frame, priority, period, 
                                 processorNum, BCET, WCET, partitionId);
            ids.push_back(id);
            pListTasks = pListTasks->NextSiblingElement("task");
        }
        XMLElement * pListWindows = pListElement->FirstChildElement("window");
        int left = 0;
        while (pListWindows != nullptr) {
            int start, stop;
            eResult = pListWindows->QueryIntAttribute("start", &start);
            XMLCheckResult(eResult);
            eResult = pListWindows->QueryIntAttribute("stop", &stop);
            XMLCheckResult(eResult);
            int ET = start - left;
            if (ET > 0) {
                Task* win = new Task(0, major_frame, 0, 0, processorNum, 0, 0, -1);
                win->_minA = win->_maxA = win->_minS = win->_maxS = left;
                win->_minF = win->_maxF = start;
                win->_BCET = win->_WCET = ET;
                addToHPforAll(tasks, win, ids);
            }
            left = stop;
            pListWindows = pListWindows->NextSiblingElement("window");
        }
        int ET = major_frame - left;
        if (ET > 0) {
            Task* win = new Task(0, major_frame, 0, 0, processorNum, 0, 0, -1);
            win->_minA = win->_maxA = win->_minS = win->_maxS = left;
            win->_minF = win->_maxF = major_frame;
            win->_BCET = win->_WCET = ET;
            addToHPforAll(tasks, win, ids);
        }
        processors[processorNum] = taskPos;
        pListElement = pListElement->NextSiblingElement("partition");
    }
}

void addToHPforAll(std::unordered_map<int,Task*>& tasks, Task* win, std::vector<int>& ids) {
    for (const auto& id : ids) {
        tasks[id]->_hp.push_back(win);
    }
}

void setLinks(XMLNode *xmlNode, std::unordered_map<int,Task*> & tasks,
              std::unordered_map<int, bool> & usd, int & maxId) {
    XMLError eResult;

    XMLElement * pListElement = xmlNode->FirstChildElement("tlink");

    int mf = (*tasks.begin()).second->_major_frame;
    int messageId = maxId + 1;
    int period;

    while (pListElement != nullptr) {
        int src, dst, bctt, wctt;
        eResult = pListElement->QueryIntAttribute("src", &src);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("dist", &dst);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("bctt", &bctt);
        XMLCheckResult(eResult);
        eResult = pListElement->QueryIntAttribute("wctt", &wctt);
        XMLCheckResult(eResult);

        if (wctt > 0) {
            period = tasks[src]->_period;
            tasks[messageId] = new Task(messageId, mf, -1, period, -1, bctt, wctt, -1);
            tasks[messageId]->_isMessage = true;
            usd[messageId] = false;

            tasks[src]->_successors.push_back(tasks[messageId]);
            tasks[messageId]->_predecessors.push_back(tasks[src]);
            tasks[dst]->_predecessors.push_back(tasks[messageId]);
            tasks[messageId]->_successors.push_back(tasks[dst]);
            messageId++;
        } else {
            tasks[src]->_successors.push_back(tasks[dst]);
            tasks[dst]->_predecessors.push_back(tasks[src]);
        }
        pListElement = pListElement->NextSiblingElement("tlink");
    }
}


void setNumGraph(const int & id,
                 std::unordered_map<int, Task*> & tasks,
                 std::unordered_map<int, bool> & usd,
                 const int & graphNum,
                 std::vector<TaskGraph> & graphs) {
    usd[id] = true;
    tasks[id]->_graphId = graphNum;
    graphs[graphNum][id] = tasks[id];
    if (tasks[id]->_isTarget) {
        graphs[graphNum]._isTarget = true;
    }
    for (auto & successor : tasks[id]->_successors) {
        if (!usd[successor->_id]) {
            setNumGraph(successor->_id, tasks, usd, graphNum, graphs);
        }
    }
    for (auto & predecessor : tasks[id]->_predecessors) {
        if (!usd[predecessor->_id]) {
            setNumGraph(predecessor->_id, tasks, usd, graphNum, graphs);
        }
    }
}


void setInfoTasks(std::unordered_map<int, Task*> & tasks,
                  std::unordered_map<int, bool> & usd,
                  std::vector<TaskGraph> & graphs,
                  Processors & processors) {
    int graphNum = 0;
    for (auto & task : tasks) {
        int id = task.second->_id;
        if (task.second->_predecessors.empty()) {
            task.second->_isSource = true;
            setExcl(tasks, task.second, 0);
        }
        if (task.second->_successors.empty()) {
            setExcl2(tasks, task.second);
        }
        if (!usd[id]) {
            graphs.push_back(TaskGraph(graphNum, task.second->_period));
            setNumGraph(id, tasks, usd, graphNum, graphs);
            graphNum++;
        }
        if (!task.second->_isMessage) {
            TaskPosition taskPosition(task.second->_graphId, id);
            processors[task.second->_processorNum].push_back(taskPosition);
        }
    }

    for (const auto & task : tasks) {
        task.second->_tExcl.insert(task.second->_tExcl.end(),
                                   task.second->_tExcl2.begin(),
                                   task.second->_tExcl2.end());
        task.second->_tExcl2.clear();
    }
}


void setExcl(std::unordered_map<int, Task*> & tasks, Task * task, int depth) {
    if (task->_setExcl) {
        if (task->_depth < depth) {
            task->_depth = depth;
            for (const auto & successor : task->_successors) {
                setExcl(tasks, successor, depth+1);
            }
        }
        else {
            return;
        }
    }
    else {
        task->_depth = depth;
        task->_setExcl = true;
        for (const auto & successor : task->_successors) {
            task->_tExcl.push_back(successor);
            setExcl(tasks, successor, depth+1);
            task->_tExcl.insert(task->_tExcl.end(),
                                successor->_tExcl.begin(),
                                successor->_tExcl.end());
        }
    }
}


void setExcl2(std::unordered_map<int, Task*> & tasks, Task * task) {
    if (task->_setExcl2) {
        return;
    }
    else {
        task->_setExcl2 = true;
        for (const auto & predecessor : task->_predecessors) {
            task->_tExcl2.push_back(predecessor);
            setExcl2(tasks, predecessor);
            task->_tExcl2.insert(task->_tExcl2.end(),
                                 predecessor->_tExcl2.begin(),
                                 predecessor->_tExcl2.end());
        }
    }
}


void expandGraphs(std::vector<TaskGraph> & graphs,
                  int & major_frame,
                  Processors & processors) {
    int size = graphs.size();

    for (int i = 0; i < size; i++) {
        int expansionFactor = major_frame / int(graphs[i]._period);
        expandGraph(graphs, i, expansionFactor, processors);
    }
}


void expandGraph(std::vector<TaskGraph> & graphs,
                 int & graphId,
                 int & expansionFactor,
                 Processors & processors) {
    for (int instanceNum = 1; instanceNum < expansionFactor; instanceNum++) {
        int newGraphId = graphs.size();
        copyGraph(graphs,
                  graphId,
                  newGraphId,
                  instanceNum,
                  processors);
    }
}


void copyGraph(std::vector<TaskGraph> & graphs,
               int & graphId,
               int & newGraphId,
               int & instanceNum,
               Processors & processors) {
    int period = graphs[graphId]._period;
    bool isTarget = graphs[graphId]._isTarget;

    TaskGraph taskGraph(newGraphId, period, instanceNum, isTarget);
    taskGraph._copyOf = graphId;

    for (const auto & task : graphs[graphId]) {
        int id = task.second->_id;
        taskGraph[id] = new Task(*task.second);
        taskGraph[id]->_instanceNum = instanceNum;
        taskGraph[id]->_graphId = newGraphId;
        if (!task.second->_isMessage) {
            TaskPosition taskPosition(newGraphId, id);
            processors[task.second->_processorNum].push_back(taskPosition);
        }
    }

    for (const auto & task : graphs[graphId]) {
        int id = task.second->_id;
        for (const auto & pred : task.second->_predecessors) {
            taskGraph[id]->_predecessors.push_back(taskGraph[pred->_id]);
        }
        for (const auto & succ : task.second->_successors) {
            taskGraph[id]->_successors.push_back(taskGraph[succ->_id]);
        }
        for (const auto & excl : task.second->_tExcl) {
            taskGraph[id]->_tExcl.push_back(taskGraph[excl->_id]);
        }
        for (const auto & hp : task.second->_hp) {
            taskGraph[id]->_hp.push_back(hp);
        }
    }
    graphs.push_back(taskGraph);
}


void sortTasks(std::vector<TaskGraph> & graphs, std::vector<Task*> & sortedQueue) {
    for (uint32_t i = 0; i < graphs.size(); i++) {
        for (const auto & task : graphs[i]) {
            sortedQueue.push_back(task.second);
        }
    }
    sort(sortedQueue.begin(),
         sortedQueue.end(),
         [](Task* task1, Task* task2) {
            if (task1->_depth != task2->_depth)
                return task1->_depth < task2->_depth;
            else
                return task1->_priority > task2->_priority;
         }
    );
}


void assignHigherPrioritySet(std::vector<TaskGraph> & graphs,
                             Processors & processors) {
    for (auto & graph : graphs) {
        for (auto & task : graph) {
            if (!task.second->_isMessage) {
                setHp(graphs, task.second, processors);
            }
        }
    }
}


void setHp(std::vector<TaskGraph> & graphs,
           Task * task,
           Processors & processors) {
    int graphId = task->_graphId;
    int copyOf = graphs[graphId]._copyOf;
    for (auto & taskPos : processors[task->_processorNum]) {
        int simProcGraphId = taskPos._graphId;
        int simProcCopyOf = graphs[simProcGraphId]._copyOf;
        Task * taskOnSimilarProc = graphs[simProcGraphId][taskPos._taskId];
        if (task->_priority >= taskOnSimilarProc->_priority) {
            continue;
        }
        else if (simProcGraphId == graphId &&
                 taskPos._taskId == task->_id) {
            continue;
        }
        else if (simProcGraphId != graphId &&
                 copyOf == simProcCopyOf) {
            continue;
        }
        else if (checkExcl(task,
                           taskOnSimilarProc,
                           graphId,
                           simProcGraphId)) {
            continue;
        }
        else if (task->_partitionId == taskOnSimilarProc->_partitionId) {
            task->_hp.push_back(taskOnSimilarProc);
        }
    }
}


bool checkExcl(Task * task1,
               Task * task2,
               const int & graphId,
               const int & simGraphId) {
    if (graphId != simGraphId) {
        return false;
    }
    for (const auto & task : task1->_tExcl) {
        if (task == task2) {
            return true;
        }
    }
    return false;
}


void computeTimeBounds(std::vector<Task*> & sortedQueue) {
    int i = 0;
    bool changed;
    do {
        changed = false;
        // std::cerr << "===" << i << "===" << std::endl;
        for (auto & t : sortedQueue) {
            if (t->_isMessage) {
                minA(t, changed);
                t->_minS = t->_minA;
                maxA(t, changed);
                t->_maxS = t->_maxA;
                t->_minF = t->_minS + t->_BCET;
                t->_maxF = t->_maxS + t->_WCET;
            } else {
                minA(t, changed);
                minS(t, changed);
                minF(t, changed);
                maxA(t, changed);
                maxS(t, changed);
                maxF(t, changed);
            }
            t->_RT = t->_maxF - t->_instanceNum * t->_period;
        }
        i++;
        for (auto & t : sortedQueue) {
            t->_tPmtor.clear();
        }
    } while (changed);
}


void minA (Task * t, bool & changed) {
    if (t->_isSource) {
        t->_minA = t->_instanceNum * t->_period;
        return;
    }
    for (auto & p : t->_predecessors) {
        if (p->_minF == -1)
            return;
        if (p->_minF > t->_minA) {
            t->_minA = p->_minF;
            changed = true;
        }
    }
}


void maxA (Task * t, bool & changed) {
    if (t->_isSource) {
        t->_maxA = t->_instanceNum * t->_period;
        return;
    }
    for (auto & p : t->_predecessors) {
        if (p->_maxF == -1)
            return;
        if (p->_isMessage) {
            if (p->_predecessors.size() != 1) {
                std::cout << "ERROR: MESSAGE HAVE NOT ONE PRED!\n";
                exit(1);
            }
            Task* mainPredecessor = *(p->_predecessors.begin());
            if (t->_processorNum == mainPredecessor->_processorNum) {
                t->_tPmtor.insert(t->_tPmtor.end(),
                                  mainPredecessor->_tPmtor.begin(),
                                  mainPredecessor->_tPmtor.end());
            }
        } else if (not t->_isMessage) {
            if (t->_processorNum == p->_processorNum) {
                t->_tPmtor.insert(t->_tPmtor.end(), p->_tPmtor.begin(),
                                  p->_tPmtor.end());
            }
        }
        if (p->_maxF > t->_maxA) {
            t->_maxA = p->_maxF;
            changed = true;
        }
    }
}


void minS (Task * t, bool & changed) {
    if (t->_minS == -1) {
        t->_minS = t->_minA;
        changed = true;
    }
    std::vector <Task *> alpha;
    for (auto & s : t->_hp) {
        if (s->_minF == -1 || s->_maxS == -1)
            return;
        if (t->_minA < s->_minF && s->_maxS <= t->_minS)
            alpha.push_back(s);
    }
    int old  = t->_minS;
    int res = t->_minA;

    for (auto & s : alpha) {
        if (s->_minF > res)
            res = s->_minF;
    }
    if (old != res) {
        t->_minS = res;
        changed = true;
    }
}


void maxS (Task * t, bool & changed) {
    if (t->_maxS == -1) {
        t->_maxS = t->_maxA;
        changed = true;
    }
    std::vector <Task *> beta;
    for (auto & s : t->_hp) {
        if (s->_minS == -1 || s->_maxF == -1)
            return;
        if (s->_minS <= t->_maxS && t->_minA < s->_maxF) {
            if (checkPmtor(t, s))
                continue;
            beta.push_back(s);
        }
    }
    int delay = 0;
    for (auto & s : beta) {
        if (s->_WCET < s->_maxF - t->_maxA) {
            delay += s->_WCET;
            t->_tPmtor.push_back(s);
        } else if (s->_maxF - t->_maxA > 0) {
            delay += s->_maxF - t->_maxA;
            t->_tPmtor.push_back(s);
        }
    }
    int res =  t->_maxA + delay;
    if (t->_maxS != res) {
        t->_maxS = res;
        changed = true;
    }
}


void minF (Task * t, bool & changed) {
    if (t->_minF == -1) {
        t->_minF = t->_minS + t->_BCET;
        changed = true;
    }
    std::vector <Task *> C;
    for (auto & s : t->_hp) {
        if (s->_minS == -1 || s->_maxS == -1)
            return;
        if (t->_minS <= s->_minS && s->_minS < t->_minF &&
            t->_minS <= s->_maxS && s->_maxS < t->_minF)
            C.push_back(s);
    }
    int res = t->_minS + t->_BCET;
    for (auto & s : C) {
        res += s->_BCET;
    }
    if (t->_minF != res) {
        t->_minF = res;
        changed = true;
    }
}


void maxF (Task * t, bool & changed) {
    if (t->_maxF == -1) {
        t->_maxF = t->_maxS + t->_WCET;
        changed = true;
    }
    std::vector <Task *> delta;
    for (auto & s : t->_hp) {
        if (s->_minS == -1 || s->_maxF == -1)
            return;
        if (t->_maxS < s->_maxF && s->_minS < t->_maxF) {
            if (checkPmtor(t, s))
                continue;
            delta.push_back(s);
            t->_tPmtor.push_back(s);
        }
    }
    int res = t->_maxS + t->_WCET;
    for (auto & s : delta) {
        res += s->_WCET;
    }
    if (t->_maxF != res) {
        t->_maxF = res;
        changed = true;
    }
}


bool checkPmtor(Task * task1, Task * task2) {
    for (const auto & task : task1->_tPmtor) {
        if (task == task2) {
            return true;
        }
    }
    return false;
}


int getWCRT(std::vector<TaskGraph> & graphs, int graphId, int taskId) {
    int maxRT = 0;
    for (auto & graph : graphs) {
        if (graph._copyOf == graphId) {
            if (maxRT < graph[taskId]->_RT) {
                maxRT = graph[taskId]->_RT;
            }
        }
    }
    return maxRT;
}


bool setWCRTs(std::vector<TaskGraph> & graphs, 
              std::unordered_map<int,Task*> & tasks) {
    bool schedulability = true;
    for (auto & task : tasks) {
        if (not task.second->_isMessage) {
            int graphId = task.second->_graphId, taskId = task.second->_id;
            task.second->_WCRT = getWCRT(graphs, graphId, taskId);
            if (task.second->_WCRT > task.second->_period) {
                schedulability = false;
            }
        }
    }
    return schedulability;
}

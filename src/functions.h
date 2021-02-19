#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "tinyxml2.h" // additional library (XML Parser)
#include "classes.h"

using namespace tinyxml2;


// prototypes of functions
bool comparator(Task * task1, Task * task2);

void setTasks(XMLNode * xmlNode,
              std::map<int,Task*> & tasks,
              std::map<int, bool> & usd,
              Processors & processors,
              int & maxId);

void addToHPforAll(std::map<int,Task*>& tasks, Task* win, std::vector<int>& ids);

void setLinks(XMLNode *xmlNode, std::map<int,Task*> & tasks,
              std::map<int, bool> & usd, int & maxId);

void setNumGraph(const int & id,
                 std::map<int, Task*> & tasks,
                 std::map<int, bool> & usd,
                 const int & graphNum,
                 std::vector<TaskGraph> & graphs);

void setInfoTasks(std::map<int, Task*> & tasks,
                  std::map<int, bool> & usd,
                  std::vector<TaskGraph> & graphs,
                  Processors & processors);

void setExcl(std::map<int, Task*> & tasks,
             Task * task,
             int depth);

void setExcl2(std::map<int, Task*> & tasks, Task * task);

void expandGraphs(std::vector<TaskGraph> & graphs,
                  int & major_frame,
                  Processors & processors);

void copyGraph(std::vector<TaskGraph> & graphs,
               int & graphId,
               int & newGraphId,
               int & instantNum,
               Processors & processors);

void assignHigherPrioritySet(std::vector<TaskGraph> & graphs,
                             Processors & processors);

void setHp(std::vector<TaskGraph> & graphs,
           Task * task,
           Processors & processors);

void setProcessors(XMLNode *xmlNode, Processors & processors);

int findGCD(int a, int b);

int findLCM(std::vector<TaskGraph> & graphs);

void expandGraph(std::vector<TaskGraph> & graphs,
                 int & graphId,
                 int & expansionFactor,
                 Processors & processors);

void sortTasks(std::vector<TaskGraph> & graphs, std::vector<Task*> & sortedQueue);

bool checkExcl(Task * task1,
               Task * task2,
               const int & graphId,
               const int & simGraphId);

void computeTimeBounds(std::vector<Task*> & sortedQueue);

void minA (Task * t, bool & changed);

void maxA (Task * t, bool & changed);

void minS (Task * t, bool & changed);

void maxS (Task * t, bool & changed);

void minF (Task * t, bool & changed);

void maxF (Task * t, bool & changed);

bool checkPmtor(Task * task1, Task * task2);

void clearSetsPmtor(std::vector<TaskGraph> & graphs);

int getWCRT(std::vector<TaskGraph> & graphs, int targetTask);

void firstInitialization(std::vector<TaskGraph> & graphs, Processors & processors, std::vector<Task*> & sortedQueue);

bool computeTimeBoundsOfTask(std::vector<TaskGraph> & graphs, const int graphId, Processors & processors,
                            Task * const task, bool fstIter, bool & wasChange);

#endif // FUNCTIONS_H

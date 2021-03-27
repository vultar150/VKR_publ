#ifndef CLASSES_H
#define CLASSES_H

#include <vector>
#include <map>

#ifndef XMLCheckResult
    #define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); exit(a_eResult); }
#endif


struct TaskPosition
{
    int _graphId;
    int _taskId;
    TaskPosition(int a = 0, int b = 0): _graphId(a), _taskId(b) {}
    bool operator ==(const TaskPosition & taskPos)
    {
        return _graphId == taskPos._graphId && _taskId == taskPos._taskId;
    }
};


class Task
{
    public:
        int _id, _major_frame, _priority;
        int _period, _processorNum;
        int _BCET,  _WCET;
        int _partitionId;
        int _instanceNum;
        int _depth, _graphId;
        int _minA,  _maxA;
        int _minS,  _maxS;
        int _minF,  _maxF;
        bool _isSource;
        bool _setExcl, _setExcl2; // help for compute exclusion set
        bool _isTarget;
        bool _isMessage;
        int _RT;
        int _WCRT;
        std::vector<Task*> _tExcl; // direct pass
        std::vector<Task*> _tExcl2; // reverse pass
        std::vector<Task*> _predecessors;
        std::vector<Task*> _successors;
        std::vector<Task*> _tPmtor;
        std::vector<Task*> _hp;
        Task(int id, int major_frame,  int priority,
             int period, int processorNum, int BC, int WC, int partitionId,
             int instanceNum = 0, int depth = -1, int graphId = -1,
             int minA = -1, int maxA = -1,  int minS = -1,
             int maxS = -1, int minF = -1,  int maxF = -1,
             bool isSource = false, bool setExcl = false,
             bool setExcl2 = false, bool isTarget = false, bool isMessage = false);
        Task(const Task & task);
        Task & operator=(const Task & task);
        bool operator==(const Task & task);
        void outInfo() const;
};


class TaskGraph: public std::map<int,Task*>
{
    public:
        int _id;
        int _period;
        int _instanceNum;
        bool _isTarget;
        int _copyOf;
        TaskGraph(int id, int P, int instanceNum = 0, bool isTarget = false);
        void outInfo() const;
};


class Processors: public std::map<int, std::vector<TaskPosition> >
{
    public:
        void outInfo() const;
};

#endif // CLASSES_H

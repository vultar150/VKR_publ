#include <iostream>
#include <cstdio>
#include "classes.h"

// Defining methods for "Task" class
Task::Task(int id, int major_frame, int priority,
           int period, int processorNum, int BC, int WC,
           int instanceNum, int depth,  int graphId,
           int minA, int maxA, int minS,
           int maxS, int minF, int maxF,
           bool isSource, bool setExcl,
           bool setExcl2, bool isTarget):
           _id(id), _major_frame(major_frame), _priority(priority),
           _period(period), _processorNum(processorNum),
           _BCET(BC), _WCET(WC), _instanceNum(instanceNum),
           _depth(depth), _graphId(graphId), _minA(minA),
           _maxA(maxA), _minS(minS), _maxS(maxS), _minF(minF), _maxF(maxF),
           _isSource(isSource), _setExcl(setExcl),
           _setExcl2(setExcl2), _isTarget(isTarget)
{
}


Task::Task(const Task & task)
{
    _id = task._id;  _major_frame = task._major_frame;
    _priority = task._priority; _period = task._period;
    _processorNum = task._processorNum;
    _BCET  = task._BCET;  _WCET = task._WCET;
    _instanceNum = task._instanceNum;
    _depth = task._depth; _graphId = task._graphId;

    _minA  = task._minA;   _maxA = task._maxA;
    _minS  = task._minS;   _maxS = task._maxS;
    _minF  = task._minF;   _maxF = task._maxF;

    _isSource = task._isSource;
    _setExcl  = task._setExcl;
    _setExcl2 = task._setExcl2;
    _isTarget = task._isTarget;

    _delays = task._delays;
}


Task & Task::operator=(const Task & task)
{
    if (this != &task)
    {
        _id = task._id;  _major_frame = task._major_frame;
        _priority = task._priority; _period = task._period;
        _processorNum = task._processorNum;
        _BCET  = task._BCET;  _WCET = task._WCET;
        _instanceNum = task._instanceNum;
        _depth = task._depth; _graphId = task._graphId;

        _minA  = task._minA;   _maxA = task._maxA;
        _minS  = task._minS;   _maxS = task._maxS;
        _minF  = task._minF;   _maxF = task._maxF;

        _isSource = task._isSource;
        _setExcl  = task._setExcl;
        _setExcl2 = task._setExcl2;
        _isTarget = task._isTarget;

        _delays = task._delays;
    }
    return *this;
}





bool Task::operator==(const Task & task)
{
    if (this == &task)
        return true;
    return _id == task._id && _graphId == task._graphId;
}



void Task::outInfo() const
{
    printf("\tid          = %d\n",        _id);
    printf("\tmajor_frame = %d\n",        _major_frame);
    printf("\tpriority    = %d\n",        _priority);
    printf("\tperiod      = %d\n",        _period);
    printf("\tgraph id    = %d\n",        _graphId);
    printf("\tdepth       = %d\n",        _depth);
    printf("\tprocessor number = %d\n",   _processorNum);
    printf("\tinstance number = %d\n",    _instanceNum);
    printf("\t[BCET, WCET] = [%d, %d]\n", _BCET, _WCET);
    printf("\t[minA, maxA] = [%d, %d]\n", _minA, _maxA);
    printf("\t[minS, maxS] = [%d, %d]\n", _minS, _maxS);
    printf("\t[minF, maxF] = [%d, %d]\n", _minF, _maxF);
    printf("\tis source task? %d\n",         _isSource);
    printf("\tis target task? %d\n",         _isTarget);

    printf("\tSet of tExcl: \n\t");
    for (auto & p : _tExcl)
        std::cout << p->_id << '\t';

    printf("\n");

    printf("\tlist of predecessors: \n\t");
    for (auto & p : _predecessors)
        std::cout << p->_id << '\t';
    printf("\n");

    printf("\tlist of successors: \n\t");
    for (auto & p : _successors)
        std::cout << p->_id << '\t';
    printf("\n");

    printf("\tSet of tPmtor: \n\t");
    for (auto & p : _tPmtor)
        std::cout << "[graph: " << p->_graphId << ',' << "task: " << p->_id << ']' << '\t';
    printf("\n");

    printf("\tSet of higher priority task: \n\t");
    for (auto & p : _hp)
        std::cout << "[graph: " << p->_graphId << ',' << "task: " << p->_id << ']' << '\t';
    printf("\n");

    printf("\n\n");
}
// end Defining methods for "Task" class



// Defining methods for "TaskGraph" class
TaskGraph::TaskGraph(int id, int P, int instanceNum, bool isTarget):
                     _id(id), _period(P), _instanceNum(instanceNum),
                     _isTarget(isTarget), _copyOf(id)
{
}


void TaskGraph::outInfo() const
{
    std::cout << "task graph id    = " << _id          << std::endl;
    std::cout << "Period           = " << _period      << std::endl;
    std::cout << "instance number  = " << _instanceNum << std::endl;
    std::cout << "Number of tasks  = " << size()       << std::endl;
    std::cout << "copy of graph    = " << _copyOf      << std::endl;

    if (_isTarget) { std::cout << "is target" << std::endl;  }
    else           { std::cout << "not target" << std::endl; }

    for (auto p = begin(); p != end(); p++)
    {
        p->second->outInfo();
    }
    printf("\n");
}
// end Defining methods for "TaskGraph" class


// Defining methods for "Processors" class
void Processors::outInfo() const
{
    for (auto p = begin(); p != end(); p++)
    {
        std::cout << "PE id = " << p->first << std::endl;
        std::cout << "Tasks: "  << std::endl;
        for (const auto & q : p->second)
        {
            printf("\tgraph id = %d , task id = %d\n", q._graphId, q._taskId);
        }
        std::cout << std::endl << std::endl;
    }
}
// end Defining methods for "Processors" class

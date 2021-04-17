import random as rand

class Node:
    def __init__(self):
        self.Inf = set()
        self.Dep = set()

def ocr(dig):
    a = "" +'"'
    a += str(dig)
    return a + '"'

node = {}
name = "data.xml"
f = open(name,'w')
f.write("<system>\n")
k = rand.randint(1, 20)
cores_num = rand.randint(1, 20)
task_num = 9*k -7
for i in range(0, task_num):
    node[i] = Node()
node_num = k
for i in range(0, k):
    n = k
    for j in range(0, k-1):
        node[i].Inf.add(n)
        node[n].Dep.add(i)
        n += 8
while(node_num < task_num - 1):
    node[node_num].Inf.add(node_num+1)
    node[node_num+1].Dep.add(node_num)
    node_num += 1
    first = node_num+1
    node[node_num].Inf.add(first)
    node[first].Dep.add(node_num)
    node_num = first
    node[first].Inf.add(node_num+1)
    node[node_num+1].Dep.add(first)
    node_num += 1
    node[node_num].Inf.add(node_num+1)
    node[node_num+1].Dep.add(node_num)
    node_num += 1
    second = node_num+1
    node[node_num].Inf.add(second)
    node[second].Dep.add(node_num)
    node_num = second
    node[second].Inf.add(task_num - 1)
    node[task_num - 1].Dep.add(second)
    node_num += 1
    node[first].Inf.add(node_num)
    node[first].Dep.add(node_num)
    node[node_num].Inf.add(node_num+1)
    node[node_num+1].Dep.add(node_num)
    node_num += 1
    node[node_num].Inf.add(second)
    node[second].Dep.add(node_num)
    node_num += 1

for c in range(0, task_num):
    inf  = ""
    dep = ""
    for i in node[c].Inf:
        inf += int((inf != ""))*',' + str(i)
    for i in node[c].Dep:
        dep += int((dep != ""))*',' + str(i)
    pr = rand.randint(0, cores_num)
    f.write('\t<task id='+ocr(c)+' prio='+ocr(c)+' proc='+ocr(pr)+' Dep="'+dep+'" Inf="'+inf+'" />\n')


for key in range(0, task_num):
    for j in node[key].Inf:
        f.write('\t<link src='+ocr(key)+' dst='+ocr(j)+' delay='+ocr(rand.randint(1, 100))+' />\n')

f.write("</system>\n")
f.close()

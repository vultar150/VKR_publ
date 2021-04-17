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
cores_num = rand.randint(1, 20)
node_num = task_num = rand.randint(40, 100) 
for i in range(0, task_num):
    node[i] = Node()
k = node_num // 4
for j in range(0, k):
    for o in range(k, node_num):
        p = rand.randint(1,101)
        if p <= 60:
            node[o].Inf.add(j)
            node[j].Dep.add(o)

for j in range(k, node_num):
    for o in range(k, node_num):
        c = False
        for y in node[o].Inf:
            if y >= k:
                c = True
                break
        p = rand.randint(1,101)
        if p <= 25 and j != o and not c and len(node[o].Dep) == 0:
            node[j].Inf.add(o)
            node[o].Dep.add(j)

for c in range(0, node_num):
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

import os as osy
import sys as system
import xml.dom.minidom as xm
import random as rand

def ocr(dig):
    a = "" +'"'
    a += str(dig)
    return a + '"'

def push(name,tree,mass):
    f = open(name,'w')
    f.write("<system>\n")
    for key in tree.keys():
        node = tree[key]
        inf = ""
        dep = ""
        for i in node[7]:
            inf += int((inf != ""))*',' + str(i)
        for i in node[8]:
            dep += int((dep != ""))*',' + str(i)
        f.write('\t<task id='+ocr(key)+' maj_fr='+ocr(node[0])+' prio='+ocr(node[4])+' bcet='+ocr(node[3])+' wcet='+ocr(node[2])+' period='+ocr(node[5])+' deadline='+ocr(node[6])+' proc='+ocr(node[1])+' Dep="'+dep+'" Inf="'+inf+'" />\n')
    for key in mass.keys():
        node = mass[key]
        f.write('\t<tlink src='+ocr(node[0])+' dist='+ocr(node[1])+' delay='+ocr(node[2])+' />\n')
    f.write("</system>\n")
    f.close()
    return None

def pop(name):
    tree = dict()
    mass = dict()
    f = open(name,'r')
    dom = xm.parse(name)
    dom.normalize()
    # maj_fr = rand.randint(1, 10000)
    maj_fr = 10000
    period = maj_fr
    tasks = dom.getElementsByTagName("task")
    for task in tasks:
        task_id = int(task.getAttribute("id"))
        wcet = rand.randint(200, 300)
        bcet = int(wcet/2)
        # wcet = int(2*wcet)
        prio = int(task.getAttribute("prio"))
        corenum = int(task.getAttribute("proc"))
        deadline = period
        tree[task_id] = [maj_fr,corenum,wcet,bcet,prio,period,deadline,set(),set()]
    links = dom.getElementsByTagName("link")
    tmp = 0
    for link in links:
        src = int(link.getAttribute("src"))
        dst = int(link.getAttribute("dst"))
        mass[tmp] = (src,dst,int(link.getAttribute("delay")))
        tree[src][7].add(dst)
        tree[dst][8].add(src)
        tmp += 1
    return tree, mass

fnamein = 'data.xml'
fnameout = 'input.xml'
tree, mass = pop(fnamein)
push(fnameout,tree,mass)

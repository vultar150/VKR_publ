import random as rand

def NOD(a,b):
    while b:
        a, b = b, a % b
    return a

def NOK(a,b):
    nok = a*b // NOD(a,b)
    return nok

def prob(a = 0, b = 10):
    return rand.randint(a,b)

class Task:
    def __init__(self,HW,id3,pr):
        self.id = id3
        self.proc = HW
        b = prob(50,100)
        # w = prob(10,200)
        w = prob(200,400)
        if b>w:
            w,b = b,w
        self.bcet = b
        self.wcet = w
        self.free = True
        self.per = 10000
        self.maj = self.per
        self.Inf = set()
        self.Dep = set()
        self.prio = pr

class core:
    def __init__(self,HW,HM,id3):
        self.Tasks = dict()
        pr = 0
        for j in range(HM):
            self.Tasks[id3+j] = Task(HW,id3,pr)
            pr += 1

def IsFree(Tree):
    for j in Tree.keys():
        if Tree[j].free:
            return j
    return -1

def Waterfall(Tree,id3,mass):
    leng = prob(1,20)
    id1 = id3
    #per = prob(1,100)
    #Tree[id3].per = per
    while 1:
        Tree[id1].free = False
        id2 = IsFree(Tree)
        if id2 == -1:
            break
        #Tree[id2].per = per
        Tree[id2].free = False
        Tree[id2].Dep.add(id1)
        Tree[id1].Inf.add(id2)
        mass += [(id1,id2,prob(0,10))]
        if prob(0,1) == 0:
            id1 = id2
        if prob(0,2) < 1: 
            break
    #print(mass)
    return Tree, mass

def UpTree(D1,D2):
    D3 = dict()
    for j in D1.keys():
        D3[j] = D1[j]
    for j in D2:
        D3[j] = D2[j]
    return D3

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
        for i in node.Inf:
            inf += int((inf != ""))*',' + str(i)
        for i in node.Dep:
            dep += int((dep != ""))*',' + str(i)
        f.write('\t<task id='+ocr(key)+' maj_fr='+ocr(node.maj)+' prio='+ocr(key)+' bcet='+ocr(node.bcet)+' wcet='+ocr(node.wcet)+' period='+ocr(node.per)+' deadline='+ocr(node.per)+' proc='+ocr(node.proc)+' Dep="'+dep+'" Inf="'+inf+'" />\n')
    for node in mass:
        #print(mass)
        f.write('\t<tlink src='+ocr(node[0])+' dist='+ocr(node[1])+' delay='+ocr(node[2])+' />\n')
    f.write("</system>\n")
    f.close()
    return None

Core = dict()
numcore = 2
# numcore = prob(3,7)
# cores_num = rand.randint(3, 7)
# cores_num = rand.randint(5, 20)
# cores_num = rand.randint(10, 40)
ind = 0
for j in range(numcore):
    HM = 3
    # HM = prob(1,20)
    Core[j] = core(j,HM,ind)
    ind += HM
Tree = dict()
for j in Core.keys():
    Tree = UpTree(Core[j].Tasks,Tree)
mass = []
while 1:
    id3 = IsFree(Tree)
    if id3 == -1:
        break
    Tree,mass = Waterfall(Tree,id3,mass)
#mfr = set()
#for j in Tree.keys():
#    if not (Tree[j].per in mfr):
#        mfr.add(Tree[j].per)
#major = 1
#for j in mfr:
#    major = NOK(j,major)
#if major > 20000:
#   pass
#for j in Tree.keys():
#    Tree[j].maj = major
push("input.xml",Tree,mass)
print("SUCCESS")

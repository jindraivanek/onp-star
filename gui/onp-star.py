# -*- coding: utf-8 -*-

#vlastnosti jednotlivych uloh (problemu)
class Uloha:
  def __init__(self, name, longname, zadaniMsg, basecmd, heurs, aproxs, graph, checkbox, m, columnLabels, globalVar, default, ranges, nezaporne=True, prb=1.0):
    self.name = name
    self.longname = longname
    self.zadaniMsg = zadaniMsg
    self.basecmd = basecmd
    self.heurs = heurs
    self.aproxs = aproxs
    self.graph = graph
    self.checkbox = checkbox
    self.m = m
    self.columnLabels = columnLabels
    self.globalVar = globalVar
    self.default = default
    self.ranges = ranges
    self.nezaporne = nezaporne
    self.prb = prb

ulohy = [
  Uloha("TSP", "Problém obchodního cestujícího (TSP)", "Zadání TSP maticí ohodnocených hran:", "./tsp", 3, 1, True, False, -1, [], [], 1.0, [[1.0, 10.0]], False),
  Uloha("VP", "Vrcholové pokrytí (VP)", "Zadání grafu pro VP trojúhelníkovou maticí sousednosti:", "./vp", 1, 2, True, True, -1, [], [], 1.0, [[1.0, 1.0]], False, 0.5),
  Uloha("PB", "Problém batohu (PB)", "Zadání objemů a cen předmětů pro PB:", "./pb", 1, 1, False, False, 2, ["objem", "cena"], [["velikost batohu:",[0.25,0.75],"velikost batohu (poměr vzhledem k celkovému objemu předmětů)"]], 1.0, [[1.0, 10.0], [1.0, 10.0]]),
  Uloha("RU", "Rozvrhování úloh (RU)", "Zadání úloh pro RU:", "./ru", 3, 2, False, False, 4, ["délka", "deadline", "penále", "koeficient"], [], 1.0, [[1.0, 10.0], [0.1, 0.9], [1.0, 10.0], [0.0, 10.0]])
]

import random, sys

#tabulka pro ulozeni zadani
class Table:
  def __init__(self, uloha):
    self.m = uloha.m
    self.graph = uloha.graph
    self.default = uloha.default
    self.checkbox = uloha.checkbox
    self.globalVar = [self.default for i in range(len(uloha.globalVar))]

  def Init(self, n):
    global uloha
    self.n = n
    if(self.graph == True): self.m = n
    self.t = [[self.default for i in range(self.m)] for i in range(self.n)]

  def RandomGraph(self, n, p, a, b):
    self.Init(n)
    for i in range(self.n):
      for j in range(self.n):
        if(self.checkbox==True and j<i): self.t[i][j] = self.t[j][i]
        if(i != j and (p == 1.0 or random.random() <= p)):
          self.t[i][j] = random.random() * (b-a) + a
        else: self.t[i][j] = -1

  def RandomRanges(self, n, ranges):
    global uloha
    self.Init(n)
    for i in range(self.n):
      for j in range(self.m):
        self.t[i][j] = random.random() * (ranges[j][1]-ranges[j][0]) + ranges[j][0]
        if(uloha.name=="RU" and j==1): 
          suma = sum(zip(*self.t)[0])
          self.t[i][j]*=suma
    for i in range(len(uloha.globalVar)):
      self.globalVar[i] = random.random() * (uloha.globalVar[i][1][1]-uloha.globalVar[i][1][0]) + uloha.globalVar[i][1][0]
      if(uloha.globalVar[i][0]=="velikost batohu:"):
        suma = sum(zip(*self.t)[0]) #secte prvni sloupec tabulky
        self.globalVar[i] *= suma
        
  def Random(self, n, uloha):
    self.Init(n)
    if(self.graph == True): self.RandomGraph(n, uloha.prb, uloha.ranges[0][0], uloha.ranges[0][1])
    else: self.RandomRanges(n, uloha.ranges)

  def Save(self, f):
    global uloha
    for i in range(len(uloha.globalVar)):
      f.write(str(self.globalVar[i])+"\n")
    f.write("%i\n" % self.n)
    for i in range(self.n):
      for j in range(self.m): 
        if(self.checkbox==True and i>j): f.write(str(self.t[j][i]) + " ")
        else: f.write(str(self.t[i][j]) + " ")
      f.write("\n")
    f.close()

  def Load(self, f):
    lines = f.readlines()
    for i in range(len(uloha.globalVar)):
      self.globalVar[i]=float(lines[i])
      lines = lines[1:]
    n = int(lines[0])
    self.Init(n)
    for i in range(self.n):
      l = lines[i + 1].split()
      for j in range(len(l)):
        if(self.checkbox==True and i>j): self.t[i][j] = self.t[j][i]
        else: self.t[i][j] = float(l[j])

  def Resize(self):
    if(len(self.t) < self.n): 
      self.t += [[self.default for i in range(self.m)] for i in range(self.n-len(self.t))]
    if(len(self.t) > self.n):
      self.t = self.t[:self.n]
    for i in range(len(self.t)):
      if(len(self.t[i]) < self.m):
        self.t[i] += [self.default for i in range(self.m-len(self.t[i]))]
      if(len(self.t[i]) > self.m):
        self.t[i] = self.t[i][:self.m]

  def SetVars(self, vars):
    for i in range(self.n):
      for j in range(self.m):
        vars[i][j].set(self.t[i][j])

#graf je tabulka n x n
class Graph(Table):
  def __init__(self, n):
    Table.__init__(self, n, True)

#nacteni GUI
from Tkinter import *
import tkFileDialog
import tkMessageBox
import re
from subprocess import *
import threading
import tempfile

def LabelWidget(frame, name, text, default, func, type, vars, widgets):
  f = Frame(frame)
  f.label = Label(f, text=text)
  f.label.pack(side=LEFT)
  if(type=="string"): vars[name] = StringVar();
  if(type=="int"): vars[name] = IntVar();
  if(type=="double"): vars[name] = DoubleVar();
  vars[name].set(default)
  f.widget = func(f, vars[name])
  f.widget.pack(side=LEFT)
  widgets[name] = f
  return f

def LabelEntry(frame, name, text, default, type, vars, widgets):
  return LabelWidget(frame, name, text, default, lambda f,v: Entry(f,textvariable=v), type, vars, widgets)

#informace o prubehu vypoctu
class Status:
  def __init__(self):
    self.vars = [None for i in range(10)]
    self.values = ["" for i in range(10)]
    self.strings = [
      "Počet expanzí: ",
      "Aktuální velikost haldy: ",
      "Odkládací mez: ",
      "Odložených položek: ",
      "Obsazená paměť (kB): ",
      "Dolní odhad: ",
      "Poměr: ",
      "Aproximace (horní odhad): ",
      "Nejlepší známé řešení: ",
      ""
    ]

  def update(self):
    for i in range(10):
      if(self.strings[i]==""): self.vars[i].set("")
      elif(self.values[i]): self.vars[i].set(self.strings[i] + self.values[i])

#spusti program a cte jeho vystup
class ThreadRun(threading.Thread):
  def __init__(self, cmd):
    threading.Thread.__init__(self)
    self.cmd = cmd
    self.log = open("onp-star.log", "w")
  def run(self):
    global status, kill, dialog_run
    self.process = Popen(self.cmd.split(" "), cwd="../bin", stdout=PIPE, stderr=STDOUT, bufsize=-1)
    while kill == False:
      l = self.process.stdout.readline()
      self.log.write(l)
      m = re.match(r"STATUS - expanzi: (\d+), open: (\d+), odklad\((\d+\.\d+)?\): (\d+), pamet: (\d+)kB, odhad: (\d+\.\d+), aprox\((\d+\.\d+)\): (\d+\.\d+)", l)
      if(m):
        for i in range(8):
          status.values[i] = m.group(i + 1)
      m = re.match(r"Aproximace: \d+\.\d+ \((.+)\)", l)
      if(m):
        for i in range(1):
          status.values[i + 8] = m.group(i + 1)
      m = re.match(r"Reseni( \(APROXIMACE\))?: (.+),", l)
      if(m):
        for i in range(1):
          status.values[i + 9] = m.group(i + 2)
        kill = True
      m = re.match(r"Error:(.*)", l)
      if(m):
        tkMessageBox.showinfo("Chyba","Chyba při běhu programu: "+m.group(1)) 
        kill = True
      status.update()
    self.log.close()
    self.process.kill()
    print "konec"
    dialog_run.btn_ok.configure(text="Zavřít", command=dialog_run.quit)
    if(status.values[9]!=""): dialog_run.reseni()
    del self

#dialog pro zobrazeni prubehu a vysledku vypoctu, zaroven i zobrazi okno se zadanim
class DialogRun:
  def __init__(self, parent, cmd):
    global status, zadaniFile
    self.cmd = cmd
    print cmd
    self.top = Toplevel(parent)
    self.zadani = Toplevel(parent)
    self.top.title("onp-star GUI")
    self.zadani.title("onp-star GUI (Prohlížeč zadání)")

    self.labels = []
    for i in (5,7,6,8,9,0,1,2,3,4):
      status.vars[i] = StringVar(); status.vars[i].set(status.strings[i])
      l = Label(self.top, textvariable=status.vars[i])
      l.pack(side=TOP)
      self.labels.append(l)

    self.btn_ok = Button(self.top, text="Zastavit výpočet", command=self.ok)
    self.btn_ok.pack(pady=5)

    self.t = TkTable(self.zadani, uloha)
    f = open(zadaniFile.name,"r")
    self.t.Load(f)
    self.t.Resize()
    for f in self.t.globalWidgets.itervalues():
      f.widget.configure(state=DISABLED, disabledbackground=f.widget["background"], disabledforeground=f.widget["foreground"])
    for r in self.t.widgets:
      for e in r:
        e.configure(state=DISABLED, disabledbackground=e["background"], disabledforeground=e["foreground"])

  def run(self):
    global kill
    kill = False
    self.thr = ThreadRun(self.cmd)
    self.thr.start()

  def reseni(self):
    global status
    #status.strings[0] = ""
    status.strings[1] = ""
    status.strings[2] = ""
    status.strings[3] = ""
    status.strings[4] = ""
    status.strings[5] = "OPTIMUM: "
    status.strings[6] = ""
    status.strings[7] = ""
    status.strings[8] = ""
    status.strings[9] = "OPTIMÁLNÍ ŘEŠENÍ: "
    status.update()

  def ok(self):
    global kill
    kill = True
    if(self.thr): del self.thr
    self.btn_ok.configure(text="Zavřít", command=self.quit)

  def quit(self):
    self.zadani.destroy()
    self.top.destroy()
    
#trida pro zobrazeni Table v GUI
class TkTable(Table):
  def __init__(self, master, uloha):
    Table.__init__(self, uloha)
    self.master = master
    self.f = Frame(master)
    self.f.grid_rowconfigure(0, weight=1)
    self.f.grid_columnconfigure(0, weight=1)

    self.xscrollbar = Scrollbar(self.f, orient=HORIZONTAL)
    self.xscrollbar.grid(row=1, column=0, sticky=E+W)

    self.yscrollbar = Scrollbar(self.f)
    self.yscrollbar.grid(row=0, column=1, sticky=N+S)
    self.canvas = Canvas(self.f, yscrollcommand=self.yscrollbar.set, xscrollcommand=self.xscrollbar.set)
    self.canvas.grid(row=0, column=0, sticky=N+S+E+W)
    self.yscrollbar.config(command=self.canvas.yview)
    self.xscrollbar.config(command=self.canvas.xview)

    self.frame = Frame(self.canvas)
    self.globalFrame = Frame(master)
    self.globalVars = {}
    self.globalWidgets = {}
    for i in range(len(uloha.globalVar)):
      LabelEntry(self.globalFrame, "global"+str(i), uloha.globalVar[i][0], uloha.default, "double", self.globalVars, self.globalWidgets).pack()
    self.globalFrame.pack(side=TOP)
    self.Init(uloha.m)
    self.Resize()
    self.f.pack()

  def FromTable(self):
    for i in range(len(self.globalVar)):
      self.globalVars["global"+str(i)].set(self.globalVar[i])
    for i in range(self.n):
      for j in range(self.m):
        self.vars[i][j].set(self.t[i][j])
  
  def ToTable(self):
    for i in range(len(self.globalVars)):
      self.globalVar[i] = self.globalVars["global"+str(i)].get()
    for i in range(self.n):
      for j in range(self.m):
        self.t[i][j] = self.vars[i][j].get()
  
  def Resize(self):
    global uloha
    Table.Resize(self)
    self.frame.destroy()
    self.frame = Frame(self.canvas)
    self.frame.rowconfigure(1, weight=1)
    self.frame.columnconfigure(1, weight=1)

    self.vars = [[DoubleVar() for i in range(self.m)] for i in range(self.n)]
    if(self.checkbox==True):
      self.widgets = [[Checkbutton(self.frame, variable=self.vars[i][j], onvalue=1.0, offvalue=-1.0, width=2) for j in range(self.m)] for i in range(self.n)]
    else:
      self.widgets = [[Entry(self.frame, textvariable=self.vars[i][j], width=4) for j in range(self.m)] for i in range(self.n)]
    for w in self.globalWidgets.itervalues(): w.pack()
    for i in range(self.n):
      Label(self.frame, text=str(i)).grid(row=i+1,column=0)
      for j in range(self.m):
        if(self.graph==True): Label(self.frame, text=str(j)).grid(row=0, column=j+1)
        else: Label(self.frame, text=uloha.columnLabels[j]).grid(row=0, column=j+1)
        if(not( (self.graph==True and i==j) or (self.checkbox==True and j<i) ) ): 
          self.widgets[i][j].grid(row=i+1, column=j+1)
    self.FromTable()
    self.frame.update_idletasks()
    bb=self.frame.grid_bbox()
    self.canvas.configure(width=bb[2]+35, height=bb[3]+35)
    self.canvas.create_window(0, 0, anchor=NW, window=self.frame)
    self.canvas.config(scrollregion=self.canvas.bbox("all"))

  def Load(self, f):
    Table.Load(self, f)
    self.Resize()
    self.FromTable()

  def Save(self, f):
    self.ToTable()
    Table.Save(self, f)

  def Validate(self, nezaporne=False):
    try:
      for v in self.globalVars.values():
        x=v.get()
        if(nezaporne==True and x<0): raise(ValueError)
      for r in self.vars:
        for v in r: 
          x=v.get()
          if(nezaporne==True and x<0): raise(ValueError)
    except ValueError:
      tkMessageBox.showinfo("Chyba","Vstup není korektní!") 
      return False
    return True

#virtualni trida pro obsah hlavniho okna
class View:
  def __init__(self, parent):
    self.frame = Frame(parent)
    self.widgets = {}
    self.vars = {}
    self.cmd = ""
    self.frame.pack()

  def LabelWidget(self, name, text, default, func, type="string"):
    return LabelWidget(self.frame, name, text, default, func, type, self.vars, self.widgets)
  
  def LabelEntry(self, name, text, default, type="string"):
    return self.LabelWidget(name, text, default, lambda f,v: Entry(f,textvariable=v), type)

  def LabelOptionMenu(self, name, text, default, type, opts):
    return self.LabelWidget(name, text, default, lambda f,v: OptionMenu(f,v,*opts), type)

  def destroy(self):
    self.frame.destroy()

#vyber problemu
class VyberView(View):
  def __init__(self, frame):
    global ulohy
    View.__init__(self, frame)

    self.uloha = IntVar()
    self.uloha.set(0) # initialize

    for i in range(len(ulohy)):
      b = Radiobutton(self.frame, text=ulohy[i].longname,
                      variable=self.uloha, value=i)
      b.pack()

#zadani problemu
class ZadaniView(View):
  def __init__(self, frame):
    global uloha
    View.__init__(self, frame)
    self.btns = Frame(self.frame)
    self.btnLoad = Button(self.btns, text="Načti zadání", command=self.Load)
    self.btnLoad.pack(side=LEFT)

    self.btnGen = Button(self.btns, text="Vygeneruj náhodné zadání", command=self.Generate)
    self.btnGen.pack(side=LEFT)

    self.btnSave = Button(self.btns, text="Ulož zadání", command=self.Save)
    self.btnSave.pack(side=LEFT)
    self.btns.pack(side=TOP)

    self.LabelEntry("size", "Velikost úlohy", 20, "int")
    self.btnChange = Button(self.widgets["size"], text="Zmeň velikost", command=self.ChangeSize)
    self.btnChange.pack(side=LEFT)
    self.widgets["size"].pack()

    Label(self.frame, text=uloha.zadaniMsg).pack()

    self.t = TkTable(self.frame, uloha)
    self.t.Random(20, uloha)
    self.t.Resize()

  def ChangeSize(self):
    if(not self.Validate()): return
    self.t.n = self.vars["size"].get()
    if(self.t.graph==True): self.t.m = self.t.n
    self.t.Resize()

  def Load(self):
    f = tkFileDialog.askopenfile()
    self.t.Load(f)
    self.t.frame.pack()
    self.vars["size"].set(self.t.n)

  def Generate(self):
    global uloha;
    if(not self.Validate()): return
    d = DialogGenerate(self.frame)
    self.frame.wait_window(d.top)
    self.t.Random(int(self.vars["size"].get()), uloha)
    self.t.Resize()
    self.t.FromTable()

  def Save(self):
    if(not self.Validate()): return
    f = tkFileDialog.asksaveasfile()
    self.t.Save(f)

  def Validate(self):
    global uloha
    try: 
      s = int(self.vars["size"].get())
      if(s < 1): raise(ValueError)
    except(ValueError):
      tkMessageBox.showinfo("Chyba","Vstup není korektní!") 
      return False
    if(not self.t.Validate(uloha.nezaporne)): return False
    return True 

#nastaveni hodnot pro generovani nahodneho zadani
class DialogGenerate:
  def __init__(self, parent):
    global uloha
    self.top = Toplevel(parent)
    if(uloha.checkbox==True):
      self.columns = 0
      self.text = "Pravděpodobnost výskytu hrany:"
      self.varPrb = DoubleVar(value=uloha.prb)
    elif(uloha.graph==True):
      self.columns = 1
      self.text = "Rozmezí ohodnocení hran grafu:"
    else:
      self.columns = uloha.m
      self.text = "Rozmezí hodnot jednotlivých sloupců:"
    self.vars = [[DoubleVar(value=uloha.ranges[i][j]) for j in range(2)] for i in range(self.columns)]
    self.globalVars = [[DoubleVar(value=uloha.globalVar[i][1][j]) for j in range(2)] for i in range(len(uloha.globalVar))]
    for i in range(len(uloha.globalVar)):
      radek = Frame(self.top)
      Label(radek, text=uloha.globalVar[i][2]).pack()
      for j in range(2):
        Entry(radek, textvariable=self.globalVars[i][j]).pack()
      radek.pack()
    Label(self.top, text=self.text).pack()
    for i in range(self.columns):
      radek = Frame(self.top)
      if(uloha.graph==False): Label(radek, text=uloha.columnLabels[i]).pack()
      for j in range(2):
        Entry(radek, textvariable=self.vars[i][j]).pack()
      radek.pack()
    if(uloha.checkbox==True):
      Entry(self.top, textvariable=self.varPrb).pack()

    b = Button(self.top, text="OK", command=self.ok)
    b.pack(pady=5)

  def ok(self):
    global uloha
    if(not self.Validate(uloha.nezaporne)): return
    self.top.destroy()
    if(uloha.checkbox==True): uloha.prb = self.varPrb.get()
    for i in range(len(self.globalVars)):
      for j in range(2):
        uloha.globalVar[i][1][j] = self.globalVars[i][j].get()
    for i in range(self.columns):
      for j in range(2):
        uloha.ranges[i][j] = self.vars[i][j].get()

  def Validate(self, nezaporne=False):
    try:
      if(uloha.checkbox==True): 
        x=self.varPrb.get()
        if(x < 0 or x > 1): raise(ValueError)
      for r in self.globalVars:
        x=r[0].get(); y=r[1].get()
        if(y < x): raise(ValueError)
        if(nezaporne==True and (x<0 or y<0)): raise(ValueError)
      for r in self.vars:
        x=r[0].get(); y=r[1].get()
        if(y < x): raise(ValueError)
        if(nezaporne==True and (x<0 or y<0)): raise(ValueError)
    except ValueError:
      tkMessageBox.showinfo("Chyba","Vstup není korektní!") 
      return False
    return True

#zadani parametru pro beh problemu
class NastaveniView(View):
  def __init__(self, frame):
    global uloha
    View.__init__(self, frame)

    self.LabelOptionMenu("heur", "Heuristika", uloha.heurs, "int", range(uloha.heurs+1)).pack()
    self.LabelOptionMenu("aprox", "Aproximace", uloha.aproxs, "int", range(uloha.aproxs+1)).pack()
    self.LabelEntry("heapMin", "Minimální velikost haldy", "50000").pack()
    self.LabelEntry("heapMax", "Maximální velikost haldy", "100000").pack()
    self.LabelEntry("memMax", "Maximální velikost paměti pro B-strom (MB)", "100").pack()

  def Validate(self):
    try: 
      d = int(self.vars["heapMin"].get())
      u = int(self.vars["heapMax"].get())
      m = int(self.vars["memMax"].get())
      if(d < 0 or u < 0 or m < 1 or d > u): raise(ValueError)
    except(ValueError):
      tkMessageBox.showinfo("Chyba","Vstup není korektní!") 
      return False
    return True 

#hlavni okno ve stylu "pruvodce"
class App:
  def __init__(self, master):
    self.master = master
    self.master.title("onp-star GUI")
    self.master.minsize(400, 200)

    self.view = None
    self.step = 0

    self.frame = Frame(master)
    self.frame.pack()

    self.button = Button(self.frame, text="QUIT", fg="red", command=self.frame.quit)
    self.button.pack(side=LEFT)

    self.next = Button(self.frame, text="Další >", command=self.Next)
    self.next.pack(side=LEFT)

    self.Next()

  def Next(self):
    global uloha, ulohy, zadaniFile
    if(self.step == 0):
      if(self.view != None): self.view.destroy()
      self.view = VyberView(self.master)
      self.next["text"] = "Další >"
      self.next["command"] = self.Next
      self.step = 1
    elif(self.step == 1):
      uloha = ulohy[self.view.uloha.get()]
      if(self.view != None): self.view.destroy()
      self.view = ZadaniView(self.master)
      self.step = 2
    elif(self.step == 2):
      if(not self.view.t.Validate(uloha.nezaporne)): return
      self.size = self.view.t.n
      zadaniFile = tempfile.NamedTemporaryFile("w", delete=False)
      self.view.t.Save(zadaniFile)
      if(self.view != None): self.view.destroy()
      self.view = NastaveniView(self.master)
      self.next["text"] = "Spusť"
      self.next["command"] = self.Run
      self.next.update()
      self.step = 3

#spusteni programu
  def Run(self):
    global uloha, dialog_run, status
    if(not self.view.Validate()): return
    status = Status()
    self.master.withdraw()
    self.cmd = uloha.basecmd + " -f " + zadaniFile.name \
      + " -d " + self.view.vars["heapMin"].get() \
      + " -u " + self.view.vars["heapMax"].get() \
      + " -m " + self.view.vars["memMax"].get() \
      + " -h " + str(self.view.vars["heur"].get()) \
      + " -a " + str(self.view.vars["aprox"].get())
    dialog_run = DialogRun(self.master, self.cmd)
    dialog_run.run()
    self.master.wait_window(dialog_run.top)
    del dialog_run

    self.step=0
    self.Next()
    self.master.deiconify()

if __name__ == '__main__':
  root = Tk()
  app = App(root)
  root.mainloop()

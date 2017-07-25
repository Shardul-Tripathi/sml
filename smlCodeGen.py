import sys
from antlr4 import *
from smlLexer import smlLexer
from smlParser import smlParser
from smlAST import *

const = 4
cdict = { # TODO: Convert pair to string
            ('y','a') : 'PutY2AGate',
            ('y','b') : 'PutY2BGate',
            ('a','y') : 'PutA2YGate',
            ('a','b') : 'PutA2BGate',
            ('b','a') : 'PutB2AGate',
            ('b','y') : 'PutB2YGate'
}

def shareName(name, circ):
    return 's_' + circ[0] + '_' + name

class smlCodeGen:
    def __init__(self):
        self.dict = {}  # name -> ['s_a_name', 's_b_name', 's_y_name'] None -> not declared, -1 -> dirty
        self.arrDict = {} # name -> refctx
        self.counter = 0
        self.defckt = 'bcirc'
        self.circ = 'acirc'
        self.file = open('output.txt', 'w')
        print('ABYParty *party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);', file=self.file)
        print('vector<Sharing*>& sharings = party->GetSharings();', file=self.file)
        print('Circuit* ycirc = sharings[S_YAO]->GetCircuitBuildRoutine();', file=self.file)
        print('Circuit* acirc = sharings[S_ARITH]->GetCircuitBuildRoutine();', file=self.file)
        print('Circuit* bcirc = sharings[S_BOOL]->GetCircuitBuildRoutine();', file=self.file)

# TODO: remove circ and use sname only
    def putInDict(self, name, sname, circ): # ident name to share name
        if name not in self.dict:
            self.dict[name] = [None, None, None]
        if circ[0] == 'a':
            self.dict[name][0] = sname
        elif circ[0] == 'b':
            self.dict[name][1] = sname
        else:
            self.dict[name][2] = sname

    def getIndex(self, circ):
        if circ[0] == 'a':
            return 0
        elif circ[0] == 'b':
            return 1
        return 2

    def convertIdent(self, sname, circ): # convert share name to circ share
        if sname[2] == circ[0]:
            return
        i = self.getIndex(circ)
        name = sname[4:]
        tgt = self.dict[name][i]
        if tgt != None and tgt != -1:
            return
        pair = (sname[2], circ[0])
        x = ''
        if pair == ('a','b'):
            x = ',ycirc'
        elif pair == ('y','a'):
            x = ',bcirc'
        tname = 's_' + circ[0] + sname[3:]
        newshare = ''
        if tgt == None:
            newshare = 'share *'
        print(newshare + tname,'=',circ+'->'+cdict[pair]+'(',sname,x+');', file=self.file)
        self.putInDict(name, tname, circ)

    def convertArr(self, sname, circ):
        if sname[2] == circ[0]:
            return
        i = self.getIndex(circ)
        name = sname[4:]
        tgt = self.dict[name][i]
        if tgt != None and tgt != -1:
            return
        pair = (sname[2], circ[0])
        tmp = ''
        if pair == ('a','b'):
            tmp = ',ycirc'
        elif pair == ('y','a'):
            tmp = ',bcirc'
        tname = 's_' + circ[0] + sname[3:]
        refctx = self.arrDict[name]
        ref = refctx.getText()
        if tgt == None:
            print('share *'+tname+ref,';', file=self.file)
        for x in range(1,refctx.getChildCount(),3):
            num = refctx.getChild(x)
            print('for (int _i'+str(x),'= 0; _i'+str(x),'<',num,'; _i'+str(x)+'++) {', file=self.file)
        ind = ''
        for x in range(1,refctx.getChildCount(),3):
            ind += '[_i' + str(x) + ']'
        print(tname+ind,'=',circ+'->'+cdict[pair]+'(',sname+ind,tmp+');', file=self.file)
        for x in range(1,refctx.getChildCount(),3):
            print('}', file=self.file)
        self.putInDict(name, tname, circ)

    def checkType(self, sname, circ):
        if sname[4:] in self.arrDict:
            self.convertArr(sname, circ)
        else:
            self.convertIdent(sname, circ)

    def checkOps(self, node):
        if isinstance(node, UnOp):
            if node.op == '!':
                return True
            else:
                return self.checkOps(node.expr)
        elif isinstance(node, Ident) or isinstance(node, ArrayPub) or isinstance(node, Constant):
            return False
        elif isinstance(node, BinOp):
            if node.op == '>' or node.op == '<' or node.op == '&&' or node.op == '&' or node.op == '||' or node.op == '|' or node.op == '^':
                return True
            else:
                return self.checkOps(node.lhs) or self.checkOps(node.rhs) # python supports short circuiting
        return False

    def checkAssign(self, node):
        rhs = node.rhs
        if isinstance(rhs, Ident) or isinstance(rhs, ArrayPub) or isinstance(rhs, Constant) or isinstance(rhs, InpExpr):
            return False
        elif isinstance(rhs, CondExpr):
            return True
        else:
            return self.checkOps(rhs)

    def convertExpr(self, expr, circ):
        if isinstance(expr, Ident):
            tmpvar = expr.name
            i = self.getIndex(circ)
            varShares = self.dict[tmpvar]
            if varShares[i] != None and varShares[i] != -1:
                return
            existing = next((item for item in varShares if item is not None and item != -1), 'No shares exist for '+expr.name)
            self.convertIdent(existing, circ)
        elif isinstance(expr, ArrayPub):
            tmpvar = expr.idname.name
            i = self.getIndex(circ)
            varShares = self.dict[tmpvar]
            if varShares[i] != None and varShares[i] != -1:
                return
            existing = next((item for item in varShares if item is not None and item != -1), 'No shares exist for '+expr.idname.name)
            self.convertArr(existing, circ)
        elif isinstance(expr, CondExpr):
            self.convertExpr(expr.condition, circ)
            self.convertExpr(expr.expr1, circ)
            self.convertExpr(expr.expr2, circ)
        elif isinstance(expr, UnOp):
            self.convertExpr(expr.expr, circ)
        elif isinstance(expr, BinOp):
            self.convertExpr(expr.lhs, circ)
            self.convertExpr(expr.rhs, circ)
        else:
            pass

    def checkBool(self, block):
        foundBool = False
        for x in block.children:
            if isinstance(x, Assign):
                foundBool |= self.checkAssign(x)
            elif isinstance(x, ForLoop):
                foundBool |= self.checkBool(x.block)
            else:
                foundBool |= self.checkBool(x)
            if foundBool:
                return True
        return False

    def convertAll(self, block, circ):
        for x in block.children:
            if isinstance(x, Assign):
                self.convertExpr(x.rhs, circ)
            elif isinstance(x, ForLoop):
                self.convertAll(x.block, circ)
            else:
                self.convertAll(x, circ)

    def codeGen(self, node, insideFor, circ=None, offset=0):
        if isinstance(node, CommSeq):
            for com in node.children:
                self.codeGen(com, insideFor, self.circ)

        elif isinstance(node, Block):
            print(' '*offset + '{', file=self.file)
            for bcom in node.children:
                self.codeGen(bcom, insideFor, circ, offset+const)
            print(' '*offset + '}', file=self.file)

        elif isinstance(node, Decl):
            if circ == None:
                circ = self.circ
            if isinstance(node.idname, Ident):
                name = node.idname.name
                sname = shareName(name, circ)
                self.putInDict(name, sname, circ)
            else:
                name = node.idname.idname.name + node.idname.ref
                sname = shareName(name, circ)
                arrname = node.idname.idname.name
                self.arrDict[arrname] = node.idname.refctx
                self.putInDict(arrname, 's_'+circ[0]+'_'+arrname, circ)
            print(' '*offset + node.data_type.idtype, name + ';', file=self.file)
            print(' '*offset + 'share *' + sname + ';', file=self.file)

        elif isinstance(node, Ident): # should always get a circ
            shareList = self.dict[node.name]
            existing = next((item for item in shareList if item is not None and item != -1), 'No shares exist for '+node.name)
            i = self.getIndex(circ)
            if shareList[i] != None and shareList[i] != -1:
                return shareList[i]
            self.convertIdent(existing, circ)
            return shareList[i]

        elif isinstance(node, ArrayPub): # should always get a circ
            shareList = self.dict[node.idname.name]
            ref = node.ref
            existing = next((item for item in shareList if item is not None and item != -1), 'No shares exist for '+node.idname.name)
            i = self.getIndex(circ)
            if shareList[i] != None and shareList[i] != -1:
                return shareList[i]+ref
            self.convertArr(existing, circ)
            return shareList[i]+ref

        elif isinstance(node, ForLoop):
            if not insideFor:
                foundBool = self.checkBool(node.block)
                if foundBool:
                    circ = self.defckt
                    self.convertAll(node.block, circ)
                else:
                    circ = 'acirc'
                    self.convertAll(node.block, circ)
            name = node.idname.name
            if node.step == '1':
                step = name + '++'
            else:
                step = name + ' += ' + node.step
            print(' '*offset+'for (uint32_t',name,'=',node.start+';',name,'<',node.end+';',step+')', file=self.file)
            self.codeGen(node.block, True, circ, offset)

        elif isinstance(node, Constant): # should always get a circ
            tmpvar = '_tmp_'+str(self.counter)
            varname = shareName(tmpvar, circ)
            self.counter += 1
            print(' '*offset+node.idtype, tmpvar, '=', node.value,';', file=self.file)
            print(' '*offset+'share *',varname,'=',circ+'->PutCONSGate(',tmpvar,',bitlen);', file=self.file)
            self.putInDict(tmpvar, varname, circ)
            return varname

        elif isinstance(node, UnOp): # should always get a circ
            tmpvar = 'tmp_'+str(self.counter)
            varname = shareName(tmpvar, circ)
            self.counter += 1
            expr = self.codeGen(node.expr, insideFor, circ, offset) # check expr type
            self.checkType(expr, circ)
            print(' '*offset+'share *',varname,'=',circ+'->PutINVGate(', expr,');', file=self.file)
            self.putInDict(tmpvar, varname, circ)
            return varname

        elif isinstance(node, BinOp):
            tmpvar = 'tmp_'+str(self.counter)
            self.counter += 1
            if node.op == '+':
                if not insideFor or circ is None:
                    circ = 'acirc'
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutADDGate(',lhs,',',rhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            elif node.op == '*':
                if not insideFor or circ is None:
                    circ = 'acirc'
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutMULGate(',lhs,',',rhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            elif node.op == '-':
                if not insideFor or circ is None:
                    circ = 'acirc'
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutSUBGate(',lhs,',',rhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            elif node.op == '>':
                if not insideFor or circ is None:
                    circ = self.defckt
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutGTGate(',lhs,',',rhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            elif node.op == '<':
                if not insideFor or circ is None:
                    circ = self.defckt
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutGTGate(',rhs,',',lhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            elif node.op == '&&' or node.op == '&':
                if not insideFor or circ is None:
                    circ = self.defckt
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutANDGate(',lhs,',',rhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            elif node.op == '||' or node.op == '|':
                if not insideFor or circ is None:
                    circ = self.defckt
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutORGate(',lhs,',',rhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            elif node.op == '^':
                if not insideFor or circ is None:
                    circ = self.defckt
                varname = shareName(tmpvar, circ)
                lhs = self.codeGen(node.lhs, insideFor, circ, offset)
                rhs = self.codeGen(node.rhs, insideFor, circ, offset)
                self.checkType(lhs, circ)
                self.checkType(rhs, circ)
                print(' '*offset+'share *',varname,'=',circ+'->PutXORGate(',lhs,',',rhs,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)
            else:
                print('I will do this later:', node.op, file=self.file)
            return varname

        elif isinstance(node, Assign):
            if isinstance(node.lhs, Ident):
                lhsArr = False
                tmpvar = node.lhs.name
            else:
                lhsArr = True
                tmpvar = node.lhs.idname.name
            rhs = node.rhs
            varShares = self.dict[tmpvar]
            if isinstance(rhs, Ident):
                if not insideFor or circ is None:
                    circ = self.circ
                varname = shareName(tmpvar, circ)
                if lhsArr:
                    varname += node.lhs.ref
                    tmpvar += node.lhs.ref
                i = self.getIndex(circ)
                rhsShareList = self.dict[rhs.name]
                existing = next((item for item in rhsShareList if item is not None and item != -1), 'No shares exist for '+rhs.name)
                if rhsShareList[i] is None or rhsShareList[i] == -1:
                    self.convertIdent(existing, circ)
                if varShares[i] is None:
                    print(' '*offset+'share *'+varname,'= create_new_share('+rhsShareList[i]+'->get_wire_ids(),',circ,');', file=self.file)
                else:
                    print(' '*offset+varname,'= create_new_share('+rhsShareList[i]+'->get_wire_ids(),',circ,');', file=self.file)

            elif isinstance(rhs, ArrayPub):
                if not insideFor or circ is None:
                    circ = self.circ
                varname = shareName(tmpvar, circ)
                if lhsArr:
                    varname += node.lhs.ref
                    tmpvar += node.lhs.ref
                i = self.getIndex(circ)
                rhsShareList = self.dict[rhs.idname.name]
                existing = next((item for item in rhsShareList if item is not None and item != -1), 'No shares exist for '+rhs.idname.name)
                if rhsShareList[i] is None or rhsShareList[i] == -1:
                    self.convertArr(existing, circ)
                copyshare = rhsShareList[i] + rhs.ref
                if varShares[i] is None:
                    print(' '*offset+'share *'+varname,'= create_new_share('+copyshare+'->get_wire_ids(),',circ,');', file=self.file)
                else:
                    print(' '*offset+varname,'= create_new_share('+copyshare+'->get_wire_ids(),',circ,');', file=self.file)

            elif isinstance(rhs, Constant):
                if not insideFor or circ is None:
                    circ = self.circ
                varname = shareName(tmpvar, circ)
                if lhsArr:
                    varname += node.lhs.ref
                    tmpvar += node.lhs.ref
                i = self.getIndex(circ)
                print(' '*offset+tmpvar,'=',rhs.value,';', file=self.file)
                if varShares[i] is None:
                    print(' '*offset+'share *'+varname,'=',circ+'->PutCONSGate(',tmpvar,',bitlen);', file=self.file)
                else:
                    print(' '*offset+varname,'=',circ+'->PutCONSGate(',tmpvar,',bitlen);', file=self.file)

            elif isinstance(rhs, BinOp):
                if rhs.op == '+':
                    if not insideFor or circ is None:
                        circ = 'acirc'
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutADDGate(',binlhs,',',binrhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutADDGate(',binlhs,',',binrhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                elif rhs.op == '*':
                    if not insideFor or circ is None:
                        circ = 'acirc'
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutMULGate(',binlhs,',',binrhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutMULGate(',binlhs,',',binrhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                elif rhs.op == '-':
                    if not insideFor or circ is None:
                        circ = 'acirc'
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutSUBGate(',binlhs,',',binrhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutSUBGate(',binlhs,',',binrhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                elif rhs.op == '>':
                    if not insideFor or circ is None:
                        circ = self.defckt
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutGTGate(',binlhs,',',binrhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutGTGate(',binlhs,',',binrhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                elif rhs.op == '<':
                    if not insideFor or circ is None:
                        circ = self.defckt
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutGTGate(',binrhs,',',binlhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutGTGate(',binrhs,',',binlhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                elif rhs.op == '&&' or rhs.op == '&':
                    if not insideFor or circ is None:
                        circ = self.defckt
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutANDGate(',binlhs,',',binrhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutANDGate(',binlhs,',',binrhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                elif rhs.op == '||' or rhs.op == '|':
                    if not insideFor or circ is None:
                        circ = self.defckt
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutORGate(',binlhs,',',binrhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutORGate(',binlhs,',',binrhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                elif rhs.op == '^':
                    if not insideFor or circ is None:
                        circ = self.defckt
                    varname = shareName(tmpvar, circ)
                    if lhsArr:
                        varname += node.lhs.ref
                        tmpvar += node.lhs.ref
                    i = self.getIndex(circ)
                    binlhs = self.codeGen(rhs.lhs, insideFor, circ, offset)
                    binrhs = self.codeGen(rhs.rhs, insideFor, circ, offset)
                    self.checkType(binlhs, circ)
                    self.checkType(binrhs, circ)
                    if varShares[i] is None:
                        print(' '*offset+'share *',varname,'=',circ+'->PutXORGate(',binlhs,',',binrhs,');', file=self.file)
                    else:
                        print(' '*offset+varname,'=',circ+'->PutXORGate(',binlhs,',',binrhs,');', file=self.file)
                    self.putInDict(tmpvar, varname, circ)
                else:
                    print('I will do this later:', node.op, file=self.file)

            elif isinstance(rhs, UnOp):
                if not insideFor or circ is None:
                    circ = self.circ
                varname = shareName(tmpvar, circ)
                if lhsArr:
                    varname += node.lhs.ref
                    tmpvar += node.lhs.ref
                i = self.getIndex(circ)
                expr = self.codeGen(rhs.expr, insideFor, circ, offset) # check expr type
                self.checkType(expr, circ)
                if varShares[i] is None:        
                    print(' '*offset+'share *',varname,'=',circ+'->PutINVGate(', expr,');', file=self.file)
                else:
                    print(' '*offset+varname,'=',circ+'->PutINVGate(', expr,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)

            elif isinstance(rhs, CondExpr):
                if not insideFor or circ is None:
                    circ = self.defckt
                varname = shareName(tmpvar, circ)
                if lhsArr:
                    varname += node.lhs.ref
                    tmpvar += node.lhs.ref
                i = self.getIndex(circ)
                sel = self.codeGen(rhs.condition, insideFor, circ, offset)
                ina = self.codeGen(rhs.expr1, insideFor, circ, offset)
                inb = self.codeGen(rhs.expr2, insideFor, circ, offset)
                self.checkType(sel, circ)
                self.checkType(ina, circ)
                self.checkType(inb, circ)
                if varShares[i] is None:
                    print(' '*offset+'share *',varname,'=',circ+'->PutMUXGate(',ina,',',inb,',',sel,');', file=self.file)
                else:
                    print(' '*offset+varname,'=',circ+'->PutMUXGate(',ina,',',inb,',',sel,');', file=self.file)
                self.putInDict(tmpvar, varname, circ)

            elif isinstance(rhs, InpExpr):
                if not insideFor or circ is None:
                    circ = self.circ
                varname = shareName(tmpvar, circ)
                if lhsArr:
                    varname += node.lhs.ref
                    tmpvar += node.lhs.ref
                i = self.getIndex(circ)
                print(' '*offset+tmpvar,'=',rhs.exprText,';', file=self.file)
                if varShares[i] is None:
                    print(' '*offset+'share *',varname,';')
                if rhs.partynum == 'input1':
                    print(' '*offset+'if (role == SERVER) {', file=self.file)
                    print(' '*(offset+const)+varname,'=',circ+'->PutINGate(',tmpvar,',bitlen,SERVER);', file=self.file)
                    print(' '*offset+'} else {', file=self.file)
                    print(' '*(offset+const)+varname,'=',circ+'->PutDummyINGate(bitlen);\n'+' '*offset+'}', file=self.file)
                else:
                    print(' '*offset+'if (role == CLIENT) {', file=self.file)
                    print(' '*(offset+const)+varname,'=',circ+'->PutINGate(',tmpvar,',bitlen,CLIENT);', file=self.file)
                    print(' '*offset+'} else {', file=self.file)
                    print(' '*(offset+const)+varname,'=',circ+'->PutDummyINGate(bitlen);\n'+' '*offset+'}', file=self.file)
                self.putInDict(tmpvar, varname, circ)

            else:
                print('I should not be here', file=self.file)
            self.circ = circ
            # INVALIDATE LHS
            for x in range(0,3):
                if x != i and varShares[x] != None:
                    varShares[x] = -1

        elif isinstance(node, CondExpr):
            if not insideFor or circ is None:
                circ = self.defckt
            tmpvar = 'tmp_'+str(self.counter)
            varname = shareName(tmpvar, circ)
            self.counter += 1
            sel = self.codeGen(node.condition, insideFor, circ, offset)
            ina = self.codeGen(node.expr1, insideFor, circ, offset)
            inb = self.codeGen(node.expr2, insideFor, circ, offset)
            self.checkType(sel, circ)
            self.checkType(ina, circ)
            self.checkType(inb, circ)
            print(' '*offset+'share *',varname,'=',circ+'->PutMUXGate(',ina,',',inb,',',sel,');', file=self.file)
            self.putInDict(tmpvar, varname, circ)
            return varname

        elif isinstance(node, OutExpr):
            tmpvar = 'tmp_'+str(self.counter)
            self.counter += 1
            expr = self.codeGen(node.expr, insideFor, circ, offset=offset)
            exprShares = self.dict[expr[4:]]
            if exprShares[0] != None and exprShares[0] != -1:
                circ = 'acirc'
            elif exprShares[1] != None and exprShares[1] != -1:
                circ = 'bcirc'
            elif exprShares[2] != None and exprShares[2] != -1:
                circ = 'ycirc'
            else:
                print('Invalid share')
                return
            varname = shareName(tmpvar, circ)
            print(' '*offset+'share *',varname,'=',circ+'->PutOUTGate(',expr,', ALL);', file=self.file)
            self.putInDict(tmpvar, varname, circ)
            print(' '*offset+'party->ExecCircuit();', file=self.file)
            print(' '*offset+'uint32_t _output =', varname + '->get_clear_value<uint32_t>();', file=self.file)

        else:
            print('I should not be here')

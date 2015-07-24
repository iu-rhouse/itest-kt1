'''
Created on Feb 1, 2014

send command portion is complete
receive command still remains

@author: zeroshiiro
'''



class Translate():
    '''
    classdocs
    '''
    cmdDict = {"reset": "@r","beep": "@b","scan": "@s","fast": "@f",
               "slow": "@l","autoexplore": "@a","i2c": "@i","analog": "@n",
               "encoders": "@e", "dock": "@d", "move": "$", "stream": "@t",
               "charger": "@c", "debug": "@g"}
    rcvdDict = {"#itc": "IRTemp1\tIRTemp2\t<empty>\t<empty>",
                "#ana": "Pin1\tPin2\tPin3\tPin4",
                "#cfs": "Bump\tWall\tCliffL2\tCliffL1\tCliffR1\tCliffR2",
                "#css": "Vwall\tInfraRx\tButtons\tBatStat\tBatTemp\tVoltage",
                "#chg": "ChgSt\tmAmp\tmAmpH\tChgSrc\toiMode",
                "#dis": "Dist",
                "#stt": "Msg",
                "#msg": "Msg\tVal"}
    
#    Velocity Parameters
    kSpeed = 200

#    Parameters to send Data
    sendHead = None
    sendData = None
    
#    Parameters for received Data
    rcvdHead = None
    rcvdData = None
    
    def __init__(self):
        '''
        Constructor
        '''
        self.operandList = ""
    
    def resetVelocity(self):
        self.kSpeed = 200
    
    def getVelocity(self):
        return self.kSpeed;
        
    def generateHeader(self, cmd):
        if(cmd not in self.cmdDict):
            print("Command not recognized")
            return "Noop"
        else:
            return self.cmdDict[cmd]
#         try:
#             return self.cmdDict[cmd]
#         except:
#             raise Exception("Command not recognized")
    
    def setOperands(self, opStr):
        self.operandList = ""
        opLen = len(opStr.split(" "))
        if(opLen == 1):
            self.operandList = self.generateOperand(int(opStr))
        elif(opLen%2 == 0):
            tempList = opStr.split(" ")
            tempOp = ""
            opCount = 0      
            for i in range(0, len(tempList), 2):
                if(tempList[i] == "left"):
                    opCount += 3
                    tempOp += self.generateOperand(int(tempList[i + 1])) + self.generateOperand(int(self.kSpeed)) + self.generateOperand(0)
                elif(tempList[i] == "right"):
                    opCount += 3
                    tempOp += "-" + self.generateOperand(int(tempList[i + 1])) + self.generateOperand(int(self.kSpeed)) + self.generateOperand(0)
                elif(tempList[i] == "forward"):
                    opCount += 3
                    tempOp += self.generateOperand(0) + self.generateOperand(int(self.kSpeed)) + self.generateOperand(int(tempList[i + 1]))
                elif(tempList[i] == "velocity"):
                    self.kSpeed = tempList[i + 1]
                else:
                  print("Sub-Command <" + tempList[i] + "> not recognized")
            self.operandList = str(opCount) + tempOp 
        else:
            print("Operand List <" + opStr + "> not Recognized")
        return self.operandList
    
    def generateOperand(self, op):
        if(len(str(op)) > 4): 
            print("Operand larger than 4 digits")
            return "0000"
        return "{0:04d}".format(op)
    
    def commandForKatie(self, usrCmd):
        cmd = usrCmd.split(" ", 1)
        if(len(cmd) == 1):
            return self.generateHeader(cmd[0])
        elif(cmd[0] == "velocity"):
            self.kSpeed = int(cmd[1])
            return "Velocity changed"
        elif(len(cmd) == 2):
            return self.generateHeader(cmd[0]) + self.setOperands(cmd[1])
    
    def convertHeader(self, hdr):
        if(hdr not in self.rcvdDict):
            print("Incorrect data header received from Katie")
            return
        else:
            return self.rcvdDict[hdr]
#         try:
#             return self.rcvdDict[hdr]
#         except:
#             raise Exception("Incorrect data header received from Katie")
    
    def dataFromKatie(self, ktData):
        hdr, data = ktData.split(" ", 1)
        return self.convertHeader(hdr) + "\n" + '\t'.join(data.split())
        

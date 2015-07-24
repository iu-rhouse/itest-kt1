'''
Created on Feb 1, 2014

@author: zeroshiiro
'''
import unittest
from translate import Translate


class TranslateTest(unittest.TestCase):

    xl = None;

    def setUp(self):
        self.xl = Translate()


    def tearDown(self):
        pass


    def testGenerateHeaderInfo(self):
        for i in ["reset", "encoders", "autoexplore", "move", "i2c"]:
            self.assertEqual(self.xl.cmdDict[i], self.xl.generateHeader(i), "it should generate header when correct command is entered")
    
    def testGenerateHeaderInfoForNonCommands(self):
        with self.assertRaises(Exception) as cm:
            self.xl.generateHeader("hello")
        self.assertEqual(str(cm.exception), "Command not recognized", "should generate error when incorrect command is entered")
        
    def testLeftPaddedNumericOperandsWithLessThan4Digits(self):
#        Normal Case
        self.assertEqual("0001", self.xl.generateOperand(1), "it should return string with 3 left padded zeros")
        self.assertEqual("0020", self.xl.generateOperand(20), "it should return string with 2 left padded zeros")
        self.assertEqual("0300", self.xl.generateOperand(300), "it should return string with 1 left padded zeros")
        self.assertEqual("4000", self.xl.generateOperand(4000), "it should return string with 0 left padded zeros")
        
    def testLeftPaddedNumericOperandsWithMoreThan4Digits(self):
#        Exception Case
        with self.assertRaises(Exception) as cm:
            self.xl.generateOperand(50000)
        self.assertEqual(str(cm.exception), "Operand larger than 4 digits", "should throw exception for incorrect number of digits")
        
#    def testSetHeaderFromCommand(self):
#        self.assertEqual(None, self.xl.sendHead, "should return None before setHeader() is called")
##        Single Command only
#        self.assertEqual("@", self.xl.setHeader("command"), "should return @ with no errors")
##        Single Command with 1 operand
#        self.assertEqual("@", self.xl.setHeader("command operand1"), "should return @ with no errors")
##        Command Chain with operands in multiples of two
#        self.assertEqual("$", self.xl.setHeader("command operand1 operand2"), "should return $ with no errors")
##        Command Chain with incorrect operands
##        self.assertRaises(Exception, "Incorrect",self.xl.setHeader, "command operand1 operand 2")
#
#    def testSetHeaderFromCommandRaisesException(self):
#        with self.assertRaises(Exception) as cm:
#            self.xl.setHeader("cmd op1 op2 op3")
#        self.assertEqual(str(cm.exception), "Incorrect Operands List", "should throw exception for incorrect number of operands")
        
    def testSetOperandsForCommands(self):
#        Command with 1 operand
        self.assertEqual("0040", self.xl.setOperands("40"), "should return left padded concatenated string for numeric list with 1 number")
#        Command with 2 operands
#        Turn Left
        self.assertEqual("3003002000000", self.xl.setOperands("left 30"), "should return left padded concatenated string for list with sub-command and operand")
#        Turn Right
        self.assertEqual("3-004502000000", self.xl.setOperands("right 45"), "should return left padded concatenated string for list with sub-command and operand")
#        Go Forward
        self.assertEqual("3000002000200", self.xl.setOperands("forward 200"), "should return left padded concatenated string for list with sub-command and operand")
        
#        Command with operands in multiples of 2
        self.assertEqual("6003002000000000002000200", self.xl.setOperands("left 30 forward 200"), "should return left padded concatenated string for list with sub-command and operand")
        
    def testSetOperandsForCommandsWithIncorrectOperandList(self):
#        Unrecognized direction
        with self.assertRaises(Exception) as cm:
            self.xl.setOperands("down 40")
        self.assertEqual(str(cm.exception), "Movement Direction not recognized", "should throw exception for unrecognized direction")
#        Unrecognized list of operands
        with self.assertRaises(Exception) as cm:
            self.xl.setOperands("up down 40")
        self.assertEqual(str(cm.exception), "Operand List not Recognized", "should throw exception for unrecognized number of operands")
        
    def testUserCommands(self):
        self.assertEqual("@r", self.xl.commandForKatie("reset"), "should return translated command for user input -  reset")
        self.assertEqual("@b0002", self.xl.commandForKatie("beep 2"), "should return translated command for user input - beep")
        self.assertEqual("$3000002000300", self.xl.commandForKatie("move forward 300"), "should return translated command for user input - move forward")
        self.assertEqual("$3009002000000", self.xl.commandForKatie("move left 90"), "should return translated command for user input - move forward")
        self.assertEqual("$3-004502000000", self.xl.commandForKatie("move right 45"), "should return translated command for user input - move forward")
        self.assertEqual("$6004502000000000001500300", self.xl.commandForKatie("move left 45 velocity 150 forward 300"), "should return translated command for user input - move forward")
        
#        Initial speed check
        self.xl.resetVelocity()
        self.assertEqual(200, self.xl.getVelocity(), "should return velocity as 200")
        self.assertEqual("Velocity changed", self.xl.commandForKatie("velocity 150"), "should change the velocity in the command sent over to katie to 150")
        self.assertEqual(150, self.xl.getVelocity(), "should return velocity as 150")
        
    def testDataHeaderRaisesExceptionForIncorrectHeaderFromKatie(self):
        with self.assertRaises(Exception) as cm:
            self.xl.convertHeader("hello")
        self.assertEqual("Incorrect data header received from Katie", str(cm.exception), "should return exception when incorrect header is supplied")
        
    def testDataFromKatie(self):
        self.assertEqual("Bump\tWall\tCliffL2\tCliffL1\tCliffR1\tCliffR2\n0000\t0000\t0000\t0000\t0000\t0000", self.xl.dataFromKatie("#cfs 0000 0000 0000 0000 0000 0000"), "should return formatted human readable")
        print(self.xl.dataFromKatie("#cfs 0000 0000 0000 0000 0000 0000"))
if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testName']
    unittest.main()
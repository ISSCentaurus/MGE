import argparse

class FindGrowth():
    def __init__(self):
        self.currentVialSet = 0 # Increment when unplugged (Sample Swapped)
        self.currentDataSet = 0 # Increment when begin logging (Every Minute)
        self.currentTemp = 0 # Increment when begin logging
        self.currentVialNum = 0 # Increment when new vial (Every ~80ms)
        self.currentInterval = 0 # Increment when new vial
        self.currentDutycycle = 0 # Increment when new vial
        
        #Vial specifc
        self.minAvg = 0
        self.firstPeak = 0
        self.minLight = 0
        self.secondPeak = 0
        self.isControlVial = False

        self.logEverything = False # CHANGE TO True TO LOG minLight FOR ALL VIALS
   
        if(self.logEverything == False):
            print("Average minLight", "Current Temp", "Current Dutycycle", "Current Interval", "Current VialSet", "Current DataSet", sep=", ")
        else:
            print("First Peak", "minLight", "Second Peak", "isControlVial", "Current Dutycycle", "Current Interval", "Current Temp", "Current VialSet", "Current DataSet", sep=", ")

    def processLine(self, line):
        if(line.startswith("*************************************")): # Sample Swapped
            self.sampleSwapped(line)
        
        if(line.startswith("NewSet")):
            self.startSet(line)
        
        if(line.startswith("Maint")):
            self.newVial(line)
        
        if(line.startswith("Light")):
            self.parseLight(line)
        
        if(line.startswith("Interval")):
            self.logInterval(line)
        
        if(line.startswith("EndSet")):
            self.endSet(line)

    def sampleSwapped(self, line):
        self.currentVialSet += 1
        self.currentDataSet = 0

    def startSet(self, line):
        self.currentDataSet += 1
        self.currentVialNum = 0
        self.minAvg = 0
        self.currentTemp = int(line[-4:],16)

    def newVial(self, line):
        self.currentVialNum += 1
        self.currentDutycycle = int(line[-4:],16)

        if(self.logEverything):
            print(self.firstPeak, self.minLight, self.secondPeak, self.isControlVial, self.currentDutycycle, self.currentInterval, self.currentTemp, self.currentVialSet, self.currentDataSet, sep=", ")

        if(not self.isControlVial):
            if(self.minAvg == 0):
                self.minAvg = self.minLight
            else:
                self.minAvg = (self.minAvg + self.minLight) / 2
    
        self.firstPeak = 0
        self.minLight = 0
        self.secondPeak = 0
        self.isControlVial = False

    def parseLight(self, line):
        lightVal = int(line[-4:],16)
        if(lightVal > self.firstPeak and self.minLight == 0):
            self.firstPeak = lightVal
        elif(self.secondPeak == 0 and lightVal < self.minLight or (lightVal < self.firstPeak and self.minLight == 0)):
            self.minLight = lightVal
        elif(lightVal > self.secondPeak and self.minLight != 0):
            self.secondPeak = lightVal
        
        if(lightVal > 512):
            self.isControlVial = True

    def logInterval(self, line):
        self.currentInterval = int(line[-4:],16)

    def endSet(self, line):
        if(self.logEverything == False):
            print(self.minAvg, self.currentTemp, self.currentDutycycle, self.currentInterval, self.currentVialSet, self.currentDataSet, sep=", ")

if __name__ == "__main__":
    process = FindGrowth()
    lines = [line.rstrip('\n') for line in open('lightLog.space.txt')]
    for i in lines: 
        process.processLine(i)

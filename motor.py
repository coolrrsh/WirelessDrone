def __init__(self, name, pin, kv=1000, WMin=0, WMax=100, debug=True, simulation=True):
    self.name = name
    self.powered = False
    self.simulation = simulation
    self.__pin = pin
    self.__kv = kv
    self.setWLimits(WMin, WMax)
    self.setDebug(debug)

    self.__W = self.__WMin
    self.__Wh = 10

    try:
        from RPIO import PWM
        self.__IO = PWM.Servo()
    except ImportError:
        self.simulation = True

def start(self):
    "Run the procedure to init the PWM"
    if not self.simulation:
        try:
            from RPIO import PWM
            self.__IO = PWM.Servo()
            self.powered = True
            #TODO Decide How to manage the WMax < 100
            #to keep anyhow the throttle range 0-100
        except ImportError:
            self.simulation = True
            self.powered = False

def stop(self):
    "Stop PWM signal"
    self.setW(0)
    if self.powered:
        self.__IO.stop_servo(self.__pin)
        self.powered = False

def increaseW(self, step=1):
    "increases W% for the motor"
    self.__W = self.__W + step
    self.setW(self.__W)

def setW(self, W):
    "Checks W% is between limits than sets it"

    PW = 0
    self.__W = W
    if self.__W < self.__WMin:
        self.__W = self.__WMin
    if self.__W > self.__WMax:
        self.__W = self.__WMax
    PW = (1000 + (self.__W) * 10)
    # Set servo to xxx us
    if self.powered:
        self.__IO.set_servo(self.__pin, PW)
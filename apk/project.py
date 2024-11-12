# Raspberry Pi 3 GPIO Pins Status And Control Using Flask Web Server and Python

import RPi.GPIO as GPIO
from flask import Flask, render_template, request
from motor import motor
app = Flask(__name__)
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
pwm1 = 12
pwm2= 32
pwm3= 33
pwm4 = 35

pwm1sts=0
pwm2sts=0
pwm3sts=0
pwm4sts=0
pwm5sts=0
pwm6sts=0
up_status=0
down_status=0
left_status=0
right_status=0
backward_status=0
forward_status=0
GPIO.setup(pwm1, GPIO.OUT)
GPIO.setup(pwm2,GPIO.OUT)
GPIO.setup(pwm3, GPIO.OUT)
GPIO.setup(pwm4,GPIO.OUT)
GPIO.output(pwm1, GPIO.LOW)
GPIO.output(pwm2, GPIO.LOW)
GPIO.output(pwm3, GPIO.LOW)
GPIO.output(pwm4, GPIO.LOW)
@app.route('/')
def index():
	pwm1sts = GPIO.input(up_status)
	pwm2sts = GPIO.input(left_status)
	pwm3sts = GPIO.input(right_status)
	pwm4sts = GPIO.input(down_status)
	pwm5sts = GPIO.input(backward_status)
	pwm6sts = GPIO.input(forward_status)
	templateData = { 
	'up_status' : pwm1sts ,
    'left_status' : pwm2sts ,
    'right_status': pwm3sts ,
	'down_status' : pwm4sts ,
	'forward_status' : pwm6sts ,
	'backward_status': pwm5sts
	}
	return render_template('index.html', **templateData)


@app.route('/<devicename>/<action>')
def do(deviceName, action):
	action="on"
	if deviceName == "up_status":
		mymotor1=motor('m1',pwm1 ,simulation=False )
		mymotor1.start()
		mymotor.setW(100)
		mymotor2=motor('m2',pwm2 ,simulation=False )
		mymotor2.start()
		mymotor.setW(100)
		mymotor3=motor('m3',pwm3,simulation=False)
		mymotor3.start()
		mynotor.setW(100)
		mymotor4=motor('m4',pwm4,simulation=False)
		mymotor4.start()
		mymotor.setW(100)
	
	if deviceName == "down_status":
		i=100
		for i in range(100):
			i=i-10
			mymotor1=motor('m1',pwm1,simulation=False)
			mymotor1.stop()
			mymotor2.setW(i)
			mymotor2=motor('m2',pwm2,simulation=False)
			mymotor2.stop()
			mymotor2.setW(i)
			mymotor3=motor('m3',pwm3,simulation=False)
			mymotor3.stop()
			mymotor3.setW(i)
			mymotor4=motor('m4',pwm4,simulation=False)
			mymotor4.stop()
			mymotor4.setW(i)
	if deviceName == "left_status":
		mymotor1=motor('m1',pwm1,simulation=False)
		mymotor1.stop()
		mymotor3=motor('m3',pwm3,simulation=False)
		mymotor3.stop()
		for i in range(5000):
			j=i+1
		mymotor1.start()
		mymotor1.setW(100)
		mymotor3.start()
		mymotor3.setW(100)
		
	if deviceName == "right_status":
		mymotor2=motor('m1',pwm2,simulation=False)
		mymotor2.stop()
		mymotor4=motor('m4',pwm4,simulation=False)
		mymotor4.stop()
		for i in range(5000):
			j=i+1
		mymotor4.start()
		mymotor4.setW(100)
		mymotor3.start()
		mymotor3.setW(100)
	return render_template('index.html')
	
if __name__ == "__main__":
    app.run(host = '0.0.0.0', debug=True)
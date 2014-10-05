#!/usr/bin/env python

import threading
import Queue
import time

"""
# For that, when we send Chinese, error happened
import sys
reload(sys)
sys.setdefaultencoding("utf-8")
"""

# Used by stk_running
STK_UNINITIALIZED   = 1
STK_INITIALIZED     = 2
STK_CONNECTE_REQ    = 3
STK_CONNECTING      = 4
STK_CONNECTE_ERR    = 5
STK_CONNECTED       = 6
STK_SOCKET_ERR      = 7
STK_USERID_ERR      = 8
STK_PASSWORD_ERR    = 9
STK_ALREADY_LOGGIN  = 10
STK_RUNNING         = 11
STK_EXITING         = 12
STK_SERVER_EXIT     = 13

STK_GENDER_UNKNOWN   = 0
STK_GENDER_BOY       = 1
STK_GENDER_GIRL      = 2

# mark user or group, used between ui and socket
STK_USER_TYPE  = 0
STK_GROUP_TYPE = 1

# Used for mark current running status
stk_running = STK_UNINITIALIZED

# uid, password, serverip
config = [0, '', '']

# User profile
profile = {'name':'', 'city':'', 'phone':0, 'gender':STK_GENDER_UNKNOWN}

# buddylist format: 
#   [[109, 'Jason', 'Shenzhen', 109, 1], [112, 'Sharmer', 'Chongqing', 112, 2], 
#    [110, 'Animy', 'Shanghai', 110, 2], [111, 'Penny', 'Xuzhou', 111, 2], 
#    [108, 'Tom', 'Beijing', 108, 1]]
buddylist = []

# grouplist format: 
#   [[707, 'Group1', [107, 108, 109, 110]], [708, 'Group2', [110, 111, 112]]]
grouplist = []

# event to notify socket we got data need to send
sendevent = threading.Event()
sendqueue = Queue.Queue()

# event to notify ui we got data need to show
uievent = threading.Event()
uiqueue = Queue.Queue()

# Tkinter sync 
syncevent = threading.Event()

def stk_get_running():
	global stk_running
	rLock = threading.RLock()
	rLock.acquire()  
	running = stk_running
	rLock.release()  
	return running
	
def stk_set_running(new_running):
	global stk_running
	rLock = threading.RLock()
	rLock.acquire()  
	stk_running = new_running
	rLock.release()  


def stk_get_config():
	global config
	rLock = threading.RLock()
	rLock.acquire()  
	config_new = config
	rLock.release()  
	return config_new
	
def stk_set_config(config_new):
	global config
	rLock = threading.RLock()
	rLock.acquire()  
	config[0] = config_new[0]
	config[1] = config_new[1]
	config[2] = config_new[2]
	rLock.release()

def stk_get_timestamp():
	tm = time.localtime(time.time())
	return time.strftime('%Y-%M-%d %H:%M:%S', tm)

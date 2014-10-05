#!/usr/bin/env python

import sys
import threading
import socket
import time
import Tkinter

import stkui
import stksocket

"""
# For that, when we send Chinese, error happened
import sys
reload(sys)
sys.setdefaultencoding("utf-8")
"""

def stk_ui():
	# Init STK UI
	#style = ttk.Style()
	#style.theme_use('classic')
	# Create and set window attribute, title, size, icon, no titlebar/border
	root = Tkinter.Tk()
	root.title('stkserver')
	root.minsize(stkui.STK_MAINWIN_WIDTH, stkui.STK_MAINWIN_HEIGHT)
	root.iconbitmap(stkui.STK_CLIENT_ICO)
	root.update_idletasks()
	root.overrideredirect(True)
	root.resizable(False, False)
	# Create other compoent
	ui = stkui.Ui(root)
	root.mainloop()		
	
def stk_socket():
	sk = stksocket.StkClient()
	while 1:
		if sk.stk_sock_run() == 0:
			break
		else:
			pass

def main_func():
	mainthread = threading.Thread(target=stk_ui)
	mainthread.start()
	sockthread = threading.Thread(target=stk_socket)
	sockthread.start()
	
	
	# Waiting for child terminated
	mainthread.join()
	
if __name__=='__main__':
	main_func()

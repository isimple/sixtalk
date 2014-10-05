#!/usr/bin/env python

import sys
import threading
import socket
import time
import Tkinter

import stkui
import stksocket
import stkuser

from stkutil import running

def stk_main():
	# Init socklist
	user = stksocket.user
	uids = user.stk_get_uids()
	fds = []
	i = len(uids)
	while(i > 0):
		fds.append([-1, 0])
		i -= 1
	stksocket.socklist = dict(zip(uids, fds))
	
	# Init STK UI
	#style = ttk.Style()
	#style.theme_use('classic')
	root = Tkinter.Tk()
	root.title('stkserver')
	root.minsize(stkui.STK_MAINWIN_WIDTH, stkui.STK_MAINWIN_HEIGHT)
	root.iconbitmap(stkui.STK_ICON_PNG)
	ui = stkui.Ui()
	# This is how update, maybe we have to do it in a thread
	#ui.stk_update_ui(110)
	root.mainloop()	
	ui.exiting = True
	ui.updatethread.join()
	print 'Ui main thread exiting...'
	
def stk_socket():
	# Set socket timeout
	socket.setdefaulttimeout(3)
	# Start server socket
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	s.bind(('', stksocket.STK_SERVER_PORT))
	s.listen(stksocket.STK_MAX_CLIENTS)
	while 1:
		try:
			c = s.accept()
		except socket.timeout:
			global running
			running = running
			if running == False:
				break;		
		else:
			t = threading.Thread(target=stksocket.stk_socket_thread, args=(c,))
			t.start()
	s.close()
	time.sleep(1)
	print 'Main socket thread exiting...'

def main_func():
	# Main Thread should be start first
	mainthread = threading.Thread(target=stk_main)
	sockthread = threading.Thread(target=stk_socket)
	mainthread.start()
	sockthread.start()
	
	time.sleep(5)
	# Wait for ui thread
	mainthread.join()
	# Now, Mark running False
	global running
	running = False
	# Wait socket thread 
	sockthread.join()
	sys.exit(0)
	
if __name__=='__main__':
	main_func()

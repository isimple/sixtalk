#!/usr/bin/env python

# Main window Mouse dragable refer following website
# http://stackoverflow.com/questions/4055267

import string
import time
import threading
import Tkinter
import tkFont
import ttk
import tkMessageBox
import sys

from stkutil import *

"""
# For that, when we send Chinese, error happened
import sys
reload(sys)
sys.setdefaultencoding("utf-8")
"""

# Window size
STK_MAINWIN_WIDTH     = 300
STK_MAINWIN_HEIGHT    = 600
# Window color
STK_MAIN_COLOR        = '#67A3CD'
# Program icon
STK_CLIENT_ICO        = 'pixmaps/client.ico'
# Close button images
STK_CLOSE_IMG_WIDTH   = 39
STK_CBTNDOWN_COLOR    = '#993232'
STK_CBTN_GIF_FILE     = 'pixmaps/btn_close.gif'
# Minimize button images
STK_MIN_IMG_WIDTH     =28 
STK_MBTNDOWN_COLOR    = '#FF9964'
STK_MBTN_GIF_FILE     = 'pixmaps/btn_min.gif'
# U Know it
STK_BEATU_1_GIF_FILE  = 'pixmaps/beauty2.gif'
STK_BEATU_2_GIF_FILE  = 'pixmaps/beauty2.gif'
# Login/Cancel button images
STK_LOGINBTN_GIF_FILE = 'pixmaps/btn_login.gif'
STK_CANCEBTN_GIF_FILE = 'pixmaps/btn_cancel.gif'
# Logining image
STK_LOGINING_GIF_FILE = 'pixmaps/logining.gif'

# user/greoup tree images
STK_AVATAR_1_GIF_FILE = 'pixmaps/avatar.gif'
STK_AVATAR_2_GIF_FILE = 'pixmaps/avatar_girl.gif'
STK_BUDDY_GIF_FILE    = 'pixmaps/buddy.gif'
STK_GROUP_GIF_FILE    = 'pixmaps/group.gif'

# chatwin ico
STK_BUDDY_ICO         = 'pixmaps/buddy.ico'
STK_GROUP_ICO         = 'pixmaps/group.ico'

STK_MESSAGE_WAV       = 'sounds/message.wav'
STK_AUDIO_WAV         = 'sounds/audio.wav'
STK_ONLINE_WAV        = 'sounds/online.wav'

class Ui:
	def __init__(self, root):
		# To store chat windows
		self.chatlist = []
		self.gchatlist = []
		# Make sure we get a refence here
		self.root = root;
		# Create a frame for window, set it mouse dragable
		self.frame = Tkinter.Frame(bg=STK_MAIN_COLOR)
		self.frame.bind('<ButtonPress-1>', self.stk_move_start)
		self.frame.bind('<ButtonRelease-1>', self.stk_move_stop)
		self.frame.bind('<B1-Motion>', self.stk_move_motion)
		# Set the Enter key to login, seems useless, WHY~
		self.root.bind('<KeyPress-Return>', self.stk_user_login)
		self.frame.pack(expand=Tkinter.YES, fill=Tkinter.BOTH)		
		# Close button
		self.cimg = Tkinter.PhotoImage(file=STK_CBTN_GIF_FILE)
		self.closebtn = Tkinter.Button(self.frame, image=self.cimg, bg=STK_MAIN_COLOR, \
			activebackground=STK_CBTNDOWN_COLOR, relief=Tkinter.GROOVE, \
			overrelief=Tkinter.RAISED, command=self.stk_quit)
		self.closebtn.place(x=STK_MAINWIN_WIDTH-STK_CLOSE_IMG_WIDTH, y=0)
		# Minmize button, For now useless, disable it
		self.mimg = Tkinter.PhotoImage(file=STK_MBTN_GIF_FILE)
		self.minbtn = Tkinter.Button(self.frame, image=self.mimg, bg=STK_MAIN_COLOR, \
			activebackground=STK_MBTNDOWN_COLOR, relief=Tkinter.GROOVE, \
			overrelief=Tkinter.RAISED, command=self.stk_iconfy, state=Tkinter.DISABLED)
		self.minbtn.place(x=STK_MAINWIN_WIDTH-STK_CLOSE_IMG_WIDTH-STK_MIN_IMG_WIDTH, y=0)
		# ~_~ - Show Girl photo
		self.g1img = Tkinter.PhotoImage(file=STK_BEATU_1_GIF_FILE)
		width = (STK_MAINWIN_WIDTH - self.g1img.width())/2
		height = (STK_MAINWIN_HEIGHT - self.g1img.height())/4
		self.g1label = Tkinter.Label(self.frame, image=self.g1img, borderwidth=0)
		self.g1label.place(x=width, y=height)
		self.g1pi = self.g1label.place_info()
		# Username,Password,Server etc
		width += 50
		height += self.g1img.height()
		self.ulabel = Tkinter.Label(self.frame, text='Username:', bg=STK_MAIN_COLOR)
		self.ulabel.place(x=width, y=height+20)
		self.ulpi = self.ulabel.place_info()
		self.utext = Tkinter.Entry(self.frame, width=25)
		self.utext.focus_set()
		self.utext.place(x=width, y=height+40)
		self.utpi = self.utext.place_info()
		self.plabel = Tkinter.Label(self.frame, text='Password:', bg=STK_MAIN_COLOR)
		self.plabel.place(x=width, y=height+60)
		self.plpi = self.plabel.place_info()
		self.ptext = Tkinter.Entry(self.frame, show='*', width=25)
		self.ptext.place(x=width, y=height+80)
		self.ptpi = self.ptext.place_info()
		self.slabel = Tkinter.Label(self.frame, text='Server IP:', bg=STK_MAIN_COLOR)
		self.slabel.place(x=width, y=height+100)
		self.slpi = self.slabel.place_info()
		self.stext = Tkinter.Entry(self.frame, width=25)
		self.stext.place(x=width, y=height+120)
		self.stpi = self.stext.place_info()
		# The login button and login result label
		self.loginimg = Tkinter.PhotoImage(file=STK_LOGINBTN_GIF_FILE)
		self.loginbtn = Tkinter.Button(self.frame, image=self.loginimg, bg=STK_MAIN_COLOR, \
			activebackground=STK_MAIN_COLOR, relief=Tkinter.GROOVE, takefocus=1, \
			overrelief=Tkinter.RAISED, command=self.stk_loginbtn_pressed)
		self.loginbtn.place(x=width, y=height+150)
		self.loginpi = self.loginbtn.place_info()
		self.reterr = Tkinter.StringVar()
		self.rlabel = Tkinter.Label(self.frame, textvariable=self.reterr, bg=STK_MAIN_COLOR, \
			fg='red', compound=Tkinter.LEFT)
		self.rlabel.place(x=width, y=height+200)
		# Create login cancel info 
		self.g2img = Tkinter.PhotoImage(file=STK_BEATU_2_GIF_FILE)
		self.g2label = Tkinter.Label(self.frame, image=self.g2img, borderwidth=0)
		self.logingimg = Tkinter.PhotoImage(file=STK_LOGINING_GIF_FILE)
		self.loginglabel = Tkinter.Label(self.frame, image=self.logingimg, borderwidth=0, \
			bg=STK_MAIN_COLOR)
		self.cancelimg = Tkinter.PhotoImage(file=STK_CANCEBTN_GIF_FILE)
		self.cancelbtn = Tkinter.Button(self.frame, image=self.cancelimg, bg=STK_MAIN_COLOR, \
			activebackground=STK_MAIN_COLOR, relief=Tkinter.GROOVE, \
			overrelief=Tkinter.RAISED, command=self.stk_cancelbtn_pressed)
	def stk_move_start(self, event):
		self.x = event.x
		self.x = event.x
		self.y = event.y
	def stk_move_stop(self, event):
		self.x = None
		self.y = None
	def stk_move_motion(self, event):
		deltax = event.x - self.x
		deltay = event.y - self.y
		x = self.root.winfo_x() + deltax
		y = self.root.winfo_y() + deltay
		self.root.geometry("+%s+%s" % (x, y))
	def stk_quit(self):
		# stops mainloop
		self.root.quit()
		# this is necessary on Windows to prevent	
		self.root.destroy()
		# Let socket thread know we exit
		stk_set_running(STK_EXITING)
	def stk_iconfy(self):
		# Seems Useless when set override redirect flag
		#self.root.overrideredirect(False)
		self.root.iconify()
	def stk_user_login(self, event):
		self.stk_loginbtn_pressed()
	def stk_loginbtn_pressed(self):
		uid = self.utext.get()
		password = self.ptext.get()
		serverip = self.stext.get()
		if uid.isdigit() == False: 
			self.rlabel.configure(bitmap='error')
			self.reterr.set('  Username is Valid')
		elif len(password) == 0:
			self.rlabel.configure(bitmap='error')
			self.reterr.set('  Password is NULL')
		elif len(serverip) == 0:
			self.rlabel.configure(bitmap='error')
			self.reterr.set('  Server IP is NULL')
		else:
			# Set config
			config = [int(uid), password, serverip]
			stk_set_config(config)
			# Clear result label 
			self.rlabel.configure(bitmap='')
			self.reterr.set('')
			# Forget login stufs
			self.g1label.place_forget()
			self.ulabel.place_forget()
			self.utext.place_forget()
			self.plabel.place_forget()
			self.ptext.place_forget()
			self.slabel.place_forget()
			self.stext.place_forget()
			self.loginbtn.place_forget()
			# Update Ui
			self.root.update_idletasks()
			# Create connecting interface
			self.stk_login_info()
			# Update Ui
			self.root.update_idletasks()
			# Set running status, So socket thread know what to do
			stk_set_running(STK_CONNECTE_REQ)
			# To prevent ui block, use a thread to do this
			self.root.after_idle(self.stk_waiting)
	def stk_login_info(self):
		# Create login cancel info 
		width = (STK_MAINWIN_WIDTH - self.g2img.width())/2
		height = (STK_MAINWIN_HEIGHT - self.g2img.height())/3 + 20
		self.g2label.place(x=width, y=height)
		self.loginglabel.place(x=width+70, y=height+180)
		self.cancelbtn.place(x=width+70, y=height+210)
	def stk_cancelbtn_pressed(self):
		# Mark running status, so socket stop its connect
		stk_set_running(STK_INITIALIZED)
		# Remove login info 
		self.g2label.place_forget()
		self.loginglabel.place_forget()
		self.cancelbtn.place_forget()
		# Show main info
		self.g1label.place(self.g1pi)
		self.ulabel.place(self.ulpi)
		self.utext.place(self.utpi)
		self.plabel.place(self.plpi)
		self.ptext.place(self.ptpi)
		self.slabel.place(self.slpi)
		self.stext.place(self.stpi)
		self.loginbtn.place(self.loginpi)
		# update Ui
		self.root.update_idletasks()
	def stk_waiting(self):
		running = stk_get_running()
		if running == STK_CONNECTED:
			# We should create buddylist now, never return
			self.stk_destory_widgets()
			self.stk_create_tree()
			self.root.update_idletasks()
			stk_set_running(STK_RUNNING)
			return 
		elif running == STK_CONNECTING or running == STK_CONNECTE_REQ: 
			# Continue waiting
			self.root.after_idle(self.stk_waiting)
			return
		elif running == STK_SOCKET_ERR or running == STK_CONNECTE_ERR:
			self.rlabel.configure(bitmap='error')
			self.reterr.set('  Connect to stkserver falied')
		elif running == STK_USERID_ERR:
			self.rlabel.configure(bitmap='error')
			self.reterr.set('  Invalid Username')
		elif running == STK_PASSWORD_ERR:
			self.rlabel.configure(bitmap='error')
			self.reterr.set('  Invalid Password')
		elif running == STK_ALREADY_LOGGIN:
			self.rlabel.configure(bitmap='error')
			self.reterr.set('  Already Logined in')
		elif running == STK_INITIALIZED:
			# user cancle the login 
			pass
		else:
			pass
		self.stk_cancelbtn_pressed()
	def stk_destory_widgets(self):
		# Anything left?
		self.g1label.destroy()
		self.ulabel.destroy()
		self.utext.destroy()
		self.plabel.destroy()
		self.ptext.destroy()
		self.slabel.destroy()
		self.stext.destroy()
		self.rlabel.destroy()
		self.loginbtn.destroy()
		self.loginglabel.destroy()
		self.g2label.destroy()
		self.cancelbtn.destroy()
	def stk_create_tree(self):
		global profile
		# avatar label
		if profile['gender'] == STK_GENDER_BOY:
			gif_file = STK_AVATAR_1_GIF_FILE
		else:
			gif_file = STK_AVATAR_2_GIF_FILE
		self.avatarimg = Tkinter.PhotoImage(file=gif_file)
		self.avatarlabel = Tkinter.Label(self.frame, image=self.avatarimg, borderwidth=0)
		self.avatarlabel.place(x=15, y=50)
		# profile label
		global config
		uid_str = str(config[0]) + ' (' + profile['name'] + ')'
		self.pflabel = Tkinter.Label(self.frame, text=uid_str, borderwidth=0, bg=STK_MAIN_COLOR)
		self.pflabel.place(x=100, y=75)
		# notebook and buddy/group treeview
		self.note = ttk.Notebook(padding=0)
		s = ttk.Style()
		s.configure('TNotebook', background=STK_MAIN_COLOR)		
		self.buddytree = ttk.Treeview(columns=('buddyinfo'), show='tree', selectmode='none')
		self.grouptree = ttk.Treeview(columns=('groupinfo'), show='tree', selectmode='none')
		self.buddytree.bind("<Double-ButtonPress-1>",self.stk_buddy_chat)
		#self.buddytree.bind("<ButtonRelease-3>", self.stk_buddy_chat)
		self.grouptree.bind("<Double-ButtonPress-1>",self.stk_group_chat)
		#self.grouptree.bind("<ButtonRelease-3>", self.stk_group_chat)
		self.note.add(self.buddytree, text=' User List ')
		self.note.add(self.grouptree, text=' Group List ')
		self.note.place(x=6, y=150, width=STK_MAINWIN_WIDTH-6*2, height=STK_MAINWIN_HEIGHT-200)
		# buddy tree
		self.stk_create_buddytree()
		# group tree
		self.stk_create_grouptree()
		# now, ui is ok, we create a thread check if anyone send message to us
		self.recvthread = threading.Thread(target=self.stk_recv_msg)
		self.recvthread.start()		
	def stk_create_buddytree(self):
		img = Tkinter.PhotoImage(file=STK_BUDDY_GIF_FILE)
		treedata = []
		global buddylist
		for item in buddylist:
			self.chatlist.append([item[0], item[1], None])
			treedata.append(str(item[0])+'   '+item[1])
		treedata.sort()
		for item in treedata:
			# Image seems can't display, why??
			self.buddytree.insert('', 'end', image=img, text=item)
	def stk_create_grouptree(self):
		img = Tkinter.PhotoImage(file=STK_GROUP_GIF_FILE)
		treedata = []
		global grouplist
		for item in grouplist:
			self.gchatlist.append([item[0], item[1], None])
			treedata.append(str(item[0])+'   '+item[1])
		treedata.sort()
		for item in treedata:
			# Image seems can't display, why??
			self.grouptree.insert('', 'end', image=img, text=item)
	def stk_update_ui(self, uid):
		# This func not used now
		for k in self.buddytree.get_children():
			item = self.buddytree.item(k)['values']
			if item[0] == uid:
				item[2] = u'online'
				self.buddytree.item(k, values=item)				
	def stk_buddy_chat(self, event):
		widget = event.widget
		iid = widget.identify_row(event.y)
		item = self.buddytree.item(iid)
		if item['text'] == '':
			pass
		else:
			uid = int(item['text'][0:3])
			self.stk_create_chatwin(uid, False)
	def stk_group_chat(self, event):
		widget = event.widget
		iid = widget.identify_row(event.y)
		item = self.grouptree.item(iid)
		if item['text'] == '':
			pass
		else:
			gid = int(item['text'][0:3])
			self.stk_create_gchatwin(gid, False)
	def stk_destory_chatwin(self, uid):
		for item in self.chatlist:
			if item[0] == uid:
				break
		index = self.chatlist.index(item)
		item[2][0].destroy()
		item[2] = None
		self.chatlist[index] = item
	def stk_create_chatwin(self, uid, notify):
		for item in self.chatlist:
			if item[0] == uid:
				break
		if item[2] != None:
			item[2][0].deiconify()
			item[2][6].focus_force()
			return
		index = self.chatlist.index(item)
		#chatwin = Tkinter.Tk()
		chatwin = Tkinter.Toplevel(self.root)
		chatwin.iconbitmap(STK_BUDDY_ICO)
		chatwin.protocol("WM_DELETE_WINDOW", lambda:self.stk_destory_chatwin(uid))
		# Chat window setting
		chatwin.title('Chat With '+item[1])
		#chatwin.resizable(False, False)
		# Create frames as container
		top = ttk.Frame(chatwin)
		interval = ttk.Frame(chatwin)
		center = ttk.Frame(chatwin)
		bottom = ttk.Frame(chatwin)
		# interval label
		label = Tkinter.Label(interval,height=1)  
		# chattext, inputtext, sendbtn/closebtn
		ft = tkFont.Font(family='Fixdsys', size=10)

		ctscrollbar = ttk.Scrollbar(top, orient=Tkinter.VERTICAL)  
		chattext = Tkinter.Text(top, wrap=Tkinter.WORD, state=Tkinter.DISABLED, \
								font=ft,width=70,height=20)
		chattext['yscrollcommand'] = ctscrollbar.set
		ctscrollbar['command'] = chattext.yview()

		itscrollbar = ttk.Scrollbar(center, orient=Tkinter.VERTICAL)  
		inputtext = Tkinter.Text(center, wrap=Tkinter.WORD, font=ft,width=70,height=8)
		inputtext['yscrollcommand'] = itscrollbar.set
		itscrollbar['command'] = inputtext.yview()
		inputtext.bind('<KeyRelease-Return>', lambda event:self.stk_send_msg2(event, uid))
		# Use KeyRelease not keyPress, otherwise there will be a '\n' remain in Text
		#inputtext.bind('<KeyPress-Return>', lambda event:self.stk_send_msg2(event, uid))
		#inputtext.bind('<KeyPress-Return>', lambda:self.stk_send_msg2(uid))
		sendbtn = Tkinter.Button(bottom, text=' Send ', \
					command=lambda:self.stk_send_msg(uid))
		closebtn = Tkinter.Button(bottom, text=' Close ', \
					command=lambda:self.stk_destory_chatwin(uid))
		# Update chatlist
		item[2] = [chatwin, top, center, bottom, chattext, ctscrollbar, \
					inputtext, itscrollbar, sendbtn, closebtn]
		self.chatlist[index] = item
		# Tag for btext
		chattext.tag_config('minfo', foreground='blue')
		chattext.tag_config('mtext', foreground='black', \
					lmargin1=10, lmargin2=10)
		chattext.tag_config('binfo', foreground='red')
		chattext.tag_config('btext', foreground='black', \
					lmargin1=10, lmargin2=10)
		# Set container position with grid
		top.pack(expand=1,fill=Tkinter.BOTH)
		interval.pack(expand=1,fill=Tkinter.BOTH)  
		center.pack(expand=1,fill=Tkinter.BOTH)  
		bottom.pack(expand=1,fill=Tkinter.BOTH)  
		# Put element into frame label
		ctscrollbar.pack(side=Tkinter.RIGHT,fill=Tkinter.Y) 
		chattext.pack(expand=1, fill=Tkinter.BOTH, padx=2,pady=8)
		label.pack(expand=1,fill=Tkinter.BOTH) 
		itscrollbar.pack(side=Tkinter.RIGHT,fill=Tkinter.Y) 
		inputtext.pack(expand=1, fill=Tkinter.BOTH, padx=2,pady=2)
		closebtn.pack(expand=1,side=Tkinter.RIGHT)
		sendbtn.pack(expand=1,side=Tkinter.RIGHT)
		# Focus input widget
		inputtext.focus_force()
		# Make sure recvthread know chat window is created now
		if notify:
			syncevent.set()
	def stk_send_msg2(self, event, uid):
		self.stk_send_msg(uid)
	def stk_send_msg(self, uid):
		for item in self.chatlist:
			if item[0] == uid:
				break
		# Get msg then delete from Text widget, the msg will auto add '\n', why?
		msgstr = item[2][6].get(1.0, Tkinter.END)
		item[2][6].delete(1.0, Tkinter.END)
		msgstr = msgstr.strip('\n')
		if msgstr == '':
			tkMessageBox.showinfo('stkclient', 'Message should not NULL...')
			item[2][6].focus_force()
			item[2][0].update_idletasks()
			return
		# Notify socket we got data to send
		sendevent.set()
		sendqueue.put([STK_USER_TYPE, uid, msgstr])
		# Show message we have send
		item[2][4].configure(state=Tkinter.NORMAL)
		head = profile['name'] + '(' + str(config[0]) + ') ' + stk_get_timestamp() + '\n'
		item[2][4].insert(Tkinter.END, head, 'minfo')
		item[2][4].insert(Tkinter.END, msgstr + '\n', 'mtext')
		item[2][4].see(Tkinter.END)
		item[2][4].configure(state=Tkinter.DISABLED)
		item[2][0].update_idletasks()
	def stk_destory_gchatwin(self, gid):
		for item in self.gchatlist:
			if item[0] == gid:
				break
		index = self.gchatlist.index(item)
		item[2][0].destroy()
		item[2] = None
		self.gchatlist[index] = item
	def stk_create_gchatwin(self, gid, notify):
		for item in self.gchatlist:
			if item[0] == gid:
				break
		if item[2] != None:
			item[2][0].deiconify()
			item[2][6].focus_force()
			return
		index = self.gchatlist.index(item)
		#chatwin = Tkinter.Tk()
		gchatwin = Tkinter.Toplevel(self.root)
		gchatwin.iconbitmap(STK_BUDDY_ICO)
		gchatwin.protocol("WM_DELETE_WINDOW", lambda:self.stk_destory_gchatwin(gid))
		# Chat window setting
		gchatwin.title('Chat With '+item[1])
		#gchatwin.resizable(False, False)
		# Create frames as container
		top = ttk.Frame(gchatwin)
		interval = ttk.Frame(gchatwin)
		center = ttk.Frame(gchatwin)
		bottom = ttk.Frame(gchatwin)
		# interval label
		label = Tkinter.Label(interval,height=1)  
		# chattext, inputtext, sendbtn/closebtn
		ft = tkFont.Font(family='Fixdsys', size=10)

		ctscrollbar = ttk.Scrollbar(top, orient=Tkinter.VERTICAL)  
		chattext = Tkinter.Text(top, wrap=Tkinter.WORD, state=Tkinter.DISABLED, \
								font=ft,width=70,height=20)
		chattext['yscrollcommand'] = ctscrollbar.set
		ctscrollbar['command'] = chattext.yview()

		itscrollbar = ttk.Scrollbar(center, orient=Tkinter.VERTICAL)  
		inputtext = Tkinter.Text(center, wrap=Tkinter.WORD, font=ft,width=70,height=8)
		inputtext['yscrollcommand'] = itscrollbar.set
		itscrollbar['command'] = inputtext.yview()
		inputtext.bind('<KeyRelease-Return>', lambda event:self.stk_send_gmsg2(event, gid))
		sendbtn = Tkinter.Button(bottom, text=' Send ', \
					command=lambda:self.stk_send_gmsg(gid))
		closebtn = Tkinter.Button(bottom, text=' Close ', \
					command=lambda:self.stk_destory_gchatwin(gid))
		# Update gchatlist
		item[2] = [gchatwin, top, center, bottom, chattext, ctscrollbar, \
					inputtext, itscrollbar, sendbtn, closebtn]
		self.gchatlist[index] = item
		# Tag for btext
		chattext.tag_config('minfo', foreground='blue')
		chattext.tag_config('mtext', foreground='black', \
					lmargin1=10, lmargin2=10)
		chattext.tag_config('binfo', foreground='red')
		chattext.tag_config('btext', foreground='black', \
					lmargin1=10, lmargin2=10)
		# Set container position with grid
		top.pack(expand=1,fill=Tkinter.BOTH)
		interval.pack(expand=1,fill=Tkinter.BOTH)  
		center.pack(expand=1,fill=Tkinter.BOTH)  
		bottom.pack(expand=1,fill=Tkinter.BOTH)  
		# Put element into frame label
		ctscrollbar.pack(side=Tkinter.RIGHT,fill=Tkinter.Y) 
		chattext.pack(expand=1, fill=Tkinter.BOTH, padx=2,pady=8)
		label.pack(expand=1,fill=Tkinter.BOTH) 
		itscrollbar.pack(side=Tkinter.RIGHT,fill=Tkinter.Y) 
		inputtext.pack(expand=1, fill=Tkinter.BOTH, padx=2,pady=2)
		closebtn.pack(expand=1,side=Tkinter.RIGHT)
		sendbtn.pack(expand=1,side=Tkinter.RIGHT)
		# Focus input widget
		inputtext.focus_force()
		# Make sure recvthread know chat window is created now
		if notify:
			syncevent.set()
	def stk_send_gmsg2(self, event, gid):
		self.stk_send_gmsg(gid)
	def stk_send_gmsg(self, gid):
		for item in self.gchatlist:
			if item[0] == gid:
				break
		# Get msg then delete from Text widget, the msg will auto add '\n', why?
		msgstr = item[2][6].get(1.0, Tkinter.END)
		item[2][6].delete(1.0, Tkinter.END)
		msgstr = msgstr.strip('\n')
		if msgstr == '':
			tkMessageBox.showinfo('stkclient', 'Message should not NULL...')
			item[2][6].focus_force()
			item[2][0].update_idletasks()
			return
		# Notify socket we got data to send
		sendevent.set()
		sendqueue.put([STK_GROUP_TYPE, gid, msgstr])
		# Show message we have send
		item[2][4].configure(state=Tkinter.NORMAL)
		head = profile['name'] + '(' + str(config[0]) + ') ' + stk_get_timestamp() + '\n'
		item[2][4].insert(Tkinter.END, head, 'minfo')
		item[2][4].insert(Tkinter.END, msgstr + '\n', 'mtext')
		item[2][4].see(Tkinter.END)
		item[2][4].configure(state=Tkinter.DISABLED)
		item[2][0].update_idletasks()
	def stk_show_msg(self, mtype, mid, mdata):
		if mtype == STK_USER_TYPE:
			for item in self.chatlist:
				if item[0] == mid:
					break
			# If chat windows is not create, create first
			if item[2] == None:
				# We cann't create window ourself, this will cause Tkinter block death
				self.root.after_idle(lambda:self.stk_create_chatwin(mid, True))
				syncevent.wait()
				time.sleep(0.1)
				if syncevent.is_set():
					syncevent.clear()
					for item in self.chatlist:
						if item[0] == mid:
							break
			# Show message we have send
			item[2][4].configure(state=Tkinter.NORMAL)
			head = item[1] + '(' + str(item[0]) + ') ' + stk_get_timestamp() + '\n'
			item[2][4].insert(Tkinter.END, head, 'binfo')
			item[2][4].insert(Tkinter.END, mdata + '\n', 'btext')
			item[2][4].see(Tkinter.END)
			item[2][4].configure(state=Tkinter.DISABLED)
			item[2][6].focus_force()
			item[2][0].update_idletasks()
			# Since we get msg from buddy, ring bell
			if sys.platform == 'win32':
				import winsound
				winsound.PlaySound(STK_MESSAGE_WAV, winsound.SND_FILENAME | winsound.SND_ASYNC)
			elif sys.platform[:5] == 'linux':
				import os  
				os.popen2('aplay -q' + STK_MESSAGE_WAV)  
			else:
				self.root.bell()
		elif mtype == STK_GROUP_TYPE:
			for item in self.gchatlist:
				if item[0] == mid[0]:
					break
			# If chat windows is not create, create first
			if item[2] == None:
				# We cann't create window ourself, this will cause Tkinter block death
				self.root.after_idle(lambda:self.stk_create_gchatwin(mid[0], True))
				syncevent.wait()
				time.sleep(0.1)
				if syncevent.is_set():
					syncevent.clear()
					for item in self.gchatlist:
						if item[0] == mid[0]:
							break
			# First, find buddy info
			for i in self.chatlist:
				if i[0] == mid[1]:
					break
			# Show message we have send
			item[2][4].configure(state=Tkinter.NORMAL)
			head = i[1] + '(' + str(i[0]) + ') ' + stk_get_timestamp() + '\n'
			item[2][4].insert(Tkinter.END, head, 'binfo')
			item[2][4].insert(Tkinter.END, mdata + '\n', 'btext')
			item[2][4].see(Tkinter.END)
			item[2][4].configure(state=Tkinter.DISABLED)
			item[2][6].focus_force()
			item[2][0].update_idletasks()
			# Since we get msg from group, ring bell
			# This is bad, need to do more work
			if sys.platform == 'win32':
				import winsound
				winsound.PlaySound(STK_MESSAGE_WAV, winsound.SND_FILENAME | winsound.SND_ASYNC)
			elif sys.platform[:5] == 'linux':
				import os  
				os.popen2('aplay -q' + STK_MESSAGE_WAV)  
			else:
				self.root.bell()
	def stk_recv_msg(self):
		while 1:
			#time.sleep(1)
			# Check whether we must quit now
			running = stk_get_running()
			if running == STK_EXITING:
				break
			# Check any msg need to send to server
			if uievent.is_set():
				uievent.clear()
				while uiqueue.empty() != True:
					data = uiqueue.get()
					self.stk_show_msg(data[0], data[1], data[2])

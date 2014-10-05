#!/usr/bin/env python

import time
import threading
import Tkinter
import tkFont
import ttk

import stkuser
from stkutil import updateinfo
from stksocket import user, group

STK_MAINWIN_WIDTH  = 500
STK_MAINWIN_HEIGHT = 300
MAIN_COLRO_STRING  = '#67A3CD'
STK_ICON_PNG       = 'pixmaps/manager.ico'

buddycolumns = ('BuddyID', 'BuddyName', 'Status', 'City', 'Phone', 'Gender')
groupcolumns = ('GroupID', 'GroupName', 'Members')

# Copy from Python-2.7.8/Demo/tkinter/ttk/treeview_multicolum.py
def sortby(tree, col, descending):
	"""Sort tree contents when a column is clicked on."""
	# grab values to sort
	data = [(tree.set(child, col), child) for child in tree.get_children('')]

	# reorder data
	data.sort(reverse=descending)
	for indx, item in enumerate(data):
		tree.move(item[1], '', indx)

	# switch the heading so that it will sort in the opposite direction
	tree.heading(col, command=lambda col=col: sortby(tree, col, int(not descending)))

class Ui:
	def __init__(self):
		self.exiting = False
		self.note = None
		self.buddytree = None
		self.grouptree = None
		self.stk_create_userlist()
		self.stk_build_buddytree()
		self.stk_build_grouptree()
		self.updatethread = threading.Thread(target=self.stk_update_thread)
		self.updatethread.start()		
	def stk_create_userlist(self):
		self.note = ttk.Notebook(padding=6)
		self.buddytree = ttk.Treeview(columns=buddycolumns, show="headings")
		self.grouptree = ttk.Treeview(columns=groupcolumns, show="headings")
		self.logtree = Tkinter.Text()
		self.note.add(self.buddytree, text='User List')
		self.note.add(self.grouptree, text='Group List')
		self.note.pack(fill=Tkinter.BOTH, expand=1)
	def stk_build_buddytree(self):
		for col in buddycolumns:
			self.buddytree.heading(col, text=col.title(),
			command=lambda c=col: sortby(self.buddytree, c, 0))
			self.buddytree.column(col, width=tkFont.Font().measure(col.title()))
		global user
		treedata = user.stk_get_users()
		for item in treedata:
			self.buddytree.insert('', 'end', values=item)
		for indx, val in enumerate(item):
			ilen = tkFont.Font().measure(val)
			if self.buddytree.column(buddycolumns[indx], width=None) < ilen:
				self.buddytree.column(buddycolumns[indx], width=ilen)
	def stk_build_grouptree(self):
		for col in groupcolumns:
			self.grouptree.heading(col, text=col.title(),
				command=lambda c=col: sortby(self.grouptree, c, 0))
			self.grouptree.column(col, width=tkFont.Font().measure(col.title()))
		global group
		treedata = group.stk_get_groups()
		for item in treedata:
			self.grouptree.insert('', 'end', values=item)
			for indx, val in enumerate(item):
				ilen = tkFont.Font().measure(val)
				if self.grouptree.column(groupcolumns[indx], width=None) < ilen:
					self.grouptree.column(groupcolumns[indx], width=ilen)				
	def stk_update_ui(self, uid, state):
		for k in self.buddytree.get_children():
			item = self.buddytree.item(k)['values']
			if item[0] == uid:
				item[2] = state
				self.buddytree.item(k, values=item)				
	def stk_update_thread(self):	
		while 1:
			time.sleep(1)
			global updateinfo
			if self.exiting == True:
				print 'Ui update thread exiting...'
				break
			if len(updateinfo) != 0:
				for info in updateinfo:
					self.stk_update_ui(info[0], info[1])					
					updateinfo.remove(info)

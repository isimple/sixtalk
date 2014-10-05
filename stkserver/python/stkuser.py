#!/usr/bin/env python

import json

STK_USER_FILE  = 'users'
STK_GROUP_FILE = 'groups'

class StkUsers:
	users = {}
	keys = []
	def __init__(self):
		f = file(STK_USER_FILE, 'r')
		self.users = json.load(f)
		self.keys = self.users.keys()
	def stk_get_usernum(self):
		return len(self.keys)
	def stk_get_uids(self):
		uids = []
		for k in self.keys:
			uids.append(self.users[k]['uid']) 
		return uids
	def stk_get_userinfo(self, uid):
		for k in self.keys:
			if self.users[k]['uid'] == uid:
				return self.users[k]
	def stk_get_nickname(self, uid):
		for k in self.keys:
			if self.users[k]['uid'] == uid:
				return self.users[k]['nickname'].encode()
	def stk_find_user(self, uid):
		for k in self.keys:
			if self.users[k]['uid'] == uid:
				return True
		return False
	def stk_add_user(self, n):
		self.users += n
	def stk_get_pass(self, uid):
		for k in self.keys:
			if self.users[k]['uid'] == uid:
				return self.users[k]['pass']
		return 'STK_UNKNOWN_USER'
	def stk_get_users(self):
		tmp = []
		for k in self.keys:
			uid = self.users[k]['uid']
			status = 'offline'
			gender = ''
			if self.users[k]['gender'] == 1:
				gender = 'boy'
			else:
				gender = 'girl'
			#if socklist[uid][1] == 1:
			#	status = 'online'
			#else:
			#	status = 'offline'
			item = (uid, self.users[k]['nickname'].encode(), status, self.users[k]['city'].encode(), 
			        self.users[k]['phone'], gender)
			tmp.append(item)
		return tmp

class StkGroups:
	groups = {}
	keys = []
	def __init__(self):
		f = file(STK_GROUP_FILE, 'r')
		self.groups = json.load(f)
		self.keys = self.groups.keys()
	def stk_get_gids(self):
		gids = []
		for k in self.keys:
			gids.append(self.groups[k]['gid'])
		return gids;
	def stk_get_groupname(self, gid):
		for k in self.keys:
			if self.groups[k]['gid'] == gid:
				return self.groups[k]['gname'].encode()
	def stk_add_group(self, n):
		self.groups += n
	def stk_get_groupinfo(self, gid):
		for k in self.keys:
			if self.groups[k]['gid'] == gid:
				return self.groups[k]
		return ''
	def stk_get_groups(self):
		tmp = []
		for k in self.keys:
			members = self.groups[k]['members'].encode()
			members = members.replace('-', ' ')
			item = (self.groups[k]['gid'], self.groups[k]['gname'].encode(), members)
			tmp.append(item)
		return tmp

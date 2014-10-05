#!/usr/bin/env python

import socket
import struct

import stkuser
from stkutil import running, socklist, updateinfo

STK_SERVER_PORT = 9007
STK_MAX_CLIENTS = 30

STK_MAX_PACKET_SIZE = 65535

STK_MAGIC       = 'ST'
STK_VERSION     = 0x0001
STK_CLIENT_FLAG = 0x00
STK_SERVER_FLAG = 0x01
STK_END         = 0x07

COMMANDS = {
	'REQ_LOGIN'       : 0x01,
	'LOGIN'           : 0x02,
	'KEEPALIVE'       : 0x03,
	'LOGOUT'          : 0x04,
	'GET_USER'        : 0x05,
	'GET_ONLINE_USER' : 0x06,
	'GET_USER_INFO'   : 0x07,
	'GET_GROUP'       : 0x08,
	'GET_GROUP_INFO'  : 0x09,
	'SEND_MSG'        : 0x0A,
	'REPLY_MSG'       : 0x0B,
	'SEND_GMSG'       : 0x0C,
	'REPLY_GMSG'      : 0x0D,
	'END'             : 0xFF
}

# user and group information
user  = stkuser.StkUsers()
group = stkuser.StkGroups()

class StkPacket:
	sp = { 'magic':'', 'version':0, 'cmd':0, 'sid':0, 'uid':0,
		'token':0, 'reserve':0, 'flag':0, 'length':0, 'data':'', 'end':0 }
	def __init__(self, buf):
		head = buf[:20]
		self.sp['data'] = buf[20:-1]
		self.sp['end'], = struct.unpack('!B', buf[-1])
		self.sp['magic'], \
		self.sp['version'], \
		self.sp['cmd'], \
		self.sp['sid'], \
		self.sp['uid'], \
		self.sp['token'], \
		self.sp['reserve'], \
		self.sp['flag'], \
		self.sp['length'] = struct.unpack('!2sHHHIIBBH', head) 

	def check_head_valid(self):
		if (self.sp['magic'] != STK_MAGIC \
			or self.sp['version'] != STK_VERSION \
			or self.sp['flag'] != STK_CLIENT_FLAG \
			or self.sp['end'] != STK_END):
			return False
		else:
			return True

	def get_stk_uid(self):
		return self.sp['uid']

	def get_stk_cmd(self):
		return self.sp['cmd']

	def get_stk_sid(self):
		return self.sp['sid']

	def get_stk_len(self):
		return self.sp['length']

	def get_stk_data(self):
		return self.sp['data']

	def show_stk_head(self):
		print 'Magic: %s' % self.sp['magic']
		print 'Version: 0x%04x' % self.sp['version']
		print 'Command: 0x%04x' % self.sp['cmd']
		print 'SessionID: %u' % self.sp['sid']
		print 'STKID: %u' % self.sp['uid']
		print 'Token: %u' % self.sp['token']
		print 'Reserved: %u' % self.sp['reserve']
		if self.sp['flag'] == 0:
			print 'Client Packet'
		else:
			print 'Server Packet'
		print 'Length: %u' % self.sp['length']
		print 'End: 0x%02u' % self.sp['end']
		print ''

class StkClient:
	sock = None
	uid = 0
	sid = 0
	state = 0
	token = 0
	reverse = 0
	def __init__(self, sock):
		self.sock = sock
	def stk_set_client(self, uid, sid):
		global socklist
		self.uid = uid
		self.sid = sid
		socklist[str(uid)] = self.sock
	def stk_get_sock(self):
		return self.sock
	def stk_reqlog_ack(self, data):
		buf = struct.pack('!2sHHHIIBBHB', STK_MAGIC, STK_VERSION, COMMANDS['REQ_LOGIN'], self.sid, 
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, 0, STK_END)
		self.sock.send(buf)
	def stk_login_ack(self, data):
		global updatedata, socklist, updateinfo
		passwd = user.stk_get_pass(self.uid)
		result = 0
		passtmp, reversetmp = struct.unpack('!32s64s', data) 
		passnew = passtmp.strip('\000')
		if passwd == 'STK_UNKNOWN_USER':
			result = 2
		elif self.state == 1:
			result = 1
		elif passwd == passnew:
			print 'STK Client %s(%u) is Login in.' % (user.stk_get_nickname(self.uid), self.uid)
			self.state = 1
			socklist[self.uid] = [self.sock, self.state]
			# Notify ui to update
			updateinfo.append([self.uid, u'online'])
			result = 0
		else:
			result = 3
		buf = struct.pack('!2sHHHIIBBHBB', STK_MAGIC, STK_VERSION, COMMANDS['LOGIN'], self.sid, 
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, 1, result, STK_END)
		self.sock.send(buf)
	def stk_keepalive_ack(self, data):
		pass
	def stk_getuser_ack(self, data):
		global user
		uids = user.stk_get_uids()
		length = 4 * (len(uids) - 1) + 2
		buf = struct.pack('!2sHHHIIBBHH', STK_MAGIC, STK_VERSION, COMMANDS['GET_USER'], self.sid, 
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, length, len(uids)-1)
		for k in uids:
			if k == self.uid:
				pass
			else:
				buf += struct.pack('!I', k)
		buf += struct.pack('!B', STK_END)
		self.sock.send(buf)
	def stk_getonlineuser_ack(self, data):
		pass
	def stk_getuserinfo_ack(self, data):
		global user
		uid, nickname, city, phone, gender = struct.unpack('!I32s16sIB', data) 
		uinfo = user.stk_get_userinfo(uid)
		length = 4 + 32 + 16 + 4 + 1
		buf = struct.pack('!2sHHHIIBBHI32s16sIBB', STK_MAGIC, STK_VERSION, COMMANDS['GET_USER_INFO'], self.sid,
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, length,  uinfo['uid'], 
				uinfo['nickname'].encode(), uinfo['city'].encode(), uinfo['phone'], uinfo['gender'], STK_END)
		self.sock.send(buf)
	def stk_getgroup_ack(self, data):
		global group
		gids = group.stk_get_gids()
		length = 4 * len(gids) + 2
		buf = struct.pack('!2sHHHIIBBHH', STK_MAGIC, STK_VERSION, COMMANDS['GET_GROUP'], self.sid, 
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, length, len(gids))
		for k in gids:
			buf += struct.pack('!I', k)
		buf += struct.pack('!B', STK_END)
		self.sock.send(buf)
	def stk_getgroupinfo_ack(self, data):
		global group
		gid, gname, membernum = struct.unpack('!I32sH', data) 
		ginfo = group.stk_get_groupinfo(gid)
		members = ginfo['members'].split('-')
		length = 4 + 32 + 2 + 4 * len(members)
		buf = struct.pack('!2sHHHIIBBHI32sH', STK_MAGIC, STK_VERSION, COMMANDS['GET_GROUP_INFO'], self.sid,
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, length,  ginfo['gid'], 
		                ginfo['gname'].encode(), len(members))
                for k in members:
			buf += struct.pack('!I', int(k))
		buf += struct.pack('!B', STK_END)
		self.sock.send(buf)
	def stk_sendmsg_ack(self, data):
		global user, socklist
		tmp = data[:4]
		msg = data[4:]
		uid, = struct.unpack('!I', tmp) 
		length = 4 + len(msg)
		buf = struct.pack('!2sHHHIIBBHI', STK_MAGIC, STK_VERSION, COMMANDS['SEND_MSG'], self.sid,
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, length, uid)
		buf += msg + struct.pack('!B', STK_END)
		psock = socklist[uid][0]
		if (psock != -1):
			psock.send(buf)
		else:
			print 'Msg From %s(%u) to %s(%u), \n  --- but %s is not online.' \
				%(user.stk_get_nickname(self.uid), self.uid, \
				user.stk_get_nickname(uid), uid, user.stk_get_nickname(uid))
	def stk_sendgmsg_ack(self, data):
		global group, socklist
		tmp = data[:4]
		msg = data[4:]
		gid, = struct.unpack('!I', tmp) 
		length = 4 + len(msg)
		buf = struct.pack('!2sHHHIIBBHI', STK_MAGIC, STK_VERSION, COMMANDS['SEND_GMSG'], self.sid,
				self.uid, self.token, self.reverse, STK_SERVER_FLAG, length, gid)
		buf += msg + struct.pack('!B', STK_END)
		ginfo = group.stk_get_groupinfo(gid)
		members = ginfo['members'].split('-')
		for k in members:
			if self.uid == int(k):
				continue
			psock = socklist[int(k)][0]
			if (psock != -1):
				psock.send(buf)
			else:
				print 'Msg form %s(%u) by %s(%u), \n  --- but %s(%u) is not online.' \
					% (group.stk_get_groupname(gid), gid, \
					user.stk_get_nickname(self.uid), self.uid, \
					user.stk_get_nickname(int(k)), int(k))					
								
def stk_socket_thread(t):
	c = t[0]
	client = StkClient(c)
	while 1:
		try:
			buf = c.recv(STK_MAX_PACKET_SIZE)
		# socket.timeout or socket.error or ...
		except socket.timeout:
			global running
			if running == False:
				break;
		except socket.error:
			# Whatever, error happen, just exit
			break;
		else:
			size = len(buf)
			if size == -1:
				print 'Recv Socket Error.'
				break;
			elif size == 0:
				print 'Peer Socket Shutdown.'
				break;
			elif size > STK_MAX_PACKET_SIZE:
				print 'Drop Packet(Too Large).'
				break;
			else:
				pass
			h = StkPacket(buf)
			#h.show_stk_head()
			if (h.check_head_valid() != True):
				print 'Bad STK Packet.'
				continue
			cmd = h.get_stk_cmd()
			uid = h.get_stk_uid()
			sid = h.get_stk_sid()
			data = h.get_stk_data()
			length = h.get_stk_len()
			del h
			if cmd == COMMANDS['REQ_LOGIN']:
				client.stk_set_client(uid, sid)
				client.stk_reqlog_ack(data)
			elif cmd == COMMANDS['LOGIN']:
				client.stk_login_ack(data)
			elif cmd == COMMANDS['KEEPALIVE']:
				client.stk_keepalive_ack(data)
			elif cmd == COMMANDS['LOGOUT']:
				pass
			elif cmd == COMMANDS['GET_USER']:
				client.stk_getuser_ack(data)
			elif cmd == COMMANDS['GET_ONLINE_USER']:
				client.stk_getonlineuser_ack(data)
			elif cmd == COMMANDS['GET_USER_INFO']:
				client.stk_getuserinfo_ack(data)
			elif cmd == COMMANDS['GET_GROUP']:
				client.stk_getgroup_ack(data)
			elif cmd == COMMANDS['GET_GROUP_INFO']:
				client.stk_getgroupinfo_ack(data)
			elif cmd == COMMANDS['SEND_MSG']:
				client.stk_sendmsg_ack(data)
			elif cmd == COMMANDS['REPLY_MSG']:
				pass
			elif cmd == COMMANDS['SEND_GMSG']:
				client.stk_sendgmsg_ack(data)
			elif cmd == COMMANDS['REPLY_GMSG']:
				pass
			else:
				print 'Unknow Command, Drop.'
				pass
	c.close
	# Notify ui to update
	global socklist, updateinfo
	socklist[uid] = [None, 0]
	updateinfo.append([uid, u'offline'])
	print 'Client socket thread exiting...'

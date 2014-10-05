#!/usr/bin/env python

import time
import socket
import struct

from stkutil import *

# All message use utf-8 encode
import sys
reload(sys)
sys.setdefaultencoding("utf-8")

STK_SERVER_PORT = 9007

STK_ERR = -1
STK_OK  = 0

STK_SOCK_OK       = 0
STK_SOCK_CLOSED   = -1
STK_SOCK_ERR      = -2
STK_SOCK_TIMEOUT  = -3
STK_PACKET_ERR    = -4

STK_CLIENT_OFFLINE   = 0
STK_CLIENT_ONLINE    = 1

STK_CLIENT_LOGIN_ERROR        = -1
STK_CLIENT_LOGIN_SUCCESS      = 0
STK_CLIENT_LOGIN_AGAIN        = 1
STK_CLIENT_LOGIN_INVALID_UID  = 2
STK_CLIENT_LOGIN_INVALID_PASS = 3

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

#user  = stkuser.StkUsers()
#group = stkuser.StkGroups()
# Format of socklist {uid:[sockfd, clientstatus], ...}
#socklist = {}

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
			or self.sp['flag'] != STK_SERVER_FLAG \
			or self.sp['end'] != STK_END):
			return False
		else:
			return True

	def get_stk_uid(self):
		return self.sp['uid']

	def get_stk_token(self):
		return self.sp['token']

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
	def __init__(self):
		self.sock = None
		self.uid = 0
		self.password = ''
		self.serverip = ''
		self.state = STK_CLIENT_OFFLINE
		self.sid = 0
		self.token = 0
		self.reverse = 0
		self.recv_buf = ''
		self.packet = None
	def stk_sock_run(self):
		while 1:
			running = stk_get_running()
			if running == STK_EXITING:
				break
			elif running == STK_CONNECTE_REQ:
				stk_set_running(STK_CONNECTING)
				ret = self.stk_connect()
				# User cancel the login Action*/
				if stk_get_running() != STK_CONNECTING:
					self.sock.close()
					continue;
				if ret == STK_SOCK_ERR:
					stk_set_running(STK_CONNECTE_ERR)
				else:
					# Connect to the server
					ret = self.stk_login()
					if ret == STK_CLIENT_LOGIN_SUCCESS:
						self.state = STK_CLIENT_ONLINE
						# Get my info
						ret = self.stk_get_profile(self.uid)
						if ret == STK_SOCK_ERR:
							print 'Get profile error!'
							stk_set_running(STK_SOCKET_ERR)
							self.sock.close()
							continue
						# Get my buddys
						ret = self.stk_get_buddy()
						if ret == STK_SOCK_ERR:
							print 'Get buddy error!'
							stk_set_running(STK_SOCKET_ERR)
							self.sock.close()
							continue
						# Get my groups
						ret = self.stk_get_group()
						if ret == STK_SOCK_ERR:
							print 'Get group error!'
							stk_set_running(STK_SOCKET_ERR)
							self.sock.close()
							continue
						else:
							stk_set_running(STK_CONNECTED)
							# Since we connected, set timeout to 0.5
							self.sock.settimeout(0.5)
							break
					elif ret == STK_CLIENT_LOGIN_ERROR:
						stk_set_running(STK_SOCKET_ERR)
						self.sock.close()
						continue
					elif ret == STK_CLIENT_LOGIN_INVALID_UID:
						stk_set_running(STK_USERID_ERR)
						self.sock.close()
						continue
					elif ret == STK_CLIENT_LOGIN_INVALID_PASS:
						stk_set_running(STK_PASSWORD_ERR)
						self.sock.close()
						continue
					elif ret == STK_CLIENT_LOGIN_AGAIN:
						stk_set_running(STK_ALREADY_LOGGIN)
						self.sock.close()
						continue
					else:
						stk_set_running(STK_SOCKET_ERR)
						self.sock.close()
						continue
		while 1:
			running = stk_get_running()
			if running == STK_EXITING:
				return 0
			elif running != STK_RUNNING:
				pass
				time.sleep(0.5)
			else:
				# Check any msg need to send to server
				if sendevent.is_set():
					sendevent.clear()
					while sendqueue.empty() != True:
						data = sendqueue.get()
						self.stk_send_msg(data[0], data[1], data[2])
				# Receive message from server, this will be blocked 
				ret = self.stk_recv_msg()
				if ret == STK_SOCK_ERR:
					stk_set_running(STK_SOCKET_ERR)
				elif ret == STK_SOCK_CLOSED:
					stk_set_running(STK_SERVER_EXIT)
	def stk_recv_data(self):
		try:
			self.recv_buf = self.sock.recv(STK_MAX_PACKET_SIZE)
		# socket.timeout or socket.error or ...
		except socket.timeout:
			return STK_SOCK_TIMEOUT
		else:
			size = len(self.recv_buf)
			if size == -1:
				print 'Connect Error'
				return STK_SOCK_ERR
			elif size == 0:
				print 'Peer Socket Shutdown'
				return STK_SOCK_CLOSED
			elif size > STK_MAX_PACKET_SIZE:
				print 'Packet too large, ignore'
				return STK_PACKET_ERR
			else:		
				self.packet = StkPacket(self.recv_buf)
				if self.packet.check_head_valid() != True:
					print 'Bad STK Packet'
					return STK_PACKET_ERR
				else:
					return STK_OK
	def stk_connect(self):
		# Get config
		config = stk_get_config()
		self.uid = config[0]
		self.password = config[1]
		self.serverip = config[2]
		# Connect 
		address = (self.serverip, STK_SERVER_PORT)  
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.settimeout(5)
		try:
			self.sock.connect(address)
		except socket.error as msg:
			s.close()
			s = None
			return STK_SOCK_ERR
		return STK_SOCK_OK
	def stk_login(self):
		buf = struct.pack('!2sHHHIIBBHB', STK_MAGIC, STK_VERSION, COMMANDS['REQ_LOGIN'], self.sid, 
				self.uid, self.token, self.reverse, STK_CLIENT_FLAG, 0, STK_END)		
		self.sock.send(buf)
		ret = self.stk_recv_data()
		if ret != STK_OK:
			return STK_CLIENT_LOGIN_ERROR
		else:
			length = 32 + 64
			buf = struct.pack('!2sHHHIIBBH32s64sB', STK_MAGIC, STK_VERSION, COMMANDS['LOGIN'], \
				self.sid, self.uid, self.token, self.reverse, STK_CLIENT_FLAG, \
				length, self.password, '', STK_END)		
			self.sock.send(buf)
			ret = self.stk_recv_data()
			self.token = self.packet.get_stk_token()
			if ret != STK_OK:
				return STK_CLIENT_LOGIN_ERROR
			else:
				result, = struct.unpack('!B', self.packet.sp['data'])
				return result
	def stk_get_profile(self, uid):
		length = 4 + 32 + 16 + 4 + 1
		buf = struct.pack('!2sHHHIIBBHI32s16sIBB', STK_MAGIC, STK_VERSION, COMMANDS['GET_USER_INFO'], \
			self.sid, self.uid, self.token, self.reverse, STK_CLIENT_FLAG, length, \
			uid, '', '', 0, STK_GENDER_UNKNOWN, STK_END)		
		self.sock.send(buf)
		ret = self.stk_recv_data()
		if ret != STK_OK:
			return STK_SOCK_ERR
		else:
			uid_n, name, city, phone, gender \
				= struct.unpack('!I32s16sIB', self.packet.sp['data'])
			if uid_n == self.uid:
				global profile
				profile['name'] = name.strip('\000')
				profile['city'] = city.strip('\000')
				profile['phone'] = phone
				profile['gender'] = gender
			else:
				global buddylist
				buddylist.append([uid_n, name.strip('\000'), city.strip('\000'), phone, gender])
			return STK_SOCK_OK
	def stk_get_buddy(self):
		buf = struct.pack('!2sHHHIIBBHB', STK_MAGIC, STK_VERSION, COMMANDS['GET_USER'], self.sid, 
				self.uid, self.token, self.reverse, STK_CLIENT_FLAG, 0, STK_END)		
		self.sock.send(buf)
		ret = self.stk_recv_data()
		if ret != STK_OK:
			return STK_SOCK_ERR
		else:
			buddy_num, = struct.unpack('!H', self.packet.sp['data'][0:2])
			buf = self.packet.sp['data'][2:]
			uids = []
			i = 0
			while i < buddy_num:
				buddy, = struct.unpack('!I', buf[i*4:i*4+4])
				uids.append(buddy)	
				i+=1
			for uid in uids:
				ret = self.stk_get_profile(uid)
				if ret == STK_SOCK_ERR:
					return ret
	def stk_get_groupinfo(self, gid):
		length = 4 + 32 + 2
		buf = struct.pack('!2sHHHIIBBHI32sHB', STK_MAGIC, STK_VERSION, COMMANDS['GET_GROUP_INFO'], \
			self.sid, self.uid, self.token, self.reverse, STK_CLIENT_FLAG, length, \
			gid, '', 0, STK_END)		
		self.sock.send(buf)
		ret = self.stk_recv_data()
		if ret != STK_OK:
			return STK_SOCK_ERR
		else:
			gid_n, gname, guid_num = struct.unpack('!I32sH', self.packet.sp['data'][0:38])
			buf = self.packet.sp['data'][38:]
			if gid_n != gid:
				return STK_SOCK_ERR
			guids = []
			i = 0
			while i < guid_num:
				guid, = struct.unpack('!I', buf[i*4:i*4+4])
				guids.append(guid)	
				i+=1
			global grouplist
			grouplist.append([gid, gname.strip('\000'), guids])
	def stk_get_group(self):
		buf = struct.pack('!2sHHHIIBBHB', STK_MAGIC, STK_VERSION, COMMANDS['GET_GROUP'], self.sid, 
				self.uid, self.token, self.reverse, STK_CLIENT_FLAG, 0, STK_END)		
		self.sock.send(buf)
		ret = self.stk_recv_data()
		if ret != STK_OK:
			return STK_SOCK_ERR
		else:
			group_num, = struct.unpack('!H', self.packet.sp['data'][0:2])
			buf = self.packet.sp['data'][2:]
			gids = []
			i = 0
			while i < group_num:
				gid, = struct.unpack('!I', buf[i*4:i*4+4])
				gids.append(gid)	
				i+=1
			for gid in gids:
				self.stk_get_groupinfo(gid)
				if ret == STK_SOCK_ERR:
					return ret				
	def stk_recv_msg(self):
		ret = self.stk_recv_data()
		if ret != STK_OK:
			return ret
		else:
			flag = False
			id_to, = struct.unpack('!I', self.packet.sp['data'][0:4])
			if self.packet.sp['cmd'] == COMMANDS['SEND_MSG']:
				mtype = STK_USER_TYPE
				if id_to != self.uid:
					print 'Oops, message is not send to me, drop it'
					return STK_PACKET_ERR
			elif self.packet.sp['cmd'] == COMMANDS['SEND_GMSG']:
				mtype = STK_GROUP_TYPE
				global grouplist
				for item in grouplist:
					if item[0] == id_to:
						flag = True
				if flag != True:
					print 'Oops, group message is not include me, drop it'
					return STK_PACKET_ERR
			if mtype == STK_USER_TYPE:
				idendify = self.packet.sp['uid']
			else:
				idendify = [id_to, self.packet.sp['uid']]
			buf = self.packet.sp['data'][4:]
			# Notify socket we got data to send
			uievent.set()
			uiqueue.put([mtype, idendify, buf])
			return STK_SOCK_OK
	def stk_send_msg(self, mtype, mid, mdata):
		if mtype == STK_USER_TYPE:
			cmd = COMMANDS['SEND_MSG']
		else:
			cmd = COMMANDS['SEND_GMSG']
		length = 4 + len(mdata)
		buf = struct.pack('!2sHHHIIBBHI', STK_MAGIC, STK_VERSION, cmd, self.sid,
				self.uid, self.token, self.reverse, STK_CLIENT_FLAG, length, mid)
		# mdata we got is a unicode string, change them all to utf-8
		buf = buf + str(mdata) + struct.pack('!B', STK_END)
		self.sock.send(buf)

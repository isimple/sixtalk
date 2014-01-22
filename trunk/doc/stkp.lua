do
	local p_Stkp = Proto("Stkp","SixTalk Protocol")
	local f_magic = ProtoField.string("Stkp.magic","Magic")
	local f_version = ProtoField.uint16("Stkp.version","Version",base.HEX)
	local f_cmd = ProtoField.uint16("Stkp.cmd","Command",base.HEX)
	local f_sid = ProtoField.uint16("Stkp.sid","Session ID")
	local f_uid = ProtoField.uint32("Stkp.uid","STK ID")
	local f_token = ProtoField.uint32("Stkp.token","Token",base.HEX)
	local f_reserve = ProtoField.uint8("Stkp.reserve","Reserved")
	local f_flag = ProtoField.uint8("Stkp.token","Flag",base.HEX,{ [0]="Client Packet", [1]="Server Packet"})
	local f_length = ProtoField.uint16("Stkp.length","Length")
	local f_end = ProtoField.uint8("Stkp.end","End",base.HEX)

	local f_password = ProtoField.bytes("Stkp.password","Password")
	local f_loginreverse = ProtoField.bytes("Stkp.loginreverse","Reverse")
	local f_loginresult = ProtoField.uint8("Stkp.loginresult","Result",base.HEX,{ [0]="Success", [1]="Already login in", [2]="Invalid Client ID", [3]="Password wrong"})

	local f_clientnum = ProtoField.uint16("Stkp.clientnum","Buddy Numbers")
	local f_clientid = ProtoField.uint32("Stkp.clientid","Buddy ID")

	local f_reqclientid = ProtoField.uint32("Stkp.reqclientid","Client ID")
	local f_nickname = ProtoField.string("Stkp.nickname","Nickname")
	local f_city = ProtoField.string("Stkp.city","City")
	local f_phone = ProtoField.uint32("Stkp.phone","Phone")
	local f_gender = ProtoField.uint32("Stkp.gender","Gender",base.HEX,{ [0]="Boy", [1]="Girl", [2]="Unknow"})

	local f_chatbuddy = ProtoField.uint32("Stkp.chatbuddy","Chat buddy ID")
	local f_msg = ProtoField.string("Stkp.msg","Chat Message")

	p_Stkp.fields = { f_magic, f_version, f_cmd, f_sid, f_uid, f_token, f_reserve, f_flag, f_length, f_end, f_password, f_loginreverse, f_loginresult, f_clientnum, f_clientid, f_reqclientid, f_nickname, f_city, f_phone, f_gender, f_chatbuddy, f_msg }
	
	local data_dis = Dissector.get("data")
	
	local function Stkp_dissector(buf,pkt,root)
		local buf_len = buf:len()
		local stkcmd = buf(4,2):uint()
		local flag = buf(17,1):uint()
		local bodylen = buf(18,2):uint()

		if buf_len < 20 then return false end
		if ((buf(0,1):uint()~=83) or (buf(1,1):uint()~=84))
			then return false end

		local t = root:add(p_Stkp,buf())
		pkt.cols.protocol = "STK Protocol"
		t:add(f_magic,buf(0,2))
		t:add(f_version,buf(2,2))
		t:add(f_cmd,buf(4,2))
		t:add(f_sid,buf(6,2))
		t:add(f_uid,buf(8,4))
		t:add(f_token,buf(12,4))
		t:add(f_reserve,buf(16,1))
		t:add(f_flag,buf(17,1))
		t:add(f_length,buf(18,2))

		if (stkcmd == 2) then
			if (flag == 0) then
				t:add(f_password,buf(20,32))
				t:add(f_loginreverse,buf(52,64))
			elseif(flag == 1) then
				t:add(f_loginresult,buf(20,1))
			end
		elseif(stkcmd == 3) then
		elseif(stkcmd == 4) then
		elseif(stkcmd == 5) then
			if (flag == 1) then
				t:add(f_clientnum,buf(20,2))
				local clientnums = buf(20,2):uint()
				for i=0,clientnums-1,1 do
					t:add(f_clientid,buf(22+i*4,4))
				end
			end

		elseif(stkcmd == 6) then
		elseif(stkcmd == 7) then
			t:add(f_reqclientid,buf(20,4))
			t:add(f_nickname,buf(24,32))
			t:add(f_city,buf(56,16))
			t:add(f_phone,buf(72,4))
			t:add(f_gender,buf(76,1))
		elseif(stkcmd == 8) then
			t:add(f_chatbuddy,buf(20,4))
			t:add(f_msg,buf(24,bodylen-4))
		elseif(stkcmd == 9) then
		end
		
		t:add(f_end,buf(20+bodylen,1))
		return true
	end
	
	function p_Stkp.dissector(buf,pkt,root) 
		if Stkp_dissector(buf,pkt,root) then
			--valid STKProtocol diagram
		else
			data_dis:call(buf,pkt,root)
		end
	end
	
	local tcp_encap_table = DissectorTable.get("tcp.port")
	tcp_encap_table:add(9007,p_Stkp)
end

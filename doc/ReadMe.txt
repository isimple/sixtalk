﻿使用说明
================================================================

--- 编译及使用方法

-- 编译环境:
-gtk版本需要先安装gtk+运行环境，笔者的gtk+版本为2.24.10
如果没有请到http://www.gtk.org/去下载并正确配置环境变量PATH

-Python版本需要Python运行环境，建议2.7.8
对于3.0及以上版本未测试，预计是无法运行的

-- 编译方法
-服务器
gtk-Windows
C:\WINDOWS\system32>cd /d N:\sixtalk\stkserver\gtk
N:\sixtalk\stkserver\gtk>runclean.bat
N:\sixtalk\stkserver\gtk>run.bat

gtk-Linux
# cd stkserver/gtk
# make 
# ./stkserver

Linux
# cd stkserver/linux
# make 
# ./stkserver

Python
直接双击stkserver.pyw

-客户端
gtk-Windows
C:\WINDOWS\system32>cd /d N:\sixtalk\stkserver\gtk
N:\sixtalk\stkclient\gtk>runclean.bat
N:\sixtalk\stkclient\gtk>run.bat

gtk-Linux
# cd stkclient/gtk
# make 
# ./stkclient

Python
直接双击stkclient.pyw

TIP:
1. 服务器默认客户端列表可查看stkserver/users文件
2. 服务器默认组列表可查看stkserver/groups文件
================================================================

--- Wireshark STKP协议插件

stkp_example.pcang是实例交互报文
为了让Wireshark支持STK Protocol
可将插件stkp.lua文件放到Wireshark安装目录下
在init.lua中最后添加下面这句话
dofile(DATA_DIR.."stkp.lua")

Wireshark插件的编写可参考如下网页
http://yoursunny.com/t/2008/Wireshark-Lua-dissector/
================================================================
如有任何问题，请联系 QQ 731711230


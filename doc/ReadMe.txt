使用说明
================================================================

--编译及使用方法

服务器
# cd stkserver
# make
# ./stkserver

客户端
# cd stkclient/linux
# make
# ./stkclient <stkserver ip> <uid>

TIP: 1. 开启多个客户端后可在命令行中进行文字聊天
     2. 服务器默认认可的客户端列表可查看stkserver/users文件

================================================================

--Wireshark STKP协议插件

stkp_example.pcang是实例交互报文
为了让Wireshark支持STK Protocol
可将插件stkp.lua文件放到Wireshark安装目录下
在init.lua中最后添加下面这句话
dofile('stkp.lua')

Wireshark插件的编写可参考如下网页
http://yoursunny.com/t/2008/Wireshark-Lua-dissector/


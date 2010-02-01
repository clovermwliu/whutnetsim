[appNodes]
node1=0
node2=1

[0]
type=TCPServer
para=NULL
Setting=BindAndListen

[0BindAndListen]
para1=80

[1]
type=TCPSend
para=1more
setting=Start

[1more]
para1=1
para2=80
para3=10000


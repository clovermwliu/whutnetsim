[AppNodes]
node0=0
node1=1

[0]
type=TCPServer
para=NULL
setting=BindAndListen

[0BindAndListen]
para1=80

[1]
type=TCPSend
para=1more
setting=Start

[1more]
para1=0
para2=80
para3=10000

[1start]
para1=0


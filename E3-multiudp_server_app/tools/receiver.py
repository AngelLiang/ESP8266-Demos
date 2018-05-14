#!/bin/sh python
# coding=utf-8
# Author: yannanxiu
"""
224.0.0.0～224.0.0.255为预留的组播地址（永久组地址），地址224.0.0.0保留不做分配，其它地址供路由协议使用；
224.0.1.0～224.0.1.255是公用组播地址，可以用于Internet；
224.0.2.0～238.255.255.255为用户可用的组播地址（临时组地址），全网范围内有效；
239.0.0.0～239.255.255.255为本地管理组播地址，仅在特定的本地范围内有效。

239.1.1.1组播测试成功！
"""

import time
import socket

SENDERIP = '192.168.10.168'
MYPORT = 7777
MYGROUP = '239.1.1.1'


def receiver():
    # create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    # allow multiple sockets to use the same PORT number
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # Bind to the port that we know will receive multicast data
    sock.bind((SENDERIP, MYPORT))
    # tell the kernel that we are a multicast socket
    # sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 255)
    # Tell the kernel that we want to add ourselves to a multicast group
    # The address for the multicast group is the third param
    status = sock.setsockopt(socket.IPPROTO_IP,
                             socket.IP_ADD_MEMBERSHIP,
                             socket.inet_aton(MYGROUP) + socket.inet_aton(SENDERIP))

    sock.setblocking(0)
    # ts = time.time()
    while 1:
        time.sleep(0.01)
        try:
            data, addr = sock.recvfrom(1024)
        except socket.error as e:
            pass
        else:
            print("Receive data!")

            print("TIME:", int(time.time()))
            print("FROM: ", addr)
            print("DATA: ", data)


if __name__ == "__main__":
    receiver()

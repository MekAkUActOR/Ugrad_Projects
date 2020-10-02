#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Dec 15 11:56:28 2019

@author: liuhaowen
"""

import socket
import tkinter
import time
import threading
import queue
import json  # json.dumps(some)打包   json.loads(some)解包
import sys
import shutil
import os
import os.path
import requests

que = queue.Queue()                             # 用于存放客户端发送的信息的队列
users = []                                      # 用于存放在线用户的信息  [conn, user, addr]
lock = threading.Lock()                         # 创建锁, 防止多个线程写入数据的顺序打乱
fileall = os.getcwd()
srpth = fileall + '/resources'
pth1 = fileall + '/server'
pth2 = fileall + '/server/resources'
# 创建文件传输文件夹
if not os.path.exists(pth1):
    os.mkdir(pth1)
if not os.path.exists(pth2):
    os.mkdir(pth2)
    if os.path.exists(srpth):
        for root, dirs, files in os.walk(srpth):
            for file in files:
                src_file = os.path.join(root, file)
                shutil.copy(src_file, pth2)
        
fileall = pth1

################################################################

IP = ''
PORT = ''
# 登陆窗口
root1 = tkinter.Tk()
root1.title('Server')
root1['height'] = 110
root1['width'] = 270
root1.resizable(0, 0)  # 限制窗口大小

IP1 = tkinter.StringVar()
IP1.set('127.0.0.1:50007')  # 默认显示的ip和端口
User = tkinter.StringVar()
User.set('')

# 服务器标签
labelIP = tkinter.Label(root1, text='Server address')
labelIP.place(x=20, y=10, width=100, height=40)

entryIP = tkinter.Entry(root1, width=80, textvariable=IP1)
entryIP.place(x=120, y=10, width=130, height=40)


# 登录按钮
def login(*args):
    global IP, PORT
    IP, PORT = entryIP.get().split(':')  # 获取IP和端口号
    PORT = int(PORT)                     # 端口号需要为int类型
    if not IP:
        tkinter.messagebox.showerror('Name type error', message='IP or PORT Empty!')
    #else:
        #root1.destroy()                  # 关闭窗口
        #tkinter.messagebox.showinfo(title='Message',message='Server start!')


root1.bind('<Return>', login)            # 回车绑定登录功能
but = tkinter.Button(root1, text='Set Server', command=login)
but.place(x=100, y=70, width=80, height=30)

root1.mainloop()

################################################################

# 将在线用户存入online列表并返回
def onlines():
    online = []
    for i in range(len(users)):
        online.append(users[i][1])
    return online


class ChatServer(threading.Thread):
    global users, que, lock

    def __init__(self, port):
        threading.Thread.__init__(self)
        self.ADDR = ('', port)
        os.chdir(sys.path[0])
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


    # 判断断开用户在users中是第几位并移出列表, 刷新客户端的在线用户显示
    def delUsers(self, conn, addr):
        a = 0
        for i in users:
            if i[0] == conn:
                users.pop(a)
                try:
                    i[0].close()
                except:
                    pass
                print(' Remaining online users: ', end='')         # 打印剩余在线用户(conn)
                d = onlines()
                self.recv(d, addr)
                print(d)
                break
            a += 1
            
        # 用于接收所有客户端发送信息的函数
    def tcp_connect(self, conn, addr):
        # 连接后将用户信息添加到users列表
        user = conn.recv(1024)                                    # 接收用户名
        user = user.decode()

        for i in range(len(users)):
            if user == users[i][1]:
                self.delUsers(users[i][0], users[i][2])

        if user == 'no':
            user = addr[0] + ':' + str(addr[1])
        users.append((conn, user, addr))
        print(' New connection:', addr, ':', user, end='')         # 打印用户名
        d = onlines()                                                   # 有新连接则刷新客户端的在线用户显示
        self.recv(d, addr)
        try:
            while True:
                data = conn.recv(1024)
                data = data.decode()
                self.recv(data, addr)                         # 保存信息到队列
            conn.close()
        except:
            print(user + ' Connection lose')
            self.delUsers(conn, addr)                             # 将断开用户移出users
            conn.close()

    # 将接收到的信息(ip,端口以及发送的信息)存入que队列
    def recv(self, data, addr):
        lock.acquire()
        try:
            que.put((addr, data))
        finally:
            lock.release()

    # 将队列que中的消息发送给所有连接到的用户
    def sendData(self):
        while True:
            if not que.empty():
                data = ''
                reply_text = ''
                message = que.get()                               # 取出队列第一个元素
                if isinstance(message[1], str):                   # 如果data是str则返回Ture
                    for i in range(len(users)):
                        # user[i][1]是用户名, users[i][2]是addr, 将message[0]改为用户名
                        try:
                            for j in range(len(users)):
                                if message[0] == users[j][2]:
                                    print(' this: message is from user[{}]'.format(j))
                                    data = ' ' + users[j][1] + '：' + message[1]
                                    break      
                            users[i][0].send(data.encode())
                        except:
                            pass
                # data = data.split(':;')[0]
                if isinstance(message[1], list):  # 同上
                    # 如果是list则打包后直接发送
                    data = json.dumps(message[1])
                    for i in range(len(users)):
                        try:
                            users[i][0].send(data.encode())
                        except:
                            pass

    def run(self):
        self.s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        self.s.bind(self.ADDR)
        self.s.listen(5)
        print('Chat server starts running...')
        q = threading.Thread(target=self.sendData)
        q.start()
        while True:
            conn, addr = self.s.accept()
            t = threading.Thread(target=self.tcp_connect, args=(conn, addr))
            t.start()
        self.s.close()

################################################################


class FileServer(threading.Thread):
    def __init__(self, port):
        threading.Thread.__init__(self)
        # self.setDaemon(True)
        self.ADDR = ('', port)
        # self.PORT = port
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        os.chdir(fileall) 
        self.first = r'./resources'
        os.chdir(self.first)                                     # 把first设为当前工作路径
        # self.conn = None

    def tcp_connect(self, conn, addr):
        print(' Connected by: ', addr)
        
        while True:
            data = conn.recv(1024)
            data = data.decode()
            if data == 'quit':
                print('Disconnected from {0}'.format(addr))
                break
            order = data.split(' ')[0]                             # 获取动作
            self.recv_func(order, data, conn)
                
        conn.close()

    # 传输当前目录列表
    def sendList(self, conn):
        listdir = os.listdir(os.getcwd())
        listdir = json.dumps(listdir)
        conn.sendall(listdir.encode())

    # 发送文件函数
    def sendFile(self, message, conn):
        name = message.split()[1]                               # 获取第二个参数(文件名)
        fileName = r'./' + name
        with open(fileName, 'rb') as f:    
            while True:
                a = f.read(1024)
                if not a:
                    break
                conn.send(a)
        time.sleep(0.2)                                          # 延时确保文件发送完整
        conn.send('EOF'.encode())

    # 保存上传的文件到当前工作目录
    def recvFile(self, message, conn):
        name = message.split()[1]                              # 获取文件名
        fileName = r'./' + name
        with open(fileName, 'wb') as f:
            while True:
                data = conn.recv(1024)
                if data == 'EOF'.encode():
                    break
                f.write(data)

    # 切换工作目录
    def cd(self, message, conn):
        message = message.split()[1]                          # 截取目录名
        # 如果是新连接或者下载上传文件后的发送则 不切换 只将当前工作目录发送过去
        if message != 'same':
            f = r'./' + message
            os.chdir(f)
        # path = ''
        path = os.getcwd().split('\\')                        # 当前工作目录
        for i in range(len(path)):
            if path[i] == 'resources':
                break
        pat = ''
        for j in range(i, len(path)):
            pat = pat + path[j] + ' '
        pat = '\\'.join(pat.split())
        # 如果切换目录超出范围则退回切换前目录
        if 'resources' not in path:
            os.chdir(fileall) 
            f = r'./resources'
            os.chdir(f)
            pat = 'resources'
        conn.send(pat.encode())

    # 判断输入的命令并执行对应的函数
    def recv_func(self, order, message, conn):
        if order == 'get':
            return self.sendFile(message, conn)
        elif order == 'put':
            return self.recvFile(message, conn)
        elif order == 'dir':
            return self.sendList(conn)
        elif order == 'cd':
            return self.cd(message, conn)

    def run(self):
        print('File server starts running...')
        self.s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
        self.s.bind(self.ADDR)
        self.s.listen(3)
        while True:
            conn, addr = self.s.accept()
            t = threading.Thread(target=self.tcp_connect, args=(conn, addr))
            t.start()
        self.s.close()

#############################################################################

if __name__ == '__main__':
    cserver = ChatServer(PORT)
    fserver = FileServer(PORT + 1)
    cserver.start()
    fserver.start()
    while True:
        time.sleep(1)
        if not cserver.isAlive():
            print("Chat connection lost...")
            sys.exit(0)
        if not fserver.isAlive():
            print("File connection lost...")
            sys.exit(0)

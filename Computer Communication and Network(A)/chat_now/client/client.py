#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Dec 15 12:36:25 2019

@author: liuhaowen
"""

import socket
import threading
import json  # json.dumps(some)打包   json.loads(some)解包
import tkinter
import tkinter.messagebox
from tkinter.scrolledtext import ScrolledText  # 导入多行文本框用到的包
import time
import requests
from tkinter import filedialog
import os
import sys
from time import sleep
import pyscreenshot as ImageGrab
from tkinter import messagebox

IP = ''
PORT = ''
user = ''
listbox1 = ''  # 用于显示在线用户的列表框
ii = 0  # 用于判断是开还是关闭列表框
users = []  # 在线用户列表
chat = '------Group chat-------'  # 聊天对象, 默认为群聊
# 登陆窗口
root1 = tkinter.Tk()
root1.title('Log in')
root1['height'] = 110
root1['width'] = 270
root1.resizable(0, 0)  # 限制窗口大小

IP1 = tkinter.StringVar()
IP1.set('127.0.0.1:50007')  # 默认显示的ip和端口
User = tkinter.StringVar()
User.set('')

# 服务器标签
labelIP = tkinter.Label(root1, text='Server address')
labelIP.place(x=20, y=10, width=100, height=20)

entryIP = tkinter.Entry(root1, width=80, textvariable=IP1)
entryIP.place(x=120, y=10, width=130, height=20)

# 用户名标签
labelUser = tkinter.Label(root1, text='Username')
labelUser.place(x=30, y=40, width=80, height=20)

entryUser = tkinter.Entry(root1, width=80, textvariable=User)
entryUser.place(x=120, y=40, width=130, height=20)


# 登录按钮
def login(*args):
    global IP, PORT, user
    IP, PORT = entryIP.get().split(':')  # 获取IP和端口号
    PORT = int(PORT)                     # 端口号需要为int类型
    user = entryUser.get()
    if not user:
        tkinter.messagebox.showerror('Name type error', message='Username Empty!')
    else:
        root1.destroy()                  # 关闭窗口


root1.bind('<Return>', login)            # 回车绑定登录功能
but = tkinter.Button(root1, text='Log in', command=login)
but.place(x=100, y=70, width=70, height=30)

root1.mainloop()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((IP, PORT))
if user:
    s.send(user.encode())  # 发送用户名
else:
    s.send('no'.encode())  # 没有输入用户名则标记no

# 如果没有用户名则将ip和端口号设置为用户名
addr = s.getsockname()  # 获取客户端ip和端口号
addr = addr[0] + ':' + str(addr[1])
if user == '':
    user = addr

# 聊天窗口
# 创建图形界面
root = tkinter.Tk()
root.title(user)  # 窗口命名为用户名
root['height'] = 400
root['width'] = 580
root.resizable(0, 0)  # 限制窗口大小

# 创建多行文本框
listbox = ScrolledText(root)
listbox.place(x=5, y=0, width=570, height=320)
# 文本框使用的字体颜色
listbox.tag_config('red', foreground='red')
listbox.tag_config('blue', foreground='blue')
listbox.tag_config('green', foreground='green')
listbox.tag_config('pink', foreground='pink')
listbox.insert(tkinter.END, 'Welcome to the chat room!', 'blue')

# 表情功能代码部分
# 四个按钮, 使用全局变量, 方便创建和销毁
b1 = ''
b2 = ''
b3 = ''
b4 = ''
# 将图片打开存入变量中
p1 = tkinter.PhotoImage(file='./emoji/facepalm.png')
p2 = tkinter.PhotoImage(file='./emoji/smirk.png')
p3 = tkinter.PhotoImage(file='./emoji/concerned.png')
p4 = tkinter.PhotoImage(file='./emoji/smart.png')
# 用字典将标记与表情图片一一对应, 用于后面接收标记判断表情贴图
dic = {'aa**': p1, 'bb**': p2, 'cc**': p3, 'dd**': p4}
ee = 0  # 判断表情面板开关的标志


# 发送表情图标记的函数, 在按钮点击事件中调用


def mark(exp):  # 参数是发的表情图标记, 发送后将按钮销毁
    global ee
    mes = exp + ':;' + user + ':;' + chat
    s.send(mes.encode())
    b1.destroy()
    b2.destroy()
    b3.destroy()
    b4.destroy()
    ee = 0


# 四个对应的函数
def bb1():
    mark('aa**')


def bb2():
    mark('bb**')


def bb3():
    mark('cc**')


def bb4():
    mark('dd**')


def express():
    global b1, b2, b3, b4, ee
    if ee == 0:
        ee = 1
        b1 = tkinter.Button(root, command=bb1, image=p1,
                            relief=tkinter.FLAT, bd=0)
        b2 = tkinter.Button(root, command=bb2, image=p2,
                            relief=tkinter.FLAT, bd=0)
        b3 = tkinter.Button(root, command=bb3, image=p3,
                            relief=tkinter.FLAT, bd=0)
        b4 = tkinter.Button(root, command=bb4, image=p4,
                            relief=tkinter.FLAT, bd=0)

        b1.place(x=5, y=248)
        b2.place(x=75, y=248)
        b3.place(x=145, y=248)
        b4.place(x=215, y=248)
    else:
        ee = 0
        b1.destroy()
        b2.destroy()
        b3.destroy()
        b4.destroy()


# 创建表情按钮
eBut = tkinter.Button(root, text='emoji', command=express)
eBut.place(x=5, y=320, width=60, height=30)

# 开始截图
def buttonCaptureClick():
    filename = 'temp.png'
    # grab()方法默认对全屏幕进行截图
    im = ImageGrab.grab()
    im.save(filename)
    im.close()


# 创建截屏按钮
sBut = tkinter.Button(root, text='Capture', command=buttonCaptureClick)
sBut.place(x=95, y=320, width=60, height=30)

# 文件功能代码部分
# 将在文件功能窗口用到的组件名都列出来, 方便重新打开时会对面板进行更新
list2 = ''  # 列表框
label = ''  # 显示路径的标签
upload = ''  # 上传按钮
close = ''  # 关闭按钮


def fileClient():
    PORT2 = PORT + 1  # 聊天室的端口为chatport+1
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((IP, PORT2))

    # 修改root窗口大小显示文件管理的组件
    root['height'] = 390
    root['width'] = 760

    # 创建列表框
    list2 = tkinter.Listbox(root)
    list2.place(x=580, y=25, width=175, height=325)

    # 将接收到的目录文件列表打印出来(dir), 显示在列表框中, 在pwd函数中调用
    def recvList(enter, lu):
        s.send(enter.encode())
        data = s.recv(4096)
        data = json.loads(data.decode())
        list2.delete(0, tkinter.END)  # 清空列表框
        lu = lu.split('\\')
        if len(lu) != 1:
            list2.insert(tkinter.END, 'Return to the previous dir')
            list2.itemconfig(0, fg='green')
        for i in range(len(data)):
            list2.insert(tkinter.END, ('' + data[i]))
            if '.' not in data[i]:
                list2.itemconfig(tkinter.END, fg='orange')
            else:
                list2.itemconfig(tkinter.END, fg='blue')

    # 创建标签显示服务端工作目录
    def lab():
        global label
        data = s.recv(1024)  # 接收目录
        lu = data.decode()
        try:
            label.destroy()
            label = tkinter.Label(root, text=lu)
            label.place(x=580, y=0, )
        except:
            label = tkinter.Label(root, text=lu)
            label.place(x=580, y=0, )
        recvList('dir', lu)

    # 进入指定目录(cd)
    def cd(message):
        s.send(message.encode())

    # 刚连接上服务端时进行一次面板刷新
    cd('cd same')
    lab()

    # 接收下载文件(get)
    def get(message):
        # print(message)
        name = message.split(' ')
        # print(name)
        name = name[1]  # 获取命令的第二个参数(文件名)
        # 选择对话框, 选择文件的保存路径
        fileName = tkinter.filedialog.asksaveasfilename(title='Save file to', initialfile=name)
        # 如果文件名非空才进行下载
        if fileName:
            s.send(message.encode())
            with open(fileName, 'wb') as f:
                while True:
                    data = s.recv(1024)
                    if data == 'EOF'.encode():
                        tkinter.messagebox.showinfo(title='Message',
                                                    message='Download completed!')
                        break
                    f.write(data)

    # 创建用于绑定在列表框上的函数
    def run(*args):
        indexs = list2.curselection()
        index = indexs[0]
        content = list2.get(index)
        # 如果有一个 . 则为文件
        if '.' in content:
            content = 'get ' + content
            get(content)
            cd('cd same')
        elif content == 'Return to the previous dir':
            content = 'cd ..'
            cd(content)
        else:
            content = 'cd ' + content
            cd(content)
        lab()  # 刷新显示页面

    # 在列表框上设置绑定事件
    list2.bind('<ButtonRelease-1>', run)

    # 上传客户端所在文件夹中指定的文件到服务端, 在函数中获取文件名, 不用传参数
    def put():
        # 选择对话框
        fileName = tkinter.filedialog.askopenfilename(title='Select upload file')
        # 如果有选择文件才继续执行
        if fileName:
            name = fileName.split('/')[-1]
            message = 'put ' + name
            s.send(message.encode())
            with open(fileName, 'rb') as f:
                while True:
                    a = f.read(1024)
                    if not a:
                        break
                    s.send(a)
                time.sleep(5)  # 延时确保文件发送完整
                s.send('EOF'.encode())
                tkinter.messagebox.showinfo(title='Message',
                                            message='Upload completed!')
        cd('cd same')
        lab()  # 上传成功后刷新显示页面

    # 创建上传按钮, 并绑定上传文件功能
    upload = tkinter.Button(root, text='Upload file', command=put)
    upload.place(x=600, y=353, height=30, width=80)

    # 关闭文件管理器, 待完善
    def closeFile():
        root['height'] = 390
        root['width'] = 580
        # 关闭连接
        s.send('quit'.encode())
        s.close()

    # 创建关闭按钮
    close = tkinter.Button(root, text='Close', command=closeFile)
    close.place(x=685, y=353, height=30, width=70)


# 创建文件按钮
fBut = tkinter.Button(root, text='File', command=fileClient)
fBut.place(x=185, y=320, width=60, height=30)

# 创建多行文本框, 显示在线用户
listbox1 = tkinter.Listbox(root)
listbox1.place(x=445, y=0, width=130, height=320)


def users():
    global listbox1, ii
    if ii == 1:
        listbox1.place(x=445, y=0, width=130, height=320)
        ii = 0
    else:
        listbox1.place_forget()  # 隐藏控件
        ii = 1


# 查看在线用户按钮
button1 = tkinter.Button(root, text='Users online', command=users)
button1.place(x=485, y=320, width=90, height=30)

# 创建输入文本框和关联变量
a = tkinter.StringVar()
a.set('')
entry = tkinter.Entry(root, width=120, textvariable=a)
entry.place(x=5, y=350, width=570, height=40)

def send(*args):
    # 没有添加的话发送信息时会提示没有聊天对象
    users.append('------Group chat-------')
    #users.append('Robot')
    print(chat)
    if chat not in users:
        tkinter.messagebox.showerror('Send error', message='There is nobody to talk to!')
        return
    if chat == user:
        tkinter.messagebox.showerror('Send error', message='Cannot talk with yourself in private!')
        return
    mes = entry.get() + ':;' + user + ':;' + chat  # 添加聊天对象标记
    s.send(mes.encode())
    a.set('')  # 发送后清空文本框


# 创建发送按钮
button = tkinter.Button(root, text='Send', command=send)
button.place(x=515, y=353, width=60, height=30)
root.bind('<Return>', send)  # 绑定回车发送信息


# 私聊功能
def private(*args):
    global chat
    # 获取点击的索引然后得到内容(用户名)
    indexs = listbox1.curselection()
    index = indexs[0]
    if index > 0:
        chat = listbox1.get(index)
        # 修改客户端名称
        if chat == '------Group chat-------':
            root.title(user)
            return
        ti = user + '  -->  ' + chat
        root.title(ti)


# 在显示用户列表框上设置绑定事件
listbox1.bind('<ButtonRelease-1>', private)


# 用于时刻接收服务端发送的信息并打印
def recv():
    global users
    while True:
        try:
            data = s.recv(1024)
            data = data.decode()
            # 没有捕获到异常则表示接收到的是在线用户列表
            try:
                data = json.loads(data)
                users = data
                listbox1.delete(0, tkinter.END)  # 清空列表框
                number = ('   Users online: ' + str(len(data)))
                listbox1.insert(tkinter.END, number)
                listbox1.itemconfig(tkinter.END, fg='green', bg="#f0f0ff")
                listbox1.insert(tkinter.END, '------Group chat-------')
                listbox1.itemconfig(tkinter.END, fg='green')
                for i in range(len(data)):
                    listbox1.insert(tkinter.END, (data[i]))
                    listbox1.itemconfig(tkinter.END, fg='green')
            except:
                data = data.split(':;')
                data1 = data[0].strip()  # 消息
                data2 = data[1]  # 发送信息的用户名
                data3 = data[2]  # 聊天对象
                markk = data1.split('：')[1]
                if (markk in dic):
                    data4 = '\n' + data2 + '：'  # 例:名字-> \n名字：
                    if data3 == '------Group chat-------':
                        if data2 == user:  # 如果是自己则将则字体变为蓝色
                            listbox.insert(tkinter.END, data4, 'blue')
                        else:
                            listbox.insert(tkinter.END, data4, 'green')  # END将信息加在最后一行
                        listbox.image_create(tkinter.END, image=dic[markk])
                    elif data2 == user or data3 == user:  # 显示私聊
                        listbox.insert(tkinter.END, data4, 'red')  # END将信息加在最后一行
                        listbox.image_create(tkinter.END, image=dic[markk])
                else:
                    data1 = '\n' + data1
                    if data3 == '------Group chat-------':
                        if data2 == user:  # 如果是自己则将则字体变为蓝色
                            listbox.insert(tkinter.END, data1, 'blue')
                        else:
                            listbox.insert(tkinter.END, data1, 'green')  # END将信息加在最后一行
                        if len(data) == 4:
                            listbox.insert(tkinter.END, '\n' + data[3], 'pink')
                    elif data2 == user or data3 == user:  # 显示私聊
                        listbox.insert(tkinter.END, data1, 'red')  # END将信息加在最后一行
                listbox.see(tkinter.END)  # 显示在最后
        except:
            pass

def reflash():
    global s, IP, PORT, user
    if messagebox.askokcancel("reflash", "Do you want to reflash the connection?"):
        s.close()
        print('reflashing...\n')
        time.sleep(1)
        m = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        m.connect((IP, PORT))
        m.send(user.encode())  # 发送用户名
        s = m
        print('reflash done\n')
    
rebu = tkinter.Button(root, text='reflash', command=reflash)
rebu.place(x=325, y=320, width=60, height=30)

            
def on_closing():
    global s
    if messagebox.askokcancel("Quit", "Do you want to quit?"):
        s.close()
        root.destroy()
        print('log out!')
    
root.protocol("WM_DELETE_WINDOW", on_closing)

r = threading.Thread(target=recv)
r.start()  # 开始线程接收信息

root.mainloop()

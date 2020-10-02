#!/usr/bin/env python 
# -*- coding:utf-8 -*-
import time
import math
import copy
from tqdm import tqdm

def file_to_bit(source_file):           #将二进制文件转换成二进制字符串
    file1 = open(source_file,'rb')
    binary=file1.read()
    file1.close()

    lenth=binary.__len__()              #统计二进制字节长度

                                        #字节转换成bit字符串
    number=[0 for i in range(0,lenth)]  #字节流转换成数字
    source=""
    for i in range(0,lenth):
        # print(i)
        number[i]=int(binary[i])
        l=bin(number[i])#辅助符号
        l=l.lstrip('0b')
        while l.__len__() < 8 :         #对每一个字节补足左边省略的‘0’
            l='0'+l
        source=source+l

    str_len=bin(lenth)
    str_len=str_len.lstrip('0b')
    str_len=str_len.zfill(24)           #记录字符串长度并用24位二进制表示，左边补‘0’

    str_len1=''
    for i in range(0,24):
        if (str_len[i]=='1'):
            str_len1=str_len1+'0'
        elif (str_len[i]=='0'):
            str_len1=str_len1+'1'

    return source,str_len1             #返回二进制字符串和文本字节数

def inversion(binary):                 #对二进制字符串按位取反，避免对全0字节进行省略
    len=binary.__len__()
    len_2=''
    for i in range(0,len):
        if (binary[i]=='1'):
            len_2=len_2+'0'
        elif (binary[i]=='0'):
            len_2=len_2+'1'
    return len_2


def frequency(source):
    num=[]
    len=source.__len__()
    temp=source
    result=dict()
    for i in range(0,int(len/8)):
        j=8*i
        num.append(temp[j:j+8])
    for i in num:
        if i in result:
            result[i]+=1
        else:
            result.update({i:1})
    return result                      #对二进制字符串以8位为单位分析出现频率，并将结果用字典存放

class node:                            #建立节点类
    def __init__(self):
        self.frequency=0
        self.name=None
        self.l=None
        self.r=None
        self.code=None
    def __lt__(self, other):           #利用富比较排序各字节出现的频率
        return self.frequency<other.frequency

def huffman(dict):                     #返回赫夫曼数的根节点
    node_ls=[]
    for i in dict:
        a = node()
        a.frequency=dict[i]
        a.name=i
        node_ls.append(a)
    while len(node_ls)>1:
        node_ls.sort(reverse=True)
        temp_1=node_ls.pop()
        temp_2=node_ls.pop()
        new=node()
        new.frequency=temp_1.frequency+temp_2.frequency
        new.l=temp_1
        new.r=temp_2
        node_ls.append(new)
    return new


def code_ls(node,result,code):              #运用递归函数构造赫夫曼树
    if node.name:
        result.update({node.name:code})
        return
    code+='0'
    code_ls(node.l,result,code)
    code = code[:-1]
    code+='1'
    code_ls(node.r,result,code)
    return result                           #用字典存放8位二进制数和相应的码字

def lz_encode_dic_make(binary_str):   # lz编码建立字典
    num = []  # 组号表
    dic = []  # 字典表
    last = []  # 码字表
    dic.append('')
    i = 0  # 用来判断读取完成没有
    j = 0  # 去掉一个后的组好
    str1 = ''
    str2 = ''
    pbar = tqdm(total=binary_str.__len__())  # 进度条
    while i < binary_str.__len__():
        pbar.update(1)
        str1 = str1 + binary_str[i]
        flag = str1 in dic
        if (flag == True and i < binary_str.__len__() - 1):
            str2 = str2 + binary_str[i]
            i = i + 1
            continue
        elif (flag == False):
            str1_copy = copy.deepcopy(str1)
            dic.append(str1_copy)
            j = dic.index(str2)
            j_copy = copy.deepcopy(j)
            num.append(j_copy)
            last.append(binary_str[i])
            str1 = ''
            str2 = ''
            i = i + 1
        else:
            str1_copy = copy.deepcopy(str1)
            dic.append(str1_copy)
            j = dic.index(str2)
            j_copy = copy.deepcopy(j)
            num.append(j_copy)
            last.append(binary_str[i])
            str1 = ''
            str2 = ''
            i = i + 1
    pbar.close()
    return num,dic,last

def lz_encode_code_make(num,dic,last):     # lz编码建立编码
    code = ''
    for i in range(0, num.__len__()):
        temp1 = math.log(i + 1, 2)  # 码字中组号长度
        temp1 = math.ceil(temp1)  # 向上取整
        temp2 = bin(num[i])
        temp2 = temp2.lstrip('0b')
        while temp2.__len__() < temp1:
            temp2 = '0' + temp2
        while i == 0 and temp2.__len__() == 0:
            temp2 = '0'
        code = code + temp2
        code = code + last[i]
    while code.__len__() % 8 != 0:  # 因为一个字节为8bit，最小单位为字节
        code = code + '0'
    code_int = int(code, 2)
    code_hex = hex(code_int)
    code_hex = code_hex.lstrip('0x')
    code_byte = bytes.fromhex(code_hex)
    return code_byte
def lz_decode_dic_make(binary_str):
    num = []  # 组号表
    last = []  # 码字表
    i = 0  # 用来判断读取完成没有
    j = 1  # 段号
    k = 0  # 用来判断读了几位了
    code_lenth = 1
    str1 = ''
    str2 = ''
    while i < binary_str.__len__():
        k = k + 1
        str1 = str1 + binary_str[i]
        temp1 = math.log(j, 2)
        temp1 = math.ceil(temp1)
        if j == 1:
            temp1 = 1
        if k < temp1 + code_lenth:
            str2 = str2 + binary_str[i]
            i = i + 1
            continue
        else:
            num1 = int(str2, 2)
            # print(num1)
            last.append(binary_str[i])
            num.append(num1)
            k = 0
            j = j + 1
            i = i + 1
            str1 = ''
            str2 = ''
    dic = []
    dic.append('')
    i = 0
    code = ''
    while i < num.__len__():
        temp3 = dic[num[i]] + last[i]
        dic.append(temp3)
        i = i + 1
    return dic

def lz_encode(source_file,res_text):
    binary_str, text_len = file_to_bit(source_file)  # 文件转换为二进制字符串
    bytes_len = inversion(text_len)
    bytes_len = int(bytes_len, 2)
    print("source file bytes:", bytes_len)#输出源文件大小

    num,dic,last=lz_encode_dic_make(binary_str) #建立lz的字典

    code_byte=lz_encode_code_make(num,dic,last)   #建立lz编码

    img2 = open(res_text, 'wb')
    img2.write(code_byte)
    img2.close()

    print("encoded file bytes:", code_byte.__len__())#输出压缩文件大小

def lz_decode(source_file,res_file):
    binary_str, text_len = file_to_bit(source_file)  # 文件转换为二进制字符串
    bytes_len = inversion(text_len)
    bytes_len = int(bytes_len, 2)
    print("encode file bytes:", bytes_len)#输出压缩文件大小

    dic=lz_decode_dic_make(binary_str)#从二进制字符串中提取出lz字典信息

    #从建立好的字典恢复源码
    code = ''
    i = 1
    while i < dic.__len__():
         temp4 = dic[i]
         code = code + temp4
         i = i + 1
    code_int = int(code, 2)
    code_hex = hex(code_int)
    code_hex = code_hex.lstrip('0x')
    code_byte = bytes.fromhex(code_hex)

    img2 = open(res_file, 'wb')
    img2.write(code_byte)
    img2.close()
    print("source file bytes:", code_byte.__len__())

def encode(source_file,res_text):
    source,text_len=file_to_bit(source_file)        #文件转换为二进制字符串
    bytes_len=inversion(text_len)
    bytes_len=int(bytes_len,2)
    print("source file bytes:",bytes_len)

    result = frequency(source)
    base_node = huffman(result)
    code_dict = code_ls(base_node,{},'')
    num=code_dict.__len__()
    a=list(code_dict.keys())

    str_len1=text_len

    for i in range(0,num):
        str_len1 = str_len1 + a[i]
        num1=code_dict[a[i]].__len__()
        num2=bin(num1)                          #记录码字位数
        num2=num2.lstrip('0b')
        num2=num2.zfill(8)
        str_len1 = str_len1 + num2
        str_len1 = str_len1 + code_dict[a[i]]         #生成记录文本字节数和赫夫曼节点编码的二进制字符串

    len=source.__len__()
    num=[]
    lenth2=str_len1.__len__()        #用lenth2记录str_len1的总长度

    lenth3=bin(lenth2)
    lenth3=lenth3.lstrip('0b')
    lenth3=lenth3.zfill(24)         #补足3个字节

    # str_len2=''
    # for i in range(0,24):
    #     if (lenth3[i]=='1'):
    #         str_len2=str_len2+'0'
    #     elif (lenth3[i]=='0'):
    #         str_len2=str_len2+'1'
    str_len2=inversion(lenth3)

                                    #按位取反
    string=str_len2+str_len1        #编码前存放数据位数（str_len2)、文本字节数和编码字典（str_len1)

    l=string.__len__()
    print("data numbers:",l/8)


    for i in range(0,int(len/8)):
        i=8*i
        num.append(source[i:i+8])
    for i in num:
        string+=code_dict[i]        #对二进制字符串以8位为单位进行编码

    while string.__len__() % 8 != 0:
        string = string + '0'

    str_int=int(string,2)
    str_hex=hex(str_int)
    str_hex=str_hex.lstrip('0x')
    code_byte=bytes.fromhex(str_hex)
    with open(res_text,'wb')as f:      #二进制写回文件
        f.write(code_byte)

    ls = (string.__len__()-l)/8
    print("code numbers:",ls)
    print("encoded file bytes:",code_byte.__len__())
    print("encode ratio:",bytes_len/ls)
    print("compression ratio",bytes_len/code_byte.__len__())



def get_data(source):              #解码使获取文本所含字节数和编码字典
    len_1=source[0:24]
    len_1=inversion(len_1)
    text_len=int(len_1,2)      #文档字节数
    len_2=source.__len__()
    temp=source[24:]
    len_2=len_2-24

    code_dict=dict()

    while(len_2>0):           #生成赫夫曼编码字典表，此字典的value值和key值与编码中的相反
        k=temp[0:8]
        l1=temp[8:16]
        l=int(l1,2)
        va=temp[16:16+l]
        code_dict.update({va:k})

        temp1=temp
        temp=''
        for i in range (16+l,len_2):
            temp=temp+temp1[i]

        len_2=len_2-16-l

    return code_dict,text_len

def decode(souce_file,res_file):
    source,len=file_to_bit(source_file)
    data_len=source[0:24]
    data_len=inversion(data_len)
    data_len=int(data_len,2)               #二进制字符串中包含文档字节数和赫夫曼字典的位数

    binary=source[24:24+data_len]          #记录文本字节数和编码字典的二进制字符串
    code_dict,text_len=get_data(binary)

    decode_str=source[24+data_len:]         #对原文档编码的字符串

    a=list(code_dict.keys())                #码字
    num=[]

    len1=inversion(len)
    len1=int(len1,2)

    print("encoded file bytes:",len1)
    j=0
    b=''
    while(text_len>0):                      #记录已经译码的字节数，直至已译码的字节数=原文档字节数
        b=b+decode_str[j]
        for i in range (0,a.__len__()):
            if(a[i]==b):                    #找出码字
                num.append(b)
                text_len=text_len-1
                b=''
                break

        j=j+1

    string=''                               #对各码字译码
    for i in num:
        string+=code_dict[i]

    str_int=int(string,2)
    str_hex=hex(str_int)
    str_hex=str_hex.lstrip('0x')
    code_byte=bytes.fromhex(str_hex)
    with open(res_file,'wb')as f:           #解码后的字符串二进制写回
        f.write(code_byte)
    print("decoded file bytes:",code_byte.__len__())

def bin_num(text):
    binary,l=file_to_bit(text)
    lenth=binary.__len__()
    num0=0
    num1=0
    for i in range(0,lenth):
        if (binary[i]=='0'):
            num0+=1
        else:
            num1+=1
    print("numbers of '0':",num0,"        ","numbers of '1':",num1)
    print("frequency of '0':",num0/lenth,"        ","frequency of '1':",num1/lenth)



if __name__=="__main__":

    print("please enter your operation",'\n',"1:huffman_encode",'\n',"2:huffman_decode",'\n',"3:lz_encode",'\n',"4:lz_decode")
    ope=input()

    if (ope=='1'):
        print("please input the encoding file path:")
        source_file=input()

        print("please input the target file path:")
        res_text=input()

        #start=time.clock()
        start = time.process_time()
        encode(source_file,res_text)
        #end=time.clock()
        end = time.process_time()
        print('finish encoding!')
        print('Running time: %s Seconds'%(end-start))

    if(ope=='2'):
        print("please input the decoding file path:")
        source_file=input()

        print("please input the target file path:")
        res_file=input()

        #start=time.clock()
        start = time.process_time()
        decode(source_file,res_file)
        #end=time.clock()
        end = time.process_time()
        print('finish decoding!')
        print('Running time: %s Seconds'%(end-start))

    if (ope=='3'):
        print("please input the encoding file path:")
        source_file=input()

        print("please input the target file path:")
        res_text=input()

        #start=time.clock()
        start = time.process_time()
        lz_encode(source_file,res_text)
        #end=time.clock()
        end = time.process_time()
        print('finish encoding!')
        print('Running time: %s Seconds'%(end-start))

    if (ope == '4'):
        print("please input the decoding file path:")
        source_file = input()

        print("please input the target file path:")
        res_file = input()

        #start = time.clock()
        start = time.process_time()
        lz_decode(source_file, res_file)
        #end = time.clock()
        end = time.process_time()
        print('finish decoding!')
        print('Running time: %s Seconds' % (end - start))

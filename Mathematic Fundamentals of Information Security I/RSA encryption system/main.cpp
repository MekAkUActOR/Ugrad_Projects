//
//  main.cpp
//  RSA encryption system simulating
//
//  Created by 刘浩文 on 2019/4/7.
//  Copyright © 2019 HaowenLiu_517021911065. All rights reserved.
//
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

int *Bezout(int x, int y, int *arry)
{
    int a = x, b = y;
    int q = 0;
    int c = 0;
    int r = abs(a), R =abs(b);
    int s = 1, S = 0;
    int t = 0, T = 1;
    
    if(R == 0)
    {
        arry[0] = r;
        arry[1] = s;
        arry[2] = t;
        return arry;
    }
    else {
        q = r / R;
        c = R;
        R = -q * R + r;
        r = c;
        while (R != 0)
        {
            c = S;
            S = -q * S + s;
            s = c;
            c = T;
            T = -q * T + t;
            t = c;
            q = r / R;
            c = R;
            R = -q * R + r;
            r = c;
        }
        arry[0] = r;
        arry[1] = S;
        arry[2] = T;
        return arry;
    }
}

void Binarycout(int n, int k)
{
    for (int i = k; i >= 0; i--)
    {
        cout << ((n>>i)&1);
    }
}

int dENCRY(int m, int e, int n)
{
    int bit[28];
    int i = 0;
    for (i = 0; i <= 27; i++)
        bit[i] = ((e>>i)&1);
    long int a = 1, b = m;
    for (i = 0; i <= 27; i++)
    {
        if (bit[i] == 1) a = (a * b) % n;
        b = (b * b) % n;
    }
    return a;
}

void RSA(string str) {
    int pri130[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127};
    int pri16900[16901];
    int i = 0, j = 0;
    int q = 0;
    
    for (i = 1; i <= 16900; i++)
        pri16900[i] = i;
    pri16900[0] = pri16900[1] = 0;
    for (i = 0; i <= 30; i++){
        q = 16900 / pri130[i];
        for (j = 2; j <= q; j++)
            pri16900[j * pri130[i]] = 0;
    }
    
    j = 0;
    int prineed[872];
    for (i = 8192; i <= 16384; i++)
        if (pri16900[i])
        {
            prineed[j] = pri16900[i];
            j++;
        }
    
    srand(int(time(0)));
    int p = prineed[rand()%872];
    q = prineed[rand()%872];
    int n = p*q;
    int phi = (p-1)*(q-1);
    cout << "p" << ": ";
    Binarycout(p, 13);
    cout << endl;
    cout << "q" << ": ";
    Binarycout(q, 13);
    cout << endl;
    cout << "n" << ": ";
    Binarycout(n, 27);
    cout << endl;
    cout << "phi" << ": ";
    Binarycout(phi, 27);
    cout << endl;
    int e = 0;
    int *arry = new int;
    while(e >= phi || e < 65000000 || arry[0] != 1)
    {
        e = 2 * rand() * rand() + 1;
        arry = Bezout(e, phi, arry);
    }
    cout << "e" << ": ";
    Binarycout(e, 27);
    cout << endl;
    int d = 0;
    if (arry[1] % phi < 0) d = (arry[1] % phi) + phi;
    else d = arry[1] % phi;
    cout << "d" << ": ";
    Binarycout(d, 27);
    cout << endl;
    delete arry;
    
    /*cout << "请输入需要加密的内容：" << endl;
    string ch;
    getline(cin,ch);*/
    string ch = str;
    
    int chASCII[1000];
    i = 0;
    int c = ch.length();
    for (i = 0; i < c; i++)  chASCII[i] = ch[i];
    cout << "明文：" << endl;
    cout << ch << endl;
    cout << "数字化：" << endl;
    for (i = 0; i < c; i++)
    {Binarycout(chASCII[i], 6);}
    cout << endl;
    
    int encry[1000];
    for (i = 0; i < c; i++)
        encry[i] = dENCRY(chASCII[i], e, n);
    cout << "加密：" << endl;
    for (i = 0; i < c; i++)
        Binarycout(encry[i], 27);
    cout << endl;
    
    int decry[1000];
    for (i = 0; i < c; i++)
        decry[i] = dENCRY(encry[i], d, n);
    cout << "解密：" << endl;
    for (i = 0; i < c; i++)
        Binarycout(decry[i], 6);
    cout << endl;
    cout << "原文：" << endl;
    for (i = 0; i < c; i++)
        cout << (char)decry[i];
    cout << endl;
}

int main(){
    string Ex, test;
    cout << "这是一个RSA公钥密码系统的简单实现：" << endl;
    Ex = "Mathematical Fundation of Information Security 201904051 517021911065";
    RSA(Ex);
    cout << "请输入您想加密的内容：" << endl;
    getline(cin,test);
    RSA(test);
    return 0;
}

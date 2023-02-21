# Lab4
## 实验目的
完成delta debugging，将fuzzer出来的crash输入精简化

## 实验原理
由于Fuzz出来的数据可能不是最优的，即将这个数据删去部分仍然可以使得程序出现crash，而delta debugging就是为了精简化Fuzz出来的数据使其长度最短

### Naive Algorithm
+ 假设要删去的长度为$len(1\le len\le n)$
  + 下标$i$从$1$开始，每次尝试删去$[i,i+len-1]$这一段，判断剩下的数据是否会使得程序crash，是则把剩下的数据加入crash集合
+ 最后遍历crash集合，找到长度最小的即可

时间复杂度$O(n^2)$

### Smart Algorithm
+ 以$n=2$并且$\Delta$为空开始
+ 测试每个$\Delta_1,\Delta_2,\cdots,\Delta_{n}$和每一个$\nabla_1,\nabla_2,\cdots,\nabla_n$
  + 假设传入的数据长度为$m$，把这个数据均分成为$n$份，长度为$\frac{m}{n}$，$\Delta_i$对应第$i$份，$\nabla_i$对应除第$i$份剩下的数据组合成的数据
+ 分好后会有以下的情况
  + 对于会造成crash的$\Delta_i$，加入到crash集合中，保持$n$大小不变继续递归去处理它
  + 对于会造成crash的$\nabla_i$，加入到crash集合中，令$n=n-1$，然后递归处理
  + $\Delta$和$\nabla$都不会造成

如果需要更精细粒度，通过增大$n$来实现即可，较好的方法是每次令$n=n\times2$

时间复杂度$O(logN)$

## 实验代码

``` python
import math

from typing import Tuple

from delta_debugger import run_target

EMPTY_STRING = b""

# Naive Algorithm-version1(2^n)
# def  _delta_debug(target: str,input:bytes,n:int,c:[],hs:{}):
#     if input in hs:
#         return
#     flag=True
#     m=len(input)
#     ulta={}
#     for i in range(m):
#         tmp=input[0:i]+input[i+1:m]
#         if run_target(target,tmp)<0:  #如果当前这个crash只要删去一个可以继续使程序false，那么就不是最小的
#             ulta[i]=True
#             flag=False
#         else:
#             ulta[i]=False
#     if flag:
#         hs[input]=True
#         c.append(input)
#     else:
#         for i in range(m):
#             if ulta[i]==True:
#                 tmp=input[0:i]+input[i+1:m]
#                 _delta_debug(target,tmp,n,c,hs)
# def delta_debug(target: str, input: bytes) -> bytes:
#     """
#     Delta-Debugging algorithm

#     TODO: Implement your algorithm for delta-debugging.

#     Hint: It may be helpful to use an auxilary function that
#     takes as input a target, input string, n and
#     returns the next input and n to try.

#     :param target: target program
#     :param input: crashing input to be minimized
#     :return: 1-minimal crashing input.
#     """
#     res=[]
#     hs={}
#     _delta_debug(target,input,1,res,hs)
#     ret=input
#     for x in res:
#         print(x)
#         if len(x) < len(ret):
#             ret=x
#     return ret                            



# Naive Algorithm-version2 O(n**2)
# def  _delta_debug(target: str,input:bytes,n:int,c:[]):
#     m=len(input)
#     for i in range(1,m):
#         for j in range(m):
#             if j+i-1>=m:
#                 break
#             l,r=j,j+i-1
#             tmp=input[0:l]+input[r+1:m]
#             if run_target(target,tmp)<0:
#                 c.append(tmp)
# def delta_debug(target: str, input: bytes) -> bytes:
#     """
#     Delta-Debugging algorithm

#     TODO: Implement your algorithm for delta-debugging.

#     Hint: It may be helpful to use an auxilary function that
#     takes as input a target, input string, n and
#     returns the next input and n to try.

#     :param target: target program
#     :param input: crashing input to be minimized
#     :return: 1-minimal crashing input.
#     """
#     c=[]
#     _delta_debug(target,input,1,c)
#     res=input
#     for x in c:
#         if len(x) < len(res):
#             res=x
#     return res


def  _delta_debug(target: str,input:bytes,n:int,c:[]):
    if n<=0:
        return
    delta=[]
    ulta=[]
    m=len(input)
    i,j=0,m//n
    while i<m:
        l,r=i,i+j-1
        if r>=m:
            r=m-1
        tmp1=input[l:r+1]
        tmp2=input[0:l]+input[r+1:m]
        delta.append(tmp1)
        ulta.append(tmp2)
        i=r+1
    for x in delta:
        if run_target(target,x)<0:
            c.append(x)
            _delta_debug(target,x,2,c)

    for x in ulta:
        if run_target(target,x)<0:
            c.append(x)
            _delta_debug(target,x,n-1,c)




    
def delta_debug(target: str, input: bytes) -> bytes:
    """
    Delta-Debugging algorithm

    TODO: Implement your algorithm for delta-debugging.

    Hint: It may be helpful to use an auxilary function that
    takes as input a target, input string, n and
    returns the next input and n to try.

    :param target: target program
    :param input: crashing input to be minimized
    :return: 1-minimal crashing input.
    """
    c=[]
    _delta_debug(target,input,2,c)
    res=input
    for x in c:
        if len(x)<len(res):
            res=x
    return res
    
```
一开始我先写暴力做法，想复杂了，我认为删去的数据是可以不连续的，所以我每次获得一个剩下的数据，都会去递归处理它，使得时间复杂度达到了$O(2^n)$。~~但删去不连续的是否会比删去连续的更优，似乎还不知道?~~

## 扩展
类似的，在AFL-Fuzz中也有类似的工具，`afl-tmin`可以帮助用户精简`crash`。
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
    
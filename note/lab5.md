# Lab5
## 实验目的
根据fuzz和delta debugging出来的数据，进行一些有意义地数据计算来评估数据的好坏或程序的鲁棒性
## 实验原理

Abstracting Predicate Counts
+ $-$ Never observed
+ $0$ False at least once,never true
+ $1$ True at least once,never false
+ $*$ Both true and false at least once

那么这个**Predicate Counts**是什么呢，可以认为就是程序里的一个判断条件，对于所有fuzz出来的数据，这个条件可能都为为真，也可能都为假，也可能都有，也可因为程序一直没有遍历到它所在的基本块而没有被观察到

### 指标定义

所有fuzz出来的数据分为两类，一类使得程序失败的failure，一类使得程序成功的success

当条件$P$被观测为真时程序失败的可能性是多少($\text{Failure}(P)$)
+ $F(P)$：failure中$P$为真的数据的数量
+ $S(P)$：success中$P$为真的数据的数量
+ $\text{Failure}(P)=\frac{F(P)}{F(P)+S(P)}$

当条件$P$被观测时程序失败的可能性是多少($\text{Context}(P)$)
+ $F(P\text{ obversed})$：failure中$P$出现的数据的数量
+ $S(P\text{ obversed})$：success中$P$出现的数据的数量
+ $\text{Context}(P)=\frac{F(P\text{ obversed})}{F(P\text{ obversed})+S(P\text{ obversed})}$

条件$P$为真是否会增加程序失败的可能性$\text{Increase}(P)$
+ $\text{Increase}(P)=\text{Failure}(P)-\text{Context}(P)$


### 对于某一个BUG分离有用的$P$
+ 算法1
  + 丢弃$\text{Increase}(P)\le 0$的$P$
  + 根据$\text{Increase}(P)$对$P$排序


### 如果程序有多个BUG如何分离$P$
+ Revised Algorithm
  + Repeat
    + 计算每个$P$的$\text{Increase}()$和$\text{F}()$
    + 对$P$排序(根据需求)
    + 将排名最大的$P^*$放入到结果列表里
    + 将$P^*$移除并且把剩下运行成功的$P$移除
    + 将最后留下的$P$重复以上操作
  + 直到没有$P$

## 实现
``` python
#! /usr/bin/env python3

from collections import defaultdict
import itertools
from pathlib import Path
from typing import Dict, Iterable, List, Set
from cbi.data_format import (
    CBILog,
    ObservationStatus,
    Predicate,
    PredicateInfo,
    PredicateType,
    Report,
)
from cbi.utils import get_logs

#实现不知道collect_observations用在哪里
def collect_observations(log: CBILog) -> Dict[Predicate, ObservationStatus]:
    """
    Traverse the CBILog and collect observation status for each predicate.

    NOTE: If you find a `Predicate(line=3, column=5, pred_type="BranchTrue")`
    in the log, then you have observed it as True,
    further it also means you've observed is complement:
    `Predicate(line=3, column=5, pred_type="BranchFalse")` as False.

    :param log: the log
    :return: a dictionary of predicates and their observation status.
    """
    observations: Dict[Predicate, ObservationStatus] = defaultdict(
        lambda: ObservationStatus.NEVER
    )

    """
    TODO: Add your code here

    Hint: The PredicateType.alternatives will come in handy.
    """
    for item in log:
        pred = Predicate(item.line,item.column,item.value)
        L = PredicateType.alternatives(item.value)
        for x in  L:
            observations[pred]=ObservationStatus.merge(observations[pred],x[1])
    return observations


def collect_all_predicates(logs: Iterable[CBILog]) -> Set[Predicate]:
    """
    Collect all predicates from the logs.

    :param logs: Collection of CBILogs
    :return: Set of all predicates found across all logs.
    """
    predicates = set()
    for CBILogEntrys in logs:
        for item in CBILogEntrys:
            x=Predicate(item.line,item.column,item.value)
            predicates.add(x)
    # TODO: Add your code here

    return predicates


def cbi(success_logs: List[CBILog], failure_logs: List[CBILog]) -> Report:
    """
    Compute the CBI report.

    :param success_logs: logs of successful runs
    :param failure_logs: logs of failing runs
    :return: the report
    """
    all_predicates = collect_all_predicates(itertools.chain(success_logs, failure_logs))
    # print(all_predicates)
    predicate_infos: Dict[Predicate, PredicateInfo] = {
        pred: PredicateInfo(pred) for pred in all_predicates
    }

    # TODO: Add your code here to compute the information for each predicate.
    for pred_info in predicate_infos.values():
        pred=pred_info.predicate
        Sp,Fp=0,0
        SpObs,FpObs=0,0
        if pred.pred_type in PredicateType.BRANCH_TYPES:
            for CBILogs in success_logs:
                collect_observations(CBILogs)
                for item in CBILogs:
                    x=Predicate(item.line,item.column,item.value)
                    if x.line==pred.line and x.column==pred.column and x.pred_type==PredicateType.BRANCH_TRUE:
                        Sp=Sp+1
                    if x.line==pred.line and x.column==pred.column and x.pred_type in PredicateType.BRANCH_TYPES:
                        SpObs=SpObs+1

            for CBILogs in failure_logs:
                collect_observations(CBILogs)
                for item in CBILogs:
                    x=Predicate(item.line,item.column,item.value)
                    if x.line==pred.line and x.column==pred.column and x.pred_type==PredicateType.BRANCH_TRUE:
                        Fp=Fp+1
                    if x.line==pred.line and x.column==pred.column and x.pred_type in PredicateType.BRANCH_TYPES:
                        FpObs=FpObs+1
        if pred.pred_type in PredicateType.RETURN_TYPES:
            continue
        pred_info.s=Sp
        pred_info.f=Fp
        pred_info.s_obs=SpObs
        pred_info.f_obs=FpObs
        # print("s={}, f={}, s_obs={}, f_obs={}".format(Sp,Fp,SpObs,FpObs))

    # Finally, create a report and return it.
    report = Report(predicate_info_list=list(predicate_infos.values()))
    return report
```
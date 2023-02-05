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

#include "DivZeroAnalysis.h"
#include "Utils.h"

namespace dataflow {

/**
 * @brief Get the Predecessors of a given instruction in the control-flow graph.
 *
 * @param Inst The instruction to get the predecessors of.
 * @return Vector of all predecessors of Inst.
 */
std::vector<Instruction *> getPredecessors(Instruction *Inst) {
  std::vector<Instruction *> Ret;
  auto Block = Inst->getParent();
  for (auto Iter = Block->rbegin(), End = Block->rend(); Iter != End; ++Iter) {
    if (&(*Iter) == Inst) {
      ++Iter;
      if (Iter != End) {
        Ret.push_back(&(*Iter));
        return Ret;
      }
      for (auto Pre = pred_begin(Block), BE = pred_end(Block); Pre != BE;
           ++Pre) {
        Ret.push_back(&(*((*Pre)->rbegin())));
      }
      return Ret;
    }
  }
  return Ret;
}

/**
 * @brief Get the successors of a given instruction in the control-flow graph.
 *
 * @param Inst The instruction to get the successors of.
 * @return Vector of all successors of Inst.
 */
std::vector<Instruction *> getSuccessors(Instruction *Inst) {
  std::vector<Instruction *> Ret;
  auto Block = Inst->getParent();
  for (auto Iter = Block->begin(), End = Block->end(); Iter != End; ++Iter) {
    if (&(*Iter) == Inst) {
      ++Iter;
      if (Iter != End) {
        Ret.push_back(&(*Iter));
        return Ret;
      }
      for (auto Succ = succ_begin(Block), BS = succ_end(Block);
           Succ != BS; ++Succ) {
        Ret.push_back(&(*((*Succ)->begin())));
      }
      return Ret;
    }
  }
  return Ret;
}

/**
 * @brief Joins two Memory objects (Mem1 and Mem2), accounting for Domain
 * values.
 *
 * @param Mem1 First memory.
 * @param Mem2 Second memory.
 * @return The joined memory.
 */
Memory *join(Memory *Mem1, Memory *Mem2) {
  /**
   * TODO: Write your code that joins two memories.
   *
   * If some instruction with domain D is either in Mem1 or Mem2, but not in
   *   both, add it with domain D to the Result.
   * If some instruction is present in Mem1 with domain D1 and in Mem2 with
   *   domain D2, then Domain::join D1 and D2 to find the new domain D,
   *   and add instruction I with domain D to the Result.
   */
    Memory *Ret=new Memory;
    for(auto &[key,value]:*Mem1){
        if(Mem2->find(key)==Mem2->end()){
            (*Ret)[key]=value;
        }else{
            (*Ret)[key]=Domain::join(value,(*Mem2)[key]);
        }
    }
    for(auto &[key,value]:*Mem2){
        //outs()<<key<<"\n";
        if(Mem1->find(key)==Mem1->end()){
            //outs()<<"==============\n";
            (*Ret)[key]=value;
        }
    }
    return Ret;
    // Memory *Tmp=Mem1;
    // for(auto &[key,value]:*Tmp){
    //     if(Mem2->find(key)==Mem2->end()){
    //         //Ret[key]=value;
    //     }else{
    //         (*Mem1)[key]=Domain::join(value,(*Mem2)[key]);
    //     }
    // }
    // for(auto &[key,value]:*Mem2){
    //     //outs()<<key<<"\n";
    //     if(Tmp->find(key)==Tmp->end()){
    //         //outs()<<"==============\n";
    //         (*Mem1)[key]=value;
    //     }
    // }
    // return Mem1;
}

void DivZeroAnalysis::flowIn(Instruction *Inst, Memory *InMem) {
  /**
   * TODO: Write your code to implement flowIn.
   *
   * For each predecessor Pred of instruction Inst, do the following:
   *   + Get the Out Memory of Pred using OutMap.
   *   + Join the Out Memory with InMem.
   */
   auto PreNodes=getPredecessors(Inst);
   Memory *Tmp=InMem;
   for(auto nodes:PreNodes){
        Memory *Out=OutMap[nodes];
        InMem=join(InMem,Out);
   }
   InMap[Inst]=InMem;
}

/**
 * @brief This function returns true if the two memories Mem1 and Mem2 are
 * equal.
 *
 * @param Mem1 First memory
 * @param Mem2 Second memory
 * @return true if the two memories are equal, false otherwise.
 */
bool equal(Memory *Mem1, Memory *Mem2) {
  /**
   * TODO: Write your code to implement check for equality of two memories.
   *
   * If any instruction I is present in one of Mem1 or Mem2,
   *   but not in both and the Domain of I is not UnInit, the memories are
   *   unequal.
   * If any instruction I is present in Mem1 with domain D1 and in Mem2
   *   with domain D2, if D1 and D2 are unequal, then the memories are unequal.
   */
  for(auto &[key,value]:*Mem1){
    if(Mem2->find(key)!=Mem2->end()){
        if(Domain::equal(*value,*(*Mem2)[key])) continue;
        else return false;
    }else{
        if(!Domain::equal(*value,Domain(Domain::Uninit))) return false;
    }
  }

  for(auto &[key,value]:*Mem2){
    if(Mem1->find(key)!=(*Mem1).end()){
        continue;
    }else{
        if(!Domain::equal(*value,Domain(Domain::Uninit))) return false;
    }
  }
  return true;
}

void DivZeroAnalysis::flowOut(Instruction *Inst, Memory *Pre, Memory *Post,
                              SetVector<Instruction *> &WorkSet) {
  /**
   * TODO: Write your code to implement flowOut.
   *
   * For each given instruction, merge abstract domain from pre-transfer memory
   * and post-transfer memory, and update the OutMap.
   * If the OutMap changed then also update the WorkSet.
   */
   
//    Memory *Merge=join(Post,Pre);
//    if(!equal(OutMap[Inst],Merge)){
//      auto SucInsts=getSuccessors(Inst);
//      for(auto SucInst:SucInsts){
//         WorkSet.insert(SucInst);
//      }
//    }
//    OutMap[Inst]=Merge;
    // printMemory(Pre);
    // printMemory(Post);
   if(!equal(Pre,Post)){
     auto SucInsts=getSuccessors(Inst);
     for(auto SucInst:SucInsts){
        WorkSet.insert(SucInst);
     }
   }
   OutMap[Inst]=Post;
}

void DivZeroAnalysis::doAnalysis(Function &F) {
  SetVector<Instruction *> WorkSet;
  /**
   * TODO: Write your code to implement the chaotic iteration algorithm
   * for the analysis.
   *
   * Initialize the WorkSet with all the instructions in the function.
   *
   * While the WorkSet is not empty:
   * - Pop an instruction from the WorkSet.
   * - Construct it's Incoming Memory using flowIn.
   * - Evaluate the instruction using transfer and create the OutMemory.
   * - Use flowOut along with the previous Out memory and the current Out
   *   memory, to check if there is a difference between the two to update the
   *   OutMap and add all successors to WorkSet.
   */
   for (inst_iterator Iter = inst_begin(F), End = inst_end(F); Iter != End; ++Iter) {
        auto Inst = &(*Iter);
        WorkSet.insert(Inst);
    }
    // auto entry = &(*inst_begin(F));
    // WorkSet.insert(entry);
    while(!WorkSet.empty()){
        auto Inst=WorkSet.front();
        WorkSet.remove(Inst);
        flowIn(Inst,InMap[Inst]);
        Memory *Pre=OutMap[Inst];
        Memory *Gen=new Memory;
        transfer(Inst,InMap[Inst],*Gen);
        // Memory *Tmp=new Memory;
        // for(auto &[key,value]:*InMap[Inst]){
        //     if(Gen->find(key)!=Gen->end()){
        //         (*Tmp)[key]=(*Gen)[key];
        //     }else{
        //         (*Tmp)[key]=value;
        //     }
        // }
        // Memory *Post=join(Tmp,Gen);
        Memory *Post=new Memory;
        for(auto &[key,value]:*InMap[Inst]){
            (*Post)[key]=value;
        }
        for(auto &[key,value]:*Gen){
            (*Post)[key]=value;
        }
        flowOut(Inst,Pre,Post,WorkSet);
        // outs()<<"====================\n";
        // outs()<<variable(Inst)<<"\n";
        // printMemory(InMap[Inst]);
        // outs()<<"-----------\n";
        // printMemory(OutMap[Inst]);
        // outs()<<"====================\n";
    }

}

} // namespace dataflow
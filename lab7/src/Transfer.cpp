#include "DivZeroAnalysis.h"
#include "Utils.h"

namespace dataflow {

/**
 * @brief Is the given instruction a user input?
 *
 * @param Inst The instruction to check.
 * @return true If it is a user input, false otherwise.
 */
bool isInput(Instruction *Inst) {
  if (auto Call = dyn_cast<CallInst>(Inst)) {
    if (auto Fun = Call->getCalledFunction()) {
      return (Fun->getName().equals("getchar") ||
              Fun->getName().equals("fgetc"));
    }
  }
  return false;
}

/**
 * Evaluate a PHINode to get its Domain.
 *
 * @param Phi PHINode to evaluate
 * @param InMem InMemory of Phi
 * @return Domain of Phi
 */
Domain *eval(PHINode *Phi, const Memory *InMem) {
  if (auto ConstantVal = Phi->hasConstantValue()) {
    return new Domain(extractFromValue(ConstantVal));
  }

  Domain *Joined = new Domain(Domain::Uninit);

  for (unsigned int i = 0; i < Phi->getNumIncomingValues(); i++) {
    auto Dom = getOrExtract(InMem, Phi->getIncomingValue(i));
    Joined = Domain::join(Joined, Dom);
  }
  return Joined;
}

/**
 * @brief Evaluate the +, -, * and / BinaryOperator instructions
 * using the Domain of its operands and return the Domain of the result.
 *
 * @param BinOp BinaryOperator to evaluate
 * @param InMem InMemory of BinOp
 * @return Domain of BinOp
 */
Domain *eval(BinaryOperator *BinOp, const Memory *InMem) {
  /**
   * TODO: Write your code here that evaluates +, -, * and /
   * based on the Domains of the operands.
   */
   Value *Op1=BinOp->getOperand(0);
   Value *Op2=BinOp->getOperand(1);
   unsigned Opcode=BinOp->getOpcode();
   Domain *E1=getOrExtract(InMem,Op1), *E2=getOrExtract(InMem,Op2);
   switch(Opcode){
    case Instruction::Add:
    case Instruction::FAdd:
        return Domain::add(E1,E2);
    case Instruction::Sub:
    case Instruction::FSub:
        return Domain::sub(E1,E2);
    case Instruction::Mul:
    case Instruction::FMul:
        return Domain::mul(E1,E2);
    case Instruction::UDiv:
    case Instruction::SDiv:
    case Instruction::FDiv:
        return Domain::div(E1,E2);
    default:
        return NULL;
   }
}

/**
 * @brief Evaluate Cast instructions.
 *
 * @param Cast Cast instruction to evaluate
 * @param InMem InMemory of Instruction
 * @return Domain of Cast
 */
Domain *eval(CastInst *Cast, const Memory *InMem) {
  /**
   * TODO: Write your code here to evaluate Cast instruction.
   */
    Value *Op=Cast->getOperand(0);
    return getOrExtract(InMem,Op);
}

/**
 * @brief Evaluate the ==, !=, <, <=, >=, and > Comparision operators using
 * the Domain of its operands to compute the Domain of the result.
 *
 * @param Cmp Comparision instruction to evaluate
 * @param InMem InMemory of Cmp
 * @return Domain of Cmp
 */
Domain *eval(CmpInst *Cmp, const Memory *InMem) {
  /**
   * TODO: Write your code here that evaluates:
   * ==, !=, <, <=, >=, and > based on the Domains of the operands.
   *
   * NOTE: There is a lot of scope for refining this, but you can just return
   * MaybeZero for comparisons other than equality.
   */
   CmpInst::Predicate cmp=Cmp->getPredicate();
   Value *Op1=Cmp->getOperand(0);
   Value *Op2=Cmp->getOperand(1);
   Domain *E1=getOrExtract(InMem,Op1), *E2=getOrExtract(InMem,Op2);
   switch(cmp){
    case CmpInst::Predicate::ICMP_EQ:
        if(Domain::equal(*E1,*E2)) return new Domain(Domain::NonZero);
        else return new Domain(Domain::Zero);
    default:
        return new Domain(Domain::MaybeZero);
   }
   return NULL;
}

void DivZeroAnalysis::transfer(Instruction *Inst, const Memory *In,
                               Memory &NOut, PointerAnalysis *PA,
                               SetVector<Value *> PointerSet) {
  if (isInput(Inst)) {
    // The instruction is a user controlled input, it can have any value.
    NOut[variable(Inst)] = new Domain(Domain::MaybeZero);
  } else if (auto Phi = dyn_cast<PHINode>(Inst)) {
    // Evaluate PHI node
    NOut[variable(Phi)] = eval(Phi, In);
  } else if (auto BinOp = dyn_cast<BinaryOperator>(Inst)) {
    // Evaluate BinaryOperator
    NOut[variable(BinOp)] = eval(BinOp, In);
  } else if (auto Cast = dyn_cast<CastInst>(Inst)) {
    // Evaluate Cast instruction
    NOut[variable(Cast)] = eval(Cast, In);
  } else if (auto Cmp = dyn_cast<CmpInst>(Inst)) {
    // Evaluate Comparision instruction
    NOut[variable(Cmp)] = eval(Cmp, In);
  } else if (auto Alloca = dyn_cast<AllocaInst>(Inst)) {
    // Do nothing here.
  } else if (auto Store = dyn_cast<StoreInst>(Inst)) {
    /**
     * TODO: Store instruction can either add new variables or overwrite existing variables into memory maps.
     * To update the memory map, we rely on the points-to graph constructed in PointerAnalysis.
     *
     * To build the abstract memory map, you need to ensure all pointer references are in-sync, and
     * will converge upon a precise abstract value. To achieve this, implement the following workflow:
     *
     * Iterate through the provided PointerSet:
     *   - If there is a may-alias (i.e., `alias()` returns true) between two variables:
     *     + Get the abstract values of each variable.
     *     + Join the abstract values using Domain::join().
     *     + Update the memory map for the current assignment with the joined abstract value.
     *     + Update the memory map for all may-alias assignments with the joined abstract value.
     *
     * Hint: You may find getOperand(), getValueOperand(), and getPointerOperand() useful.
     */
     
    Value *Pointer = Store->getPointerOperand();
    Value *Value = Store->getValueOperand();
    std::string strpointer=variable(Pointer);
    Domain *E=getOrExtract(In,Value);
    if(NOut.find(strpointer)==NOut.end()) NOut[strpointer]=E;
    else NOut[strpointer]=Domain::join(NOut[strpointer],E);
    for(auto val:PointerSet){
        std::string str=variable(val);
        if (AllocaInst *Alloca = dyn_cast<AllocaInst>(val)) str=address(Alloca);
        if(PA->alias(strpointer,str)){
            if(NOut.find(variable(val))==NOut.end()) NOut[variable(val)]=E;
            else NOut[variable(val)]=Domain::join(NOut[variable(val)],E);
        }
    }
    for(auto val1:PointerSet){
        std::string str1=variable(val1);
        if (AllocaInst *Alloca1 = dyn_cast<AllocaInst>(val1)) str1=address(Alloca1);
        for(auto val2:PointerSet){
            std::string str2=variable(val2);
            if (AllocaInst *Alloca2 = dyn_cast<AllocaInst>(val2)) str2=address(Alloca2);
            if(str2!=str1){
                if(PA->alias(str1,str2)){
                    if(NOut.find(str1)==NOut.end() && NOut.find(str2)==NOut.end()) continue;
                    else if(NOut.find(str1)==NOut.end() && NOut.find(str2)!=NOut.end()) NOut[str1]=NOut[str2];
                    else if(NOut.find(str1)!=NOut.end() && NOut.find(str2)!=NOut.end())NOut[str1]=Domain::join(NOut[str1],NOut[str2]);
                }
            }
        }
    }

  } else if (auto Load = dyn_cast<LoadInst>(Inst)) {
    /**
     * TODO: Rely on the existing variables defined within the `In` memory to
     * know what abstract domain should be for the new variable
     * introduced by a load instruction.
     *
     * If the memory map already contains the variable, propagate the existing
     * abstract value to NOut.
     * Otherwise, initialize the memory map for it.
     *
     * Hint: You may use getPointerOperand().
     */
     Value *Pointer=Load->getPointerOperand();
     if(NOut.find(variable(Load))==NOut.end()) NOut[variable(Load)]=getOrExtract(In,Pointer);
     else NOut[variable(Load)]=Domain::join(NOut[variable(Load)],getOrExtract(In,Pointer));
     
  } else if (auto Branch = dyn_cast<BranchInst>(Inst)) {
    // Analysis is flow-insensitive, so do nothing here.
  } else if (auto Call = dyn_cast<CallInst>(Inst)) {
    /**
     * TODO: Populate the NOut with an appropriate abstract domain.
     *
     * You only need to consider calls with int return type.
     */
     std::string cur_value=variable(Call);
     Function *func=Call->getCaller ();
     Type *RetType=func->getReturnType ();
     if(auto ptr=dyn_cast<IntegerType>(RetType)){
        NOut[variable(Cmp)]=new Domain(Domain::MaybeZero);
     }else NOut[variable(Cmp)]=new Domain(Domain::Uninit);
  } else if (auto Return = dyn_cast<ReturnInst>(Inst)) {
    // Analysis is intra-procedural, so do nothing here.
  } else {
    errs() << "Unhandled instruction: " << *Inst << "\n";
  }
}

} // namespace dataflow
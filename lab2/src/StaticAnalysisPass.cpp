#include "Instrument.h"
#include "Utils.h"

using namespace llvm;

namespace instrument {

const auto PASS_NAME = "StaticAnalysisPass";
const auto PASS_DESC = "Static Analysis Pass";

bool Instrument::runOnFunction(Function &F) {
  auto FunctionName = F.getName().str();
  outs() << "Running " << PASS_DESC << " on function " << FunctionName << "\n";

  outs() << "Locating Instructions\n";
  for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
    Instruction &Inst = (*Iter);
    llvm::DebugLoc DebugLoc = Inst.getDebugLoc();
    if (!DebugLoc) {
      // Skip Instruction if it doesn't have debug information.
      continue;
    }

    int Line = DebugLoc.getLine();
    int Col = DebugLoc.getCol();
    outs() << Line << ", " << Col << "\n";

    /**
     * TODO: Add code to check if the instruction is a BinaryOperator and if so,
     * print the information about its location and operands as specified in the
     * Lab document.
     */
	 if(!Inst.isBinaryOp () ) continue;
	 Value *op1=Inst.getOperand(0);
	 Value *op2=Inst.getOperand(1);

	 outs()<<getBinOpName(getBinOpSymbol((Instruction::BinaryOps)Inst.getOpcode()))<<" at Line "<<Line<<" ,Colnum "<<Col<<" and fisrt oprand is "<<variable(op1)<<" second oprand is "<<variable(op2)<<"\n";
  }
  return false;
}

char Instrument::ID = 1;
static RegisterPass<Instrument> X(PASS_NAME, PASS_NAME, false, false);

} // namespace instrument

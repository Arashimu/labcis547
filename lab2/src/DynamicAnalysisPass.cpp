#include "Instrument.h"
#include "Utils.h"

using namespace llvm;

namespace instrument {

const auto PASS_NAME = "DynamicAnalysisPass";
const auto PASS_DESC = "Dynamic Analysis Pass";
const auto COVERAGE_FUNCTION_NAME = "__coverage__";
const auto BINOP_OPERANDS_FUNCTION_NAME = "__binop_op__";

void instrumentCoverage(Module *M, Instruction &I, int Line, int Col);
void instrumentBinOpOperands(Module *M, BinaryOperator *BinOp, int Line,
                             int Col);

bool Instrument::runOnFunction(Function &F) {
  auto FunctionName = F.getName().str();
  outs() << "Running " << PASS_DESC << " on function " << FunctionName << "\n";

  outs() << "Instrument Instructions\n";
  LLVMContext &Context = F.getContext();
  Module *M = F.getParent();

  Type *VoidType = Type::getVoidTy(Context);
  Type *Int32Type = Type::getInt32Ty(Context);
  Type *Int8Type = Type::getInt8Ty(Context);

  M->getOrInsertFunction(COVERAGE_FUNCTION_NAME, VoidType, Int32Type,
                         Int32Type);

  M->getOrInsertFunction(BINOP_OPERANDS_FUNCTION_NAME, VoidType, Int8Type,
                         Int32Type, Int32Type, Int32Type, Int32Type);

  for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
    Instruction &Inst = (*Iter);
    llvm::DebugLoc DebugLoc = Inst.getDebugLoc();
    if (!DebugLoc) {
      // Skip Instruction if it doesn't have debug information.
      continue;
    }

    int Line = DebugLoc.getLine();
    int Col = DebugLoc.getCol();
    instrumentCoverage(M, Inst, Line, Col);

    /**
     * TODO: Add code to check if the instruction is a BinaryOperator and if so,
     * instrument the instruction as specified in the Lab document.
     */
	 if(!Inst.isBinaryOp () ) continue;
	 instrumentBinOpOperands(M,(BinaryOperator *)&Inst,Line,Col);
  }

  return true;
}
void instrumentCoverage(Module *M, Instruction &I, int Line, int Col) {
  auto &Context = M->getContext();
  auto *Int32Type = Type::getInt32Ty(Context);

  auto LineVal = ConstantInt::get(Int32Type, Line);
  auto ColVal = ConstantInt::get(Int32Type, Col);

  std::vector<Value *> Args = {LineVal, ColVal};

  auto *CoverageFunction = M->getFunction(COVERAGE_FUNCTION_NAME);
  CallInst::Create(CoverageFunction, Args, "", &I);
}

void instrumentBinOpOperands(Module *M, BinaryOperator *BinOp, int Line,
                             int Col) {
  auto &Context = M->getContext();
  auto *Int32Type = Type::getInt32Ty(Context);
  auto *CharType = Type::getInt8Ty(Context);

  auto binop = ConstantInt::get(CharType,getBinOpSymbol(BinOp->getOpcode()));
  auto LineVal = ConstantInt::get(Int32Type,Line);
  auto ColVal = ConstantInt::get(Int32Type,Col);
  
  std::vector<Value *> Args = {binop, LineVal, ColVal, BinOp->getOperand(0),BinOp->getOperand(1)};
  auto *BinOpFunction = M->getFunction(BINOP_OPERANDS_FUNCTION_NAME);
  CallInst::Create(BinOpFunction, Args, "", BinOp);

  /**
   * TODO: Add code to instrument the BinaryOperator to print
   * its location, operation type and the runtime values of its
   * operands.
   */
}

char Instrument::ID = 1;
static RegisterPass<Instrument> X(PASS_NAME, PASS_NAME, false, false);

} // namespace instrument

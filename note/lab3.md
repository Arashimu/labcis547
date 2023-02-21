# Lab3

## Fuzz整体框架
基于覆盖率引导的Fuzzer

![](lab3/1.jpg)


数据突变的一些思路
+ Replace bytes with random values.
+ Swap adjacent bytes.
+ Cycle through all values for each byte.
+ Remove a random byte.
+ Insert a random byte.
+ 当然还可以对数据进行异或之类的
  
## 实验内容
+ 结合利用llvm实现的代码覆盖函数完成Fuzzer函数
+ 自定义一些Fuzzer的突变(Mutate)函数
+ 给输入选择函数添加一些恰当的逻辑使得Fuzz效率更高
+ 给反馈函数添加一些恰当的逻辑使得Fuzz效率更高

代码主体
``` cpp
while (true) {
  input <- selectInput()                  // Pick seed input
  mutation <- selectMutationFn()          // Pick mutation function
  mutatedInput <- mutation(input)         // Mutate input
  test(Target, MutatedInput)              // Run target with mutated input 将突变数据输入目标程序并运行
  feedBack(Target, MutatedInput);         // Provide feedback from the run
}
```

## 实验代码
``` cpp
/**
 * NOTE: You should feel free to manipulate any content in this .cpp file.
 * This means that if you want you can change almost everything,
 * as long as the fuzzer runs with the same cli interface.
 * This also means that if you're happy with some of the provided default
 * implementation, you don't have to modify it.
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>

#include "Utils.h"

#define ARG_EXIST_CHECK(Name, Arg)                                             \
  {                                                                            \
    struct stat Buffer;                                                        \
    if (stat(Arg, &Buffer)) {                                                  \
      fprintf(stderr, "%s not found\n", Arg);                                  \
      return 1;                                                                \
    }                                                                          \
  }                                                                            \
  std::string Name(Arg);

#define DBG                                                                    \
  std::cout << "Hit F::" << __FILE__ << " ::L" << __LINE__ << std::endl

/**
 * @brief Type Signature of Mutation Function.
 * MutationFn takes a string as input and returns a string.
 *
 * MutationFn: string -> string
 */
typedef std::string MutationFn(std::string);

/**
 * Struct that holds useful information about
 * one run of the program.
 *
 * @param Passed       did the program run without crashing?
 * @param Mutation     mutation function used for this run.
 * @param Input        parent input used for generating input for this run.
 * @param MutatedInput input string for this run.
 */
struct RunInfo {
  bool Passed;
  bool Better;
  MutationFn *Mutation;
  std::string Input, MutatedInput;
};

/************************************************/
/*            Global state variables            */
/************************************************/
/**
 * Note: Feel free to add/remove/change any of the following variables.
 * Depending on what you want to keep track of during fuzzing.
 */
// Collection of strings used to generate inputs
std::vector<std::string> SeedInputs;

// Variable to store coverage related information.
std::vector<std::string> CoverageState;

// Coverage related information from previous step.
std::vector<std::string> PrevCoverageState;

/**
 * @brief Variable to keep track of some Mutation related state.
 * Feel free to change/ignore this if you want to.
 */
int MutationState = 0;

/**
 * @brief Variable to keep track of some state related to strategy selection.
 * Feel free to change/ignore this if you want to.
 */
int StrategyState = 0;

/************************************************/
/*    Implement your select input algorithm     */
/************************************************/

/**
 * @brief Select a string that will be mutated to generate a new input.
 * Sample code picks the first input string from SeedInputs.
 *
 * TODO: Implement your logic for selecting a input to mutate.
 * If you require, you can use the Info variable to help make a
 * decision while selecting a Seed but it is not necessary for the lab.
 *
 * @param RunInfo struct with information about the previous run.
 * @return Pointer to a string.
 */
std::string selectInput(RunInfo Info) {
  int Index = 0;
  if(PrevCoverageState.size()<CoverageState.size()) return Info.MutatedInput; 
//   if(Info.Passed) return Info.MutatedInput;
  if(Index < SeedInputs.size()) return SeedInputs[Index++];
  else return SeedInputs[Index=0];
//   return SeedInputs.back();
}

/*********************************************/
/*       Implement mutation startegies       */
/*********************************************/

const char ALPHA[] = "abcdefghijklmnopqrstuvwxyz\n\0";
const int LENGTH_ALPHA = sizeof(ALPHA);

/**
 * Here we provide a two sample mutation functions
 * that take as input a string and returns a string.
 */

/**
 * @brief Mutation Strategy that does nothing.
 *
 * @param Original Original input string.
 * @return std::string mutated string.
 */
std::string mutationA(std::string Original) { return Original; }

/**
 * @brief Mutation Strategy that inserts a random
 * alpha numeric char at a random location in Original.
 *
 * @param Original Original input string.
 * @return std::string mutated string.
 */
std::string mutationB(std::string Original) {
  if (Original.length() <= 0)
    return Original;

  int Index = rand() % (Original.length()-1);
  return Original.insert(Index, 1, ALPHA[rand() % LENGTH_ALPHA]);
}

/**
 * TODO: Add your own mutation functions below.
 * Make sure to update the MutationFns vector to include your functions.
 *
 * Some ideas: swap adjacent chars, increment/decrement a char.
 *
 * Get creative with your strategies.
 */

 /* Replace bytes with random values. */
std::string mutationC(std::string Original){
	int n=Original.length()-1;
	if (Original.length() <= 0)
    	return Original;
	int Index = rand() % Original.length();
	Original[Index]=ALPHA[rand() % LENGTH_ALPHA];
	return Original;

}

/* Swap adjacent bytes. */
std::string mutationD(std::string Original){
	int n=Original.length()-1;
	if(n+1<=0) return Original;
	int Index = rand() % n;
	std::swap(Original[Index],Original[Index==n-1?0:Index+1]);
	return Original;
}
/* Cycle through all values for each byte. */
std::string mutationE(std::string Original){
	int n=Original.length()-1;
	if(n+1<=0) return Original;
	int k=rand() % n;
	std::string tmp="";
	for(int i=n-k;i<n;i++)
		tmp.push_back(Original[i]);
	for(int i=0;i<n-k;i++)
		tmp.push_back(Original[i]);
	tmp.push_back('\n');
	tmp.push_back('\0');
	return tmp;
	
}

/* Remove a random byte. */

std::string mutationF(std::string Original){
	int n=Original.length()-1;
	if(n+1<=0) return Original;
	int Index=rand()%n;
	return Original.erase(Index,1);
}
/**
 * @brief Vector containing all the available mutation functions
 *
 * TODO: Update the definition to include any mutations you implement.
 * For example if you implement mutationC then update it to be:
 * std::vector<MutationFn *> MutationFns = {mutationA, mutationB, mutationC};
 */
std::vector<MutationFn *> MutationFns = {mutationA, mutationB, mutationC, mutationD, mutationE,mutationF};

/**
 * @brief Select a mutation function to apply to the seed input.
 * Sample code picks a random Strategy.
 *
 * TODO: Add your own logic to select a mutation function from MutationFns.
 * Hint: You may want to make use of any global state you store
 * during feedback to make decisions on what MutationFn to choose.
 *
 * @param RunInfo struct with information about the current run.
 * @returns a pointer to a MutationFn
 */
MutationFn *selectMutationFn(RunInfo &Info) {
  if(CoverageState.size() > PrevCoverageState.size()) return MutationFns[MutationState];
  int Strat = rand() % MutationFns.size();
  return MutationFns[MutationState=Strat];
}

/*********************************************/
/*     Implement your feedback algorithm     */
/*********************************************/
/**
 * Update the internal state of the fuzzer using coverage feedback.
 *
 * @param Target name of target binary
 * @param Info RunInfo
 */
void feedBack(std::string &Target, RunInfo &Info) {
  std::vector<std::string> RawCoverageData;
  readCoverageFile(Target, RawCoverageData);

  PrevCoverageState = CoverageState;

  CoverageState.clear();

  /**
   * TODO: Implement your logic to use the coverage information from the test
   * phase to guide fuzzing. The sky is the limit!
   *
   * Hint: You want to rely on some amount of randomness to make decisions.
   *
   * You have the Coverage information of the previous test in
   * PrevCoverageState. And the raw coverage data is loaded into RawCoverageData
   * from the Target.cov file. You can either use this raw data directly or
   * process it (not-necessary). If you do some processing, make sure to update
   * CoverageState to make it available in the next call to feedback.
   */
  if(CoverageState.size() < RawCoverageData.size()) SeedInputs.push_back(Info.MutatedInput);
  CoverageState.assign(RawCoverageData.begin(),
                       RawCoverageData.end()); // No extra processing

}

int Freq = 1000;
int Count = 0;
int PassCount = 0;

bool test(std::string &Target, std::string &Input, std::string &OutDir) {
  // Clean up old coverage file before running
  std::string CoveragePath = Target + ".cov";
  std::remove(CoveragePath.c_str());

  ++Count;
  int ReturnCode = runTarget(Target, Input);
  if (ReturnCode == 127) {
    fprintf(stderr, "%s not found\n", Target.c_str());
    exit(1);
  }
  fprintf(stderr, "\e[A\rTried %d inputs, %d crashes found\n", Count,
          failureCount);
  if (ReturnCode == 0) {
    if (PassCount++ % Freq == 0)
      storePassingInput(Input, OutDir);
    return true;
  } else {
    storeCrashingInput(Input, OutDir);
    return false;
  }
}

/**
 * @brief Fuzz the Target program and store the results to OutDir
 *
 * @param Target Target (instrumented) program binary.
 * @param OutDir Directory to store fuzzing results.
 */
void fuzz(std::string Target, std::string OutDir) {
  struct RunInfo Info;
  while (true) {
    std::string Input = selectInput(Info);
    Info = RunInfo();
    Info.Input = Input;
    Info.Mutation = selectMutationFn(Info);
    Info.MutatedInput = Info.Mutation(Info.Input);
    Info.Passed = test(Target, Info.MutatedInput, OutDir);
    feedBack(Target, Info);
  }
}

/**
 * Usage:
 * ./fuzzer [target] [seed input dir] [output dir] [frequency] [random seed]
 */
int main(int argc, char **argv) {
  if (argc < 4) {
    printf("usage %s [target] [seed input dir] [output dir] [frequency "
           "(optional)] [seed (optional arg)]\n",
           argv[0]);
    return 1;
  }

  ARG_EXIST_CHECK(Target, argv[1]);
  ARG_EXIST_CHECK(SeedInputDir, argv[2]);
  ARG_EXIST_CHECK(OutDir, argv[3]);

  if (argc >= 5)
    Freq = strtol(argv[4], NULL, 10);

  int RandomSeed = argc > 5 ? strtol(argv[5], NULL, 10) : (int)time(NULL);

  srand(RandomSeed);
  storeSeed(OutDir, RandomSeed);
  initialize(OutDir);

  if (readSeedInputs(SeedInputs, SeedInputDir)) {
    fprintf(stderr, "Cannot read seed input directory\n");
    return 1;
  }
  fprintf(stderr, "Fuzzing %s...\n\n", Target.c_str());
  fuzz(Target, OutDir);
  return 0;
}
```

实验过程中思考比较就的就是Feedback的过程，根据一开始的图的提示，每次我们要判断当前突变的结果是否覆盖到更多的代码分支，也就是基于覆盖率的引导，而实验也贴心地帮助我们计算了`CoverageData`数据，所以直接判断`CoverageData`包含的分支数量即可。每次只把有新覆盖分支生成的数据加入数据集合
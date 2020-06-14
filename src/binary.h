#pragma once

#include "options.h"
#include <llvm/IR/Module.h>
#include <string>

bool CreateBinary(llvm::Module *module, const std::string &fileName, EmitType emit);

llvm::Module *CreateModule();

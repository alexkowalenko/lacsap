#pragma once

#include "options.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wshadow"
#include <llvm/IR/Module.h>
#pragma clang diagnostic pop

#include <string>

bool CreateBinary(llvm::Module *module, const std::string &fileName, EmitType emit);

llvm::Module *CreateModule();

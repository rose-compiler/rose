// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparseAsm.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define PUT_COMMENTS_IN_COLUMN_ON_RIGHT  1

// DQ (10/28/2009): Removing magic numbers to specify register names (using enum values instead).
// const char* regnames8l[16] = {"al", "cl", "dl", "bl", "spl", "bpl", "sil", "dil", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
// const char* regnames8h[16] = {"ah", "ch", "dh", "bh", "", "", "", "", "", "", "", "", "", "", "", ""};
// const char* regnames16[16] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
// const char* regnames32[16] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
// const char* regnames64[16] = {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

/** Returns the string representation of an assembly instruction, sans address. */
std::string unparseInstruction(SgAsmInstruction* insn) {
    /* Mnemonic */
    if (!insn) return "BOGUS:NULL";
    std::string result = unparseMnemonic(insn);
    result += std::string((result.size() >= 7 ? 1 : 7-result.size()), ' ');

    /* Operands */
    SgAsmOperandList* opList = insn->get_operandList();
    const SgAsmExpressionPtrList& operands = opList->get_operands();
    for (size_t i = 0; i < operands.size(); ++i) {
        if (i != 0) result += ", ";
        result += unparseExpression(operands[i]);
    }

#if !PUT_COMMENTS_IN_COLUMN_ON_RIGHT
    /* Comment */
    if (insn->get_comment()!="")
        result += " <" + insn->get_comment() + ">";
#endif

    return result;
}

/** Returns the string representation of an assembly instruction with address. */
std::string unparseInstructionWithAddress(SgAsmInstruction* insn) {
    if (!insn) return "BOGUS:NULL";
    return StringUtility::intToHex(insn->get_address()) + ":" + unparseInstruction(insn);
}

/** Returns a string representing the part of the assembly instruction before the first operand. */
std::string unparseMnemonic(SgAsmInstruction *insn) {
    switch (insn->variantT()) {
        case V_SgAsmx86Instruction:
            return unparseX86Mnemonic(isSgAsmx86Instruction(insn));
        case V_SgAsmArmInstruction:
            return unparseArmMnemonic(isSgAsmArmInstruction(insn));
        case V_SgAsmPowerpcInstruction:
            return unparsePowerpcMnemonic(isSgAsmPowerpcInstruction(insn));

		default:
			{
#ifdef _MSC_VER
			  std::cerr <<"Unhandled variant " <<insn->class_name() <<std::endl;
#else
			  std::cerr <<"Unhandled variant " <<insn->class_name() <<" in " <<__func__ <<std::endl;
#endif
              abort();
			  return "error in unparseMnemonic";
			}
    }
}

/** Returns the string representation of an instruction operand (SgAsmExpression), the format of which might depend on the
 *  instruction with which it's associated. */
std::string unparseExpression(SgAsmExpression *expr) {
    /* Find the instruction with which this expression is associated. */
    SgAsmInstruction *insn = NULL;
    for (SgNode *node=expr; !insn && node; node=node->get_parent()) {
        insn = isSgAsmInstruction(node);
    }

    /* The expression is possibly not linked into the tree yet. Assume x86 if that happens. */
    if (!insn)
        return unparseX86Expression(expr, false);
        
    switch (insn->variantT()) {
        case V_SgAsmx86Instruction:
            return unparseX86Expression(expr);
        case V_SgAsmArmInstruction:
            return unparseArmExpression(expr);
        case V_SgAsmPowerpcInstruction:
            return unparsePowerpcExpression(expr);
		default:
		   {
#ifdef _MSC_VER
			std::cerr <<"Unhandled variant " <<insn->class_name() << std::endl;
#else
			std::cerr <<"Unhandled variant " <<insn->class_name() <<" in " <<__func__ <<std::endl;
#endif
			abort();
#ifdef _MSC_VER
         // DQ (11/29/2009): MSVC reports a warning for a path that does not have a return stmt.
            return "ERROR in unparseMnemonic()";
#endif
		   }
    }
}

// This is adapted from Robb's code: Works like hexdump -C to display N bytes of DATA
std::string
hexdump ( rose_addr_t base_addr, const char *prefix, const SgUnsignedCharList & data, size_t maxLength )
   {
  // This function is used to prepend a string representing the memory contents of the instruction.
  // In the output this make is visually more clear when strings, null padding, and internal 
  // executable format tables are being disassembled mistakenly as instructions. It many cases 
  // it makes since to disassemble such sections of the binary file since code can be hidden there.

  // Note that the "prefix" can be used for whitespace to intent the output.

     std::string returnString;

     char buffer[1024];
     size_t n = data.size();

  // ROSE_ASSERT(n < maxLength);

     for (size_t i = 0; i < n; i += maxLength)
        {
       // Output the memory contents in fixed size chunks of size == maxLength; 
       // then output the associated instrcutions. Note that since most instructions
       // are smaller than the selected maxLength, most will be output on a single line.
          if (i > 0)
               returnString += "\n";

          sprintf(buffer,"%s0x%08"PRIx64, prefix, base_addr+i);
          returnString += buffer;

       // Output the hexadecimal representation
          for (size_t j = 0; j < maxLength; j++)
             {
            // if (8 == j) returnString += " ";

               if (i+j < n)
                  {
                    sprintf(buffer," %02x", data[i+j]);
                    returnString += buffer;
                  }
                 else
                  {
                 // 3 spaces to match the format string
                 // puts("   ");
                    returnString += "   ";
                  }
             }

          returnString += " |";

       // Output the character representation (where it is a printable character)
       // for (size_t j = 0; j < maxLength && i+j < n; j++)
          for (size_t j = 0; j < maxLength; j++)
             {
               if (i+j < n)
                  {
                    if (isprint(data[i+j]))
                       {
                         if (data[i+j] == '"')
                            {
                           // Replace double quotes with "." so that output will not be a problem for emacs!
                              returnString += ".";
                            }
                           else
                            {
                              returnString += data[i+j];
                            }
                       }
                      else
                       {
                         returnString += ".";
                       }
                  }
                 else
                  {
                 // 1 spaces to match the format string
                    returnString += " ";
                  }
             }

          returnString += "|";
        }

     return returnString;
   }

// DQ (8/23/2008): I think this should take an SgAsmStatement
// string unparseAsmStatement(SgAsmNode* stmt)
std::string
unparseAsmStatement(SgAsmStatement* stmt)
   {
  // This function should use the same mechanism as the source code
  // for output of strings so that it can eventually work with QROSE.

     ROSE_ASSERT (stmt != NULL);

#define RIGHT_COMMENT_COLUMN_BOUNDARY   55

     std::string result;
     std::string commentString;
     if (stmt->get_comment().empty() == false)
        {
          commentString = stmt->get_comment();

#if !PUT_COMMENTS_IN_COLUMN_ON_RIGHT
          result = "/* " + commentString + " */\n";
#endif
        }

     /* Virtual address and raw bytes */
     SgAsmInstruction* asmInstruction = isSgAsmInstruction(stmt);
     if (asmInstruction != NULL)
        {
          size_t max_length = 6;
          result += hexdump(stmt->get_address(),"",asmInstruction->get_raw_bytes(),max_length);
          result += " :: ";
        }

     switch (stmt->variantT())
        {
          case V_SgAsmx86Instruction:
          case V_SgAsmArmInstruction:
          case V_SgAsmPowerpcInstruction:
             {
            // return result + unparseInstructionWithAddress(isSgAsmInstruction(stmt)) + '\n';
#if PUT_COMMENTS_IN_COLUMN_ON_RIGHT
                std::string instructionString = unparseInstructionWithAddress(isSgAsmInstruction(stmt));
               result += instructionString;
               if (commentString.empty() == false)
                  {
                    size_t instructionStringSize = instructionString.size();
                    for (size_t i = instructionStringSize; i <= RIGHT_COMMENT_COLUMN_BOUNDARY; i++)
                       {
                      // Format the comments nicely
                         if (i == instructionStringSize || i == RIGHT_COMMENT_COLUMN_BOUNDARY)
                              result += " ";
                           else
                              result += "-";
                       }
                    result = result + "/* " + commentString + " */\n";

                    return result;
                  }
                 else
                  {
                    return result + '\n';
                  }
#else
               return result + unparseInstructionWithAddress(isSgAsmInstruction(stmt)) + '\n';
#endif
             }

          case V_SgAsmBlock:
             {
               SgAsmBlock* blk = isSgAsmBlock(stmt);
               result = result + "/* Block " + StringUtility::intToHex(blk->get_address()) + " */\n";
               for (size_t i = 0; i < blk->get_statementList().size(); ++i)
                  {
                    result += unparseAsmStatement(blk->get_statementList()[i]);
                  }
               return result;
             }

          case V_SgAsmFunctionDeclaration:
             {
               SgAsmFunctionDeclaration* blk = isSgAsmFunctionDeclaration(stmt);
               result = result + "/* Function " + blk->get_name() + " at " + StringUtility::intToHex(blk->get_address()) + " */\n";
               for (size_t i = 0; i < blk->get_statementList().size(); ++i)
                  {
                    result += unparseAsmStatement(blk->get_statementList()[i]);
                  }
               return result;
             }

          default:
             {
               std::cerr << "Unhandled variant " << stmt->class_name() << " in unparseX86Statement" << std::endl;
               ROSE_ASSERT (false);
#ifdef _MSC_VER
            // DQ (11/29/2009): MSVC reports a warning for a path that does not have a return stmt.
               return "ERROR in unparseMnemonic()";
#endif
             }
        }
   }

std::string
unparseAsmInterpretation(SgAsmInterpretation* interp)
{
    std::string heading;
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    if (1==headers.size()) {
        heading = "/* Interpretation " + std::string(headers[0]->format_name()) + " */\n";
    } else {
        heading = "/* Interpretation including:\n";
        for (size_t i=0; i<headers.size(); i++) {
            heading += std::string(" *    ") + headers[i]->format_name() + " from " + headers[i]->get_file()->get_name() + "\n";
        }
        heading += " */\n";
    }
    return heading + (interp->get_global_block() ? unparseAsmStatement(interp->get_global_block()) : "/* No global block */");
}

// void unparseAsmStatementToFile(const string& filename, SgAsmNode* stmt) {
void
unparseAsmStatementToFile(const std::string& filename, SgAsmStatement* stmt)
   {
     ROSE_ASSERT (stmt != NULL);
     std::ofstream of(filename.c_str());
     of << unparseAsmStatement(stmt);
   }

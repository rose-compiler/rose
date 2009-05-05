#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

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

    /* Comment */
    if (insn->get_comment()!="")
        result += " <" + insn->get_comment() + ">";
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
            std::cerr <<"Unhandled variant " <<insn->class_name() <<" in " <<__func__ <<std::endl;
            abort();
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

    switch (insn->variantT()) {
        case V_SgAsmx86Instruction:
            return unparseX86Expression(expr);
        case V_SgAsmArmInstruction:
            return unparseArmExpression(expr);
        case V_SgAsmPowerpcInstruction:
            return unparsePowerpcExpression(expr);
        default:
            std::cerr <<"Unhandled variant " <<insn->class_name() <<" in " <<__func__ <<std::endl;
            abort();
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

SgAsmFile*
get_asmFile(SgAsmStatement* stmt)
   {
     SgNode* parent = stmt;

  // printf ("In get_file(): starting at: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
     while (isSgAsmFile(parent) == NULL)
        {
       // printf ("parent = %p = %s \n",parent,parent->class_name().c_str());
          parent = parent->get_parent();
        }

     ROSE_ASSERT(parent != NULL);
     ROSE_ASSERT(isSgAsmFile(parent) != NULL);

     return isSgAsmFile(parent);
   }


// DQ (8/23/2008): I think this should take an SgAsmStatement
// string unparseAsmStatement(SgAsmNode* stmt)
std::string
unparseAsmStatement(SgAsmStatement* stmt)
   {
  // This function should use the same mechanism as the source code
  // for output of strings so that it can eventually work with QROSE.

     ROSE_ASSERT (stmt != NULL);

  // printf ("In unparseAsmStatement(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());

     std::string result;
     if (stmt->get_comment().empty() == false)
          result = "/* " + stmt->get_comment() + " */\n";

#if 0
     string addressString = "/* Address: " + StringUtility::intToHex(stmt->get_address()) + " */";
     printf ("comment empty = %s addressString = %s \n",stmt->get_comment().empty() ? "true" : "false", addressString.c_str());
#endif

#if 1
     SgAsmFile* asmFile = get_asmFile(stmt);
     ROSE_ASSERT(asmFile != NULL);
     SgAsmGenericFile* genericFile = asmFile->get_genericFile();
     ROSE_ASSERT(genericFile != NULL);

     SgAsmInstruction* asmInstruction = isSgAsmInstruction(stmt);

     if (asmInstruction != NULL)
        {
       // result += hexdump(stmt->get_address(),"---",&(genericFile->get_data()[0]),asmInstruction->get_raw_bytes().size());
          size_t max_length = 6;
#if 0
          printf ("asmInstruction = %p = %s = %s (length = %zu) \n",asmInstruction,asmInstruction->class_name().c_str(),
               asmInstruction->get_mnemonic().c_str(),asmInstruction->get_raw_bytes().size());
#endif
          result += hexdump(stmt->get_address(),"",asmInstruction->get_raw_bytes(),max_length);
          result += " :: ";
        }
       else
        {
       // result += "----------";
        }
#endif

     switch (stmt->variantT())
        {
          case V_SgAsmx86Instruction:
          case V_SgAsmArmInstruction:
          case V_SgAsmPowerpcInstruction:
             {
               return result + unparseInstructionWithAddress(isSgAsmInstruction(stmt)) + '\n';
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
             }
        }
   }

std::string
unparseAsmInterpretation(SgAsmInterpretation* interp)
   {
     return "/* Interpretation " + std::string(interp->get_header()->format_name()) + " */\n" + (interp->get_global_block() ? unparseAsmStatement(interp->get_global_block()) : "/* No global block */");
   }

// void unparseAsmStatementToFile(const string& filename, SgAsmNode* stmt) {
void
unparseAsmStatementToFile(const std::string& filename, SgAsmStatement* stmt)
   {
     ROSE_ASSERT (stmt != NULL);
     std::ofstream of(filename.c_str());
     of << unparseAsmStatement(stmt);
   }

void
unparseAsmFileToFile(const std::string& filename, SgAsmFile* file)
   {
     ROSE_ASSERT (file != NULL);
     std::ofstream of(filename.c_str());
     const SgAsmInterpretationPtrList& interps = file->get_interpretations();

     for (size_t i = 0; i < interps.size(); ++i)
        {
          of << unparseAsmInterpretation(interps[i]) << '\n';
        }
   }

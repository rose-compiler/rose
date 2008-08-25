#include "rose.h"
#include <stdint.h>
SgAsmx86Instruction* createx86Instruction(uint64_t address, const std::string& mnemonic) {
switch (mnemonic.size()) {
  case 2:
    switch (mnemonic[0]) {
      case 'b': { // 'b'
        if (mnemonic[1] == 't') { // 'bt'
          {return new SgAsmx86Instruction(address,  mnemonic, x86_bt);}
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'n') { // 'in'
          {return new SgAsmx86Instruction(address,  mnemonic, x86_in);}
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'a': { // 'ja'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_ja);}
          }
          case 'b': { // 'jb'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_jb);}
          }
          case 'c': { // 'jc'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_jb);}
          }
          case 'e': { // 'je'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_je);}
          }
          case 'g': { // 'jg'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_jg);}
          }
          case 'l': { // 'jl'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_jl);}
          }
          case 'o': { // 'jo'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_jo);}
          }
          case 'p': { // 'jp'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_jpe);}
          }
          case 's': { // 'js'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_js);}
          }
          case 'z': { // 'jz'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_je);}
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'r') { // 'or'
          {return new SgAsmx86Instruction(address,  mnemonic, x86_or);}
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
    }
  case 3:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        switch (mnemonic[1]) {
          case 'a': { // 'aa'
            switch (mnemonic[2]) {
              case 'a': { // 'aaa'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_aaa);}
              }
              case 'd': { // 'aad'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_aad);}
              }
              case 'm': { // 'aam'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_aam);}
              }
              case 's': { // 'aas'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_aas);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'd': { // 'ad'
            switch (mnemonic[2]) {
              case 'c': { // 'adc'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_adc);}
              }
              case 'd': { // 'add'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_add);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_and);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'b': { // 'b'
        switch (mnemonic[1]) {
          case 's': { // 'bs'
            switch (mnemonic[2]) {
              case 'f': { // 'bsf'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_bsf);}
              }
              case 'r': { // 'bsr'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_bsr);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 't': { // 'bt'
            switch (mnemonic[2]) {
              case 'c': { // 'btc'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_btc);}
              }
              case 'l': { // 'btl'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_bt);}
              }
              case 'r': { // 'btr'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_btr);}
              }
              case 's': { // 'bts'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_bts);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'c': { // 'c'
        switch (mnemonic[1]) {
          case 'b': { // 'cb'
            if (mnemonic[2] == 'w') { // 'cbw'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_cbw);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'd': { // 'cd'
            if (mnemonic[2] == 'q') { // 'cdq'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_cdq);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'cl'
            switch (mnemonic[2]) {
              case 'c': { // 'clc'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_clc);}
              }
              case 'd': { // 'cld'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_cld);}
              }
              case 'i': { // 'cli'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_cli);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'm': { // 'cm'
            switch (mnemonic[2]) {
              case 'c': { // 'cmc'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_cmc);}
              }
              case 'p': { // 'cmp'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_cmp);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'q': { // 'cq'
            if (mnemonic[2] == 'o') { // 'cqo'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_cqo);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'w': { // 'cw'
            if (mnemonic[2] == 'd') { // 'cwd'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_cwd);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'd': { // 'd'
        switch (mnemonic[1]) {
          case 'a': { // 'da'
            switch (mnemonic[2]) {
              case 'a': { // 'daa'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_daa);}
              }
              case 's': { // 'das'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_das);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'e': { // 'de'
            if (mnemonic[2] == 'c') { // 'dec'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_dec);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'i': { // 'di'
            if (mnemonic[2] == 'v') { // 'div'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_div);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 's') { // 'es'
          if (mnemonic[2] == 'c') { // 'esc'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_esc);}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_fld);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'fs'
            if (mnemonic[2] == 't') { // 'fst'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_fst);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'h': { // 'h'
        if (mnemonic[1] == 'l') { // 'hl'
          if (mnemonic[2] == 't') { // 'hlt'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_hlt);}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'n') { // 'in'
          switch (mnemonic[2]) {
            case 'b': { // 'inb'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_in);}
            }
            case 'c': { // 'inc'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_inc);}
            }
            case 'd': { // 'ind'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_in);}
            }
            case 'l': { // 'inl'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_in);}
            }
            case 't': { // 'int'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_int);}
            }
            case 'w': { // 'inw'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_in);}
            }
            default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'a': { // 'ja'
            if (mnemonic[2] == 'e') { // 'jae'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_jae);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'b': { // 'jb'
            if (mnemonic[2] == 'e') { // 'jbe'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_jbe);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'g': { // 'jg'
            if (mnemonic[2] == 'e') { // 'jge'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_jge);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'jl'
            if (mnemonic[2] == 'e') { // 'jle'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_jle);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'm': { // 'jm'
            if (mnemonic[2] == 'p') { // 'jmp'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_jmp);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'n': { // 'jn'
            switch (mnemonic[2]) {
              case 'a': { // 'jna'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jbe);}
              }
              case 'b': { // 'jnb'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jae);}
              }
              case 'c': { // 'jnc'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jae);}
              }
              case 'e': { // 'jne'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jne);}
              }
              case 'g': { // 'jng'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jle);}
              }
              case 'l': { // 'jnl'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jge);}
              }
              case 'o': { // 'jno'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jno);}
              }
              case 'p': { // 'jnp'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jpo);}
              }
              case 's': { // 'jns'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jns);}
              }
              case 'z': { // 'jnz'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jne);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'p': { // 'jp'
            switch (mnemonic[2]) {
              case 'e': { // 'jpe'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jpe);}
              }
              case 'o': { // 'jpo'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jpo);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'd': { // 'ld'
            if (mnemonic[2] == 's') { // 'lds'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_lds);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'e': { // 'le'
            switch (mnemonic[2]) {
              case 'a': { // 'lea'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_lea);}
              }
              case 's': { // 'les'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_les);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'f': { // 'lf'
            if (mnemonic[2] == 's') { // 'lfs'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_lfs);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'g': { // 'lg'
            if (mnemonic[2] == 's') { // 'lgs'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_lgs);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'ls'
            if (mnemonic[2] == 's') { // 'lss'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_lss);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'm': { // 'm'
        switch (mnemonic[1]) {
          case 'o': { // 'mo'
            if (mnemonic[2] == 'v') { // 'mov'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_mov);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'u': { // 'mu'
            if (mnemonic[2] == 'l') { // 'mul'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_mul);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'n': { // 'n'
        switch (mnemonic[1]) {
          case 'e': { // 'ne'
            if (mnemonic[2] == 'g') { // 'neg'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_neg);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'o': { // 'no'
            switch (mnemonic[2]) {
              case 'p': { // 'nop'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_nop);}
              }
              case 't': { // 'not'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_not);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_out);}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'p': { // 'p'
        if (mnemonic[1] == 'o') { // 'po'
          if (mnemonic[2] == 'p') { // 'pop'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_pop);}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'r': { // 'r'
        switch (mnemonic[1]) {
          case 'c': { // 'rc'
            switch (mnemonic[2]) {
              case 'l': { // 'rcl'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_rcl);}
              }
              case 'r': { // 'rcr'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_rcr);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'e': { // 're'
            if (mnemonic[2] == 't') { // 'ret'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_ret);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'o': { // 'ro'
            switch (mnemonic[2]) {
              case 'l': { // 'rol'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_rol);}
              }
              case 'r': { // 'ror'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_ror);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'a': { // 'sa'
            switch (mnemonic[2]) {
              case 'l': { // 'sal'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_shl);}
              }
              case 'r': { // 'sar'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_sar);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'b': { // 'sb'
            if (mnemonic[2] == 'b') { // 'sbb'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_sbb);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'h': { // 'sh'
            switch (mnemonic[2]) {
              case 'l': { // 'shl'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_shl);}
              }
              case 'r': { // 'shr'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_shr);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 't': { // 'st'
            switch (mnemonic[2]) {
              case 'c': { // 'stc'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_stc);}
              }
              case 'd': { // 'std'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_std);}
              }
              case 'i': { // 'sti'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_sti);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_sub);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'u': { // 'u'
        if (mnemonic[1] == 'd') { // 'ud'
          if (mnemonic[2] == '2') { // 'ud2'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_ud2);}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'x': { // 'x'
        if (mnemonic[1] == 'o') { // 'xo'
          if (mnemonic[2] == 'r') { // 'xor'
            {return new SgAsmx86Instruction(address,  mnemonic, x86_xor);}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
    }
  case 4:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        if (mnemonic[1] == 'r') { // 'ar'
          if (mnemonic[2] == 'p') { // 'arp'
            if (mnemonic[3] == 'l') { // 'arpl'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_arpl);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'b': { // 'b'
        if (mnemonic[1] == 't') { // 'bt'
          switch (mnemonic[2]) {
            case 'c': { // 'btc'
              if (mnemonic[3] == 'l') { // 'btcl'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_btc);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            }
            case 'r': { // 'btr'
              if (mnemonic[3] == 'l') { // 'btrl'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_btr);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            }
            case 's': { // 'bts'
              if (mnemonic[3] == 'l') { // 'btsl'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_bts);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            }
            default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'c': { // 'c'
        switch (mnemonic[1]) {
          case 'a': { // 'ca'
            if (mnemonic[2] == 'l') { // 'cal'
              if (mnemonic[3] == 'l') { // 'call'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_call);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'd': { // 'cd'
            if (mnemonic[2] == 'q') { // 'cdq'
              if (mnemonic[3] == 'e') { // 'cdqe'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_cdqe);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'w': { // 'cw'
            if (mnemonic[2] == 'd') { // 'cwd'
              if (mnemonic[3] == 'e') { // 'cwde'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_cwde);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'm') { // 'em'
          if (mnemonic[2] == 'm') { // 'emm'
            if (mnemonic[3] == 's') { // 'emms'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_emms);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case 'a': { // 'fa'
            switch (mnemonic[2]) {
              case 'b': { // 'fab'
                if (mnemonic[3] == 's') { // 'fabs'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fabs);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'd': { // 'fad'
                if (mnemonic[3] == 'd') { // 'fadd'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fadd);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'c': { // 'fc'
            switch (mnemonic[2]) {
              case 'h': { // 'fch'
                if (mnemonic[3] == 's') { // 'fchs'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fchs);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'o': { // 'fco'
                switch (mnemonic[3]) {
                  case 'm': { // 'fcom'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fcom);}
                  }
                  case 's': { // 'fcos'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fcos);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_fdiv);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'l': { // 'fil'
                if (mnemonic[3] == 'd') { // 'fild'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fild);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fist);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              switch (mnemonic[3]) {
                case '1': { // 'fld1'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fld1);}
                }
                case 'l': { // 'fldl'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fld);}
                }
                case 's': { // 'flds'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fld);}
                }
                case 't': { // 'fldt'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fld);}
                }
                case 'z': { // 'fldz'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fldz);}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'm': { // 'fm'
            if (mnemonic[2] == 'u') { // 'fmu'
              if (mnemonic[3] == 'l') { // 'fmul'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_fmul);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'n': { // 'fn'
            if (mnemonic[2] == 'o') { // 'fno'
              if (mnemonic[3] == 'p') { // 'fnop'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_fnop);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'i': { // 'fsi'
                if (mnemonic[3] == 'n') { // 'fsin'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fsin);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 't': { // 'fst'
                switch (mnemonic[3]) {
                  case 'l': { // 'fstl'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fst);}
                  }
                  case 'p': { // 'fstp'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fstp);}
                  }
                  case 's': { // 'fsts'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fst);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fsub);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 't': { // 'ft'
            if (mnemonic[2] == 's') { // 'fts'
              if (mnemonic[3] == 't') { // 'ftst'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_ftst);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'x': { // 'fx'
            if (mnemonic[2] == 'c') { // 'fxc'
              if (mnemonic[3] == 'h') { // 'fxch'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_fxch);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'i': { // 'i'
        switch (mnemonic[1]) {
          case 'd': { // 'id'
            if (mnemonic[2] == 'i') { // 'idi'
              if (mnemonic[3] == 'v') { // 'idiv'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_idiv);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'm': { // 'im'
            if (mnemonic[2] == 'u') { // 'imu'
              if (mnemonic[3] == 'l') { // 'imul'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_imul);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'n': { // 'in'
            switch (mnemonic[2]) {
              case 's': { // 'ins'
                switch (mnemonic[3]) {
                  case 'b': { // 'insb'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_insb);}
                  }
                  case 'd': { // 'insd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_insd);}
                  }
                  case 'w': { // 'insw'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_insw);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 't': { // 'int'
                if (mnemonic[3] == 'o') { // 'into'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_into);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'r': { // 'ir'
            if (mnemonic[2] == 'e') { // 'ire'
              if (mnemonic[3] == 't') { // 'iret'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_iret);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'c': { // 'jc'
            if (mnemonic[2] == 'x') { // 'jcx'
              if (mnemonic[3] == 'z') { // 'jcxz'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_jcxz);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'n': { // 'jn'
            switch (mnemonic[2]) {
              case 'a': { // 'jna'
                if (mnemonic[3] == 'e') { // 'jnae'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_jb);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'b': { // 'jnb'
                if (mnemonic[3] == 'e') { // 'jnbe'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_ja);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'g': { // 'jng'
                if (mnemonic[3] == 'e') { // 'jnge'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_jl);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'l': { // 'jnl'
                if (mnemonic[3] == 'e') { // 'jnle'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_jg);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'a': { // 'la'
            if (mnemonic[2] == 'h') { // 'lah'
              if (mnemonic[3] == 'f') { // 'lahf'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_lahf);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'o': { // 'lo'
            switch (mnemonic[2]) {
              case 'c': { // 'loc'
                if (mnemonic[3] == 'k') { // 'lock'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_lock);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'o': { // 'loo'
                if (mnemonic[3] == 'p') { // 'loop'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_loop);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'p': { // 'p'
        switch (mnemonic[1]) {
          case 'a': { // 'pa'
            if (mnemonic[2] == 'n') { // 'pan'
              if (mnemonic[3] == 'd') { // 'pand'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_pand);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'o': { // 'po'
            if (mnemonic[2] == 'p') { // 'pop'
              switch (mnemonic[3]) {
                case 'a': { // 'popa'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_popa);}
                }
                case 'f': { // 'popf'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_popf);}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'u': { // 'pu'
            if (mnemonic[2] == 's') { // 'pus'
              if (mnemonic[3] == 'h') { // 'push'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_push);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'x': { // 'px'
            if (mnemonic[2] == 'o') { // 'pxo'
              if (mnemonic[3] == 'r') { // 'pxor'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_pxor);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'a': { // 'sa'
            if (mnemonic[2] == 'h') { // 'sah'
              if (mnemonic[3] == 'f') { // 'sahf'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_sahf);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'e': { // 'se'
            if (mnemonic[2] == 't') { // 'set'
              switch (mnemonic[3]) {
                case 'a': { // 'seta'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_seta);}
                }
                case 'b': { // 'setb'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_setb);}
                }
                case 'c': { // 'setc'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_setb);}
                }
                case 'e': { // 'sete'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_sete);}
                }
                case 'g': { // 'setg'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_setg);}
                }
                case 'l': { // 'setl'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_setl);}
                }
                case 'o': { // 'seto'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_seto);}
                }
                case 'p': { // 'setp'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_setpe);}
                }
                case 's': { // 'sets'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_sets);}
                }
                case 'z': { // 'setz'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_sete);}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'h': { // 'sh'
            switch (mnemonic[2]) {
              case 'l': { // 'shl'
                if (mnemonic[3] == 'd') { // 'shld'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_shld);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'r': { // 'shr'
                if (mnemonic[3] == 'd') { // 'shrd'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_shrd);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'sl'
            if (mnemonic[2] == 'd') { // 'sld'
              if (mnemonic[3] == 't') { // 'sldt'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_sldt);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 't': { // 't'
        if (mnemonic[1] == 'e') { // 'te'
          if (mnemonic[2] == 's') { // 'tes'
            if (mnemonic[3] == 't') { // 'test'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_test);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'v': { // 'v'
        if (mnemonic[1] == 'e') { // 've'
          if (mnemonic[2] == 'r') { // 'ver'
            switch (mnemonic[3]) {
              case 'r': { // 'verr'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_verr);}
              }
              case 'w': { // 'verw'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_verw);}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'w': { // 'w'
        if (mnemonic[1] == 'a') { // 'wa'
          if (mnemonic[2] == 'i') { // 'wai'
            if (mnemonic[3] == 't') { // 'wait'
              {return new SgAsmx86Instruction(address,  mnemonic, x86_wait);}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'x': { // 'x'
        switch (mnemonic[1]) {
          case 'a': { // 'xa'
            if (mnemonic[2] == 'd') { // 'xad'
              if (mnemonic[3] == 'd') { // 'xadd'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_xadd);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'c': { // 'xc'
            if (mnemonic[2] == 'h') { // 'xch'
              if (mnemonic[3] == 'g') { // 'xchg'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_xchg);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'xl'
            if (mnemonic[2] == 'a') { // 'xla'
              if (mnemonic[3] == 't') { // 'xlat'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_xlatb);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
    }
  case 5:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        switch (mnemonic[1]) {
          case 'd': { // 'ad'
            if (mnemonic[2] == 'd') { // 'add'
              if (mnemonic[3] == 's') { // 'adds'
                switch (mnemonic[4]) {
                  case 'd': { // 'addsd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_addsd);}
                  }
                  case 's': { // 'addss'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_addss);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              if (mnemonic[3] == 'p') { // 'andp'
                if (mnemonic[4] == 'd') { // 'andpd'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_andpd);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'b': { // 'b'
        switch (mnemonic[1]) {
          case 'o': { // 'bo'
            if (mnemonic[2] == 'u') { // 'bou'
              if (mnemonic[3] == 'n') { // 'boun'
                if (mnemonic[4] == 'd') { // 'bound'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_bound);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'bs'
            if (mnemonic[2] == 'w') { // 'bsw'
              if (mnemonic[3] == 'a') { // 'bswa'
                if (mnemonic[4] == 'p') { // 'bswap'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_bswap);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'c': { // 'c'
        switch (mnemonic[1]) {
          case 'm': { // 'cm'
            switch (mnemonic[2]) {
              case 'o': { // 'cmo'
                if (mnemonic[3] == 'v') { // 'cmov'
                  switch (mnemonic[4]) {
                    case 'a': { // 'cmova'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmova);}
                    }
                    case 'b': { // 'cmovb'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovb);}
                    }
                    case 'c': { // 'cmovc'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovb);}
                    }
                    case 'e': { // 'cmove'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmove);}
                    }
                    case 'g': { // 'cmovg'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovg);}
                    }
                    case 'l': { // 'cmovl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovl);}
                    }
                    case 'o': { // 'cmovo'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovo);}
                    }
                    case 'p': { // 'cmovp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovpe);}
                    }
                    case 's': { // 'cmovs'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovs);}
                    }
                    case 'z': { // 'cmovz'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmove);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'p': { // 'cmp'
                if (mnemonic[3] == 's') { // 'cmps'
                  switch (mnemonic[4]) {
                    case 'b': { // 'cmpsb'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmpsb);}
                    }
                    case 'd': { // 'cmpsd'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmpsd);}
                    }
                    case 'q': { // 'cmpsq'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmpsq);}
                    }
                    case 'w': { // 'cmpsw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmpsw);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'p': { // 'cp'
            if (mnemonic[2] == 'u') { // 'cpu'
              if (mnemonic[3] == 'i') { // 'cpui'
                if (mnemonic[4] == 'd') { // 'cpuid'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_cpuid);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'd': { // 'd'
        if (mnemonic[1] == 'i') { // 'di'
          if (mnemonic[2] == 'v') { // 'div'
            if (mnemonic[3] == 's') { // 'divs'
              if (mnemonic[4] == 'd') { // 'divsd'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_divsd);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'n') { // 'en'
          if (mnemonic[2] == 't') { // 'ent'
            if (mnemonic[3] == 'e') { // 'ente'
              if (mnemonic[4] == 'r') { // 'enter'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_enter);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case '2': { // 'f2'
            if (mnemonic[2] == 'x') { // 'f2x'
              if (mnemonic[3] == 'm') { // 'f2xm'
                if (mnemonic[4] == '1') { // 'f2xm1'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_f2xm1);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'a': { // 'fa'
            if (mnemonic[2] == 'd') { // 'fad'
              if (mnemonic[3] == 'd') { // 'fadd'
                switch (mnemonic[4]) {
                  case 'l': { // 'faddl'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fadd);}
                  }
                  case 'p': { // 'faddp'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_faddp);}
                  }
                  case 's': { // 'fadds'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fadd);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'c': { // 'fc'
            if (mnemonic[2] == 'o') { // 'fco'
              if (mnemonic[3] == 'm') { // 'fcom'
                switch (mnemonic[4]) {
                  case 'l': { // 'fcoml'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fcom);}
                  }
                  case 'p': { // 'fcomp'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fcomp);}
                  }
                  case 's': { // 'fcoms'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fcom);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                switch (mnemonic[4]) {
                  case 'l': { // 'fdivl'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fdiv);}
                  }
                  case 'p': { // 'fdivp'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fdivp);}
                  }
                  case 'r': { // 'fdivr'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fdivr);}
                  }
                  case 's': { // 'fdivs'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fdiv);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'f': { // 'ff'
            if (mnemonic[2] == 'r') { // 'ffr'
              if (mnemonic[3] == 'e') { // 'ffre'
                if (mnemonic[4] == 'e') { // 'ffree'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_ffree);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'a': { // 'fia'
                if (mnemonic[3] == 'd') { // 'fiad'
                  if (mnemonic[4] == 'd') { // 'fiadd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fiadd);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'd': { // 'fid'
                if (mnemonic[3] == 'i') { // 'fidi'
                  if (mnemonic[4] == 'v') { // 'fidiv'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fidiv);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'l': { // 'fil'
                if (mnemonic[3] == 'd') { // 'fild'
                  if (mnemonic[4] == 'l') { // 'fildl'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fild);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'm': { // 'fim'
                if (mnemonic[3] == 'u') { // 'fimu'
                  if (mnemonic[4] == 'l') { // 'fimul'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fimul);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fistl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fist);}
                    }
                    case 'p': { // 'fistp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fistp);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              switch (mnemonic[3]) {
                case 'c': { // 'fldc'
                  if (mnemonic[4] == 'w') { // 'fldcw'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fldcw);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'p': { // 'fldp'
                  if (mnemonic[4] == 'i') { // 'fldpi'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fldpi);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'm': { // 'fm'
            if (mnemonic[2] == 'u') { // 'fmu'
              if (mnemonic[3] == 'l') { // 'fmul'
                switch (mnemonic[4]) {
                  case 'l': { // 'fmull'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fmul);}
                  }
                  case 'p': { // 'fmulp'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fmulp);}
                  }
                  case 's': { // 'fmuls'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fmul);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'p': { // 'fp'
            switch (mnemonic[2]) {
              case 'r': { // 'fpr'
                if (mnemonic[3] == 'e') { // 'fpre'
                  if (mnemonic[4] == 'm') { // 'fprem'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fprem);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 't': { // 'fpt'
                if (mnemonic[3] == 'a') { // 'fpta'
                  if (mnemonic[4] == 'n') { // 'fptan'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fptan);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'a': { // 'fsa'
                if (mnemonic[3] == 'v') { // 'fsav'
                  if (mnemonic[4] == 'e') { // 'fsave'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fsave);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'q': { // 'fsq'
                if (mnemonic[3] == 'r') { // 'fsqr'
                  if (mnemonic[4] == 't') { // 'fsqrt'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fsqrt);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 't': { // 'fst'
                if (mnemonic[3] == 'p') { // 'fstp'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fstpl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fstp);}
                    }
                    case 's': { // 'fstps'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fstp);}
                    }
                    case 't': { // 'fstpt'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fstp);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fsubl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fsub);}
                    }
                    case 'p': { // 'fsubp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fsubp);}
                    }
                    case 'r': { // 'fsubr'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fsubr);}
                    }
                    case 's': { // 'fsubs'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fsub);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'u': { // 'fu'
            if (mnemonic[2] == 'c') { // 'fuc'
              if (mnemonic[3] == 'o') { // 'fuco'
                if (mnemonic[4] == 'm') { // 'fucom'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fucom);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'w': { // 'fw'
            if (mnemonic[2] == 'a') { // 'fwa'
              if (mnemonic[3] == 'i') { // 'fwai'
                if (mnemonic[4] == 't') { // 'fwait'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fwait);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'y': { // 'fy'
            if (mnemonic[2] == 'l') { // 'fyl'
              if (mnemonic[3] == '2') { // 'fyl2'
                if (mnemonic[4] == 'x') { // 'fyl2x'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_fyl2x);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'r') { // 'ir'
          if (mnemonic[2] == 'e') { // 'ire'
            if (mnemonic[3] == 't') { // 'iret'
              switch (mnemonic[4]) {
                case 'd': { // 'iretd'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_iret);}
                }
                case 'l': { // 'iretl'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_iret);}
                }
                case 'q': { // 'iretq'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_iret);}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'e': { // 'je'
            if (mnemonic[2] == 'c') { // 'jec'
              if (mnemonic[3] == 'x') { // 'jecx'
                if (mnemonic[4] == 'z') { // 'jecxz'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_jecxz);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'r': { // 'jr'
            if (mnemonic[2] == 'c') { // 'jrc'
              if (mnemonic[3] == 'x') { // 'jrcx'
                if (mnemonic[4] == 'z') { // 'jrcxz'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_jrcxz);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'e': { // 'le'
            if (mnemonic[2] == 'a') { // 'lea'
              if (mnemonic[3] == 'v') { // 'leav'
                if (mnemonic[4] == 'e') { // 'leave'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_leave);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'o': { // 'lo'
            switch (mnemonic[2]) {
              case 'd': { // 'lod'
                if (mnemonic[3] == 's') { // 'lods'
                  switch (mnemonic[4]) {
                    case 'b': { // 'lodsb'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_lodsb);}
                    }
                    case 'd': { // 'lodsd'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_lodsd);}
                    }
                    case 'q': { // 'lodsq'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_lodsq);}
                    }
                    case 'w': { // 'lodsw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_lodsw);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'o': { // 'loo'
                if (mnemonic[3] == 'p') { // 'loop'
                  switch (mnemonic[4]) {
                    case 'e': { // 'loope'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_loopz);}
                    }
                    case 'l': { // 'loopl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_loop);}
                    }
                    case 'z': { // 'loopz'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_loopz);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'm': { // 'm'
        if (mnemonic[1] == 'o') { // 'mo'
          if (mnemonic[2] == 'v') { // 'mov'
            switch (mnemonic[3]) {
              case 's': { // 'movs'
                switch (mnemonic[4]) {
                  case 'b': { // 'movsb'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movsb);}
                  }
                  case 'd': { // 'movsd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movsd);}
                  }
                  case 'q': { // 'movsq'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movsq);}
                  }
                  case 's': { // 'movss'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movss);}
                  }
                  case 'w': { // 'movsw'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movsw);}
                  }
                  case 'x': { // 'movsx'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movsx);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'z': { // 'movz'
                if (mnemonic[4] == 'x') { // 'movzx'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_movzx);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            if (mnemonic[3] == 's') { // 'outs'
              switch (mnemonic[4]) {
                case 'b': { // 'outsb'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_outsb);}
                }
                case 'd': { // 'outsd'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_outsd);}
                }
                case 'w': { // 'outsw'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_outsw);}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'p': { // 'p'
        switch (mnemonic[1]) {
          case 'a': { // 'pa'
            switch (mnemonic[2]) {
              case 'd': { // 'pad'
                if (mnemonic[3] == 'd') { // 'padd'
                  switch (mnemonic[4]) {
                    case 'b': { // 'paddb'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_padd);}
                    }
                    case 'd': { // 'paddd'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_padd);}
                    }
                    case 'q': { // 'paddq'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_padd);}
                    }
                    case 'w': { // 'paddw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_padd);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'n': { // 'pan'
                if (mnemonic[3] == 'd') { // 'pand'
                  if (mnemonic[4] == 'n') { // 'pandn'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_pandn);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'u': { // 'pau'
                if (mnemonic[3] == 's') { // 'paus'
                  if (mnemonic[4] == 'e') { // 'pause'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_pause);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'o': { // 'po'
            if (mnemonic[2] == 'p') { // 'pop'
              switch (mnemonic[3]) {
                case 'a': { // 'popa'
                  if (mnemonic[4] == 'd') { // 'popad'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_popad);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'f': { // 'popf'
                  switch (mnemonic[4]) {
                    case 'd': { // 'popfd'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_popfd);}
                    }
                    case 'q': { // 'popfq'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_popfq);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'ps'
            switch (mnemonic[2]) {
              case 'l': { // 'psl'
                if (mnemonic[3] == 'l') { // 'psll'
                  switch (mnemonic[4]) {
                    case 'd': { // 'pslld'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psll);}
                    }
                    case 'q': { // 'psllq'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psll);}
                    }
                    case 'w': { // 'psllw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psll);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'r': { // 'psr'
                if (mnemonic[3] == 'l') { // 'psrl'
                  switch (mnemonic[4]) {
                    case 'd': { // 'psrld'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psrl);}
                    }
                    case 'q': { // 'psrlq'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psrl);}
                    }
                    case 'w': { // 'psrlw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psrl);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'u': { // 'psu'
                if (mnemonic[3] == 'b') { // 'psub'
                  switch (mnemonic[4]) {
                    case 'b': { // 'psubb'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psub);}
                    }
                    case 'd': { // 'psubd'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psub);}
                    }
                    case 'w': { // 'psubw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_psub);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'u': { // 'pu'
            if (mnemonic[2] == 's') { // 'pus'
              if (mnemonic[3] == 'h') { // 'push'
                switch (mnemonic[4]) {
                  case 'a': { // 'pusha'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_pusha);}
                  }
                  case 'f': { // 'pushf'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_pushf);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'r': { // 'r'
        if (mnemonic[1] == 'd') { // 'rd'
          if (mnemonic[2] == 't') { // 'rdt'
            if (mnemonic[3] == 's') { // 'rdts'
              if (mnemonic[4] == 'c') { // 'rdtsc'
                {return new SgAsmx86Instruction(address,  mnemonic, x86_rdtsc);}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'c': { // 'sc'
            if (mnemonic[2] == 'a') { // 'sca'
              if (mnemonic[3] == 's') { // 'scas'
                switch (mnemonic[4]) {
                  case 'b': { // 'scasb'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_scasb);}
                  }
                  case 'd': { // 'scasd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_scasd);}
                  }
                  case 'q': { // 'scasq'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_scasq);}
                  }
                  case 'w': { // 'scasw'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_scasw);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'e': { // 'se'
            if (mnemonic[2] == 't') { // 'set'
              switch (mnemonic[3]) {
                case 'a': { // 'seta'
                  if (mnemonic[4] == 'e') { // 'setae'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_setb);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'b': { // 'setb'
                  if (mnemonic[4] == 'e') { // 'setbe'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_setbe);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'g': { // 'setg'
                  if (mnemonic[4] == 'e') { // 'setge'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_setge);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'l': { // 'setl'
                  if (mnemonic[4] == 'e') { // 'setle'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_setle);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'n': { // 'setn'
                  switch (mnemonic[4]) {
                    case 'a': { // 'setna'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setbe);}
                    }
                    case 'b': { // 'setnb'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setae);}
                    }
                    case 'c': { // 'setnc'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setae);}
                    }
                    case 'e': { // 'setne'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setne);}
                    }
                    case 'g': { // 'setng'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setle);}
                    }
                    case 'l': { // 'setnl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setge);}
                    }
                    case 'o': { // 'setno'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setno);}
                    }
                    case 'p': { // 'setnp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setpo);}
                    }
                    case 's': { // 'setns'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setns);}
                    }
                    case 'z': { // 'setnz'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setne);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'p': { // 'setp'
                  switch (mnemonic[4]) {
                    case 'e': { // 'setpe'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setpe);}
                    }
                    case 'o': { // 'setpo'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_setpo);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'sl'
            if (mnemonic[2] == 'd') { // 'sld'
              if (mnemonic[3] == 't') { // 'sldt'
                if (mnemonic[4] == 'l') { // 'sldtl'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_sldt);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 't': { // 'st'
            if (mnemonic[2] == 'o') { // 'sto'
              if (mnemonic[3] == 's') { // 'stos'
                switch (mnemonic[4]) {
                  case 'b': { // 'stosb'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_stosb);}
                  }
                  case 'd': { // 'stosd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_stosd);}
                  }
                  case 'q': { // 'stosq'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_stosq);}
                  }
                  case 'w': { // 'stosw'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_stosw);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              if (mnemonic[3] == 's') { // 'subs'
                switch (mnemonic[4]) {
                  case 'd': { // 'subsd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_subsd);}
                  }
                  case 's': { // 'subss'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_subss);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'x': { // 'x'
        switch (mnemonic[1]) {
          case 'a': { // 'xa'
            if (mnemonic[2] == 'd') { // 'xad'
              if (mnemonic[3] == 'd') { // 'xadd'
                if (mnemonic[4] == 'l') { // 'xaddl'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_xadd);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'xl'
            if (mnemonic[2] == 'a') { // 'xla'
              if (mnemonic[3] == 't') { // 'xlat'
                if (mnemonic[4] == 'b') { // 'xlatb'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_xlatb);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'o': { // 'xo'
            if (mnemonic[2] == 'r') { // 'xor'
              if (mnemonic[3] == 'p') { // 'xorp'
                switch (mnemonic[4]) {
                  case 'd': { // 'xorpd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_xorpd);}
                  }
                  case 's': { // 'xorps'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_xorps);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
    }
  case 6:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        if (mnemonic[1] == 'n') { // 'an'
          if (mnemonic[2] == 'd') { // 'and'
            if (mnemonic[3] == 'n') { // 'andn'
              if (mnemonic[4] == 'p') { // 'andnp'
                switch (mnemonic[5]) {
                  case 'd': { // 'andnpd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_andnpd);}
                  }
                  case 's': { // 'andnps'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_andnpd);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'b': { // 'b'
        if (mnemonic[1] == 'o') { // 'bo'
          if (mnemonic[2] == 'u') { // 'bou'
            if (mnemonic[3] == 'n') { // 'boun'
              if (mnemonic[4] == 'd') { // 'bound'
                if (mnemonic[5] == 'l') { // 'boundl'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_bound);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'c': { // 'c'
        if (mnemonic[1] == 'm') { // 'cm'
          if (mnemonic[2] == 'o') { // 'cmo'
            if (mnemonic[3] == 'v') { // 'cmov'
              switch (mnemonic[4]) {
                case 'a': { // 'cmova'
                  if (mnemonic[5] == 'e') { // 'cmovae'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovae);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'b': { // 'cmovb'
                  if (mnemonic[5] == 'e') { // 'cmovbe'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovbe);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'g': { // 'cmovg'
                  if (mnemonic[5] == 'e') { // 'cmovge'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovge);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'l': { // 'cmovl'
                  if (mnemonic[5] == 'e') { // 'cmovle'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovle);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'n': { // 'cmovn'
                  switch (mnemonic[5]) {
                    case 'a': { // 'cmovna'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovbe);}
                    }
                    case 'b': { // 'cmovnb'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovae);}
                    }
                    case 'c': { // 'cmovnc'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovae);}
                    }
                    case 'e': { // 'cmovne'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovne);}
                    }
                    case 'g': { // 'cmovng'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovle);}
                    }
                    case 'l': { // 'cmovnl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovge);}
                    }
                    case 'o': { // 'cmovno'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovno);}
                    }
                    case 'p': { // 'cmovnp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovpo);}
                    }
                    case 's': { // 'cmovns'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovns);}
                    }
                    case 'z': { // 'cmovnz'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovne);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'p': { // 'cmovp'
                  switch (mnemonic[5]) {
                    case 'e': { // 'cmovpe'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovpe);}
                    }
                    case 'o': { // 'cmovpo'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovpo);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'd': { // 'd'
        if (mnemonic[1] == 'a') { // 'da'
          if (mnemonic[2] == 't') { // 'dat'
            if (mnemonic[3] == 'a') { // 'data'
              if (mnemonic[4] == '1') { // 'data1'
                if (mnemonic[5] == '6') { // 'data16'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_data16);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'n') { // 'en'
          if (mnemonic[2] == 't') { // 'ent'
            if (mnemonic[3] == 'e') { // 'ente'
              if (mnemonic[4] == 'r') { // 'enter'
                if (mnemonic[5] == 'l') { // 'enterl'
                  {return new SgAsmx86Instruction(address,  mnemonic, x86_enter);}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case 'c': { // 'fc'
            if (mnemonic[2] == 'o') { // 'fco'
              if (mnemonic[3] == 'm') { // 'fcom'
                if (mnemonic[4] == 'p') { // 'fcomp'
                  switch (mnemonic[5]) {
                    case 'l': { // 'fcompl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fcomp);}
                    }
                    case 'p': { // 'fcompp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fcompp);}
                    }
                    case 's': { // 'fcomps'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fcomp);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                if (mnemonic[4] == 'r') { // 'fdivr'
                  switch (mnemonic[5]) {
                    case 'l': { // 'fdivrl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fdivr);}
                    }
                    case 'p': { // 'fdivrp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fdivrp);}
                    }
                    case 's': { // 'fdivrs'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fdivr);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'a': { // 'fia'
                if (mnemonic[3] == 'd') { // 'fiad'
                  if (mnemonic[4] == 'd') { // 'fiadd'
                    if (mnemonic[5] == 'l') { // 'fiaddl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fiadd);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'd': { // 'fid'
                if (mnemonic[3] == 'i') { // 'fidi'
                  if (mnemonic[4] == 'v') { // 'fidiv'
                    if (mnemonic[5] == 'l') { // 'fidivl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fidiv);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'm': { // 'fim'
                if (mnemonic[3] == 'u') { // 'fimu'
                  if (mnemonic[4] == 'l') { // 'fimul'
                    if (mnemonic[5] == 'l') { // 'fimull'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fimul);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  if (mnemonic[4] == 'p') { // 'fistp'
                    if (mnemonic[5] == 'l') { // 'fistpl'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fistp);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              if (mnemonic[3] == 'l') { // 'fldl'
                switch (mnemonic[4]) {
                  case '2': { // 'fldl2'
                    switch (mnemonic[5]) {
                      case 'e': { // 'fldl2e'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_fldl2e);}
                      }
                      case 't': { // 'fldl2t'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_fldl2t);}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  }
                  case 'g': { // 'fldlg'
                    if (mnemonic[5] == '2') { // 'fldlg2'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fldlg2);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  }
                  case 'n': { // 'fldln'
                    if (mnemonic[5] == '2') { // 'fldln2'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fldln2);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'n': { // 'fn'
            if (mnemonic[2] == 's') { // 'fns'
              if (mnemonic[3] == 't') { // 'fnst'
                switch (mnemonic[4]) {
                  case 'c': { // 'fnstc'
                    if (mnemonic[5] == 'w') { // 'fnstcw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fnstcw);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  }
                  case 's': { // 'fnsts'
                    if (mnemonic[5] == 'w') { // 'fnstsw'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fnstsw);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'p': { // 'fp'
            if (mnemonic[2] == 'a') { // 'fpa'
              if (mnemonic[3] == 't') { // 'fpat'
                if (mnemonic[4] == 'a') { // 'fpata'
                  if (mnemonic[5] == 'n') { // 'fpatan'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fpatan);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'c': { // 'fsc'
                if (mnemonic[3] == 'a') { // 'fsca'
                  if (mnemonic[4] == 'l') { // 'fscal'
                    if (mnemonic[5] == 'e') { // 'fscale'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fscale);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  if (mnemonic[4] == 'r') { // 'fsubr'
                    switch (mnemonic[5]) {
                      case 'l': { // 'fsubrl'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_fsubr);}
                      }
                      case 'p': { // 'fsubrp'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_fsubrp);}
                      }
                      case 's': { // 'fsubrs'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_fsubr);}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'u': { // 'fu'
            if (mnemonic[2] == 'c') { // 'fuc'
              if (mnemonic[3] == 'o') { // 'fuco'
                if (mnemonic[4] == 'm') { // 'fucom'
                  switch (mnemonic[5]) {
                    case 'i': { // 'fucomi'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fucom);}
                    }
                    case 'p': { // 'fucomp'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_fucomp);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'x': { // 'fx'
            if (mnemonic[2] == 's') { // 'fxs'
              if (mnemonic[3] == 'a') { // 'fxsa'
                if (mnemonic[4] == 'v') { // 'fxsav'
                  if (mnemonic[5] == 'e') { // 'fxsave'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fxsave);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'o') { // 'lo'
          if (mnemonic[2] == 'o') { // 'loo'
            if (mnemonic[3] == 'p') { // 'loop'
              if (mnemonic[4] == 'n') { // 'loopn'
                switch (mnemonic[5]) {
                  case 'e': { // 'loopne'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_loopnz);}
                  }
                  case 'z': { // 'loopnz'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_loopnz);}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'm': { // 'm'
        if (mnemonic[1] == 'o') { // 'mo'
          if (mnemonic[2] == 'v') { // 'mov'
            switch (mnemonic[3]) {
              case 'a': { // 'mova'
                if (mnemonic[4] == 'p') { // 'movap'
                  if (mnemonic[5] == 's') { // 'movaps'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movaps);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'd': { // 'movd'
                if (mnemonic[4] == 'q') { // 'movdq'
                  if (mnemonic[5] == 'u') { // 'movdqu'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movdqu);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'l': { // 'movl'
                if (mnemonic[4] == 'p') { // 'movlp'
                  if (mnemonic[5] == 'd') { // 'movlpd'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movlpd);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              case 'n': { // 'movn'
                if (mnemonic[4] == 't') { // 'movnt'
                  if (mnemonic[5] == 'q') { // 'movntq'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_movntq);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'p': { // 'p'
        if (mnemonic[1] == 'u') { // 'pu'
          if (mnemonic[2] == 's') { // 'pus'
            if (mnemonic[3] == 'h') { // 'push'
              switch (mnemonic[4]) {
                case 'a': { // 'pusha'
                  if (mnemonic[5] == 'd') { // 'pushad'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_pushad);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'f': { // 'pushf'
                  switch (mnemonic[5]) {
                    case 'd': { // 'pushfd'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_pushfd);}
                    }
                    case 'q': { // 'pushfq'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_pushfq);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 's': { // 's'
        if (mnemonic[1] == 'e') { // 'se'
          if (mnemonic[2] == 't') { // 'set'
            if (mnemonic[3] == 'n') { // 'setn'
              switch (mnemonic[4]) {
                case 'b': { // 'setnb'
                  if (mnemonic[5] == 'e') { // 'setnbe'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_seta);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'g': { // 'setng'
                  if (mnemonic[5] == 'e') { // 'setnge'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_setl);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                case 'l': { // 'setnl'
                  if (mnemonic[5] == 'e') { // 'setnle'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_setg);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
    }
  case 7:
    switch (mnemonic[0]) {
      case 'c': { // 'c'
        if (mnemonic[1] == 'm') { // 'cm'
          switch (mnemonic[2]) {
            case 'o': { // 'cmo'
              if (mnemonic[3] == 'v') { // 'cmov'
                if (mnemonic[4] == 'n') { // 'cmovn'
                  switch (mnemonic[5]) {
                    case 'a': { // 'cmovna'
                      if (mnemonic[6] == 'e') { // 'cmovnae'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovb);}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                      }
                    }
                    case 'b': { // 'cmovnb'
                      if (mnemonic[6] == 'e') { // 'cmovnbe'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_cmova);}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                      }
                    }
                    case 'g': { // 'cmovng'
                      if (mnemonic[6] == 'e') { // 'cmovnge'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovl);}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                      }
                    }
                    case 'l': { // 'cmovnl'
                      if (mnemonic[6] == 'e') { // 'cmovnle'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_cmovg);}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                      }
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            }
            case 'p': { // 'cmp'
              if (mnemonic[3] == 'x') { // 'cmpx'
                if (mnemonic[4] == 'c') { // 'cmpxc'
                  if (mnemonic[5] == 'h') { // 'cmpxch'
                    if (mnemonic[6] == 'g') { // 'cmpxchg'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmpxchg);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            }
            default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'f': { // 'f'
        if (mnemonic[1] == 'u') { // 'fu'
          if (mnemonic[2] == 'c') { // 'fuc'
            if (mnemonic[3] == 'o') { // 'fuco'
              if (mnemonic[4] == 'm') { // 'fucom'
                if (mnemonic[5] == 'p') { // 'fucomp'
                  if (mnemonic[6] == 'p') { // 'fucompp'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_fucompp);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'd') { // 'ld'
          if (mnemonic[2] == 'm') { // 'ldm'
            if (mnemonic[3] == 'x') { // 'ldmx'
              if (mnemonic[4] == 'c') { // 'ldmxc'
                if (mnemonic[5] == 's') { // 'ldmxcs'
                  if (mnemonic[6] == 'r') { // 'ldmxcsr'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_ldmxcsr);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'p': { // 'p'
        switch (mnemonic[1]) {
          case 'a': { // 'pa'
            if (mnemonic[2] == 'd') { // 'pad'
              if (mnemonic[3] == 'd') { // 'padd'
                if (mnemonic[4] == 'u') { // 'paddu'
                  if (mnemonic[5] == 's') { // 'paddus'
                    switch (mnemonic[6]) {
                      case 'b': { // 'paddusb'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_paddus);}
                      }
                      case 'w': { // 'paddusw'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_paddus);}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 'c': { // 'pc'
            if (mnemonic[2] == 'm') { // 'pcm'
              if (mnemonic[3] == 'p') { // 'pcmp'
                if (mnemonic[4] == 'e') { // 'pcmpe'
                  if (mnemonic[5] == 'q') { // 'pcmpeq'
                    switch (mnemonic[6]) {
                      case 'b': { // 'pcmpeqb'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_pcmpeq);}
                      }
                      case 'd': { // 'pcmpeqd'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_pcmpeq);}
                      }
                      case 'w': { // 'pcmpeqw'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_pcmpeq);}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          case 's': { // 'ps'
            if (mnemonic[2] == 'u') { // 'psu'
              if (mnemonic[3] == 'b') { // 'psub'
                if (mnemonic[4] == 'u') { // 'psubu'
                  if (mnemonic[5] == 's') { // 'psubus'
                    switch (mnemonic[6]) {
                      case 'b': { // 'psubusb'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_psubus);}
                      }
                      case 'w': { // 'psubusw'
                        {return new SgAsmx86Instruction(address,  mnemonic, x86_psubus);}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 's': { // 's'
        if (mnemonic[1] == 't') { // 'st'
          if (mnemonic[2] == 'm') { // 'stm'
            if (mnemonic[3] == 'x') { // 'stmx'
              if (mnemonic[4] == 'c') { // 'stmxc'
                if (mnemonic[5] == 's') { // 'stmxcs'
                  if (mnemonic[6] == 'r') { // 'stmxcsr'
                    {return new SgAsmx86Instruction(address,  mnemonic, x86_stmxcsr);}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      case 'u': { // 'u'
        if (mnemonic[1] == 'c') { // 'uc'
          if (mnemonic[2] == 'o') { // 'uco'
            if (mnemonic[3] == 'm') { // 'ucom'
              if (mnemonic[4] == 'i') { // 'ucomi'
                if (mnemonic[5] == 's') { // 'ucomis'
                  switch (mnemonic[6]) {
                    case 'd': { // 'ucomisd'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_ucomisd);}
                    }
                    case 's': { // 'ucomiss'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_ucomiss);}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
    }
  case 9:
    if (mnemonic[0] == 'c') { // 'c'
      if (mnemonic[1] == 'm') { // 'cm'
        if (mnemonic[2] == 'p') { // 'cmp'
          if (mnemonic[3] == 'x') { // 'cmpx'
            if (mnemonic[4] == 'c') { // 'cmpxc'
              if (mnemonic[5] == 'h') { // 'cmpxch'
                if (mnemonic[6] == 'g') { // 'cmpxchg'
                  if (mnemonic[7] == '8') { // 'cmpxchg8'
                    if (mnemonic[8] == 'b') { // 'cmpxchg8b'
                      {return new SgAsmx86Instruction(address,  mnemonic, x86_cmpxchg8b);}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
        }
      } else {
        return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
      }
    } else {
      return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
    }
  default: return new SgAsmx86Instruction(address,  mnemonic, x86_unknown_instruction);
}
}
SgAsmArmInstruction* createArmInstruction(uint64_t address, const std::string& mnemonic) {
switch (mnemonic.size()) {
  case 1:
    if (mnemonic[0] == 'b') { // 'b'
      {return new SgAsmArmInstruction(address,  mnemonic, arm_b);}
    } else {
      return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
    }
  case 2:
    if (mnemonic[0] == 'b') { // 'b'
      switch (mnemonic[1]) {
        case 'l': { // 'bl'
          {return new SgAsmArmInstruction(address,  mnemonic, arm_bl);}
        }
        case 'x': { // 'bx'
          {return new SgAsmArmInstruction(address,  mnemonic, arm_bx);}
        }
        default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
      }
    } else {
      return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
    }
  case 3:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        switch (mnemonic[1]) {
          case 'd': { // 'ad'
            switch (mnemonic[2]) {
              case 'c': { // 'adc'
                {return new SgAsmArmInstruction(address,  mnemonic, arm_adc);}
              }
              case 'd': { // 'add'
                {return new SgAsmArmInstruction(address,  mnemonic, arm_add);}
              }
              default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_and);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'b': { // 'b'
        if (mnemonic[1] == 'i') { // 'bi'
          if (mnemonic[2] == 'c') { // 'bic'
            {return new SgAsmArmInstruction(address,  mnemonic, arm_bic);}
          } else {
            return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'c': { // 'c'
        if (mnemonic[1] == 'm') { // 'cm'
          switch (mnemonic[2]) {
            case 'n': { // 'cmn'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_cmn);}
            }
            case 'p': { // 'cmp'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_cmp);}
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'o') { // 'eo'
          if (mnemonic[2] == 'r') { // 'eor'
            {return new SgAsmArmInstruction(address,  mnemonic, arm_eor);}
          } else {
            return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'd') { // 'ld'
          switch (mnemonic[2]) {
            case 'm': { // 'ldm'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_ldm);}
            }
            case 'r': { // 'ldr'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_ldr);}
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'm': { // 'm'
        switch (mnemonic[1]) {
          case 'l': { // 'ml'
            if (mnemonic[2] == 'a') { // 'mla'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_mla);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 'o': { // 'mo'
            if (mnemonic[2] == 'v') { // 'mov'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_mov);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 'r': { // 'mr'
            if (mnemonic[2] == 's') { // 'mrs'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_mrs);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 's': { // 'ms'
            if (mnemonic[2] == 'r') { // 'msr'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_msr);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 'u': { // 'mu'
            if (mnemonic[2] == 'l') { // 'mul'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_mul);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 'v': { // 'mv'
            if (mnemonic[2] == 'n') { // 'mvn'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_mvn);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'r') { // 'or'
          if (mnemonic[2] == 'r') { // 'orr'
            {return new SgAsmArmInstruction(address,  mnemonic, arm_orr);}
          } else {
            return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'r': { // 'r'
        if (mnemonic[1] == 's') { // 'rs'
          switch (mnemonic[2]) {
            case 'b': { // 'rsb'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_rsb);}
            }
            case 'c': { // 'rsc'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_rsc);}
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'b': { // 'sb'
            if (mnemonic[2] == 'c') { // 'sbc'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_sbc);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 't': { // 'st'
            switch (mnemonic[2]) {
              case 'm': { // 'stm'
                {return new SgAsmArmInstruction(address,  mnemonic, arm_stm);}
              }
              case 'r': { // 'str'
                {return new SgAsmArmInstruction(address,  mnemonic, arm_str);}
              }
              default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_sub);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 'w': { // 'sw'
            switch (mnemonic[2]) {
              case 'i': { // 'swi'
                {return new SgAsmArmInstruction(address,  mnemonic, arm_swi);}
              }
              case 'p': { // 'swp'
                {return new SgAsmArmInstruction(address,  mnemonic, arm_swp);}
              }
              default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 't': { // 't'
        switch (mnemonic[1]) {
          case 'e': { // 'te'
            if (mnemonic[2] == 'q') { // 'teq'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_teq);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          case 's': { // 'ts'
            if (mnemonic[2] == 't') { // 'tst'
              {return new SgAsmArmInstruction(address,  mnemonic, arm_tst);}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
    }
  case 5:
    switch (mnemonic[0]) {
      case 's': { // 's'
        if (mnemonic[1] == 'm') { // 'sm'
          switch (mnemonic[2]) {
            case 'l': { // 'sml'
              if (mnemonic[3] == 'a') { // 'smla'
                if (mnemonic[4] == 'l') { // 'smlal'
                  {return new SgAsmArmInstruction(address,  mnemonic, arm_smlal);}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
              }
            }
            case 'u': { // 'smu'
              if (mnemonic[3] == 'l') { // 'smul'
                if (mnemonic[4] == 'l') { // 'smull'
                  {return new SgAsmArmInstruction(address,  mnemonic, arm_smull);}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
              }
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      case 'u': { // 'u'
        if (mnemonic[1] == 'm') { // 'um'
          switch (mnemonic[2]) {
            case 'l': { // 'uml'
              if (mnemonic[3] == 'a') { // 'umla'
                if (mnemonic[4] == 'l') { // 'umlal'
                  {return new SgAsmArmInstruction(address,  mnemonic, arm_umlal);}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
              }
            }
            case 'u': { // 'umu'
              if (mnemonic[3] == 'l') { // 'umul'
                if (mnemonic[4] == 'l') { // 'umull'
                  {return new SgAsmArmInstruction(address,  mnemonic, arm_umull);}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
              }
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
        }
      }
      default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
    }
  default: return new SgAsmArmInstruction(address,  mnemonic, arm_unknown_instruction);
}
}

#include "rose.h"
#include <stdint.h>
SgAsmx86Instruction* createx86Instruction(uint64_t address, const std::string& mnemonic) {
switch (mnemonic.size()) {
  case 2:
    switch (mnemonic[0]) {
      case 'b': { // 'b'
        if (mnemonic[1] == 't') { // 'bt'
          {return new SgAsmx86Bt(address,  mnemonic, "");}
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'n') { // 'in'
          {return new SgAsmx86In(address,  mnemonic, "");}
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'a': { // 'ja'
            {return new SgAsmx86Ja(address,  mnemonic, "");}
          }
          case 'b': { // 'jb'
            {return new SgAsmx86Jb(address,  mnemonic, "");}
          }
          case 'c': { // 'jc'
            {return new SgAsmx86Jc(address,  mnemonic, "");}
          }
          case 'e': { // 'je'
            {return new SgAsmx86Je(address,  mnemonic, "");}
          }
          case 'g': { // 'jg'
            {return new SgAsmx86Jg(address,  mnemonic, "");}
          }
          case 'l': { // 'jl'
            {return new SgAsmx86Jl(address,  mnemonic, "");}
          }
          case 'o': { // 'jo'
            {return new SgAsmx86Jo(address,  mnemonic, "");}
          }
          case 'p': { // 'jp'
            {return new SgAsmx86Jpe(address,  mnemonic, "");}
          }
          case 's': { // 'js'
            {return new SgAsmx86Js(address,  mnemonic, "");}
          }
          case 'z': { // 'jz'
            {return new SgAsmx86Je(address,  mnemonic, "");}
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'r') { // 'or'
          {return new SgAsmx86Or(address,  mnemonic, "");}
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, "");
    }
  case 3:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        switch (mnemonic[1]) {
          case 'a': { // 'aa'
            switch (mnemonic[2]) {
              case 'a': { // 'aaa'
                {return new SgAsmx86Aaa(address,  mnemonic, "");}
              }
              case 'd': { // 'aad'
                {return new SgAsmx86Aad(address,  mnemonic, "");}
              }
              case 'm': { // 'aam'
                {return new SgAsmx86Aam(address,  mnemonic, "");}
              }
              case 's': { // 'aas'
                {return new SgAsmx86Aas(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'd': { // 'ad'
            switch (mnemonic[2]) {
              case 'c': { // 'adc'
                {return new SgAsmx86Adc(address,  mnemonic, "");}
              }
              case 'd': { // 'add'
                {return new SgAsmx86Add(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              {return new SgAsmx86And(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'b': { // 'b'
        switch (mnemonic[1]) {
          case 's': { // 'bs'
            switch (mnemonic[2]) {
              case 'f': { // 'bsf'
                {return new SgAsmx86Bsf(address,  mnemonic, "");}
              }
              case 'r': { // 'bsr'
                {return new SgAsmx86Bsr(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 't': { // 'bt'
            switch (mnemonic[2]) {
              case 'c': { // 'btc'
                {return new SgAsmx86Btc(address,  mnemonic, "");}
              }
              case 'l': { // 'btl'
                {return new SgAsmx86Bt(address,  mnemonic, "");}
              }
              case 'r': { // 'btr'
                {return new SgAsmx86Btr(address,  mnemonic, "");}
              }
              case 's': { // 'bts'
                {return new SgAsmx86Bts(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'c': { // 'c'
        switch (mnemonic[1]) {
          case 'b': { // 'cb'
            if (mnemonic[2] == 'w') { // 'cbw'
              {return new SgAsmx86Cbw(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'd': { // 'cd'
            if (mnemonic[2] == 'q') { // 'cdq'
              {return new SgAsmx86Cwd(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'cl'
            switch (mnemonic[2]) {
              case 'c': { // 'clc'
                {return new SgAsmx86Clc(address,  mnemonic, "");}
              }
              case 'd': { // 'cld'
                {return new SgAsmx86Cld(address,  mnemonic, "");}
              }
              case 'i': { // 'cli'
                {return new SgAsmx86Cli(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'm': { // 'cm'
            switch (mnemonic[2]) {
              case 'c': { // 'cmc'
                {return new SgAsmx86Cmc(address,  mnemonic, "");}
              }
              case 'p': { // 'cmp'
                {return new SgAsmx86Cmp(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'q': { // 'cq'
            if (mnemonic[2] == 'o') { // 'cqo'
              {return new SgAsmx86Cwd(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'w': { // 'cw'
            if (mnemonic[2] == 'd') { // 'cwd'
              {return new SgAsmx86Cwd(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'd': { // 'd'
        switch (mnemonic[1]) {
          case 'a': { // 'da'
            switch (mnemonic[2]) {
              case 'a': { // 'daa'
                {return new SgAsmx86Daa(address,  mnemonic, "");}
              }
              case 's': { // 'das'
                {return new SgAsmx86Das(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'e': { // 'de'
            if (mnemonic[2] == 'c') { // 'dec'
              {return new SgAsmx86Dec(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'i': { // 'di'
            if (mnemonic[2] == 'v') { // 'div'
              {return new SgAsmx86Div(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 's') { // 'es'
          if (mnemonic[2] == 'c') { // 'esc'
            {return new SgAsmx86Esc(address,  mnemonic, "");}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              {return new SgAsmx86Fld(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'fs'
            if (mnemonic[2] == 't') { // 'fst'
              {return new SgAsmx86Fst(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'h': { // 'h'
        if (mnemonic[1] == 'l') { // 'hl'
          if (mnemonic[2] == 't') { // 'hlt'
            {return new SgAsmx86Hlt(address,  mnemonic, "");}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'n') { // 'in'
          switch (mnemonic[2]) {
            case 'b': { // 'inb'
              {return new SgAsmx86In(address,  mnemonic, "");}
            }
            case 'c': { // 'inc'
              {return new SgAsmx86Inc(address,  mnemonic, "");}
            }
            case 'd': { // 'ind'
              {return new SgAsmx86In(address,  mnemonic, "");}
            }
            case 'l': { // 'inl'
              {return new SgAsmx86In(address,  mnemonic, "");}
            }
            case 's': { // 'ins'
              {return new SgAsmx86Ins(address,  mnemonic, "");}
            }
            case 't': { // 'int'
              {return new SgAsmx86Int(address,  mnemonic, "");}
            }
            case 'w': { // 'inw'
              {return new SgAsmx86In(address,  mnemonic, "");}
            }
            default: return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'a': { // 'ja'
            if (mnemonic[2] == 'e') { // 'jae'
              {return new SgAsmx86Jae(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'b': { // 'jb'
            if (mnemonic[2] == 'e') { // 'jbe'
              {return new SgAsmx86Jbe(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'g': { // 'jg'
            if (mnemonic[2] == 'e') { // 'jge'
              {return new SgAsmx86Jge(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'jl'
            if (mnemonic[2] == 'e') { // 'jle'
              {return new SgAsmx86Jle(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'm': { // 'jm'
            if (mnemonic[2] == 'p') { // 'jmp'
              {return new SgAsmx86Jmp(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'jn'
            switch (mnemonic[2]) {
              case 'a': { // 'jna'
                {return new SgAsmx86Jbe(address,  mnemonic, "");}
              }
              case 'b': { // 'jnb'
                {return new SgAsmx86Jae(address,  mnemonic, "");}
              }
              case 'c': { // 'jnc'
                {return new SgAsmx86Jnc(address,  mnemonic, "");}
              }
              case 'e': { // 'jne'
                {return new SgAsmx86Jne(address,  mnemonic, "");}
              }
              case 'g': { // 'jng'
                {return new SgAsmx86Jle(address,  mnemonic, "");}
              }
              case 'l': { // 'jnl'
                {return new SgAsmx86Jge(address,  mnemonic, "");}
              }
              case 'o': { // 'jno'
                {return new SgAsmx86Jno(address,  mnemonic, "");}
              }
              case 'p': { // 'jnp'
                {return new SgAsmx86Jpo(address,  mnemonic, "");}
              }
              case 's': { // 'jns'
                {return new SgAsmx86Jns(address,  mnemonic, "");}
              }
              case 'z': { // 'jnz'
                {return new SgAsmx86Jne(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'p': { // 'jp'
            switch (mnemonic[2]) {
              case 'e': { // 'jpe'
                {return new SgAsmx86Jpe(address,  mnemonic, "");}
              }
              case 'o': { // 'jpo'
                {return new SgAsmx86Jpo(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'd': { // 'ld'
            if (mnemonic[2] == 's') { // 'lds'
              {return new SgAsmx86Lds(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'e': { // 'le'
            switch (mnemonic[2]) {
              case 'a': { // 'lea'
                {return new SgAsmx86Lea(address,  mnemonic, "");}
              }
              case 's': { // 'les'
                {return new SgAsmx86Les(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'f': { // 'lf'
            if (mnemonic[2] == 's') { // 'lfs'
              {return new SgAsmx86Lfs(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'g': { // 'lg'
            if (mnemonic[2] == 's') { // 'lgs'
              {return new SgAsmx86Lgs(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'ls'
            if (mnemonic[2] == 's') { // 'lss'
              {return new SgAsmx86Lss(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'm': { // 'm'
        switch (mnemonic[1]) {
          case 'o': { // 'mo'
            if (mnemonic[2] == 'v') { // 'mov'
              {return new SgAsmx86Mov(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'mu'
            if (mnemonic[2] == 'l') { // 'mul'
              {return new SgAsmx86Mul(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'n': { // 'n'
        switch (mnemonic[1]) {
          case 'e': { // 'ne'
            if (mnemonic[2] == 'g') { // 'neg'
              {return new SgAsmx86Neg(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'no'
            switch (mnemonic[2]) {
              case 'p': { // 'nop'
                {return new SgAsmx86Nop(address,  mnemonic, "");}
              }
              case 't': { // 'not'
                {return new SgAsmx86Not(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            {return new SgAsmx86Out(address,  mnemonic, "");}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'p': { // 'p'
        if (mnemonic[1] == 'o') { // 'po'
          if (mnemonic[2] == 'p') { // 'pop'
            {return new SgAsmx86Pop(address,  mnemonic, "");}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'r': { // 'r'
        switch (mnemonic[1]) {
          case 'c': { // 'rc'
            switch (mnemonic[2]) {
              case 'l': { // 'rcl'
                {return new SgAsmx86Rcl(address,  mnemonic, "");}
              }
              case 'r': { // 'rcr'
                {return new SgAsmx86Rcr(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'e': { // 're'
            switch (mnemonic[2]) {
              case 'p': { // 'rep'
                {return new SgAsmx86Rep(address,  mnemonic, "");}
              }
              case 't': { // 'ret'
                {return new SgAsmx86Ret(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'ro'
            switch (mnemonic[2]) {
              case 'l': { // 'rol'
                {return new SgAsmx86Rol(address,  mnemonic, "");}
              }
              case 'r': { // 'ror'
                {return new SgAsmx86Ror(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'a': { // 'sa'
            switch (mnemonic[2]) {
              case 'l': { // 'sal'
                {return new SgAsmx86Shl(address,  mnemonic, "");}
              }
              case 'r': { // 'sar'
                {return new SgAsmx86Sar(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'b': { // 'sb'
            if (mnemonic[2] == 'b') { // 'sbb'
              {return new SgAsmx86Sbb(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'h': { // 'sh'
            switch (mnemonic[2]) {
              case 'l': { // 'shl'
                {return new SgAsmx86Shl(address,  mnemonic, "");}
              }
              case 'r': { // 'shr'
                {return new SgAsmx86Shr(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 't': { // 'st'
            switch (mnemonic[2]) {
              case 'c': { // 'stc'
                {return new SgAsmx86Stc(address,  mnemonic, "");}
              }
              case 'd': { // 'std'
                {return new SgAsmx86Std(address,  mnemonic, "");}
              }
              case 'i': { // 'sti'
                {return new SgAsmx86Sti(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              {return new SgAsmx86Sub(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'u': { // 'u'
        if (mnemonic[1] == 'd') { // 'ud'
          if (mnemonic[2] == '2') { // 'ud2'
            {return new SgAsmx86Ud2(address,  mnemonic, "");}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'x': { // 'x'
        if (mnemonic[1] == 'o') { // 'xo'
          if (mnemonic[2] == 'r') { // 'xor'
            {return new SgAsmx86Xor(address,  mnemonic, "");}
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, "");
    }
  case 4:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        if (mnemonic[1] == 'r') { // 'ar'
          if (mnemonic[2] == 'p') { // 'arp'
            if (mnemonic[3] == 'l') { // 'arpl'
              {return new SgAsmx86Arpl(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'b': { // 'b'
        if (mnemonic[1] == 't') { // 'bt'
          switch (mnemonic[2]) {
            case 'c': { // 'btc'
              if (mnemonic[3] == 'l') { // 'btcl'
                {return new SgAsmx86Btc(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            }
            case 'r': { // 'btr'
              if (mnemonic[3] == 'l') { // 'btrl'
                {return new SgAsmx86Btr(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            }
            case 's': { // 'bts'
              if (mnemonic[3] == 'l') { // 'btsl'
                {return new SgAsmx86Bts(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            }
            default: return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'c': { // 'c'
        switch (mnemonic[1]) {
          case 'a': { // 'ca'
            if (mnemonic[2] == 'l') { // 'cal'
              if (mnemonic[3] == 'l') { // 'call'
                {return new SgAsmx86Call(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'd': { // 'cd'
            if (mnemonic[2] == 'q') { // 'cdq'
              if (mnemonic[3] == 'e') { // 'cdqe'
                {return new SgAsmx86Cbw(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'm': { // 'cm'
            if (mnemonic[2] == 'p') { // 'cmp'
              if (mnemonic[3] == 's') { // 'cmps'
                {return new SgAsmx86Cmps(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'w': { // 'cw'
            if (mnemonic[2] == 'd') { // 'cwd'
              if (mnemonic[3] == 'e') { // 'cwde'
                {return new SgAsmx86Cbw(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'm') { // 'em'
          if (mnemonic[2] == 'm') { // 'emm'
            if (mnemonic[3] == 's') { // 'emms'
              {return new SgAsmx86Emms(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case 'a': { // 'fa'
            switch (mnemonic[2]) {
              case 'b': { // 'fab'
                if (mnemonic[3] == 's') { // 'fabs'
                  {return new SgAsmx86Fabs(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'd': { // 'fad'
                if (mnemonic[3] == 'd') { // 'fadd'
                  {return new SgAsmx86Fadd(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'c': { // 'fc'
            switch (mnemonic[2]) {
              case 'h': { // 'fch'
                if (mnemonic[3] == 's') { // 'fchs'
                  {return new SgAsmx86Fchs(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'o': { // 'fco'
                switch (mnemonic[3]) {
                  case 'm': { // 'fcom'
                    {return new SgAsmx86Fcom(address,  mnemonic, "");}
                  }
                  case 's': { // 'fcos'
                    {return new SgAsmx86Fcos(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                {return new SgAsmx86Fdiv(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'l': { // 'fil'
                if (mnemonic[3] == 'd') { // 'fild'
                  {return new SgAsmx86Fild(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  {return new SgAsmx86Fist(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              switch (mnemonic[3]) {
                case '1': { // 'fld1'
                  {return new SgAsmx86Fld1(address,  mnemonic, "");}
                }
                case 'l': { // 'fldl'
                  {return new SgAsmx86Fld(address,  mnemonic, "");}
                }
                case 's': { // 'flds'
                  {return new SgAsmx86Fld(address,  mnemonic, "");}
                }
                case 't': { // 'fldt'
                  {return new SgAsmx86Fld(address,  mnemonic, "");}
                }
                case 'z': { // 'fldz'
                  {return new SgAsmx86Fldz(address,  mnemonic, "");}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'm': { // 'fm'
            if (mnemonic[2] == 'u') { // 'fmu'
              if (mnemonic[3] == 'l') { // 'fmul'
                {return new SgAsmx86Fmul(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'fn'
            if (mnemonic[2] == 'o') { // 'fno'
              if (mnemonic[3] == 'p') { // 'fnop'
                {return new SgAsmx86Fnop(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'i': { // 'fsi'
                if (mnemonic[3] == 'n') { // 'fsin'
                  {return new SgAsmx86Fsin(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 't': { // 'fst'
                switch (mnemonic[3]) {
                  case 'l': { // 'fstl'
                    {return new SgAsmx86Fst(address,  mnemonic, "");}
                  }
                  case 'p': { // 'fstp'
                    {return new SgAsmx86Fstp(address,  mnemonic, "");}
                  }
                  case 's': { // 'fsts'
                    {return new SgAsmx86Fst(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  {return new SgAsmx86Fsub(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 't': { // 'ft'
            if (mnemonic[2] == 's') { // 'fts'
              if (mnemonic[3] == 't') { // 'ftst'
                {return new SgAsmx86Ftst(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'x': { // 'fx'
            if (mnemonic[2] == 'c') { // 'fxc'
              if (mnemonic[3] == 'h') { // 'fxch'
                {return new SgAsmx86Fxch(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'i': { // 'i'
        switch (mnemonic[1]) {
          case 'd': { // 'id'
            if (mnemonic[2] == 'i') { // 'idi'
              if (mnemonic[3] == 'v') { // 'idiv'
                {return new SgAsmx86IDiv(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'm': { // 'im'
            if (mnemonic[2] == 'u') { // 'imu'
              if (mnemonic[3] == 'l') { // 'imul'
                {return new SgAsmx86IMul(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'in'
            switch (mnemonic[2]) {
              case 's': { // 'ins'
                switch (mnemonic[3]) {
                  case 'b': { // 'insb'
                    {return new SgAsmx86Ins(address,  mnemonic, "");}
                  }
                  case 'd': { // 'insd'
                    {return new SgAsmx86Ins(address,  mnemonic, "");}
                  }
                  case 'q': { // 'insq'
                    {return new SgAsmx86Ins(address,  mnemonic, "");}
                  }
                  case 'w': { // 'insw'
                    {return new SgAsmx86Ins(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 't': { // 'int'
                if (mnemonic[3] == 'o') { // 'into'
                  {return new SgAsmx86Into(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'r': { // 'ir'
            if (mnemonic[2] == 'e') { // 'ire'
              if (mnemonic[3] == 't') { // 'iret'
                {return new SgAsmx86IRet(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'c': { // 'jc'
            if (mnemonic[2] == 'x') { // 'jcx'
              if (mnemonic[3] == 'z') { // 'jcxz'
                {return new SgAsmx86Jcxz(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'jn'
            switch (mnemonic[2]) {
              case 'a': { // 'jna'
                if (mnemonic[3] == 'e') { // 'jnae'
                  {return new SgAsmx86Jb(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'b': { // 'jnb'
                if (mnemonic[3] == 'e') { // 'jnbe'
                  {return new SgAsmx86Ja(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'g': { // 'jng'
                if (mnemonic[3] == 'e') { // 'jnge'
                  {return new SgAsmx86Jl(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'l': { // 'jnl'
                if (mnemonic[3] == 'e') { // 'jnle'
                  {return new SgAsmx86Jg(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'a': { // 'la'
            if (mnemonic[2] == 'h') { // 'lah'
              if (mnemonic[3] == 'f') { // 'lahf'
                {return new SgAsmx86Lahf(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'lo'
            switch (mnemonic[2]) {
              case 'c': { // 'loc'
                if (mnemonic[3] == 'k') { // 'lock'
                  {return new SgAsmx86Lock(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'd': { // 'lod'
                if (mnemonic[3] == 's') { // 'lods'
                  {return new SgAsmx86Lods(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'o': { // 'loo'
                if (mnemonic[3] == 'p') { // 'loop'
                  {return new SgAsmx86Loop(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'm': { // 'm'
        if (mnemonic[1] == 'o') { // 'mo'
          if (mnemonic[2] == 'v') { // 'mov'
            if (mnemonic[3] == 's') { // 'movs'
              {return new SgAsmx86Movs(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            if (mnemonic[3] == 's') { // 'outs'
              {return new SgAsmx86Outs(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'p': { // 'p'
        switch (mnemonic[1]) {
          case 'a': { // 'pa'
            if (mnemonic[2] == 'n') { // 'pan'
              if (mnemonic[3] == 'd') { // 'pand'
                {return new SgAsmx86Pand(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'po'
            if (mnemonic[2] == 'p') { // 'pop'
              switch (mnemonic[3]) {
                case 'a': { // 'popa'
                  {return new SgAsmx86Popa(address,  mnemonic, "");}
                }
                case 'f': { // 'popf'
                  {return new SgAsmx86Popf(address,  mnemonic, "");}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'pu'
            if (mnemonic[2] == 's') { // 'pus'
              if (mnemonic[3] == 'h') { // 'push'
                {return new SgAsmx86Push(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'x': { // 'px'
            if (mnemonic[2] == 'o') { // 'pxo'
              if (mnemonic[3] == 'r') { // 'pxor'
                {return new SgAsmx86Pxor(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'r': { // 'r'
        if (mnemonic[1] == 'e') { // 're'
          if (mnemonic[2] == 'p') { // 'rep'
            switch (mnemonic[3]) {
              case 'e': { // 'repe'
                {return new SgAsmx86Repe(address,  mnemonic, "");}
              }
              case 'z': { // 'repz'
                {return new SgAsmx86Repe(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'a': { // 'sa'
            if (mnemonic[2] == 'h') { // 'sah'
              if (mnemonic[3] == 'f') { // 'sahf'
                {return new SgAsmx86Sahf(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'c': { // 'sc'
            if (mnemonic[2] == 'a') { // 'sca'
              if (mnemonic[3] == 's') { // 'scas'
                {return new SgAsmx86Scas(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'e': { // 'se'
            if (mnemonic[2] == 't') { // 'set'
              switch (mnemonic[3]) {
                case 'a': { // 'seta'
                  {return new SgAsmx86Seta(address,  mnemonic, "");}
                }
                case 'b': { // 'setb'
                  {return new SgAsmx86Setb(address,  mnemonic, "");}
                }
                case 'c': { // 'setc'
                  {return new SgAsmx86Setb(address,  mnemonic, "");}
                }
                case 'e': { // 'sete'
                  {return new SgAsmx86Sete(address,  mnemonic, "");}
                }
                case 'g': { // 'setg'
                  {return new SgAsmx86Setg(address,  mnemonic, "");}
                }
                case 'l': { // 'setl'
                  {return new SgAsmx86Setl(address,  mnemonic, "");}
                }
                case 'o': { // 'seto'
                  {return new SgAsmx86Seto(address,  mnemonic, "");}
                }
                case 'p': { // 'setp'
                  {return new SgAsmx86Setpe(address,  mnemonic, "");}
                }
                case 's': { // 'sets'
                  {return new SgAsmx86Sets(address,  mnemonic, "");}
                }
                case 'z': { // 'setz'
                  {return new SgAsmx86Sete(address,  mnemonic, "");}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'h': { // 'sh'
            switch (mnemonic[2]) {
              case 'l': { // 'shl'
                if (mnemonic[3] == 'd') { // 'shld'
                  {return new SgAsmx86Shld(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'r': { // 'shr'
                if (mnemonic[3] == 'd') { // 'shrd'
                  {return new SgAsmx86Shrd(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'sl'
            if (mnemonic[2] == 'd') { // 'sld'
              if (mnemonic[3] == 't') { // 'sldt'
                {return new SgAsmx86Sldt(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 't': { // 'st'
            if (mnemonic[2] == 'o') { // 'sto'
              if (mnemonic[3] == 's') { // 'stos'
                {return new SgAsmx86Stos(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 't': { // 't'
        if (mnemonic[1] == 'e') { // 'te'
          if (mnemonic[2] == 's') { // 'tes'
            if (mnemonic[3] == 't') { // 'test'
              {return new SgAsmx86Test(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'v': { // 'v'
        if (mnemonic[1] == 'e') { // 've'
          if (mnemonic[2] == 'r') { // 'ver'
            switch (mnemonic[3]) {
              case 'r': { // 'verr'
                {return new SgAsmx86Verr(address,  mnemonic, "");}
              }
              case 'w': { // 'verw'
                {return new SgAsmx86Verw(address,  mnemonic, "");}
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'w': { // 'w'
        if (mnemonic[1] == 'a') { // 'wa'
          if (mnemonic[2] == 'i') { // 'wai'
            if (mnemonic[3] == 't') { // 'wait'
              {return new SgAsmx86Wait(address,  mnemonic, "");}
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'x': { // 'x'
        switch (mnemonic[1]) {
          case 'a': { // 'xa'
            if (mnemonic[2] == 'd') { // 'xad'
              if (mnemonic[3] == 'd') { // 'xadd'
                {return new SgAsmx86Xadd(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'c': { // 'xc'
            if (mnemonic[2] == 'h') { // 'xch'
              if (mnemonic[3] == 'g') { // 'xchg'
                {return new SgAsmx86Xchg(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'xl'
            if (mnemonic[2] == 'a') { // 'xla'
              if (mnemonic[3] == 't') { // 'xlat'
                {return new SgAsmx86Xlat(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, "");
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
                    {return new SgAsmx86Addsd(address,  mnemonic, "");}
                  }
                  case 's': { // 'addss'
                    {return new SgAsmx86Addss(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              if (mnemonic[3] == 'p') { // 'andp'
                if (mnemonic[4] == 'd') { // 'andpd'
                  {return new SgAsmx86Andpd(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'b': { // 'b'
        switch (mnemonic[1]) {
          case 'o': { // 'bo'
            if (mnemonic[2] == 'u') { // 'bou'
              if (mnemonic[3] == 'n') { // 'boun'
                if (mnemonic[4] == 'd') { // 'bound'
                  {return new SgAsmx86Bound(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'bs'
            if (mnemonic[2] == 'w') { // 'bsw'
              if (mnemonic[3] == 'a') { // 'bswa'
                if (mnemonic[4] == 'p') { // 'bswap'
                  {return new SgAsmx86Bswap(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
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
                      {return new SgAsmx86CMova(address,  mnemonic, "");}
                    }
                    case 'b': { // 'cmovb'
                      {return new SgAsmx86CMovb(address,  mnemonic, "");}
                    }
                    case 'c': { // 'cmovc'
                      {return new SgAsmx86CMovc(address,  mnemonic, "");}
                    }
                    case 'e': { // 'cmove'
                      {return new SgAsmx86CMove(address,  mnemonic, "");}
                    }
                    case 'g': { // 'cmovg'
                      {return new SgAsmx86CMovg(address,  mnemonic, "");}
                    }
                    case 'l': { // 'cmovl'
                      {return new SgAsmx86CMovl(address,  mnemonic, "");}
                    }
                    case 'o': { // 'cmovo'
                      {return new SgAsmx86CMovo(address,  mnemonic, "");}
                    }
                    case 'p': { // 'cmovp'
                      {return new SgAsmx86CMovp(address,  mnemonic, "");}
                    }
                    case 's': { // 'cmovs'
                      {return new SgAsmx86CMovs(address,  mnemonic, "");}
                    }
                    case 'z': { // 'cmovz'
                      {return new SgAsmx86CMove(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'p': { // 'cmp'
                if (mnemonic[3] == 's') { // 'cmps'
                  switch (mnemonic[4]) {
                    case 'b': { // 'cmpsb'
                      {return new SgAsmx86Cmps(address,  mnemonic, "");}
                    }
                    case 'd': { // 'cmpsd'
                      {return new SgAsmx86Cmps(address,  mnemonic, "");}
                    }
                    case 'q': { // 'cmpsq'
                      {return new SgAsmx86Cmps(address,  mnemonic, "");}
                    }
                    case 'w': { // 'cmpsw'
                      {return new SgAsmx86Cmps(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'p': { // 'cp'
            if (mnemonic[2] == 'u') { // 'cpu'
              if (mnemonic[3] == 'i') { // 'cpui'
                if (mnemonic[4] == 'd') { // 'cpuid'
                  {return new SgAsmx86Cpuid(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'd': { // 'd'
        if (mnemonic[1] == 'i') { // 'di'
          if (mnemonic[2] == 'v') { // 'div'
            if (mnemonic[3] == 's') { // 'divs'
              if (mnemonic[4] == 'd') { // 'divsd'
                {return new SgAsmx86Divsd(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'n') { // 'en'
          if (mnemonic[2] == 't') { // 'ent'
            if (mnemonic[3] == 'e') { // 'ente'
              if (mnemonic[4] == 'r') { // 'enter'
                {return new SgAsmx86Enter(address,  mnemonic, "");}
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case '2': { // 'f2'
            if (mnemonic[2] == 'x') { // 'f2x'
              if (mnemonic[3] == 'm') { // 'f2xm'
                if (mnemonic[4] == '1') { // 'f2xm1'
                  {return new SgAsmx86F2xm1(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'a': { // 'fa'
            if (mnemonic[2] == 'd') { // 'fad'
              if (mnemonic[3] == 'd') { // 'fadd'
                switch (mnemonic[4]) {
                  case 'l': { // 'faddl'
                    {return new SgAsmx86Fadd(address,  mnemonic, "");}
                  }
                  case 'p': { // 'faddp'
                    {return new SgAsmx86Faddp(address,  mnemonic, "");}
                  }
                  case 's': { // 'fadds'
                    {return new SgAsmx86Fadd(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'c': { // 'fc'
            if (mnemonic[2] == 'o') { // 'fco'
              if (mnemonic[3] == 'm') { // 'fcom'
                switch (mnemonic[4]) {
                  case 'l': { // 'fcoml'
                    {return new SgAsmx86Fcom(address,  mnemonic, "");}
                  }
                  case 'p': { // 'fcomp'
                    {return new SgAsmx86Fcomp(address,  mnemonic, "");}
                  }
                  case 's': { // 'fcoms'
                    {return new SgAsmx86Fcom(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                switch (mnemonic[4]) {
                  case 'l': { // 'fdivl'
                    {return new SgAsmx86Fdiv(address,  mnemonic, "");}
                  }
                  case 'p': { // 'fdivp'
                    {return new SgAsmx86Fdivp(address,  mnemonic, "");}
                  }
                  case 'r': { // 'fdivr'
                    {return new SgAsmx86Fdivr(address,  mnemonic, "");}
                  }
                  case 's': { // 'fdivs'
                    {return new SgAsmx86Fdiv(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'f': { // 'ff'
            if (mnemonic[2] == 'r') { // 'ffr'
              if (mnemonic[3] == 'e') { // 'ffre'
                if (mnemonic[4] == 'e') { // 'ffree'
                  {return new SgAsmx86Ffree(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'a': { // 'fia'
                if (mnemonic[3] == 'd') { // 'fiad'
                  if (mnemonic[4] == 'd') { // 'fiadd'
                    {return new SgAsmx86Fiadd(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'd': { // 'fid'
                if (mnemonic[3] == 'i') { // 'fidi'
                  if (mnemonic[4] == 'v') { // 'fidiv'
                    {return new SgAsmx86Fidiv(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'l': { // 'fil'
                if (mnemonic[3] == 'd') { // 'fild'
                  if (mnemonic[4] == 'l') { // 'fildl'
                    {return new SgAsmx86Fild(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'm': { // 'fim'
                if (mnemonic[3] == 'u') { // 'fimu'
                  if (mnemonic[4] == 'l') { // 'fimul'
                    {return new SgAsmx86Fimul(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fistl'
                      {return new SgAsmx86Fist(address,  mnemonic, "");}
                    }
                    case 'p': { // 'fistp'
                      {return new SgAsmx86Fistp(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              switch (mnemonic[3]) {
                case 'c': { // 'fldc'
                  if (mnemonic[4] == 'w') { // 'fldcw'
                    {return new SgAsmx86Fldcw(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'p': { // 'fldp'
                  if (mnemonic[4] == 'i') { // 'fldpi'
                    {return new SgAsmx86Fldpi(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'm': { // 'fm'
            if (mnemonic[2] == 'u') { // 'fmu'
              if (mnemonic[3] == 'l') { // 'fmul'
                switch (mnemonic[4]) {
                  case 'l': { // 'fmull'
                    {return new SgAsmx86Fmul(address,  mnemonic, "");}
                  }
                  case 'p': { // 'fmulp'
                    {return new SgAsmx86Fmulp(address,  mnemonic, "");}
                  }
                  case 's': { // 'fmuls'
                    {return new SgAsmx86Fmul(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'p': { // 'fp'
            switch (mnemonic[2]) {
              case 'r': { // 'fpr'
                if (mnemonic[3] == 'e') { // 'fpre'
                  if (mnemonic[4] == 'm') { // 'fprem'
                    {return new SgAsmx86Fprem(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 't': { // 'fpt'
                if (mnemonic[3] == 'a') { // 'fpta'
                  if (mnemonic[4] == 'n') { // 'fptan'
                    {return new SgAsmx86Fptan(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'a': { // 'fsa'
                if (mnemonic[3] == 'v') { // 'fsav'
                  if (mnemonic[4] == 'e') { // 'fsave'
                    {return new SgAsmx86Fsave(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'q': { // 'fsq'
                if (mnemonic[3] == 'r') { // 'fsqr'
                  if (mnemonic[4] == 't') { // 'fsqrt'
                    {return new SgAsmx86Fsqrt(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 't': { // 'fst'
                if (mnemonic[3] == 'p') { // 'fstp'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fstpl'
                      {return new SgAsmx86Fstp(address,  mnemonic, "");}
                    }
                    case 's': { // 'fstps'
                      {return new SgAsmx86Fstp(address,  mnemonic, "");}
                    }
                    case 't': { // 'fstpt'
                      {return new SgAsmx86Fstp(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fsubl'
                      {return new SgAsmx86Fsub(address,  mnemonic, "");}
                    }
                    case 'p': { // 'fsubp'
                      {return new SgAsmx86Fsubp(address,  mnemonic, "");}
                    }
                    case 'r': { // 'fsubr'
                      {return new SgAsmx86Fsubr(address,  mnemonic, "");}
                    }
                    case 's': { // 'fsubs'
                      {return new SgAsmx86Fsub(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'fu'
            if (mnemonic[2] == 'c') { // 'fuc'
              if (mnemonic[3] == 'o') { // 'fuco'
                if (mnemonic[4] == 'm') { // 'fucom'
                  {return new SgAsmx86Fucom(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'w': { // 'fw'
            if (mnemonic[2] == 'a') { // 'fwa'
              if (mnemonic[3] == 'i') { // 'fwai'
                if (mnemonic[4] == 't') { // 'fwait'
                  {return new SgAsmx86Fwait(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'y': { // 'fy'
            if (mnemonic[2] == 'l') { // 'fyl'
              if (mnemonic[3] == '2') { // 'fyl2'
                if (mnemonic[4] == 'x') { // 'fyl2x'
                  {return new SgAsmx86Fyl2x(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'r') { // 'ir'
          if (mnemonic[2] == 'e') { // 'ire'
            if (mnemonic[3] == 't') { // 'iret'
              switch (mnemonic[4]) {
                case 'd': { // 'iretd'
                  {return new SgAsmx86IRet(address,  mnemonic, "");}
                }
                case 'l': { // 'iretl'
                  {return new SgAsmx86IRet(address,  mnemonic, "");}
                }
                case 'q': { // 'iretq'
                  {return new SgAsmx86IRet(address,  mnemonic, "");}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'e': { // 'je'
            if (mnemonic[2] == 'c') { // 'jec'
              if (mnemonic[3] == 'x') { // 'jecx'
                if (mnemonic[4] == 'z') { // 'jecxz'
                  {return new SgAsmx86Jcxz(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'r': { // 'jr'
            if (mnemonic[2] == 'c') { // 'jrc'
              if (mnemonic[3] == 'x') { // 'jrcx'
                if (mnemonic[4] == 'z') { // 'jrcxz'
                  {return new SgAsmx86Jcxz(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'e': { // 'le'
            if (mnemonic[2] == 'a') { // 'lea'
              if (mnemonic[3] == 'v') { // 'leav'
                if (mnemonic[4] == 'e') { // 'leave'
                  {return new SgAsmx86Leave(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'lo'
            switch (mnemonic[2]) {
              case 'd': { // 'lod'
                if (mnemonic[3] == 's') { // 'lods'
                  switch (mnemonic[4]) {
                    case 'b': { // 'lodsb'
                      {return new SgAsmx86Lods(address,  mnemonic, "");}
                    }
                    case 'd': { // 'lodsd'
                      {return new SgAsmx86Lods(address,  mnemonic, "");}
                    }
                    case 'q': { // 'lodsq'
                      {return new SgAsmx86Lods(address,  mnemonic, "");}
                    }
                    case 'w': { // 'lodsw'
                      {return new SgAsmx86Lods(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'o': { // 'loo'
                if (mnemonic[3] == 'p') { // 'loop'
                  switch (mnemonic[4]) {
                    case 'e': { // 'loope'
                      {return new SgAsmx86Loopz(address,  mnemonic, "");}
                    }
                    case 'l': { // 'loopl'
                      {return new SgAsmx86Loop(address,  mnemonic, "");}
                    }
                    case 'z': { // 'loopz'
                      {return new SgAsmx86Loopz(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'm': { // 'm'
        if (mnemonic[1] == 'o') { // 'mo'
          if (mnemonic[2] == 'v') { // 'mov'
            switch (mnemonic[3]) {
              case 's': { // 'movs'
                switch (mnemonic[4]) {
                  case 'b': { // 'movsb'
                    {return new SgAsmx86Movs(address,  mnemonic, "");}
                  }
                  case 'd': { // 'movsd'
                    {return new SgAsmx86Movs(address,  mnemonic, "");}
                  }
                  case 'q': { // 'movsq'
                    {return new SgAsmx86Movs(address,  mnemonic, "");}
                  }
                  case 's': { // 'movss'
                    {return new SgAsmx86Movss(address,  mnemonic, "");}
                  }
                  case 'w': { // 'movsw'
                    {return new SgAsmx86Movs(address,  mnemonic, "");}
                  }
                  case 'x': { // 'movsx'
                    {return new SgAsmx86Movsx(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'z': { // 'movz'
                if (mnemonic[4] == 'x') { // 'movzx'
                  {return new SgAsmx86Movzx(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            if (mnemonic[3] == 's') { // 'outs'
              switch (mnemonic[4]) {
                case 'b': { // 'outsb'
                  {return new SgAsmx86Outs(address,  mnemonic, "");}
                }
                case 'd': { // 'outsd'
                  {return new SgAsmx86Outs(address,  mnemonic, "");}
                }
                case 'q': { // 'outsq'
                  {return new SgAsmx86Outs(address,  mnemonic, "");}
                }
                case 'w': { // 'outsw'
                  {return new SgAsmx86Outs(address,  mnemonic, "");}
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
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
                      {return new SgAsmx86Padd(address,  mnemonic, "");}
                    }
                    case 'd': { // 'paddd'
                      {return new SgAsmx86Padd(address,  mnemonic, "");}
                    }
                    case 'q': { // 'paddq'
                      {return new SgAsmx86Padd(address,  mnemonic, "");}
                    }
                    case 'w': { // 'paddw'
                      {return new SgAsmx86Padd(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'n': { // 'pan'
                if (mnemonic[3] == 'd') { // 'pand'
                  if (mnemonic[4] == 'n') { // 'pandn'
                    {return new SgAsmx86Pandn(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'u': { // 'pau'
                if (mnemonic[3] == 's') { // 'paus'
                  if (mnemonic[4] == 'e') { // 'pause'
                    {return new SgAsmx86Pause(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'po'
            if (mnemonic[2] == 'p') { // 'pop'
              switch (mnemonic[3]) {
                case 'a': { // 'popa'
                  if (mnemonic[4] == 'd') { // 'popad'
                    {return new SgAsmx86Popa(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'f': { // 'popf'
                  switch (mnemonic[4]) {
                    case 'd': { // 'popfd'
                      {return new SgAsmx86Popf(address,  mnemonic, "");}
                    }
                    case 'q': { // 'popfq'
                      {return new SgAsmx86Popf(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'ps'
            switch (mnemonic[2]) {
              case 'l': { // 'psl'
                if (mnemonic[3] == 'l') { // 'psll'
                  switch (mnemonic[4]) {
                    case 'd': { // 'pslld'
                      {return new SgAsmx86Psll(address,  mnemonic, "");}
                    }
                    case 'q': { // 'psllq'
                      {return new SgAsmx86Psll(address,  mnemonic, "");}
                    }
                    case 'w': { // 'psllw'
                      {return new SgAsmx86Psll(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'r': { // 'psr'
                if (mnemonic[3] == 'l') { // 'psrl'
                  switch (mnemonic[4]) {
                    case 'd': { // 'psrld'
                      {return new SgAsmx86Psrl(address,  mnemonic, "");}
                    }
                    case 'q': { // 'psrlq'
                      {return new SgAsmx86Psrl(address,  mnemonic, "");}
                    }
                    case 'w': { // 'psrlw'
                      {return new SgAsmx86Psrl(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'u': { // 'psu'
                if (mnemonic[3] == 'b') { // 'psub'
                  switch (mnemonic[4]) {
                    case 'b': { // 'psubb'
                      {return new SgAsmx86Psub(address,  mnemonic, "");}
                    }
                    case 'd': { // 'psubd'
                      {return new SgAsmx86Psub(address,  mnemonic, "");}
                    }
                    case 'w': { // 'psubw'
                      {return new SgAsmx86Psub(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'pu'
            if (mnemonic[2] == 's') { // 'pus'
              if (mnemonic[3] == 'h') { // 'push'
                switch (mnemonic[4]) {
                  case 'a': { // 'pusha'
                    {return new SgAsmx86Pusha(address,  mnemonic, "");}
                  }
                  case 'f': { // 'pushf'
                    {return new SgAsmx86Pushf(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'r': { // 'r'
        switch (mnemonic[1]) {
          case 'd': { // 'rd'
            if (mnemonic[2] == 't') { // 'rdt'
              if (mnemonic[3] == 's') { // 'rdts'
                if (mnemonic[4] == 'c') { // 'rdtsc'
                  {return new SgAsmx86Rdtsc(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'e': { // 're'
            if (mnemonic[2] == 'p') { // 'rep'
              if (mnemonic[3] == 'n') { // 'repn'
                switch (mnemonic[4]) {
                  case 'e': { // 'repne'
                    {return new SgAsmx86Repne(address,  mnemonic, "");}
                  }
                  case 'z': { // 'repnz'
                    {return new SgAsmx86Repne(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'c': { // 'sc'
            if (mnemonic[2] == 'a') { // 'sca'
              if (mnemonic[3] == 's') { // 'scas'
                switch (mnemonic[4]) {
                  case 'b': { // 'scasb'
                    {return new SgAsmx86Scas(address,  mnemonic, "");}
                  }
                  case 'd': { // 'scasd'
                    {return new SgAsmx86Scas(address,  mnemonic, "");}
                  }
                  case 'q': { // 'scasq'
                    {return new SgAsmx86Scas(address,  mnemonic, "");}
                  }
                  case 'w': { // 'scasw'
                    {return new SgAsmx86Scas(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'e': { // 'se'
            if (mnemonic[2] == 't') { // 'set'
              switch (mnemonic[3]) {
                case 'a': { // 'seta'
                  if (mnemonic[4] == 'e') { // 'setae'
                    {return new SgAsmx86Setb(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'b': { // 'setb'
                  if (mnemonic[4] == 'e') { // 'setbe'
                    {return new SgAsmx86Setbe(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'g': { // 'setg'
                  if (mnemonic[4] == 'e') { // 'setge'
                    {return new SgAsmx86Setge(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'l': { // 'setl'
                  if (mnemonic[4] == 'e') { // 'setle'
                    {return new SgAsmx86Setle(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'n': { // 'setn'
                  switch (mnemonic[4]) {
                    case 'a': { // 'setna'
                      {return new SgAsmx86Setbe(address,  mnemonic, "");}
                    }
                    case 'b': { // 'setnb'
                      {return new SgAsmx86Setae(address,  mnemonic, "");}
                    }
                    case 'c': { // 'setnc'
                      {return new SgAsmx86Setae(address,  mnemonic, "");}
                    }
                    case 'e': { // 'setne'
                      {return new SgAsmx86Setne(address,  mnemonic, "");}
                    }
                    case 'g': { // 'setng'
                      {return new SgAsmx86Setle(address,  mnemonic, "");}
                    }
                    case 'l': { // 'setnl'
                      {return new SgAsmx86Setge(address,  mnemonic, "");}
                    }
                    case 'o': { // 'setno'
                      {return new SgAsmx86Setno(address,  mnemonic, "");}
                    }
                    case 'p': { // 'setnp'
                      {return new SgAsmx86Setpo(address,  mnemonic, "");}
                    }
                    case 's': { // 'setns'
                      {return new SgAsmx86Setns(address,  mnemonic, "");}
                    }
                    case 'z': { // 'setnz'
                      {return new SgAsmx86Setne(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'p': { // 'setp'
                  switch (mnemonic[4]) {
                    case 'e': { // 'setpe'
                      {return new SgAsmx86Setpe(address,  mnemonic, "");}
                    }
                    case 'o': { // 'setpo'
                      {return new SgAsmx86Setpo(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'sl'
            if (mnemonic[2] == 'd') { // 'sld'
              if (mnemonic[3] == 't') { // 'sldt'
                if (mnemonic[4] == 'l') { // 'sldtl'
                  {return new SgAsmx86Sldt(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 't': { // 'st'
            if (mnemonic[2] == 'o') { // 'sto'
              if (mnemonic[3] == 's') { // 'stos'
                switch (mnemonic[4]) {
                  case 'b': { // 'stosb'
                    {return new SgAsmx86Stos(address,  mnemonic, "");}
                  }
                  case 'd': { // 'stosd'
                    {return new SgAsmx86Stos(address,  mnemonic, "");}
                  }
                  case 'q': { // 'stosq'
                    {return new SgAsmx86Stos(address,  mnemonic, "");}
                  }
                  case 'w': { // 'stosw'
                    {return new SgAsmx86Stos(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              if (mnemonic[3] == 's') { // 'subs'
                switch (mnemonic[4]) {
                  case 'd': { // 'subsd'
                    {return new SgAsmx86Subsd(address,  mnemonic, "");}
                  }
                  case 's': { // 'subss'
                    {return new SgAsmx86Subss(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'x': { // 'x'
        switch (mnemonic[1]) {
          case 'a': { // 'xa'
            if (mnemonic[2] == 'd') { // 'xad'
              if (mnemonic[3] == 'd') { // 'xadd'
                if (mnemonic[4] == 'l') { // 'xaddl'
                  {return new SgAsmx86Xadd(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'xl'
            if (mnemonic[2] == 'a') { // 'xla'
              if (mnemonic[3] == 't') { // 'xlat'
                if (mnemonic[4] == 'b') { // 'xlatb'
                  {return new SgAsmx86Xlat(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'xo'
            if (mnemonic[2] == 'r') { // 'xor'
              if (mnemonic[3] == 'p') { // 'xorp'
                switch (mnemonic[4]) {
                  case 'd': { // 'xorpd'
                    {return new SgAsmx86Xorpd(address,  mnemonic, "");}
                  }
                  case 's': { // 'xorps'
                    {return new SgAsmx86Xorps(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, "");
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
                    {return new SgAsmx86Andnpd(address,  mnemonic, "");}
                  }
                  case 's': { // 'andnps'
                    {return new SgAsmx86Andnpd(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'b': { // 'b'
        if (mnemonic[1] == 'o') { // 'bo'
          if (mnemonic[2] == 'u') { // 'bou'
            if (mnemonic[3] == 'n') { // 'boun'
              if (mnemonic[4] == 'd') { // 'bound'
                if (mnemonic[5] == 'l') { // 'boundl'
                  {return new SgAsmx86Bound(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'c': { // 'c'
        if (mnemonic[1] == 'm') { // 'cm'
          if (mnemonic[2] == 'o') { // 'cmo'
            if (mnemonic[3] == 'v') { // 'cmov'
              switch (mnemonic[4]) {
                case 'a': { // 'cmova'
                  if (mnemonic[5] == 'e') { // 'cmovae'
                    {return new SgAsmx86CMovae(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'b': { // 'cmovb'
                  if (mnemonic[5] == 'e') { // 'cmovbe'
                    {return new SgAsmx86CMovbe(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'g': { // 'cmovg'
                  if (mnemonic[5] == 'e') { // 'cmovge'
                    {return new SgAsmx86CMovge(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'l': { // 'cmovl'
                  if (mnemonic[5] == 'e') { // 'cmovle'
                    {return new SgAsmx86CMovle(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'n': { // 'cmovn'
                  switch (mnemonic[5]) {
                    case 'a': { // 'cmovna'
                      {return new SgAsmx86CMovbe(address,  mnemonic, "");}
                    }
                    case 'b': { // 'cmovnb'
                      {return new SgAsmx86CMovae(address,  mnemonic, "");}
                    }
                    case 'c': { // 'cmovnc'
                      {return new SgAsmx86CMovnc(address,  mnemonic, "");}
                    }
                    case 'e': { // 'cmovne'
                      {return new SgAsmx86CMovne(address,  mnemonic, "");}
                    }
                    case 'g': { // 'cmovng'
                      {return new SgAsmx86CMovle(address,  mnemonic, "");}
                    }
                    case 'l': { // 'cmovnl'
                      {return new SgAsmx86CMovge(address,  mnemonic, "");}
                    }
                    case 'o': { // 'cmovno'
                      {return new SgAsmx86CMovno(address,  mnemonic, "");}
                    }
                    case 'p': { // 'cmovnp'
                      {return new SgAsmx86CMovnp(address,  mnemonic, "");}
                    }
                    case 's': { // 'cmovns'
                      {return new SgAsmx86CMovns(address,  mnemonic, "");}
                    }
                    case 'z': { // 'cmovnz'
                      {return new SgAsmx86CMovne(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'p': { // 'cmovp'
                  switch (mnemonic[5]) {
                    case 'e': { // 'cmovpe'
                      {return new SgAsmx86CMovp(address,  mnemonic, "");}
                    }
                    case 'o': { // 'cmovpo'
                      {return new SgAsmx86CMovnp(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'd': { // 'd'
        if (mnemonic[1] == 'a') { // 'da'
          if (mnemonic[2] == 't') { // 'dat'
            if (mnemonic[3] == 'a') { // 'data'
              if (mnemonic[4] == '1') { // 'data1'
                if (mnemonic[5] == '6') { // 'data16'
                  {return new SgAsmx86Data16(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'n') { // 'en'
          if (mnemonic[2] == 't') { // 'ent'
            if (mnemonic[3] == 'e') { // 'ente'
              if (mnemonic[4] == 'r') { // 'enter'
                if (mnemonic[5] == 'l') { // 'enterl'
                  {return new SgAsmx86Enter(address,  mnemonic, "");}
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
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
                      {return new SgAsmx86Fcomp(address,  mnemonic, "");}
                    }
                    case 'p': { // 'fcompp'
                      {return new SgAsmx86Fcompp(address,  mnemonic, "");}
                    }
                    case 's': { // 'fcomps'
                      {return new SgAsmx86Fcomp(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                if (mnemonic[4] == 'r') { // 'fdivr'
                  switch (mnemonic[5]) {
                    case 'l': { // 'fdivrl'
                      {return new SgAsmx86Fdivr(address,  mnemonic, "");}
                    }
                    case 'p': { // 'fdivrp'
                      {return new SgAsmx86Fdivrp(address,  mnemonic, "");}
                    }
                    case 's': { // 'fdivrs'
                      {return new SgAsmx86Fdivr(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'a': { // 'fia'
                if (mnemonic[3] == 'd') { // 'fiad'
                  if (mnemonic[4] == 'd') { // 'fiadd'
                    if (mnemonic[5] == 'l') { // 'fiaddl'
                      {return new SgAsmx86Fiadd(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'd': { // 'fid'
                if (mnemonic[3] == 'i') { // 'fidi'
                  if (mnemonic[4] == 'v') { // 'fidiv'
                    if (mnemonic[5] == 'l') { // 'fidivl'
                      {return new SgAsmx86Fidiv(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'm': { // 'fim'
                if (mnemonic[3] == 'u') { // 'fimu'
                  if (mnemonic[4] == 'l') { // 'fimul'
                    if (mnemonic[5] == 'l') { // 'fimull'
                      {return new SgAsmx86Fimul(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  if (mnemonic[4] == 'p') { // 'fistp'
                    if (mnemonic[5] == 'l') { // 'fistpl'
                      {return new SgAsmx86Fistp(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              if (mnemonic[3] == 'l') { // 'fldl'
                switch (mnemonic[4]) {
                  case '2': { // 'fldl2'
                    switch (mnemonic[5]) {
                      case 'e': { // 'fldl2e'
                        {return new SgAsmx86Fldl2e(address,  mnemonic, "");}
                      }
                      case 't': { // 'fldl2t'
                        {return new SgAsmx86Fldl2t(address,  mnemonic, "");}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  }
                  case 'g': { // 'fldlg'
                    if (mnemonic[5] == '2') { // 'fldlg2'
                      {return new SgAsmx86Fldlg2(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  }
                  case 'n': { // 'fldln'
                    if (mnemonic[5] == '2') { // 'fldln2'
                      {return new SgAsmx86Fldln2(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'fn'
            if (mnemonic[2] == 's') { // 'fns'
              if (mnemonic[3] == 't') { // 'fnst'
                switch (mnemonic[4]) {
                  case 'c': { // 'fnstc'
                    if (mnemonic[5] == 'w') { // 'fnstcw'
                      {return new SgAsmx86Fnstcw(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  }
                  case 's': { // 'fnsts'
                    if (mnemonic[5] == 'w') { // 'fnstsw'
                      {return new SgAsmx86Fnstsw(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'p': { // 'fp'
            if (mnemonic[2] == 'a') { // 'fpa'
              if (mnemonic[3] == 't') { // 'fpat'
                if (mnemonic[4] == 'a') { // 'fpata'
                  if (mnemonic[5] == 'n') { // 'fpatan'
                    {return new SgAsmx86Fpatan(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'c': { // 'fsc'
                if (mnemonic[3] == 'a') { // 'fsca'
                  if (mnemonic[4] == 'l') { // 'fscal'
                    if (mnemonic[5] == 'e') { // 'fscale'
                      {return new SgAsmx86Fscale(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  if (mnemonic[4] == 'r') { // 'fsubr'
                    switch (mnemonic[5]) {
                      case 'l': { // 'fsubrl'
                        {return new SgAsmx86Fsubr(address,  mnemonic, "");}
                      }
                      case 'p': { // 'fsubrp'
                        {return new SgAsmx86Fsubrp(address,  mnemonic, "");}
                      }
                      case 's': { // 'fsubrs'
                        {return new SgAsmx86Fsubr(address,  mnemonic, "");}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'fu'
            if (mnemonic[2] == 'c') { // 'fuc'
              if (mnemonic[3] == 'o') { // 'fuco'
                if (mnemonic[4] == 'm') { // 'fucom'
                  switch (mnemonic[5]) {
                    case 'i': { // 'fucomi'
                      {return new SgAsmx86Fucom(address,  mnemonic, "");}
                    }
                    case 'p': { // 'fucomp'
                      {return new SgAsmx86Fucomp(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'x': { // 'fx'
            if (mnemonic[2] == 's') { // 'fxs'
              if (mnemonic[3] == 'a') { // 'fxsa'
                if (mnemonic[4] == 'v') { // 'fxsav'
                  if (mnemonic[5] == 'e') { // 'fxsave'
                    {return new SgAsmx86Fxsave(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'o') { // 'lo'
          if (mnemonic[2] == 'o') { // 'loo'
            if (mnemonic[3] == 'p') { // 'loop'
              if (mnemonic[4] == 'n') { // 'loopn'
                switch (mnemonic[5]) {
                  case 'e': { // 'loopne'
                    {return new SgAsmx86Loopnz(address,  mnemonic, "");}
                  }
                  case 'z': { // 'loopnz'
                    {return new SgAsmx86Loopnz(address,  mnemonic, "");}
                  }
                  default: return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'm': { // 'm'
        if (mnemonic[1] == 'o') { // 'mo'
          if (mnemonic[2] == 'v') { // 'mov'
            switch (mnemonic[3]) {
              case 'a': { // 'mova'
                if (mnemonic[4] == 'p') { // 'movap'
                  if (mnemonic[5] == 's') { // 'movaps'
                    {return new SgAsmx86Movaps(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'd': { // 'movd'
                if (mnemonic[4] == 'q') { // 'movdq'
                  if (mnemonic[5] == 'u') { // 'movdqu'
                    {return new SgAsmx86Movdqu(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'l': { // 'movl'
                if (mnemonic[4] == 'p') { // 'movlp'
                  if (mnemonic[5] == 'd') { // 'movlpd'
                    {return new SgAsmx86Movlpd(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              case 'n': { // 'movn'
                if (mnemonic[4] == 't') { // 'movnt'
                  if (mnemonic[5] == 'q') { // 'movntq'
                    {return new SgAsmx86Movntq(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              }
              default: return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'p': { // 'p'
        if (mnemonic[1] == 'u') { // 'pu'
          if (mnemonic[2] == 's') { // 'pus'
            if (mnemonic[3] == 'h') { // 'push'
              switch (mnemonic[4]) {
                case 'a': { // 'pusha'
                  if (mnemonic[5] == 'd') { // 'pushad'
                    {return new SgAsmx86Pusha(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'f': { // 'pushf'
                  switch (mnemonic[5]) {
                    case 'd': { // 'pushfd'
                      {return new SgAsmx86Pushf(address,  mnemonic, "");}
                    }
                    case 'q': { // 'pushfq'
                      {return new SgAsmx86Pushf(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 's': { // 's'
        if (mnemonic[1] == 'e') { // 'se'
          if (mnemonic[2] == 't') { // 'set'
            if (mnemonic[3] == 'n') { // 'setn'
              switch (mnemonic[4]) {
                case 'b': { // 'setnb'
                  if (mnemonic[5] == 'e') { // 'setnbe'
                    {return new SgAsmx86Seta(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'g': { // 'setng'
                  if (mnemonic[5] == 'e') { // 'setnge'
                    {return new SgAsmx86Setl(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                case 'l': { // 'setnl'
                  if (mnemonic[5] == 'e') { // 'setnle'
                    {return new SgAsmx86Setg(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                }
                default: return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, "");
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
                        {return new SgAsmx86CMovb(address,  mnemonic, "");}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, "");
                      }
                    }
                    case 'b': { // 'cmovnb'
                      if (mnemonic[6] == 'e') { // 'cmovnbe'
                        {return new SgAsmx86CMova(address,  mnemonic, "");}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, "");
                      }
                    }
                    case 'g': { // 'cmovng'
                      if (mnemonic[6] == 'e') { // 'cmovnge'
                        {return new SgAsmx86CMovl(address,  mnemonic, "");}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, "");
                      }
                    }
                    case 'l': { // 'cmovnl'
                      if (mnemonic[6] == 'e') { // 'cmovnle'
                        {return new SgAsmx86CMovg(address,  mnemonic, "");}
                      } else {
                        return new SgAsmx86Instruction(address,  mnemonic, "");
                      }
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            }
            case 'p': { // 'cmp'
              if (mnemonic[3] == 'x') { // 'cmpx'
                if (mnemonic[4] == 'c') { // 'cmpxc'
                  if (mnemonic[5] == 'h') { // 'cmpxch'
                    if (mnemonic[6] == 'g') { // 'cmpxchg'
                      {return new SgAsmx86Cmpxchg(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            }
            default: return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'f': { // 'f'
        if (mnemonic[1] == 'u') { // 'fu'
          if (mnemonic[2] == 'c') { // 'fuc'
            if (mnemonic[3] == 'o') { // 'fuco'
              if (mnemonic[4] == 'm') { // 'fucom'
                if (mnemonic[5] == 'p') { // 'fucomp'
                  if (mnemonic[6] == 'p') { // 'fucompp'
                    {return new SgAsmx86Fucompp(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'd') { // 'ld'
          if (mnemonic[2] == 'm') { // 'ldm'
            if (mnemonic[3] == 'x') { // 'ldmx'
              if (mnemonic[4] == 'c') { // 'ldmxc'
                if (mnemonic[5] == 's') { // 'ldmxcs'
                  if (mnemonic[6] == 'r') { // 'ldmxcsr'
                    {return new SgAsmx86Ldmxcsr(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
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
                        {return new SgAsmx86Paddus(address,  mnemonic, "");}
                      }
                      case 'w': { // 'paddusw'
                        {return new SgAsmx86Paddus(address,  mnemonic, "");}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          case 'c': { // 'pc'
            if (mnemonic[2] == 'm') { // 'pcm'
              if (mnemonic[3] == 'p') { // 'pcmp'
                if (mnemonic[4] == 'e') { // 'pcmpe'
                  if (mnemonic[5] == 'q') { // 'pcmpeq'
                    switch (mnemonic[6]) {
                      case 'b': { // 'pcmpeqb'
                        {return new SgAsmx86Pcmpeq(address,  mnemonic, "");}
                      }
                      case 'd': { // 'pcmpeqd'
                        {return new SgAsmx86Pcmpeq(address,  mnemonic, "");}
                      }
                      case 'w': { // 'pcmpeqw'
                        {return new SgAsmx86Pcmpeq(address,  mnemonic, "");}
                      }
                      default: return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      case 's': { // 's'
        if (mnemonic[1] == 't') { // 'st'
          if (mnemonic[2] == 'm') { // 'stm'
            if (mnemonic[3] == 'x') { // 'stmx'
              if (mnemonic[4] == 'c') { // 'stmxc'
                if (mnemonic[5] == 's') { // 'stmxcs'
                  if (mnemonic[6] == 'r') { // 'stmxcsr'
                    {return new SgAsmx86Stmxcsr(address,  mnemonic, "");}
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
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
                      {return new SgAsmx86Ucomisd(address,  mnemonic, "");}
                    }
                    case 's': { // 'ucomiss'
                      {return new SgAsmx86Ucomiss(address,  mnemonic, "");}
                    }
                    default: return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmx86Instruction(address,  mnemonic, "");
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
                      {return new SgAsmx86Cmpxchg8b(address,  mnemonic, "");}
                    } else {
                      return new SgAsmx86Instruction(address,  mnemonic, "");
                    }
                  } else {
                    return new SgAsmx86Instruction(address,  mnemonic, "");
                  }
                } else {
                  return new SgAsmx86Instruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmx86Instruction(address,  mnemonic, "");
              }
            } else {
              return new SgAsmx86Instruction(address,  mnemonic, "");
            }
          } else {
            return new SgAsmx86Instruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmx86Instruction(address,  mnemonic, "");
        }
      } else {
        return new SgAsmx86Instruction(address,  mnemonic, "");
      }
    } else {
      return new SgAsmx86Instruction(address,  mnemonic, "");
    }
  default: return new SgAsmx86Instruction(address,  mnemonic, "");
}
}
SgAsmArmInstruction* createArmInstruction(uint64_t address, const std::string& mnemonic) {
switch (mnemonic.size()) {
  case 1:
    if (mnemonic[0] == 'b') { // 'b'
      {return new SgAsmArmB(address,  mnemonic, "");}
    } else {
      return new SgAsmArmInstruction(address,  mnemonic, "");
    }
  case 2:
    if (mnemonic[0] == 'b') { // 'b'
      switch (mnemonic[1]) {
        case 'l': { // 'bl'
          {return new SgAsmArmBl(address,  mnemonic, "");}
        }
        case 'x': { // 'bx'
          {return new SgAsmArmBx(address,  mnemonic, "");}
        }
        default: return new SgAsmArmInstruction(address,  mnemonic, "");
      }
    } else {
      return new SgAsmArmInstruction(address,  mnemonic, "");
    }
  case 3:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        switch (mnemonic[1]) {
          case 'd': { // 'ad'
            switch (mnemonic[2]) {
              case 'c': { // 'adc'
                {return new SgAsmArmAdc(address,  mnemonic, "");}
              }
              case 'd': { // 'add'
                {return new SgAsmArmAdd(address,  mnemonic, "");}
              }
              default: return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              {return new SgAsmArmAnd(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'b': { // 'b'
        if (mnemonic[1] == 'i') { // 'bi'
          if (mnemonic[2] == 'c') { // 'bic'
            {return new SgAsmArmBic(address,  mnemonic, "");}
          } else {
            return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'c': { // 'c'
        if (mnemonic[1] == 'm') { // 'cm'
          switch (mnemonic[2]) {
            case 'n': { // 'cmn'
              {return new SgAsmArmCmn(address,  mnemonic, "");}
            }
            case 'p': { // 'cmp'
              {return new SgAsmArmCmp(address,  mnemonic, "");}
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'o') { // 'eo'
          if (mnemonic[2] == 'r') { // 'eor'
            {return new SgAsmArmEor(address,  mnemonic, "");}
          } else {
            return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'd') { // 'ld'
          switch (mnemonic[2]) {
            case 'm': { // 'ldm'
              {return new SgAsmArmLdm(address,  mnemonic, "");}
            }
            case 'r': { // 'ldr'
              {return new SgAsmArmLdr(address,  mnemonic, "");}
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'm': { // 'm'
        switch (mnemonic[1]) {
          case 'l': { // 'ml'
            if (mnemonic[2] == 'a') { // 'mla'
              {return new SgAsmArmMla(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 'o': { // 'mo'
            if (mnemonic[2] == 'v') { // 'mov'
              {return new SgAsmArmMov(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 'r': { // 'mr'
            if (mnemonic[2] == 's') { // 'mrs'
              {return new SgAsmArmMrs(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'ms'
            if (mnemonic[2] == 'r') { // 'msr'
              {return new SgAsmArmMsr(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'mu'
            if (mnemonic[2] == 'l') { // 'mul'
              {return new SgAsmArmMul(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 'v': { // 'mv'
            if (mnemonic[2] == 'n') { // 'mvn'
              {return new SgAsmArmMvn(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'r') { // 'or'
          if (mnemonic[2] == 'r') { // 'orr'
            {return new SgAsmArmOrr(address,  mnemonic, "");}
          } else {
            return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'r': { // 'r'
        if (mnemonic[1] == 's') { // 'rs'
          switch (mnemonic[2]) {
            case 'b': { // 'rsb'
              {return new SgAsmArmRsb(address,  mnemonic, "");}
            }
            case 'c': { // 'rsc'
              {return new SgAsmArmRsc(address,  mnemonic, "");}
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'b': { // 'sb'
            if (mnemonic[2] == 'c') { // 'sbc'
              {return new SgAsmArmSbc(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 't': { // 'st'
            switch (mnemonic[2]) {
              case 'm': { // 'stm'
                {return new SgAsmArmStm(address,  mnemonic, "");}
              }
              case 'r': { // 'str'
                {return new SgAsmArmStr(address,  mnemonic, "");}
              }
              default: return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              {return new SgAsmArmSub(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 'w': { // 'sw'
            switch (mnemonic[2]) {
              case 'i': { // 'swi'
                {return new SgAsmArmSwi(address,  mnemonic, "");}
              }
              case 'p': { // 'swp'
                {return new SgAsmArmSwp(address,  mnemonic, "");}
              }
              default: return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 't': { // 't'
        switch (mnemonic[1]) {
          case 'e': { // 'te'
            if (mnemonic[2] == 'q') { // 'teq'
              {return new SgAsmArmTeq(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          case 's': { // 'ts'
            if (mnemonic[2] == 't') { // 'tst'
              {return new SgAsmArmTst(address,  mnemonic, "");}
            } else {
              return new SgAsmArmInstruction(address,  mnemonic, "");
            }
          }
          default: return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmArmInstruction(address,  mnemonic, "");
    }
  case 5:
    switch (mnemonic[0]) {
      case 's': { // 's'
        if (mnemonic[1] == 'm') { // 'sm'
          switch (mnemonic[2]) {
            case 'l': { // 'sml'
              if (mnemonic[3] == 'a') { // 'smla'
                if (mnemonic[4] == 'l') { // 'smlal'
                  {return new SgAsmArmSmlal(address,  mnemonic, "");}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, "");
              }
            }
            case 'u': { // 'smu'
              if (mnemonic[3] == 'l') { // 'smul'
                if (mnemonic[4] == 'l') { // 'smull'
                  {return new SgAsmArmSmull(address,  mnemonic, "");}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, "");
              }
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      case 'u': { // 'u'
        if (mnemonic[1] == 'm') { // 'um'
          switch (mnemonic[2]) {
            case 'l': { // 'uml'
              if (mnemonic[3] == 'a') { // 'umla'
                if (mnemonic[4] == 'l') { // 'umlal'
                  {return new SgAsmArmUmlal(address,  mnemonic, "");}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, "");
              }
            }
            case 'u': { // 'umu'
              if (mnemonic[3] == 'l') { // 'umul'
                if (mnemonic[4] == 'l') { // 'umull'
                  {return new SgAsmArmUmull(address,  mnemonic, "");}
                } else {
                  return new SgAsmArmInstruction(address,  mnemonic, "");
                }
              } else {
                return new SgAsmArmInstruction(address,  mnemonic, "");
              }
            }
            default: return new SgAsmArmInstruction(address,  mnemonic, "");
          }
        } else {
          return new SgAsmArmInstruction(address,  mnemonic, "");
        }
      }
      default: return new SgAsmArmInstruction(address,  mnemonic, "");
    }
  default: return new SgAsmArmInstruction(address,  mnemonic, "");
}
}

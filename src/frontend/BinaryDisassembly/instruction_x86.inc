switch (mnemonic.size()) {
  case 2:
    switch (mnemonic[0]) {
      case 'b': { // 'b'
        if (mnemonic[1] == 't') { // 'bt'
          {instruction = new SgAsmx86Bt(address,  mnemonic, ""); goto done;}
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'n') { // 'in'
          {instruction = new SgAsmx86In(address,  mnemonic, ""); goto done;}
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'a': { // 'ja'
            {instruction = new SgAsmx86Ja(address,  mnemonic, ""); goto done;}
          }
          case 'b': { // 'jb'
            {instruction = new SgAsmx86Jb(address,  mnemonic, ""); goto done;}
          }
          case 'c': { // 'jc'
            {instruction = new SgAsmx86Jc(address,  mnemonic, ""); goto done;}
          }
          case 'e': { // 'je'
            {instruction = new SgAsmx86Je(address,  mnemonic, ""); goto done;}
          }
          case 'g': { // 'jg'
            {instruction = new SgAsmx86Jg(address,  mnemonic, ""); goto done;}
          }
          case 'l': { // 'jl'
            {instruction = new SgAsmx86Jl(address,  mnemonic, ""); goto done;}
          }
          case 'o': { // 'jo'
            {instruction = new SgAsmx86Jo(address,  mnemonic, ""); goto done;}
          }
          case 'p': { // 'jp'
            {instruction = new SgAsmx86Jpe(address,  mnemonic, ""); goto done;}
          }
          case 's': { // 'js'
            {instruction = new SgAsmx86Js(address,  mnemonic, ""); goto done;}
          }
          case 'z': { // 'jz'
            {instruction = new SgAsmx86Je(address,  mnemonic, ""); goto done;}
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'r') { // 'or'
          {instruction = new SgAsmx86Or(address,  mnemonic, ""); goto done;}
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
    }
  case 3:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        switch (mnemonic[1]) {
          case 'a': { // 'aa'
            switch (mnemonic[2]) {
              case 'a': { // 'aaa'
                {instruction = new SgAsmx86Aaa(address,  mnemonic, ""); goto done;}
              }
              case 'd': { // 'aad'
                {instruction = new SgAsmx86Aad(address,  mnemonic, ""); goto done;}
              }
              case 'm': { // 'aam'
                {instruction = new SgAsmx86Aam(address,  mnemonic, ""); goto done;}
              }
              case 's': { // 'aas'
                {instruction = new SgAsmx86Aas(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'd': { // 'ad'
            switch (mnemonic[2]) {
              case 'c': { // 'adc'
                {instruction = new SgAsmx86Adc(address,  mnemonic, ""); goto done;}
              }
              case 'd': { // 'add'
                {instruction = new SgAsmx86Add(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              {instruction = new SgAsmx86And(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'b': { // 'b'
        switch (mnemonic[1]) {
          case 's': { // 'bs'
            switch (mnemonic[2]) {
              case 'f': { // 'bsf'
                {instruction = new SgAsmx86Bsf(address,  mnemonic, ""); goto done;}
              }
              case 'r': { // 'bsr'
                {instruction = new SgAsmx86Bsr(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 't': { // 'bt'
            switch (mnemonic[2]) {
              case 'c': { // 'btc'
                {instruction = new SgAsmx86Btc(address,  mnemonic, ""); goto done;}
              }
              case 'r': { // 'btr'
                {instruction = new SgAsmx86Btr(address,  mnemonic, ""); goto done;}
              }
              case 's': { // 'bts'
                {instruction = new SgAsmx86Bts(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'c': { // 'c'
        switch (mnemonic[1]) {
          case 'b': { // 'cb'
            if (mnemonic[2] == 'w') { // 'cbw'
              {instruction = new SgAsmx86Cbw(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'd': { // 'cd'
            if (mnemonic[2] == 'q') { // 'cdq'
              {instruction = new SgAsmx86Cwd(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'l': { // 'cl'
            switch (mnemonic[2]) {
              case 'c': { // 'clc'
                {instruction = new SgAsmx86Clc(address,  mnemonic, ""); goto done;}
              }
              case 'd': { // 'cld'
                {instruction = new SgAsmx86Cld(address,  mnemonic, ""); goto done;}
              }
              case 'i': { // 'cli'
                {instruction = new SgAsmx86Cli(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'm': { // 'cm'
            switch (mnemonic[2]) {
              case 'c': { // 'cmc'
                {instruction = new SgAsmx86Cmc(address,  mnemonic, ""); goto done;}
              }
              case 'p': { // 'cmp'
                {instruction = new SgAsmx86Cmp(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'q': { // 'cq'
            if (mnemonic[2] == 'o') { // 'cqo'
              {instruction = new SgAsmx86Cwd(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'w': { // 'cw'
            if (mnemonic[2] == 'd') { // 'cwd'
              {instruction = new SgAsmx86Cwd(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'd': { // 'd'
        switch (mnemonic[1]) {
          case 'a': { // 'da'
            switch (mnemonic[2]) {
              case 'a': { // 'daa'
                {instruction = new SgAsmx86Daa(address,  mnemonic, ""); goto done;}
              }
              case 's': { // 'das'
                {instruction = new SgAsmx86Das(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'e': { // 'de'
            if (mnemonic[2] == 'c') { // 'dec'
              {instruction = new SgAsmx86Dec(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'i': { // 'di'
            if (mnemonic[2] == 'v') { // 'div'
              {instruction = new SgAsmx86Div(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 's') { // 'es'
          if (mnemonic[2] == 'c') { // 'esc'
            {instruction = new SgAsmx86Esc(address,  mnemonic, ""); goto done;}
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              {instruction = new SgAsmx86Fld(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 's': { // 'fs'
            if (mnemonic[2] == 't') { // 'fst'
              {instruction = new SgAsmx86Fst(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'h': { // 'h'
        if (mnemonic[1] == 'l') { // 'hl'
          if (mnemonic[2] == 't') { // 'hlt'
            {instruction = new SgAsmx86Hlt(address,  mnemonic, ""); goto done;}
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'i': { // 'i'
        if (mnemonic[1] == 'n') { // 'in'
          switch (mnemonic[2]) {
            case 'c': { // 'inc'
              {instruction = new SgAsmx86Inc(address,  mnemonic, ""); goto done;}
            }
            case 's': { // 'ins'
              {instruction = new SgAsmx86Ins(address,  mnemonic, ""); goto done;}
            }
            case 't': { // 'int'
              {instruction = new SgAsmx86Int(address,  mnemonic, ""); goto done;}
            }
            default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'a': { // 'ja'
            if (mnemonic[2] == 'e') { // 'jae'
              {instruction = new SgAsmx86Jae(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'b': { // 'jb'
            if (mnemonic[2] == 'e') { // 'jbe'
              {instruction = new SgAsmx86Jbe(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'g': { // 'jg'
            if (mnemonic[2] == 'e') { // 'jge'
              {instruction = new SgAsmx86Jge(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'l': { // 'jl'
            if (mnemonic[2] == 'e') { // 'jle'
              {instruction = new SgAsmx86Jle(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'm': { // 'jm'
            if (mnemonic[2] == 'p') { // 'jmp'
              {instruction = new SgAsmx86Jmp(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'n': { // 'jn'
            switch (mnemonic[2]) {
              case 'a': { // 'jna'
                {instruction = new SgAsmx86Jbe(address,  mnemonic, ""); goto done;}
              }
              case 'b': { // 'jnb'
                {instruction = new SgAsmx86Jae(address,  mnemonic, ""); goto done;}
              }
              case 'c': { // 'jnc'
                {instruction = new SgAsmx86Jnc(address,  mnemonic, ""); goto done;}
              }
              case 'e': { // 'jne'
                {instruction = new SgAsmx86Jne(address,  mnemonic, ""); goto done;}
              }
              case 'g': { // 'jng'
                {instruction = new SgAsmx86Jle(address,  mnemonic, ""); goto done;}
              }
              case 'l': { // 'jnl'
                {instruction = new SgAsmx86Jge(address,  mnemonic, ""); goto done;}
              }
              case 'o': { // 'jno'
                {instruction = new SgAsmx86Jno(address,  mnemonic, ""); goto done;}
              }
              case 'p': { // 'jnp'
                {instruction = new SgAsmx86Jpo(address,  mnemonic, ""); goto done;}
              }
              case 's': { // 'jns'
                {instruction = new SgAsmx86Jns(address,  mnemonic, ""); goto done;}
              }
              case 'z': { // 'jnz'
                {instruction = new SgAsmx86Jne(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'p': { // 'jp'
            switch (mnemonic[2]) {
              case 'e': { // 'jpe'
                {instruction = new SgAsmx86Jpe(address,  mnemonic, ""); goto done;}
              }
              case 'o': { // 'jpo'
                {instruction = new SgAsmx86Jpo(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'd': { // 'ld'
            if (mnemonic[2] == 's') { // 'lds'
              {instruction = new SgAsmx86Lds(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'e': { // 'le'
            switch (mnemonic[2]) {
              case 'a': { // 'lea'
                {instruction = new SgAsmx86Lea(address,  mnemonic, ""); goto done;}
              }
              case 's': { // 'les'
                {instruction = new SgAsmx86Les(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'f': { // 'lf'
            if (mnemonic[2] == 's') { // 'lfs'
              {instruction = new SgAsmx86Lfs(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'g': { // 'lg'
            if (mnemonic[2] == 's') { // 'lgs'
              {instruction = new SgAsmx86Lgs(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 's': { // 'ls'
            if (mnemonic[2] == 's') { // 'lss'
              {instruction = new SgAsmx86Lss(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'm': { // 'm'
        switch (mnemonic[1]) {
          case 'o': { // 'mo'
            if (mnemonic[2] == 'v') { // 'mov'
              {instruction = new SgAsmx86Mov(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'u': { // 'mu'
            if (mnemonic[2] == 'l') { // 'mul'
              {instruction = new SgAsmx86Mul(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'n': { // 'n'
        switch (mnemonic[1]) {
          case 'e': { // 'ne'
            if (mnemonic[2] == 'g') { // 'neg'
              {instruction = new SgAsmx86Neg(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'o': { // 'no'
            switch (mnemonic[2]) {
              case 'p': { // 'nop'
                {instruction = new SgAsmx86Nop(address,  mnemonic, ""); goto done;}
              }
              case 't': { // 'not'
                {instruction = new SgAsmx86Not(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            {instruction = new SgAsmx86Out(address,  mnemonic, ""); goto done;}
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'p': { // 'p'
        if (mnemonic[1] == 'o') { // 'po'
          if (mnemonic[2] == 'p') { // 'pop'
            {instruction = new SgAsmx86Pop(address,  mnemonic, ""); goto done;}
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'r': { // 'r'
        switch (mnemonic[1]) {
          case 'c': { // 'rc'
            switch (mnemonic[2]) {
              case 'l': { // 'rcl'
                {instruction = new SgAsmx86Rcl(address,  mnemonic, ""); goto done;}
              }
              case 'r': { // 'rcr'
                {instruction = new SgAsmx86Rcr(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'e': { // 're'
            switch (mnemonic[2]) {
              case 'p': { // 'rep'
                {instruction = new SgAsmx86Rep(address,  mnemonic, ""); goto done;}
              }
              case 't': { // 'ret'
                {instruction = new SgAsmx86Ret(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'o': { // 'ro'
            switch (mnemonic[2]) {
              case 'l': { // 'rol'
                {instruction = new SgAsmx86Rol(address,  mnemonic, ""); goto done;}
              }
              case 'r': { // 'ror'
                {instruction = new SgAsmx86Ror(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'a': { // 'sa'
            switch (mnemonic[2]) {
              case 'l': { // 'sal'
                {instruction = new SgAsmx86Shl(address,  mnemonic, ""); goto done;}
              }
              case 'r': { // 'sar'
                {instruction = new SgAsmx86Sar(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'b': { // 'sb'
            if (mnemonic[2] == 'b') { // 'sbb'
              {instruction = new SgAsmx86Sbb(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'h': { // 'sh'
            switch (mnemonic[2]) {
              case 'l': { // 'shl'
                {instruction = new SgAsmx86Shl(address,  mnemonic, ""); goto done;}
              }
              case 'r': { // 'shr'
                {instruction = new SgAsmx86Shr(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 't': { // 'st'
            switch (mnemonic[2]) {
              case 'c': { // 'stc'
                {instruction = new SgAsmx86Stc(address,  mnemonic, ""); goto done;}
              }
              case 'd': { // 'std'
                {instruction = new SgAsmx86Std(address,  mnemonic, ""); goto done;}
              }
              case 'i': { // 'sti'
                {instruction = new SgAsmx86Sti(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              {instruction = new SgAsmx86Sub(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'u': { // 'u'
        if (mnemonic[1] == 'd') { // 'ud'
          if (mnemonic[2] == '2') { // 'ud2'
            {instruction = new SgAsmx86Ud2(address,  mnemonic, ""); goto done;}
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'x': { // 'x'
        if (mnemonic[1] == 'o') { // 'xo'
          if (mnemonic[2] == 'r') { // 'xor'
            {instruction = new SgAsmx86Xor(address,  mnemonic, ""); goto done;}
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
    }
  case 4:
    switch (mnemonic[0]) {
      case 'c': { // 'c'
        switch (mnemonic[1]) {
          case 'a': { // 'ca'
            if (mnemonic[2] == 'l') { // 'cal'
              if (mnemonic[3] == 'l') { // 'call'
                {instruction = new SgAsmx86Call(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'd': { // 'cd'
            if (mnemonic[2] == 'q') { // 'cdq'
              if (mnemonic[3] == 'e') { // 'cdqe'
                {instruction = new SgAsmx86Cbw(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'm': { // 'cm'
            if (mnemonic[2] == 'p') { // 'cmp'
              if (mnemonic[3] == 's') { // 'cmps'
                {instruction = new SgAsmx86Cmps(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'w': { // 'cw'
            if (mnemonic[2] == 'd') { // 'cwd'
              if (mnemonic[3] == 'e') { // 'cwde'
                {instruction = new SgAsmx86Cbw(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case 'a': { // 'fa'
            switch (mnemonic[2]) {
              case 'b': { // 'fab'
                if (mnemonic[3] == 's') { // 'fabs'
                  {instruction = new SgAsmx86Fabs(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'd': { // 'fad'
                if (mnemonic[3] == 'd') { // 'fadd'
                  {instruction = new SgAsmx86Fadd(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'c': { // 'fc'
            switch (mnemonic[2]) {
              case 'h': { // 'fch'
                if (mnemonic[3] == 's') { // 'fchs'
                  {instruction = new SgAsmx86Fchs(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'o': { // 'fco'
                switch (mnemonic[3]) {
                  case 'm': { // 'fcom'
                    {instruction = new SgAsmx86Fcom(address,  mnemonic, ""); goto done;}
                  }
                  case 's': { // 'fcos'
                    {instruction = new SgAsmx86Fcos(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                {instruction = new SgAsmx86Fdiv(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'l': { // 'fil'
                if (mnemonic[3] == 'd') { // 'fild'
                  {instruction = new SgAsmx86Fild(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  {instruction = new SgAsmx86Fist(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              switch (mnemonic[3]) {
                case '1': { // 'fld1'
                  {instruction = new SgAsmx86Fld1(address,  mnemonic, ""); goto done;}
                }
                case 'l': { // 'fldl'
                  {instruction = new SgAsmx86Fld(address,  mnemonic, ""); goto done;}
                }
                case 's': { // 'flds'
                  {instruction = new SgAsmx86Fld(address,  mnemonic, ""); goto done;}
                }
                case 'z': { // 'fldz'
                  {instruction = new SgAsmx86Fldz(address,  mnemonic, ""); goto done;}
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'm': { // 'fm'
            if (mnemonic[2] == 'u') { // 'fmu'
              if (mnemonic[3] == 'l') { // 'fmul'
                {instruction = new SgAsmx86Fmul(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'n': { // 'fn'
            if (mnemonic[2] == 'o') { // 'fno'
              if (mnemonic[3] == 'p') { // 'fnop'
                {instruction = new SgAsmx86Fnop(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'i': { // 'fsi'
                if (mnemonic[3] == 'n') { // 'fsin'
                  {instruction = new SgAsmx86Fsin(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 't': { // 'fst'
                switch (mnemonic[3]) {
                  case 'l': { // 'fstl'
                    {instruction = new SgAsmx86Fst(address,  mnemonic, ""); goto done;}
                  }
                  case 'p': { // 'fstp'
                    {instruction = new SgAsmx86Fstp(address,  mnemonic, ""); goto done;}
                  }
                  case 's': { // 'fsts'
                    {instruction = new SgAsmx86Fst(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  {instruction = new SgAsmx86Fsub(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 't': { // 'ft'
            if (mnemonic[2] == 's') { // 'fts'
              if (mnemonic[3] == 't') { // 'ftst'
                {instruction = new SgAsmx86Ftst(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'x': { // 'fx'
            if (mnemonic[2] == 'c') { // 'fxc'
              if (mnemonic[3] == 'h') { // 'fxch'
                {instruction = new SgAsmx86Fxch(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'i': { // 'i'
        switch (mnemonic[1]) {
          case 'd': { // 'id'
            if (mnemonic[2] == 'i') { // 'idi'
              if (mnemonic[3] == 'v') { // 'idiv'
                {instruction = new SgAsmx86IDiv(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'm': { // 'im'
            if (mnemonic[2] == 'u') { // 'imu'
              if (mnemonic[3] == 'l') { // 'imul'
                {instruction = new SgAsmx86IMul(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'n': { // 'in'
            switch (mnemonic[2]) {
              case 's': { // 'ins'
                switch (mnemonic[3]) {
                  case 'b': { // 'insb'
                    {instruction = new SgAsmx86Ins(address,  mnemonic, ""); goto done;}
                  }
                  case 'd': { // 'insd'
                    {instruction = new SgAsmx86Ins(address,  mnemonic, ""); goto done;}
                  }
                  case 'w': { // 'insw'
                    {instruction = new SgAsmx86Ins(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              }
              case 't': { // 'int'
                if (mnemonic[3] == 'o') { // 'into'
                  {instruction = new SgAsmx86Into(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'r': { // 'ir'
            if (mnemonic[2] == 'e') { // 'ire'
              if (mnemonic[3] == 't') { // 'iret'
                {instruction = new SgAsmx86IRet(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'j': { // 'j'
        switch (mnemonic[1]) {
          case 'c': { // 'jc'
            if (mnemonic[2] == 'x') { // 'jcx'
              if (mnemonic[3] == 'z') { // 'jcxz'
                {instruction = new SgAsmx86Jcxz(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'n': { // 'jn'
            switch (mnemonic[2]) {
              case 'a': { // 'jna'
                if (mnemonic[3] == 'e') { // 'jnae'
                  {instruction = new SgAsmx86Jb(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'b': { // 'jnb'
                if (mnemonic[3] == 'e') { // 'jnbe'
                  {instruction = new SgAsmx86Ja(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'g': { // 'jng'
                if (mnemonic[3] == 'e') { // 'jnge'
                  {instruction = new SgAsmx86Jl(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'l': { // 'jnl'
                if (mnemonic[3] == 'e') { // 'jnle'
                  {instruction = new SgAsmx86Jg(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'a': { // 'la'
            if (mnemonic[2] == 'h') { // 'lah'
              if (mnemonic[3] == 'f') { // 'lahf'
                {instruction = new SgAsmx86Lahf(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'o': { // 'lo'
            switch (mnemonic[2]) {
              case 'c': { // 'loc'
                if (mnemonic[3] == 'k') { // 'lock'
                  {instruction = new SgAsmx86Lock(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'd': { // 'lod'
                if (mnemonic[3] == 's') { // 'lods'
                  {instruction = new SgAsmx86Lods(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'o': { // 'loo'
                if (mnemonic[3] == 'p') { // 'loop'
                  {instruction = new SgAsmx86Loop(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'm': { // 'm'
        if (mnemonic[1] == 'o') { // 'mo'
          if (mnemonic[2] == 'v') { // 'mov'
            if (mnemonic[3] == 's') { // 'movs'
              {instruction = new SgAsmx86Movs(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            if (mnemonic[3] == 's') { // 'outs'
              {instruction = new SgAsmx86Outs(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'p': { // 'p'
        switch (mnemonic[1]) {
          case 'o': { // 'po'
            if (mnemonic[2] == 'p') { // 'pop'
              switch (mnemonic[3]) {
                case 'a': { // 'popa'
                  {instruction = new SgAsmx86Popa(address,  mnemonic, ""); goto done;}
                }
                case 'f': { // 'popf'
                  {instruction = new SgAsmx86Popf(address,  mnemonic, ""); goto done;}
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'u': { // 'pu'
            if (mnemonic[2] == 's') { // 'pus'
              if (mnemonic[3] == 'h') { // 'push'
                {instruction = new SgAsmx86Push(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'x': { // 'px'
            if (mnemonic[2] == 'o') { // 'pxo'
              if (mnemonic[3] == 'r') { // 'pxor'
                {instruction = new SgAsmx86Pxor(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'r': { // 'r'
        if (mnemonic[1] == 'e') { // 're'
          if (mnemonic[2] == 'p') { // 'rep'
            switch (mnemonic[3]) {
              case 'e': { // 'repe'
                {instruction = new SgAsmx86Repe(address,  mnemonic, ""); goto done;}
              }
              case 'z': { // 'repz'
                {instruction = new SgAsmx86Repe(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'a': { // 'sa'
            if (mnemonic[2] == 'h') { // 'sah'
              if (mnemonic[3] == 'f') { // 'sahf'
                {instruction = new SgAsmx86Sahf(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'c': { // 'sc'
            if (mnemonic[2] == 'a') { // 'sca'
              if (mnemonic[3] == 's') { // 'scas'
                {instruction = new SgAsmx86Scas(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'e': { // 'se'
            if (mnemonic[2] == 't') { // 'set'
              switch (mnemonic[3]) {
                case 'a': { // 'seta'
                  {instruction = new SgAsmx86Seta(address,  mnemonic, ""); goto done;}
                }
                case 'b': { // 'setb'
                  {instruction = new SgAsmx86Setb(address,  mnemonic, ""); goto done;}
                }
                case 'c': { // 'setc'
                  {instruction = new SgAsmx86Setb(address,  mnemonic, ""); goto done;}
                }
                case 'e': { // 'sete'
                  {instruction = new SgAsmx86Sete(address,  mnemonic, ""); goto done;}
                }
                case 'g': { // 'setg'
                  {instruction = new SgAsmx86Setg(address,  mnemonic, ""); goto done;}
                }
                case 'l': { // 'setl'
                  {instruction = new SgAsmx86Setl(address,  mnemonic, ""); goto done;}
                }
                case 'o': { // 'seto'
                  {instruction = new SgAsmx86Seto(address,  mnemonic, ""); goto done;}
                }
                case 'p': { // 'setp'
                  {instruction = new SgAsmx86Setpe(address,  mnemonic, ""); goto done;}
                }
                case 's': { // 'sets'
                  {instruction = new SgAsmx86Sets(address,  mnemonic, ""); goto done;}
                }
                case 'z': { // 'setz'
                  {instruction = new SgAsmx86Sete(address,  mnemonic, ""); goto done;}
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'h': { // 'sh'
            switch (mnemonic[2]) {
              case 'l': { // 'shl'
                if (mnemonic[3] == 'd') { // 'shld'
                  {instruction = new SgAsmx86Shld(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'r': { // 'shr'
                if (mnemonic[3] == 'd') { // 'shrd'
                  {instruction = new SgAsmx86Shrd(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'l': { // 'sl'
            if (mnemonic[2] == 'd') { // 'sld'
              if (mnemonic[3] == 't') { // 'sldt'
                {instruction = new SgAsmx86Sldt(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 't': { // 'st'
            if (mnemonic[2] == 'o') { // 'sto'
              if (mnemonic[3] == 's') { // 'stos'
                {instruction = new SgAsmx86Stos(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 't': { // 't'
        if (mnemonic[1] == 'e') { // 'te'
          if (mnemonic[2] == 's') { // 'tes'
            if (mnemonic[3] == 't') { // 'test'
              {instruction = new SgAsmx86Test(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'v': { // 'v'
        if (mnemonic[1] == 'e') { // 've'
          if (mnemonic[2] == 'r') { // 'ver'
            switch (mnemonic[3]) {
              case 'r': { // 'verr'
                {instruction = new SgAsmx86Verr(address,  mnemonic, ""); goto done;}
              }
              case 'w': { // 'verw'
                {instruction = new SgAsmx86Verw(address,  mnemonic, ""); goto done;}
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'w': { // 'w'
        if (mnemonic[1] == 'a') { // 'wa'
          if (mnemonic[2] == 'i') { // 'wai'
            if (mnemonic[3] == 't') { // 'wait'
              {instruction = new SgAsmx86Wait(address,  mnemonic, ""); goto done;}
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'x': { // 'x'
        switch (mnemonic[1]) {
          case 'a': { // 'xa'
            if (mnemonic[2] == 'd') { // 'xad'
              if (mnemonic[3] == 'd') { // 'xadd'
                {instruction = new SgAsmx86Xadd(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'c': { // 'xc'
            if (mnemonic[2] == 'h') { // 'xch'
              if (mnemonic[3] == 'g') { // 'xchg'
                {instruction = new SgAsmx86Xchg(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'l': { // 'xl'
            if (mnemonic[2] == 'a') { // 'xla'
              if (mnemonic[3] == 't') { // 'xlat'
                {instruction = new SgAsmx86Xlat(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
    }
  case 5:
    switch (mnemonic[0]) {
      case 'b': { // 'b'
        switch (mnemonic[1]) {
          case 'o': { // 'bo'
            if (mnemonic[2] == 'u') { // 'bou'
              if (mnemonic[3] == 'n') { // 'boun'
                if (mnemonic[4] == 'd') { // 'bound'
                  {instruction = new SgAsmx86Bound(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 's': { // 'bs'
            if (mnemonic[2] == 'w') { // 'bsw'
              if (mnemonic[3] == 'a') { // 'bswa'
                if (mnemonic[4] == 'p') { // 'bswap'
                  {instruction = new SgAsmx86Bswap(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
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
                      {instruction = new SgAsmx86CMova(address,  mnemonic, ""); goto done;}
                    }
                    case 'b': { // 'cmovb'
                      {instruction = new SgAsmx86CMovb(address,  mnemonic, ""); goto done;}
                    }
                    case 'c': { // 'cmovc'
                      {instruction = new SgAsmx86CMovc(address,  mnemonic, ""); goto done;}
                    }
                    case 'e': { // 'cmove'
                      {instruction = new SgAsmx86CMove(address,  mnemonic, ""); goto done;}
                    }
                    case 'g': { // 'cmovg'
                      {instruction = new SgAsmx86CMovg(address,  mnemonic, ""); goto done;}
                    }
                    case 'l': { // 'cmovl'
                      {instruction = new SgAsmx86CMovl(address,  mnemonic, ""); goto done;}
                    }
                    case 'o': { // 'cmovo'
                      {instruction = new SgAsmx86CMovo(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'cmovp'
                      {instruction = new SgAsmx86CMovp(address,  mnemonic, ""); goto done;}
                    }
                    case 's': { // 'cmovs'
                      {instruction = new SgAsmx86CMovs(address,  mnemonic, ""); goto done;}
                    }
                    case 'z': { // 'cmovz'
                      {instruction = new SgAsmx86CMove(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'p': { // 'cmp'
                if (mnemonic[3] == 's') { // 'cmps'
                  switch (mnemonic[4]) {
                    case 'b': { // 'cmpsb'
                      {instruction = new SgAsmx86Cmps(address,  mnemonic, ""); goto done;}
                    }
                    case 'd': { // 'cmpsd'
                      {instruction = new SgAsmx86Cmps(address,  mnemonic, ""); goto done;}
                    }
                    case 'w': { // 'cmpsw'
                      {instruction = new SgAsmx86Cmps(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'p': { // 'cp'
            if (mnemonic[2] == 'u') { // 'cpu'
              if (mnemonic[3] == 'i') { // 'cpui'
                if (mnemonic[4] == 'd') { // 'cpuid'
                  {instruction = new SgAsmx86Cpuid(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'n') { // 'en'
          if (mnemonic[2] == 't') { // 'ent'
            if (mnemonic[3] == 'e') { // 'ente'
              if (mnemonic[4] == 'r') { // 'enter'
                {instruction = new SgAsmx86Enter(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'f': { // 'f'
        switch (mnemonic[1]) {
          case '2': { // 'f2'
            if (mnemonic[2] == 'x') { // 'f2x'
              if (mnemonic[3] == 'm') { // 'f2xm'
                if (mnemonic[4] == '1') { // 'f2xm1'
                  {instruction = new SgAsmx86F2xm1(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'a': { // 'fa'
            if (mnemonic[2] == 'd') { // 'fad'
              if (mnemonic[3] == 'd') { // 'fadd'
                switch (mnemonic[4]) {
                  case 'l': { // 'faddl'
                    {instruction = new SgAsmx86Fadd(address,  mnemonic, ""); goto done;}
                  }
                  case 'p': { // 'faddp'
                    {instruction = new SgAsmx86Faddp(address,  mnemonic, ""); goto done;}
                  }
                  case 's': { // 'fadds'
                    {instruction = new SgAsmx86Fadd(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'c': { // 'fc'
            if (mnemonic[2] == 'o') { // 'fco'
              if (mnemonic[3] == 'm') { // 'fcom'
                switch (mnemonic[4]) {
                  case 'l': { // 'fcoml'
                    {instruction = new SgAsmx86Fcom(address,  mnemonic, ""); goto done;}
                  }
                  case 'p': { // 'fcomp'
                    {instruction = new SgAsmx86Fcomp(address,  mnemonic, ""); goto done;}
                  }
                  case 's': { // 'fcoms'
                    {instruction = new SgAsmx86Fcom(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                switch (mnemonic[4]) {
                  case 'l': { // 'fdivl'
                    {instruction = new SgAsmx86Fdiv(address,  mnemonic, ""); goto done;}
                  }
                  case 'p': { // 'fdivp'
                    {instruction = new SgAsmx86Fdivp(address,  mnemonic, ""); goto done;}
                  }
                  case 'r': { // 'fdivr'
                    {instruction = new SgAsmx86Fdivr(address,  mnemonic, ""); goto done;}
                  }
                  case 's': { // 'fdivs'
                    {instruction = new SgAsmx86Fdiv(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'f': { // 'ff'
            if (mnemonic[2] == 'r') { // 'ffr'
              if (mnemonic[3] == 'e') { // 'ffre'
                if (mnemonic[4] == 'e') { // 'ffree'
                  {instruction = new SgAsmx86Ffree(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'a': { // 'fia'
                if (mnemonic[3] == 'd') { // 'fiad'
                  if (mnemonic[4] == 'd') { // 'fiadd'
                    {instruction = new SgAsmx86Fiadd(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'd': { // 'fid'
                if (mnemonic[3] == 'i') { // 'fidi'
                  if (mnemonic[4] == 'v') { // 'fidiv'
                    {instruction = new SgAsmx86Fidiv(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'l': { // 'fil'
                if (mnemonic[3] == 'd') { // 'fild'
                  if (mnemonic[4] == 'l') { // 'fildl'
                    {instruction = new SgAsmx86Fild(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'm': { // 'fim'
                if (mnemonic[3] == 'u') { // 'fimu'
                  if (mnemonic[4] == 'l') { // 'fimul'
                    {instruction = new SgAsmx86Fimul(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fistl'
                      {instruction = new SgAsmx86Fist(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'fistp'
                      {instruction = new SgAsmx86Fistp(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              switch (mnemonic[3]) {
                case 'c': { // 'fldc'
                  if (mnemonic[4] == 'w') { // 'fldcw'
                    {instruction = new SgAsmx86Fldcw(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'p': { // 'fldp'
                  if (mnemonic[4] == 'i') { // 'fldpi'
                    {instruction = new SgAsmx86Fldpi(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'm': { // 'fm'
            if (mnemonic[2] == 'u') { // 'fmu'
              if (mnemonic[3] == 'l') { // 'fmul'
                switch (mnemonic[4]) {
                  case 'l': { // 'fmull'
                    {instruction = new SgAsmx86Fmul(address,  mnemonic, ""); goto done;}
                  }
                  case 'p': { // 'fmulp'
                    {instruction = new SgAsmx86Fmulp(address,  mnemonic, ""); goto done;}
                  }
                  case 's': { // 'fmuls'
                    {instruction = new SgAsmx86Fmul(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'p': { // 'fp'
            switch (mnemonic[2]) {
              case 'r': { // 'fpr'
                if (mnemonic[3] == 'e') { // 'fpre'
                  if (mnemonic[4] == 'm') { // 'fprem'
                    {instruction = new SgAsmx86Fprem(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 't': { // 'fpt'
                if (mnemonic[3] == 'a') { // 'fpta'
                  if (mnemonic[4] == 'n') { // 'fptan'
                    {instruction = new SgAsmx86Fptan(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'a': { // 'fsa'
                if (mnemonic[3] == 'v') { // 'fsav'
                  if (mnemonic[4] == 'e') { // 'fsave'
                    {instruction = new SgAsmx86Fsave(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'q': { // 'fsq'
                if (mnemonic[3] == 'r') { // 'fsqr'
                  if (mnemonic[4] == 't') { // 'fsqrt'
                    {instruction = new SgAsmx86Fsqrt(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 't': { // 'fst'
                if (mnemonic[3] == 'p') { // 'fstp'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fstpl'
                      {instruction = new SgAsmx86Fstp(address,  mnemonic, ""); goto done;}
                    }
                    case 's': { // 'fstps'
                      {instruction = new SgAsmx86Fstp(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  switch (mnemonic[4]) {
                    case 'l': { // 'fsubl'
                      {instruction = new SgAsmx86Fsub(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'fsubp'
                      {instruction = new SgAsmx86Fsubp(address,  mnemonic, ""); goto done;}
                    }
                    case 'r': { // 'fsubr'
                      {instruction = new SgAsmx86Fsubr(address,  mnemonic, ""); goto done;}
                    }
                    case 's': { // 'fsubs'
                      {instruction = new SgAsmx86Fsub(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'u': { // 'fu'
            if (mnemonic[2] == 'c') { // 'fuc'
              if (mnemonic[3] == 'o') { // 'fuco'
                if (mnemonic[4] == 'm') { // 'fucom'
                  {instruction = new SgAsmx86Fucom(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'w': { // 'fw'
            if (mnemonic[2] == 'a') { // 'fwa'
              if (mnemonic[3] == 'i') { // 'fwai'
                if (mnemonic[4] == 't') { // 'fwait'
                  {instruction = new SgAsmx86Fwait(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'y': { // 'fy'
            if (mnemonic[2] == 'l') { // 'fyl'
              if (mnemonic[3] == '2') { // 'fyl2'
                if (mnemonic[4] == 'x') { // 'fyl2x'
                  {instruction = new SgAsmx86Fyl2x(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'j': { // 'j'
        if (mnemonic[1] == 'e') { // 'je'
          if (mnemonic[2] == 'c') { // 'jec'
            if (mnemonic[3] == 'x') { // 'jecx'
              if (mnemonic[4] == 'z') { // 'jecxz'
                {instruction = new SgAsmx86Jcxz(address,  mnemonic, ""); goto done;}
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'l': { // 'l'
        switch (mnemonic[1]) {
          case 'e': { // 'le'
            if (mnemonic[2] == 'a') { // 'lea'
              if (mnemonic[3] == 'v') { // 'leav'
                if (mnemonic[4] == 'e') { // 'leave'
                  {instruction = new SgAsmx86Leave(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'o': { // 'lo'
            switch (mnemonic[2]) {
              case 'd': { // 'lod'
                if (mnemonic[3] == 's') { // 'lods'
                  switch (mnemonic[4]) {
                    case 'b': { // 'lodsb'
                      {instruction = new SgAsmx86Lods(address,  mnemonic, ""); goto done;}
                    }
                    case 'd': { // 'lodsd'
                      {instruction = new SgAsmx86Lods(address,  mnemonic, ""); goto done;}
                    }
                    case 'w': { // 'lodsw'
                      {instruction = new SgAsmx86Lods(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'o': { // 'loo'
                if (mnemonic[3] == 'p') { // 'loop'
                  switch (mnemonic[4]) {
                    case 'e': { // 'loope'
                      {instruction = new SgAsmx86Loopz(address,  mnemonic, ""); goto done;}
                    }
                    case 'z': { // 'loopz'
                      {instruction = new SgAsmx86Loopz(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'm': { // 'm'
        if (mnemonic[1] == 'o') { // 'mo'
          if (mnemonic[2] == 'v') { // 'mov'
            switch (mnemonic[3]) {
              case 's': { // 'movs'
                switch (mnemonic[4]) {
                  case 'b': { // 'movsb'
                    {instruction = new SgAsmx86Movs(address,  mnemonic, ""); goto done;}
                  }
                  case 'd': { // 'movsd'
                    {instruction = new SgAsmx86Movs(address,  mnemonic, ""); goto done;}
                  }
                  case 'w': { // 'movsw'
                    {instruction = new SgAsmx86Movs(address,  mnemonic, ""); goto done;}
                  }
                  case 'x': { // 'movsx'
                    {instruction = new SgAsmx86Movsx(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              }
              case 'z': { // 'movz'
                if (mnemonic[4] == 'x') { // 'movzx'
                  {instruction = new SgAsmx86Movzx(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'u') { // 'ou'
          if (mnemonic[2] == 't') { // 'out'
            if (mnemonic[3] == 's') { // 'outs'
              switch (mnemonic[4]) {
                case 'b': { // 'outsb'
                  {instruction = new SgAsmx86Outs(address,  mnemonic, ""); goto done;}
                }
                case 'd': { // 'outsd'
                  {instruction = new SgAsmx86Outs(address,  mnemonic, ""); goto done;}
                }
                case 'w': { // 'outsw'
                  {instruction = new SgAsmx86Outs(address,  mnemonic, ""); goto done;}
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'p': { // 'p'
        switch (mnemonic[1]) {
          case 'o': { // 'po'
            if (mnemonic[2] == 'p') { // 'pop'
              switch (mnemonic[3]) {
                case 'a': { // 'popa'
                  if (mnemonic[4] == 'd') { // 'popad'
                    {instruction = new SgAsmx86Popa(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'f': { // 'popf'
                  if (mnemonic[4] == 'd') { // 'popfd'
                    {instruction = new SgAsmx86Popf(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'u': { // 'pu'
            if (mnemonic[2] == 's') { // 'pus'
              if (mnemonic[3] == 'h') { // 'push'
                switch (mnemonic[4]) {
                  case 'a': { // 'pusha'
                    {instruction = new SgAsmx86Pusha(address,  mnemonic, ""); goto done;}
                  }
                  case 'f': { // 'pushf'
                    {instruction = new SgAsmx86Pushf(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'r': { // 'r'
        switch (mnemonic[1]) {
          case 'd': { // 'rd'
            if (mnemonic[2] == 't') { // 'rdt'
              if (mnemonic[3] == 's') { // 'rdts'
                if (mnemonic[4] == 'c') { // 'rdtsc'
                  {instruction = new SgAsmx86Rdtsc(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'e': { // 're'
            if (mnemonic[2] == 'p') { // 'rep'
              if (mnemonic[3] == 'n') { // 'repn'
                switch (mnemonic[4]) {
                  case 'e': { // 'repne'
                    {instruction = new SgAsmx86Repne(address,  mnemonic, ""); goto done;}
                  }
                  case 'z': { // 'repnz'
                    {instruction = new SgAsmx86Repne(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'c': { // 'sc'
            if (mnemonic[2] == 'a') { // 'sca'
              if (mnemonic[3] == 's') { // 'scas'
                switch (mnemonic[4]) {
                  case 'b': { // 'scasb'
                    {instruction = new SgAsmx86Scas(address,  mnemonic, ""); goto done;}
                  }
                  case 'd': { // 'scasd'
                    {instruction = new SgAsmx86Scas(address,  mnemonic, ""); goto done;}
                  }
                  case 'w': { // 'scasw'
                    {instruction = new SgAsmx86Scas(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'e': { // 'se'
            if (mnemonic[2] == 't') { // 'set'
              switch (mnemonic[3]) {
                case 'a': { // 'seta'
                  if (mnemonic[4] == 'e') { // 'setae'
                    {instruction = new SgAsmx86Setb(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'b': { // 'setb'
                  if (mnemonic[4] == 'e') { // 'setbe'
                    {instruction = new SgAsmx86Setbe(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'g': { // 'setg'
                  if (mnemonic[4] == 'e') { // 'setge'
                    {instruction = new SgAsmx86Setge(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'l': { // 'setl'
                  if (mnemonic[4] == 'e') { // 'setle'
                    {instruction = new SgAsmx86Setle(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'n': { // 'setn'
                  switch (mnemonic[4]) {
                    case 'a': { // 'setna'
                      {instruction = new SgAsmx86Setbe(address,  mnemonic, ""); goto done;}
                    }
                    case 'b': { // 'setnb'
                      {instruction = new SgAsmx86Setae(address,  mnemonic, ""); goto done;}
                    }
                    case 'c': { // 'setnc'
                      {instruction = new SgAsmx86Setae(address,  mnemonic, ""); goto done;}
                    }
                    case 'e': { // 'setne'
                      {instruction = new SgAsmx86Setne(address,  mnemonic, ""); goto done;}
                    }
                    case 'g': { // 'setng'
                      {instruction = new SgAsmx86Setle(address,  mnemonic, ""); goto done;}
                    }
                    case 'l': { // 'setnl'
                      {instruction = new SgAsmx86Setge(address,  mnemonic, ""); goto done;}
                    }
                    case 'o': { // 'setno'
                      {instruction = new SgAsmx86Setno(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'setnp'
                      {instruction = new SgAsmx86Setpo(address,  mnemonic, ""); goto done;}
                    }
                    case 's': { // 'setns'
                      {instruction = new SgAsmx86Setns(address,  mnemonic, ""); goto done;}
                    }
                    case 'z': { // 'setnz'
                      {instruction = new SgAsmx86Setne(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                }
                case 'p': { // 'setp'
                  switch (mnemonic[4]) {
                    case 'e': { // 'setpe'
                      {instruction = new SgAsmx86Setpe(address,  mnemonic, ""); goto done;}
                    }
                    case 'o': { // 'setpo'
                      {instruction = new SgAsmx86Setpo(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'l': { // 'sl'
            if (mnemonic[2] == 'd') { // 'sld'
              if (mnemonic[3] == 't') { // 'sldt'
                if (mnemonic[4] == 'l') { // 'sldtl'
                  {instruction = new SgAsmx86Sldt(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 't': { // 'st'
            if (mnemonic[2] == 'o') { // 'sto'
              if (mnemonic[3] == 's') { // 'stos'
                switch (mnemonic[4]) {
                  case 'b': { // 'stosb'
                    {instruction = new SgAsmx86Stos(address,  mnemonic, ""); goto done;}
                  }
                  case 'd': { // 'stosd'
                    {instruction = new SgAsmx86Stos(address,  mnemonic, ""); goto done;}
                  }
                  case 'w': { // 'stosw'
                    {instruction = new SgAsmx86Stos(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'x': { // 'x'
        switch (mnemonic[1]) {
          case 'a': { // 'xa'
            if (mnemonic[2] == 'd') { // 'xad'
              if (mnemonic[3] == 'd') { // 'xadd'
                if (mnemonic[4] == 'l') { // 'xaddl'
                  {instruction = new SgAsmx86Xadd(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'o': { // 'xo'
            if (mnemonic[2] == 'r') { // 'xor'
              if (mnemonic[3] == 'p') { // 'xorp'
                switch (mnemonic[4]) {
                  case 'd': { // 'xorpd'
                    {instruction = new SgAsmx86Xorpd(address,  mnemonic, ""); goto done;}
                  }
                  case 's': { // 'xorps'
                    {instruction = new SgAsmx86Xorps(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
    }
  case 6:
    switch (mnemonic[0]) {
      case 'c': { // 'c'
        if (mnemonic[1] == 'm') { // 'cm'
          if (mnemonic[2] == 'o') { // 'cmo'
            if (mnemonic[3] == 'v') { // 'cmov'
              switch (mnemonic[4]) {
                case 'a': { // 'cmova'
                  if (mnemonic[5] == 'e') { // 'cmovae'
                    {instruction = new SgAsmx86CMovae(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'b': { // 'cmovb'
                  if (mnemonic[5] == 'e') { // 'cmovbe'
                    {instruction = new SgAsmx86CMovbe(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'g': { // 'cmovg'
                  if (mnemonic[5] == 'e') { // 'cmovge'
                    {instruction = new SgAsmx86CMovge(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'l': { // 'cmovl'
                  if (mnemonic[5] == 'e') { // 'cmovle'
                    {instruction = new SgAsmx86CMovle(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'n': { // 'cmovn'
                  switch (mnemonic[5]) {
                    case 'a': { // 'cmovna'
                      {instruction = new SgAsmx86CMovbe(address,  mnemonic, ""); goto done;}
                    }
                    case 'b': { // 'cmovnb'
                      {instruction = new SgAsmx86CMovae(address,  mnemonic, ""); goto done;}
                    }
                    case 'c': { // 'cmovnc'
                      {instruction = new SgAsmx86CMovnc(address,  mnemonic, ""); goto done;}
                    }
                    case 'e': { // 'cmovne'
                      {instruction = new SgAsmx86CMovne(address,  mnemonic, ""); goto done;}
                    }
                    case 'g': { // 'cmovng'
                      {instruction = new SgAsmx86CMovle(address,  mnemonic, ""); goto done;}
                    }
                    case 'l': { // 'cmovnl'
                      {instruction = new SgAsmx86CMovge(address,  mnemonic, ""); goto done;}
                    }
                    case 'o': { // 'cmovno'
                      {instruction = new SgAsmx86CMovno(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'cmovnp'
                      {instruction = new SgAsmx86CMovnp(address,  mnemonic, ""); goto done;}
                    }
                    case 's': { // 'cmovns'
                      {instruction = new SgAsmx86CMovns(address,  mnemonic, ""); goto done;}
                    }
                    case 'z': { // 'cmovnz'
                      {instruction = new SgAsmx86CMovne(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                }
                case 'p': { // 'cmovp'
                  switch (mnemonic[5]) {
                    case 'e': { // 'cmovpe'
                      {instruction = new SgAsmx86CMovp(address,  mnemonic, ""); goto done;}
                    }
                    case 'o': { // 'cmovpo'
                      {instruction = new SgAsmx86CMovnp(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'd': { // 'd'
        if (mnemonic[1] == 'a') { // 'da'
          if (mnemonic[2] == 't') { // 'dat'
            if (mnemonic[3] == 'a') { // 'data'
              if (mnemonic[4] == '1') { // 'data1'
                if (mnemonic[5] == '6') { // 'data16'
                  {instruction = new SgAsmx86Data16(address,  mnemonic, ""); goto done;}
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
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
                      {instruction = new SgAsmx86Fcomp(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'fcompp'
                      {instruction = new SgAsmx86Fcompp(address,  mnemonic, ""); goto done;}
                    }
                    case 's': { // 'fcomps'
                      {instruction = new SgAsmx86Fcomp(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'd': { // 'fd'
            if (mnemonic[2] == 'i') { // 'fdi'
              if (mnemonic[3] == 'v') { // 'fdiv'
                if (mnemonic[4] == 'r') { // 'fdivr'
                  switch (mnemonic[5]) {
                    case 'l': { // 'fdivrl'
                      {instruction = new SgAsmx86Fdivr(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'fdivrp'
                      {instruction = new SgAsmx86Fdivrp(address,  mnemonic, ""); goto done;}
                    }
                    case 's': { // 'fdivrs'
                      {instruction = new SgAsmx86Fdivr(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'i': { // 'fi'
            switch (mnemonic[2]) {
              case 'a': { // 'fia'
                if (mnemonic[3] == 'd') { // 'fiad'
                  if (mnemonic[4] == 'd') { // 'fiadd'
                    if (mnemonic[5] == 'l') { // 'fiaddl'
                      {instruction = new SgAsmx86Fiadd(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'd': { // 'fid'
                if (mnemonic[3] == 'i') { // 'fidi'
                  if (mnemonic[4] == 'v') { // 'fidiv'
                    if (mnemonic[5] == 'l') { // 'fidivl'
                      {instruction = new SgAsmx86Fidiv(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'm': { // 'fim'
                if (mnemonic[3] == 'u') { // 'fimu'
                  if (mnemonic[4] == 'l') { // 'fimul'
                    if (mnemonic[5] == 'l') { // 'fimull'
                      {instruction = new SgAsmx86Fimul(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 's': { // 'fis'
                if (mnemonic[3] == 't') { // 'fist'
                  if (mnemonic[4] == 'p') { // 'fistp'
                    if (mnemonic[5] == 'l') { // 'fistpl'
                      {instruction = new SgAsmx86Fistp(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'l': { // 'fl'
            if (mnemonic[2] == 'd') { // 'fld'
              if (mnemonic[3] == 'l') { // 'fldl'
                switch (mnemonic[4]) {
                  case '2': { // 'fldl2'
                    switch (mnemonic[5]) {
                      case 'e': { // 'fldl2e'
                        {instruction = new SgAsmx86Fldl2e(address,  mnemonic, ""); goto done;}
                      }
                      case 't': { // 'fldl2t'
                        {instruction = new SgAsmx86Fldl2t(address,  mnemonic, ""); goto done;}
                      }
                      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                    }
                  }
                  case 'g': { // 'fldlg'
                    if (mnemonic[5] == '2') { // 'fldlg2'
                      {instruction = new SgAsmx86Fldlg2(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  }
                  case 'n': { // 'fldln'
                    if (mnemonic[5] == '2') { // 'fldln2'
                      {instruction = new SgAsmx86Fldln2(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'n': { // 'fn'
            if (mnemonic[2] == 's') { // 'fns'
              if (mnemonic[3] == 't') { // 'fnst'
                switch (mnemonic[4]) {
                  case 'c': { // 'fnstc'
                    if (mnemonic[5] == 'w') { // 'fnstcw'
                      {instruction = new SgAsmx86Fnstcw(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  }
                  case 's': { // 'fnsts'
                    if (mnemonic[5] == 'w') { // 'fnstsw'
                      {instruction = new SgAsmx86Fnstsw(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'p': { // 'fp'
            if (mnemonic[2] == 'a') { // 'fpa'
              if (mnemonic[3] == 't') { // 'fpat'
                if (mnemonic[4] == 'a') { // 'fpata'
                  if (mnemonic[5] == 'n') { // 'fpatan'
                    {instruction = new SgAsmx86Fpatan(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 's': { // 'fs'
            switch (mnemonic[2]) {
              case 'c': { // 'fsc'
                if (mnemonic[3] == 'a') { // 'fsca'
                  if (mnemonic[4] == 'l') { // 'fscal'
                    if (mnemonic[5] == 'e') { // 'fscale'
                      {instruction = new SgAsmx86Fscale(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              case 'u': { // 'fsu'
                if (mnemonic[3] == 'b') { // 'fsub'
                  if (mnemonic[4] == 'r') { // 'fsubr'
                    switch (mnemonic[5]) {
                      case 'l': { // 'fsubrl'
                        {instruction = new SgAsmx86Fsubr(address,  mnemonic, ""); goto done;}
                      }
                      case 'p': { // 'fsubrp'
                        {instruction = new SgAsmx86Fsubrp(address,  mnemonic, ""); goto done;}
                      }
                      case 's': { // 'fsubrs'
                        {instruction = new SgAsmx86Fsubr(address,  mnemonic, ""); goto done;}
                      }
                      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              }
              default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
            }
          }
          case 'u': { // 'fu'
            if (mnemonic[2] == 'c') { // 'fuc'
              if (mnemonic[3] == 'o') { // 'fuco'
                if (mnemonic[4] == 'm') { // 'fucom'
                  switch (mnemonic[5]) {
                    case 'i': { // 'fucomi'
                      {instruction = new SgAsmx86Fucom(address,  mnemonic, ""); goto done;}
                    }
                    case 'p': { // 'fucomp'
                      {instruction = new SgAsmx86Fucomp(address,  mnemonic, ""); goto done;}
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'x': { // 'fx'
            if (mnemonic[2] == 's') { // 'fxs'
              if (mnemonic[3] == 'a') { // 'fxsa'
                if (mnemonic[4] == 'v') { // 'fxsav'
                  if (mnemonic[5] == 'e') { // 'fxsave'
                    {instruction = new SgAsmx86Fxsave(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'o') { // 'lo'
          if (mnemonic[2] == 'o') { // 'loo'
            if (mnemonic[3] == 'p') { // 'loop'
              if (mnemonic[4] == 'n') { // 'loopn'
                switch (mnemonic[5]) {
                  case 'e': { // 'loopne'
                    {instruction = new SgAsmx86Loopnz(address,  mnemonic, ""); goto done;}
                  }
                  case 'z': { // 'loopnz'
                    {instruction = new SgAsmx86Loopnz(address,  mnemonic, ""); goto done;}
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'p': { // 'p'
        if (mnemonic[1] == 'u') { // 'pu'
          if (mnemonic[2] == 's') { // 'pus'
            if (mnemonic[3] == 'h') { // 'push'
              switch (mnemonic[4]) {
                case 'a': { // 'pusha'
                  if (mnemonic[5] == 'd') { // 'pushad'
                    {instruction = new SgAsmx86Pusha(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                case 'f': { // 'pushf'
                  if (mnemonic[5] == 'd') { // 'pushfd'
                    {instruction = new SgAsmx86Pushf(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                }
                default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'c': { // 'sc'
            if (mnemonic[2] == 'l') { // 'scl'
              if (mnemonic[3] == 'a') { // 'scla'
                if (mnemonic[4] == 't') { // 'sclat'
                  if (mnemonic[5] == 'b') { // 'sclatb'
                    {instruction = new SgAsmx86Xlat(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          case 'e': { // 'se'
            if (mnemonic[2] == 't') { // 'set'
              if (mnemonic[3] == 'n') { // 'setn'
                switch (mnemonic[4]) {
                  case 'b': { // 'setnb'
                    if (mnemonic[5] == 'e') { // 'setnbe'
                      {instruction = new SgAsmx86Seta(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  }
                  case 'g': { // 'setng'
                    if (mnemonic[5] == 'e') { // 'setnge'
                      {instruction = new SgAsmx86Setl(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  }
                  case 'l': { // 'setnl'
                    if (mnemonic[5] == 'e') { // 'setnle'
                      {instruction = new SgAsmx86Setg(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  }
                  default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          }
          default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
        }
      }
      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
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
                        {instruction = new SgAsmx86CMovb(address,  mnemonic, ""); goto done;}
                      } else {
                        instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                      }
                    }
                    case 'b': { // 'cmovnb'
                      if (mnemonic[6] == 'e') { // 'cmovnbe'
                        {instruction = new SgAsmx86CMova(address,  mnemonic, ""); goto done;}
                      } else {
                        instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                      }
                    }
                    case 'g': { // 'cmovng'
                      if (mnemonic[6] == 'e') { // 'cmovnge'
                        {instruction = new SgAsmx86CMovl(address,  mnemonic, ""); goto done;}
                      } else {
                        instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                      }
                    }
                    case 'l': { // 'cmovnl'
                      if (mnemonic[6] == 'e') { // 'cmovnle'
                        {instruction = new SgAsmx86CMovg(address,  mnemonic, ""); goto done;}
                      } else {
                        instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                      }
                    }
                    default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            }
            case 'p': { // 'cmp'
              if (mnemonic[3] == 'x') { // 'cmpx'
                if (mnemonic[4] == 'c') { // 'cmpxc'
                  if (mnemonic[5] == 'h') { // 'cmpxch'
                    if (mnemonic[6] == 'g') { // 'cmpxchg'
                      {instruction = new SgAsmx86Cmpxchg(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            }
            default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      case 'f': { // 'f'
        if (mnemonic[1] == 'u') { // 'fu'
          if (mnemonic[2] == 'c') { // 'fuc'
            if (mnemonic[3] == 'o') { // 'fuco'
              if (mnemonic[4] == 'm') { // 'fucom'
                if (mnemonic[5] == 'p') { // 'fucomp'
                  if (mnemonic[6] == 'p') { // 'fucompp'
                    {instruction = new SgAsmx86Fucompp(address,  mnemonic, ""); goto done;}
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      }
      default: {std::cerr << "ERROR ::: Found bad instruction name '" << mnemonic << "'\n"; /*abort();*/}
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
                      {instruction = new SgAsmx86Cmpxchg8b(address,  mnemonic, ""); goto done;}
                    } else {
                      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                    }
                  } else {
                    instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                  }
                } else {
                  instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
                }
              } else {
                instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
              }
            } else {
              instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
            }
          } else {
            instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
          }
        } else {
          instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
        }
      } else {
        instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
      }
    } else {
      instruction = new SgAsmx86Instruction(address,  mnemonic, ""); goto done;
    }
  default: instruction = new SgAsmx86Instruction(address,  mnemonic, "");
}
done: ;

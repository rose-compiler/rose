switch (mnemonic.size()) {
  case 1:
    if (mnemonic[0] == 'b') { // 'b'
      {instruction = new SgAsmArmB(address,  mnemonic, ""); goto done_arm;}
    } else {
      instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
    }
  case 2:
    if (mnemonic[0] == 'b') { // 'b'
      switch (mnemonic[1]) {
        case 'l': { // 'bl'
          {instruction = new SgAsmArmBl(address,  mnemonic, ""); goto done_arm;}
        }
        case 'x': { // 'bx'
          {instruction = new SgAsmArmBx(address,  mnemonic, ""); goto done_arm;}
        }
        default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
      }
    } else {
      instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
    }
  case 3:
    switch (mnemonic[0]) {
      case 'a': { // 'a'
        switch (mnemonic[1]) {
          case 'd': { // 'ad'
            switch (mnemonic[2]) {
              case 'c': { // 'adc'
                {instruction = new SgAsmArmAdc(address,  mnemonic, ""); goto done_arm;}
              }
              case 'd': { // 'add'
                {instruction = new SgAsmArmAdd(address,  mnemonic, ""); goto done_arm;}
              }
              default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 'n': { // 'an'
            if (mnemonic[2] == 'd') { // 'and'
              {instruction = new SgAsmArmAnd(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'b': { // 'b'
        if (mnemonic[1] == 'i') { // 'bi'
          if (mnemonic[2] == 'c') { // 'bic'
            {instruction = new SgAsmArmBic(address,  mnemonic, ""); goto done_arm;}
          } else {
            instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'c': { // 'c'
        if (mnemonic[1] == 'm') { // 'cm'
          switch (mnemonic[2]) {
            case 'n': { // 'cmn'
              {instruction = new SgAsmArmCmn(address,  mnemonic, ""); goto done_arm;}
            }
            case 'p': { // 'cmp'
              {instruction = new SgAsmArmCmp(address,  mnemonic, ""); goto done_arm;}
            }
            default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'e': { // 'e'
        if (mnemonic[1] == 'o') { // 'eo'
          if (mnemonic[2] == 'r') { // 'eor'
            {instruction = new SgAsmArmEor(address,  mnemonic, ""); goto done_arm;}
          } else {
            instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'l': { // 'l'
        if (mnemonic[1] == 'd') { // 'ld'
          switch (mnemonic[2]) {
            case 'm': { // 'ldm'
              {instruction = new SgAsmArmLdm(address,  mnemonic, ""); goto done_arm;}
            }
            case 'r': { // 'ldr'
              {instruction = new SgAsmArmLdr(address,  mnemonic, ""); goto done_arm;}
            }
            default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'm': { // 'm'
        switch (mnemonic[1]) {
          case 'l': { // 'ml'
            if (mnemonic[2] == 'a') { // 'mla'
              {instruction = new SgAsmArmMla(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 'o': { // 'mo'
            if (mnemonic[2] == 'v') { // 'mov'
              {instruction = new SgAsmArmMov(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 'r': { // 'mr'
            if (mnemonic[2] == 's') { // 'mrs'
              {instruction = new SgAsmArmMrs(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 's': { // 'ms'
            if (mnemonic[2] == 'r') { // 'msr'
              {instruction = new SgAsmArmMsr(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 'u': { // 'mu'
            if (mnemonic[2] == 'l') { // 'mul'
              {instruction = new SgAsmArmMul(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 'v': { // 'mv'
            if (mnemonic[2] == 'n') { // 'mvn'
              {instruction = new SgAsmArmMvn(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'o': { // 'o'
        if (mnemonic[1] == 'r') { // 'or'
          if (mnemonic[2] == 'r') { // 'orr'
            {instruction = new SgAsmArmOrr(address,  mnemonic, ""); goto done_arm;}
          } else {
            instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'r': { // 'r'
        if (mnemonic[1] == 's') { // 'rs'
          switch (mnemonic[2]) {
            case 'b': { // 'rsb'
              {instruction = new SgAsmArmRsb(address,  mnemonic, ""); goto done_arm;}
            }
            case 'c': { // 'rsc'
              {instruction = new SgAsmArmRsc(address,  mnemonic, ""); goto done_arm;}
            }
            default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 's': { // 's'
        switch (mnemonic[1]) {
          case 'b': { // 'sb'
            if (mnemonic[2] == 'c') { // 'sbc'
              {instruction = new SgAsmArmSbc(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 't': { // 'st'
            switch (mnemonic[2]) {
              case 'm': { // 'stm'
                {instruction = new SgAsmArmStm(address,  mnemonic, ""); goto done_arm;}
              }
              case 'r': { // 'str'
                {instruction = new SgAsmArmStr(address,  mnemonic, ""); goto done_arm;}
              }
              default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 'u': { // 'su'
            if (mnemonic[2] == 'b') { // 'sub'
              {instruction = new SgAsmArmSub(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 'w': { // 'sw'
            switch (mnemonic[2]) {
              case 'i': { // 'swi'
                {instruction = new SgAsmArmSwi(address,  mnemonic, ""); goto done_arm;}
              }
              case 'p': { // 'swp'
                {instruction = new SgAsmArmSwp(address,  mnemonic, ""); goto done_arm;}
              }
              default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 't': { // 't'
        switch (mnemonic[1]) {
          case 'e': { // 'te'
            if (mnemonic[2] == 'q') { // 'teq'
              {instruction = new SgAsmArmTeq(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          case 's': { // 'ts'
            if (mnemonic[2] == 't') { // 'tst'
              {instruction = new SgAsmArmTst(address,  mnemonic, ""); goto done_arm;}
            } else {
              instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
            }
          }
          default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
    }
  case 5:
    switch (mnemonic[0]) {
      case 's': { // 's'
        if (mnemonic[1] == 'm') { // 'sm'
          switch (mnemonic[2]) {
            case 'l': { // 'sml'
              if (mnemonic[3] == 'a') { // 'smla'
                if (mnemonic[4] == 'l') { // 'smlal'
                  {instruction = new SgAsmArmSmlal(address,  mnemonic, ""); goto done_arm;}
                } else {
                  instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
                }
              } else {
                instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
              }
            }
            case 'u': { // 'smu'
              if (mnemonic[3] == 'l') { // 'smul'
                if (mnemonic[4] == 'l') { // 'smull'
                  {instruction = new SgAsmArmSmull(address,  mnemonic, ""); goto done_arm;}
                } else {
                  instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
                }
              } else {
                instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
              }
            }
            default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      case 'u': { // 'u'
        if (mnemonic[1] == 'm') { // 'um'
          switch (mnemonic[2]) {
            case 'l': { // 'uml'
              if (mnemonic[3] == 'a') { // 'umla'
                if (mnemonic[4] == 'l') { // 'umlal'
                  {instruction = new SgAsmArmUmlal(address,  mnemonic, ""); goto done_arm;}
                } else {
                  instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
                }
              } else {
                instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
              }
            }
            case 'u': { // 'umu'
              if (mnemonic[3] == 'l') { // 'umul'
                if (mnemonic[4] == 'l') { // 'umull'
                  {instruction = new SgAsmArmUmull(address,  mnemonic, ""); goto done_arm;}
                } else {
                  instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
                }
              } else {
                instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
              }
            }
            default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
          }
        } else {
          instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
        }
      }
      default: instruction = new SgAsmInstruction(address,  mnemonic, ""); goto done_arm;
    }
  default: instruction = new SgAsmInstruction(address,  mnemonic, "");
}
done_arm: ;

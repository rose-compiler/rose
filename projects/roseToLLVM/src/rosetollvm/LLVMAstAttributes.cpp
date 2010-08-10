#include <complex>
#include <rosetollvm/LLVMAstAttributes.h>

using namespace std;

const string LLVMAstAttributes::getTemp(TEMP_KIND k) {
    stringstream out;
    switch(k)
    {
        case TEMP_INT:
            out << "%";
            break;
        case TEMP_INCREMENT:
            out << "%.incr";
            break;
        case TEMP_DECREMENT:
            out << "%.decr";
            break;
        case TEMP_NEGATION:
            out << "%.neg";
            break;
        case TEMP_ADD:
            out << "%.add";
            break;
        case TEMP_ADD_PTR:
            out << "%.add.ptr";
            break;
        case TEMP_SUBTRACT:
            out << "%.sub";
            break;
        case TEMP_SUBTRACT_PTR:
            out << "%.sub.ptr";
            break;
        case TEMP_MULTIPLY:
            out << "%.mul";
            break;
        case TEMP_DIVIDE:
            out << "%.div";
            break;
        case TEMP_REMAINDER:
            out << "%.rem";
            break;
        case TEMP_COMPARE:
            out << "%.cmp";
            break;
        case TEMP_ARRAY:
            out << "%.arraydecay";
            break;
        case TEMP_ARRAY_INDEX:
            out << "%.arrayidx";
            break;
        case TEMP_CALL:
            out << "%.call";
            break;
        case TEMP_IF_TRUE: 
            out << "if.true";
            break;
        case TEMP_IF_FALSE:
            out << "if.false";
            break;
        case TEMP_IF_END:
            out << "if.end";
            break;
        case TEMP_WHILE_CONDITION:
            out << "while.cond";
            break;
        case TEMP_WHILE_BODY:
            out << "while.body";
            break;
        case TEMP_WHILE_END:
            out << "while.end";
            break;
        case TEMP_DO_CONDITION:
            out << "do.cond";
            break;
        case TEMP_DO_BODY:
            out << "do.body";
            break;
        case TEMP_DO_END:
            out << "do.end";
            break;
        case TEMP_FOR_CONDITION:
            out << "for.cond";
            break;
        case TEMP_FOR_BODY:
            out << "for.body";
            break;
        case TEMP_FOR_INCREMENT:
            out << "for.inc";
            break;
        case TEMP_FOR_END:
            out << "for.end";
            break;
        case TEMP_SWITCH_END_LABEL:
            out << "sw.end";
            break;
        case TEMP_CASE_LABEL:
            out << "sw.case";
            break;
        case TEMP_DEFAULT_LABEL:
            out << "sw.default";
            break;
        case TEMP_RETURN:
            out << "%.return";
            break;
        case TEMP_CONVERT:
            out << "%.conv";
            break;
        case TEMP_NOT:
            out << "%.not";
            break;
        case TEMP_AND:
            out << "%.and";
            break;
        case TEMP_OR:
            out << "%.or";
            break;
        case TEMP_XOR:
            out << "%.xor";
            break;
        case TEMP_COND:
            out << "%.cond";
            break;
        case TEMP_COND_TRUE: 
            out << "cond.true";
            break;
        case TEMP_COND_FALSE:
            out << "cond.false";
            break;
        case TEMP_COND_END:
            out << "cond.end";
            break;
        case TEMP_LOGICAL_AND_RHS:
            out << "land.rhs";
            break;
        case TEMP_LOGICAL_AND_END:
            out << "land.end";
            break;
        case TEMP_LOGICAL_OR_RHS:
            out << "lor.rhs";
            break;
        case TEMP_LOGICAL_OR_END:
            out << "lor.end";
            break;
        case TEMP_SHIFT_LEFT:
            out << "%.shl";
            break;
        case TEMP_SHIFT_RIGHT:
            out << "%.shr";
            break;
        case TEMP_ANONYMOUS:
            out << "%union.anon";
            break;
        case TEMP_LHS_POINTER_TO_INT:
            out << "%sub.ptr.lhs.cast";
            break;
        case TEMP_RHS_POINTER_TO_INT:
            out << "%sub.ptr.rhs.cast";
            break;
        case TEMP_COERCE:
            out << "%.coerce";
            break;
        case TEMP_AGGREGATE:
            out << "%.agg.tmp";
            break;
        case TEMP_POINTER_DIFFERENCE_ARITHMETIC_SHIFT_RIGHT:
            out << "%sub.ptr.shr";
            break;
        case TEMP_POINTER_DIFFERENCE_DIVISION:
            out << "%sub.ptr.div";
            break;
        default:
            out << "%.tmp";
            break;
    }
    out << (k == TEMP_INT ? tmp_int_count++ : tmp_count++);
    return out.str();
}

const string LLVMAstAttributes::getFunctionTemp(string function_name, string var_name) {
    stringstream out;
    out << "@" << function_name << "." << var_name << tmp_count++;
    return out.str();
}

const string LLVMAstAttributes::getFunctionName(SgFunctionSymbol *sym) {
     string function_name = "@";
     function_name.append(sym -> get_name().getString());
     return function_name;
}

const string LLVMAstAttributes::getFunctionSignature(SgFunctionSymbol *sym) {
     stringstream out;
 
     SgFunctionType *function_type = isSgFunctionType(sym -> get_type());
     assert(function_type);
     SgType *return_type = function_type -> get_return_type();
     if (isSgClassType(getSourceType(return_type))) {
         int size = integralStructureType(return_type);
         if (size == 0) {
             out << "void";
         }
         else {
             out << "i" << (size * 8);
         }
     }
     else {
         out << ((StringAstAttribute *) return_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
     }
     out << " " << getFunctionName(sym);
     out << ((StringAstAttribute *) function_type -> getAttribute(Control::LLVM_FUNCTION_ARGUMENTS)) -> getValue();

     return out.str();
}

const string LLVMAstAttributes::getGlobalStringConstantName(int index) {
    stringstream out;
    out << "@\"\\01LC" << index << "\"";
    return out.str();
}

const string LLVMAstAttributes::getGlobalStringReference(int index) {
    const char *data = getString(index);
    stringstream out;
    out << "getelementptr ([" << getStringLength(index) << " x i8]* @\"\\01LC" << index << "\", i32 0, i32 0)";
    return out.str();
}

const string LLVMAstAttributes::filter(const string in) {
    stringstream out;
    for (int i = 0; i < in.length(); i++) {
        out << in[i];
        if (in[i] == '\\') {
            if (in[i + 1] == 'n') {
                i++;
                out << "0A";
            }
            else if (in[i + 1] == 't') {
                i++;
                out << (hex) << ((int) '\t');
            }
            else if (in[i + 1] == 'b') {
                i++;
                out << (hex) << ((int) '\b');
            }
            else if (in[i + 1] == 'r') {
                i++;
                out << (hex) << ((int) '\r');
            }
            else if (in[i + 1] == 'f') {
                i++;
                out << (hex) << ((int) '\f');
            }
            else if (in[i + 1] == 'v') {
                i++;
                out << (hex) << ((int) '\v');
            }
            else if (in[i + 1] == 'a') {
                i++;
                out << (hex) << ((int) '\a');
            }
            else if (in[i + 1] == '?') {
                i++;
                out << (hex) << ((int) '\?');
            }
            else if (in[i + 1] == '\"' || in[i + 1] == '\'' || in[i + 1] == '\\') {
                i++;
                out << (hex) << ((int) in[i]);
            }
        }
    }

    out << "\\00";
    return out.str();
}

const string LLVMAstAttributes::filter(const string in, int size) {
    string out = filter(in);
    for (int i = in.size() + 1; i < size; i++) { // +1 because filter() already added a \0 gate to the string.
        out += "\\00";
    }
    return out;
}

int LLVMAstAttributes::getLength(const char *in) {
    int length = 0;
    for (const char *p = in; *p; p++) {
        length++;
        if (*p == '\\' && isHex(p[1]) && isHex(p[2]))
            p += 2;
    }

    return length;
}

const string LLVMAstAttributes::setLLVMTypeName(SgType *type) {
    StringAstAttribute *attr = (StringAstAttribute *) type -> getAttribute(Control::LLVM_TYPE);
    string str;
    int size;

    assert(type != NULL);

    if (attr != NULL) {
        str = attr -> getValue();
    }
    else {
        if (dynamic_cast<SgTypeVoid *>(type)) {
            str = "void";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(0));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
        }
        else if (dynamic_cast<SgTypeBool *>(type)) {
            str = "i1";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(1));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(1));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeChar *>(type) || dynamic_cast<SgTypeSignedChar *>(type) || dynamic_cast<SgTypeUnsignedChar *>(type)) {
            size = sizeof(char); // compute the size of a char on this machine.
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(1));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeShort *>(type) || dynamic_cast<SgTypeUnsignedShort *>(type)) {
            size = sizeof(short); // compute the size of a short on this machine.
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeInt *>(type) || dynamic_cast<SgTypeUnsignedInt *>(type) || dynamic_cast<SgEnumType *>(type)) {
            size = sizeof(int);
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeLong *>(type) ||
                 dynamic_cast<SgTypeUnsignedLong *>(type)) {
            size = sizeof(long);
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeLongLong *>(type) || dynamic_cast<SgTypeUnsignedLongLong *>(type)) {
            size = sizeof(long long);
            str = getIntegerBitSize(size);
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0"));
        }
        else if (dynamic_cast<SgTypeFloat *>(type)) {
            size = sizeof(float);
            str = "float";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0.0e+00"));
        }
        else if (dynamic_cast<SgTypeDouble *>(type)) {
            size = sizeof(double);
            str = "double";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0.0e+00"));
        }
        else if (dynamic_cast<SgTypeLongDouble *>(type)) {
            size = sizeof(long double);
            str = "x86_fp80";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0.0e+00"));
        }
        else if (dynamic_cast<SgTypeComplex *>(type)) {
            SgTypeComplex* complex_type = isSgTypeComplex(type);
            SgType *component_type = complex_type -> get_base_type();
            string component_type_name = setLLVMTypeName(component_type);
            if (isSgTypeFloat(component_type)) {
                size = sizeof(complex<float>);
            }
            else if (isSgTypeDouble(component_type)) {
                size = sizeof(complex<double>);
            }
            else if (isSgTypeLongDouble(component_type)) {
                size = sizeof(complex<long double>);
            }
            else {
                assert(! "know how to process this kind of complex type");
            }
            str = "complex";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("0.0e+00"));
            assert(! "yet support complex type");
        }
        else if (dynamic_cast<SgArrayType *>(type)) {
            SgArrayType* array_type = isSgArrayType(type);
            SgType *element_type = array_type -> get_base_type();
            string element_type_name = setLLVMTypeName(element_type);
            /**
             *
             *
             *
             *
             *            size_t array_size = SageInterface::getArrayElementCount (array_type);
             */
            SgUnsignedLongVal *specified_size = isSgUnsignedLongVal(array_type -> get_index());
            size_t array_size = (specified_size ? specified_size -> get_value() : 1); // compute number of elements in this array.
            int element_size = ((IntAstAttribute *) element_type -> getAttribute(Control::LLVM_SIZE)) -> getValue();
            std::ostringstream out;
            out << "[" << array_size << " x " << element_type_name << "]";
            str = out.str();
            size = array_size * element_size;
            control.SetAttribute(type, Control::LLVM_AGGREGATE, new AggregateAstAttribute(array_type));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) element_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("null"));
        }
        else if (dynamic_cast<SgPointerType *>(type)) {
            SgPointerType *pointer_type = isSgPointerType(type);
            size = sizeof(void *);
            str = (isSgTypeVoid(getSourceType(pointer_type -> get_base_type())) ? "i8" : setLLVMTypeName(pointer_type -> get_base_type()));
            str += "*";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute("null"));
        }
        else if (dynamic_cast<SgFunctionType *>(type)) {
            SgFunctionType *n = isSgFunctionType(type);
            SgType *return_type = n -> get_return_type();
            assert(return_type);
            assert(return_type != type);
            string return_type_name = setLLVMTypeName(return_type);
            if (isSgClassType(getSourceType(return_type))) {
                int size = integralStructureType(return_type);
                if (size == 0) {
                    str = "void";
                }
                else {
                    stringstream out;
                    out << "i" << (size * 8);
                    str = out.str();
                }
            }
            else {
                str = return_type_name;
            }

            string args_signature = "(";
            vector<SgType *> arg_type = n -> get_arguments();
            if (isSgClassType(getSourceType(return_type)) && integralStructureType(return_type) == 0) {
                args_signature.append(return_type_name);
                args_signature.append("* noalias sret");
                if (arg_type.size() > 0)
                    args_signature.append(", ");
            }
            for (int i = 0; i < arg_type.size(); i++) {
                 SgType *type = arg_type[i];
                 string parm_type_name = setLLVMTypeName(type);
                 SgArrayType *array_type = isSgArrayType(type);
                 if (array_type) { // convert array argument into a pointer.
                     parm_type_name = ((StringAstAttribute *) array_type -> get_base_type() -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                     parm_type_name += "*";
                 }
                 else if (isSgClassType(type)) { // only a pointer to a structure is passed.
                     parm_type_name += "*";
                 }

                args_signature.append(parm_type_name);
                if (i + 1 < arg_type.size())
                    args_signature.append(", ");
            }

            /**
             * If a compiler-generated function type declaration does not have a final argument with type SgTypeEllipsis, add that final argument
             * so that any function call to that function will look legitimate.
             */
            if (n -> attributeExists(Control::LLVM_COMPILER_GENERATED) && (arg_type.size() == 0 || (! isSgTypeEllipse(arg_type[arg_type.size() - 1])))) {
                args_signature.append("...");
            }

            args_signature.append(")");
            control.SetAttribute(type, Control::LLVM_FUNCTION_ARGUMENTS, new StringAstAttribute(args_signature));

            str.append(" ");
            str.append(args_signature);
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
        }
        else if (dynamic_cast<SgTypeString *>(type)) { // This type is only used internally by Rose.
            size = sizeof(void *);
            str = "i8 *";
            control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(size));
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
        }
        else if (dynamic_cast<SgTypeEllipse *>(type)) {
            str = "...";
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
        }
        else if (dynamic_cast<SgModifierType *>(type)) {
            SgModifierType *n = isSgModifierType(type);
            SgType *base_type = n -> get_base_type();
            str = setLLVMTypeName(base_type);
            /**
             * TODO: probably need a more systematic way to "inherit" all the type attributes from another type?
             */
           control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            if (base_type -> attributeExists(Control::LLVM_SIZE))
                control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()));
            if (base_type -> attributeExists(Control::LLVM_ALIGN_TYPE))
                control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            if (base_type -> attributeExists(Control::LLVM_DEFAULT_VALUE))
                control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue()));
            if (base_type -> attributeExists(Control::LLVM_FUNCTION_ARGUMENTS))
                control.SetAttribute(type, Control::LLVM_FUNCTION_ARGUMENTS, new StringAstAttribute(((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue()));
        }
        else if (dynamic_cast<SgTypedefType *>(type)) {
            SgTypedefType *n = isSgTypedefType(type);
            SgType *base_type = n -> get_base_type();
            str = setLLVMTypeName(base_type);

            /**
             * TODO: probably need a more systematic way to "inherit" all the type attributes from another type?
             */
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
            if (base_type -> attributeExists(Control::LLVM_SIZE))
                control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()));
            if (base_type -> attributeExists(Control::LLVM_ALIGN_TYPE))
                control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            if (base_type -> attributeExists(Control::LLVM_DEFAULT_VALUE))
                control.SetAttribute(type, Control::LLVM_DEFAULT_VALUE, new StringAstAttribute(((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue()));
            if (base_type -> attributeExists(Control::LLVM_FUNCTION_ARGUMENTS))
                control.SetAttribute(type, Control::LLVM_FUNCTION_ARGUMENTS, new StringAstAttribute(((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_DEFAULT_VALUE)) -> getValue()));
        }
        else if (dynamic_cast<SgClassType *>(type)) {
            SgClassType *n = isSgClassType(type);

            /**
             * TODO: Remove this when bug is fixed!
             *
             * There is a bug in Rose that causes multiple nodes to be allocated for a class type.  We test for the duplicate case here and produce the correct result for it.
             */
            if (class_map[n -> get_qualified_name().getString()] != NULL) {
                SgClassType *base_type = class_map[n -> get_qualified_name().getString()] -> getClassType();
                str = ((StringAstAttribute *) base_type -> getAttribute(Control::LLVM_TYPE)) -> getValue();
                control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
                control.SetAttribute(type, Control::LLVM_SIZE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_SIZE)) -> getValue()));
                control.SetAttribute(type, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(((IntAstAttribute *) base_type -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue()));
            }
            else {
                SgClassDeclaration *decl = (SgClassDeclaration *) n -> get_declaration();
                if (decl -> get_class_type() == SgClassDeclaration::e_union) {
                    if (decl -> get_isUnNamed())
                        str = getTemp(TEMP_ANONYMOUS);
                    else {
                        str = "%union.";
                        // TODO: Add name of enclosing scope here to make sure that there are no name collisions !
                        str += n -> get_name().getString();
                    }
                }
                else {
                    str = "%struct.";
                    // TODO: Add name of enclosing scope here to make sure that there are no name collisions !
                    str += n -> get_name().getString();
                }
                control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
                processClassDeclaration(n);
            }
        }
        else if (dynamic_cast<SgTypeDefault *>(type)) { // This type is only used internally by Rose.
            SgTypeDefault *n = isSgTypeDefault(type);
            str = type -> class_name(); // Just do something, anything!
            control.SetAttribute(type, Control::LLVM_TYPE, new StringAstAttribute(str));
        }
        else {
             cerr << "Don't know how to process type: " << type -> class_name();
             SgNamedType *named_type = isSgNamedType(type);
             if (named_type)
                 cerr << " - " << named_type -> get_name().getString();
             cerr << endl;
             assert(0);
        }
    }

    return str;
}


void LLVMAstAttributes::processClassDeclaration(SgClassType *n)
{
    DeclarationsAstAttribute *attribute = new DeclarationsAstAttribute(n);

    assert(class_map[n -> get_qualified_name().getString()] == NULL);
    class_map[n -> get_qualified_name().getString()] = attribute;

    SgClassDeclaration *declaration = isSgClassDeclaration(n -> get_declaration()),
                       *defining_declaration = isSgClassDeclaration(declaration -> get_definingDeclaration());

    if (! defining_declaration) {
         insertGlobalDeclaration(declaration);
         control.SetAttribute(declaration, Control::LLVM_LOCAL_DECLARATIONS, attribute); // needed because of Rose bug!
         control.SetAttribute(n, Control::LLVM_SIZE, new IntAstAttribute(0)); // an (LLVM) opaque type!
    }
    else {
        /**
         * TODO: Need to compute size of structure (sizeof) and its alignment (alignment of first field for structure,
         * largest alignment for union.
         * NOTE: there is a function called get_packingAlignment(). However, it always returns 0.
         */
        int size = 0,
            alignment = 0;

        vector<SgType *> pointer_decls;

        insertGlobalDeclaration(defining_declaration);
        control.SetAttribute(defining_declaration, Control::LLVM_LOCAL_DECLARATIONS, attribute); // needed because of Rose bug!
        SgClassDefinition *def = isSgClassDefinition(defining_declaration -> get_definition());
        assert(def);
        vector<SgDeclarationStatement *> decls = def -> get_members();
        for (int i = 0; i < decls.size(); i++) {
            if (dynamic_cast<SgVariableDeclaration *>(decls[i])) {
                vector<SgInitializedName *> vars = isSgVariableDeclaration(decls[i]) -> get_variables();
                for (int k = 0; k < vars.size(); k++) {
                    SgInitializedName *var = vars[k];

                    size_t field_size,
                           field_alignment;

                    /**
                     * If we are dealing with a pointer, we compute its size here instead of making a recursive call so setLLVMType.
                     * That is because the pointer could have a base type that involves this class and since the definition of the
                     * type is pending, the base type cannot yet be processed.  One particular hard case is a pointer to a function
                     *  that takes this class as parameter or returns an object of this class type.
                     */
                    if (isSgPointerType(getSourceType(var -> get_type()))) {
                        pointer_decls.push_back(var -> get_type());
                        field_size = sizeof(void *);
                        field_alignment = sizeof(void *);
                    }
                    else {
                        setLLVMTypeName(var -> get_type());
                        field_size = ((IntAstAttribute *) var -> get_type() -> getAttribute(Control::LLVM_SIZE)) -> getValue(),
                        field_alignment = ((IntAstAttribute *) var -> get_type() -> getAttribute(Control::LLVM_ALIGN_TYPE)) -> getValue();
                    }

                    /**
                     * The structure inherits the alignment of its largest element
                     */
                    if (field_alignment > alignment) {
                        alignment = field_alignment;
                    }

                    /**
                     * The structure inherits the alignment of its largest element
                     */
                    if (defining_declaration -> get_class_type() == SgClassDeclaration::e_union) {
                        if (field_size > size) {
                            size = field_size;
                        }
                    }
                    else {
                        /**
                         * If the current prefix is not aligned with the new field, pad it!
                         */
                        if (size % field_alignment) {
                            int pad_size = alignment - (size % alignment);
                            control.SetAttribute(var, Control::LLVM_STRUCTURE_PADDING, new IntAstAttribute(pad_size));
                            size += pad_size;
                            attribute -> insertPadding(pad_size);
                        }
                        size += field_size;
                    }
                    attribute -> addSgInitializedName(var);
                    string name = "%";
                    name += var -> get_name().getString();
                    attribute -> insertName(name);
                    control.SetAttribute(var, Control::LLVM_NAME, new StringAstAttribute(name));
                }
            }
        }

        if (alignment > 0) {
            control.SetAttribute(n, Control::LLVM_ALIGN_TYPE, new IntAstAttribute(alignment));

            /**
             * Always pad the structure so that its size is a multiple of the alignment.
             */
            if (size % alignment) {
                int pad_size = alignment - (size % alignment);
                control.SetAttribute(n, Control::LLVM_STRUCTURE_PADDING, new IntAstAttribute(pad_size));
                size += pad_size;
            }
        }
        control.SetAttribute(n, Control::LLVM_SIZE, new IntAstAttribute(size));

        /**
         * Now that we have a size for the structure, we can process pointer types that might depend on it.
         */
        for (int i = 0; i< pointer_decls.size(); i++) {
            setLLVMTypeName(pointer_decls[i]);
        }
    }

    return;
}

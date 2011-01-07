
#ifndef __CUDA_TRAVERSAL_
#define __CUDA_TRAVERSAL_

#include <rose.h>

#include <vector>

class CudaTraversal : public AstSimpleProcessing {
	public:
		CudaTraversal() :
			p_for_stmts(),
			p_kernel(NULL),
			p_caller(NULL),
			p_kernel_call_site(NULL)
		{}
		
		virtual void visit(SgNode * n) {
			switch (n->variantT()) {
				case V_SgFunctionDeclaration:
				{
					SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(n);
					ROSE_ASSERT(func_decl != NULL);
					std::string func_name = func_decl->get_name().getString();
//					std::cout << "Found SgFunctionDeclaration: " << func_name << std::endl;
					if (func_name == "caller")
						p_caller = func_decl;
					if (func_name == "kernel")
						p_kernel = func_decl;
					break;
				}
				case V_SgForStatement:
				{
					SgForStatement * for_stmt = isSgForStatement(n);
					ROSE_ASSERT(for_stmt != NULL);
//					std::cout << "Found SgForStatement." << std::endl;
					p_for_stmts.push_back(for_stmt);
					break;
				}
				case V_SgFunctionCallExp:
				{
					SgFunctionCallExp * func_call = isSgFunctionCallExp(n);
					ROSE_ASSERT(func_call != NULL);
					SgFunctionRefExp * func_ref = isSgFunctionRefExp(func_call->get_function());
					ROSE_ASSERT(func_ref != NULL);
//					std::cout << "Found SgFunctionCallExp: " << func_ref->getAssociatedFunctionDeclaration ()->get_name().getString() << std::endl;
					if (func_ref->getAssociatedFunctionDeclaration()->get_name().getString() == "kernel")
						p_kernel_call_site = func_call;
					break;
				}
                                case V_SgSourceFile: // fix the file suffix, Liao 12/29/2010
                                {
                                     SgSourceFile * sfile = isSgSourceFile (n);
                                     ROSE_ASSERT (sfile != NULL);
                                     sfile->set_Cuda_only(true);
                                }
				default:{}
			}
		}
		
		virtual void atTraversalEnd() {

			if (p_kernel != NULL && p_caller != NULL && p_kernel_call_site != NULL) {
				if (!p_for_stmts.empty()) {
					if (p_for_stmts.size() != 2) {
						std::cerr << "# nested loops != 2" << std::endl;
						ROSE_ASSERT(false);
					}
					else
						translateToCuda();
				}
				else {
					std::cerr << "Problem detection ! (for loop)" << std::endl;
					ROSE_ASSERT(false);
				}
			}
			else {
				std::cerr << "Problem of detection ! (kernel or caller or call site)" << std::endl;
				ROSE_ASSERT(false);
			}
		}
	
	protected:
		void translateToCuda() {
			std::cout << "Start translation" << std::endl;
		
			Sg_File_Info* sourceLocation = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
			ROSE_ASSERT(sourceLocation != NULL);
	
			// Modify 'kernel' declaration
			{
				p_kernel->get_functionModifier().setCudaKernel();
				
				SgFunctionParameterList * new_kernel_param_list = new SgFunctionParameterList(sourceLocation);
				SgInitializedNamePtrList & old_kernel_param_list = p_kernel->get_args();

				SgInitializedNamePtrList::iterator it_old_kernel_param_list;
				for (it_old_kernel_param_list = old_kernel_param_list.begin(); it_old_kernel_param_list != old_kernel_param_list.end(); it_old_kernel_param_list++) {
					std::string param_name = (*it_old_kernel_param_list)->get_name().getString();
					if (param_name != "i" && param_name != "j")
						new_kernel_param_list->append_arg(*it_old_kernel_param_list);
				}

				p_kernel->set_parameterList(new_kernel_param_list);
			}
			// Modify 'caller' code
			{	
				p_caller->get_functionModifier().setCudaHost();
			
				SgBasicBlock* block = new SgBasicBlock(sourceLocation, NULL);
				
				SgName name_cuda_malloc("cudaMalloc");
				SgFunctionDeclaration * decl_cuda_malloc = SageBuilder::buildNondefiningFunctionDeclaration(
					name_cuda_malloc,
					SageBuilder::buildVoidType(),
					SageBuilder::buildFunctionParameterList(),
					block
				);
				
				SgName name_cuda_copy  ("cudaMemcpy");
				SgFunctionDeclaration * decl_cuda_copy = SageBuilder::buildNondefiningFunctionDeclaration(
					name_cuda_copy,
					SageBuilder::buildVoidType(),
					SageBuilder::buildFunctionParameterList(),
					block
				);
				
				SgName name_cuda_free  ("cudaFree");
				SgFunctionDeclaration * decl_cuda_free = SageBuilder::buildNondefiningFunctionDeclaration(
					name_cuda_free,
					SageBuilder::buildVoidType(),
					SageBuilder::buildFunctionParameterList(),
					block
				);
				
				// Get 'caller' params
				
					SgVariableSymbol * param_in_var_sym;
					SgVariableSymbol * param_out_var_sym;
					SgVariableSymbol * param_size_var_sym;
		
					SgInitializedNamePtrList::iterator it_param_list;		
					for (it_param_list = p_caller->get_args().begin(); it_param_list != p_caller->get_args().end(); it_param_list++) {
						std::string param_name = (*it_param_list)->get_name().getString();
						if (param_name == "in")
							param_in_var_sym = new SgVariableSymbol(*it_param_list);
						else if (param_name == "out")
							param_out_var_sym = new SgVariableSymbol(*it_param_list);
						else if (param_name == "size")
							param_size_var_sym = new SgVariableSymbol(*it_param_list);
					}
					
				// Allocation size
				
					SgType * alloc_type = param_in_var_sym->get_declaration()->get_type();
					SgExpression * alloc_init_exp = SageBuilder::buildMultiplyOp(
						SageBuilder::buildMultiplyOp(
							new SgVarRefExp(sourceLocation, param_size_var_sym),
							new SgVarRefExp(sourceLocation, param_size_var_sym)
						),
						SageBuilder::buildSizeOfOp(alloc_type)
					);
			
					SgVariableDeclaration * size_var_decl = SageBuilder::buildVariableDeclaration(
						"size_alloc",
						SageBuilder::buildIntType(),
						SageBuilder::buildAssignInitializer(alloc_init_exp)
					);
					SageInterface::appendStatement (size_var_decl, block);
					SgVariableSymbol * size_var_sym = new SgVariableSymbol(*(size_var_decl->get_variables().begin()));
					
				// Device pointers
				
					SgVariableDeclaration * device_in_var_decl = SageBuilder::buildVariableDeclaration(
						"d_in",
						SageBuilder::buildPointerType(SageBuilder::buildFloatType())
					);
					SageInterface::appendStatement (device_in_var_decl, block);
					SgVariableSymbol * device_in_var_sym = new SgVariableSymbol(*(device_in_var_decl->get_variables().begin()));
					device_in_var_decl->get_declarationModifier().get_storageModifier().setCudaGlobal(); // FALSE: test only
				
					SgVariableDeclaration * device_out_var_decl = SageBuilder::buildVariableDeclaration(
						"d_out",
						SageBuilder::buildPointerType(SageBuilder::buildFloatType())
					);
					SageInterface::appendStatement (device_out_var_decl, block);
					SgVariableSymbol * device_out_var_sym = new SgVariableSymbol(*(device_out_var_decl->get_variables().begin()));
					
				
				// Device Alloc d_in
				
					SgExprListExp * cuda_malloc_in_args = new SgExprListExp(sourceLocation);
					cuda_malloc_in_args->append_expression(SageBuilder::buildCastExp(
						SageBuilder::buildAddressOfOp(new SgVarRefExp(sourceLocation, device_in_var_sym)),
						SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType()))
					));
					cuda_malloc_in_args->append_expression(
						new SgVarRefExp(sourceLocation, size_var_sym)
					);
					SgFunctionCallExp * cuda_malloc_in_func_call = new SgFunctionCallExp(
						sourceLocation,
						new SgFunctionRefExp(
							sourceLocation,
							new SgFunctionSymbol(decl_cuda_malloc),
							decl_cuda_malloc->get_type()
						),
						cuda_malloc_in_args
					);
					SageInterface::appendStatement (new SgExprStatement(sourceLocation, cuda_malloc_in_func_call), block);				
		
				// Device Alloc d_out
				
					SgExprListExp * cuda_malloc_out_args = new SgExprListExp(sourceLocation);
					cuda_malloc_out_args->append_expression(SageBuilder::buildCastExp(
						SageBuilder::buildAddressOfOp(new SgVarRefExp(sourceLocation, device_out_var_sym)),
						SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType()))
					));
					cuda_malloc_out_args->append_expression(
						new SgVarRefExp(sourceLocation, size_var_sym)
					);
					SgFunctionCallExp * cuda_malloc_out_func_call = new SgFunctionCallExp(		
						sourceLocation,
						new SgFunctionRefExp(
							sourceLocation,
							new SgFunctionSymbol(decl_cuda_malloc),
							decl_cuda_malloc->get_type()		
						),
						cuda_malloc_out_args
					);
					SageInterface::appendStatement (new SgExprStatement(sourceLocation, cuda_malloc_out_func_call), block);
				
				// Copy in
				
					SgExprListExp * cuda_copy_in_args = new SgExprListExp(sourceLocation);
					cuda_copy_in_args->append_expression(
						new SgVarRefExp(sourceLocation, device_in_var_sym)
					);
					cuda_copy_in_args->append_expression(
						new SgVarRefExp(sourceLocation, param_in_var_sym)
					);
					cuda_copy_in_args->append_expression(
						new SgVarRefExp(sourceLocation, size_var_sym)
					);
//					cuda_copy_in_args->append_expression(
//						new SgVarRefExp(sourceLocation, )
//					);
					SgFunctionCallExp * cuda_copy_in_func_call = new SgFunctionCallExp(		
						sourceLocation,
						new SgFunctionRefExp(
							sourceLocation,		
							new SgFunctionSymbol(decl_cuda_copy),
							decl_cuda_copy->get_type()
						),
						cuda_copy_in_args
					);
					SageInterface::appendStatement (new SgExprStatement(sourceLocation, cuda_copy_in_func_call), block);
		
				// Dim declaration
	
					SgVariableDeclaration * dim_grid_var_decl = SageBuilder::buildVariableDeclaration(
						"dimGrid",
						SageBuilder::buildIntType()
					);
					SageInterface::appendStatement (dim_grid_var_decl, block);
					SgVariableSymbol * dim_grid_var_sym = new SgVariableSymbol(*(dim_grid_var_decl->get_variables().begin()));
				
					SgVariableDeclaration * dim_blocks_var_decl = SageBuilder::buildVariableDeclaration(
						"dimBlock",
						SageBuilder::buildIntType()
					);
					SageInterface::appendStatement (dim_blocks_var_decl, block);		
					SgVariableSymbol * dim_blocks_var_sym = new SgVariableSymbol(*(dim_blocks_var_decl->get_variables().begin()));
		
				// Kernel call
		
					SgExprListExp * new_kernel_args_list = new SgExprListExp(sourceLocation);
					SgExpressionPtrList & old_kernel_args_list = p_kernel_call_site->get_args()->get_expressions();
		
					SgExpressionPtrList::iterator it_old_kernel_args_list;
					for (it_old_kernel_args_list = old_kernel_args_list.begin(); it_old_kernel_args_list != old_kernel_args_list.end(); it_old_kernel_args_list++) {
						std::string arg_name = isSgVarRefExp(*it_old_kernel_args_list)->get_symbol()->get_name().getString();
						if (arg_name != "i" && arg_name != "j")
							new_kernel_args_list->append_expression(*it_old_kernel_args_list);		
					}
					
					SgCudaKernelCallExp * cuda_call_site = new SgCudaKernelCallExp(
						sourceLocation,
						new SgFunctionRefExp(
							sourceLocation,
							new SgFunctionSymbol(p_kernel_call_site->getAssociatedFunctionDeclaration()),
							p_kernel_call_site->getAssociatedFunctionDeclaration()->get_type()
						),
						new_kernel_args_list,
						new SgCudaKernelExecConfig(
							sourceLocation,
							new SgVarRefExp(sourceLocation, dim_grid_var_sym),
							new SgVarRefExp(sourceLocation, dim_blocks_var_sym),
							NULL,
							NULL
						)
					);
	
					SageInterface::appendStatement (new SgExprStatement(sourceLocation, cuda_call_site), block);
		
				// ...
		
				SgExprStatement * cuda_copy_out;
				SgExprStatement * cuda_free_in;
				SgExprStatement * cuda_free_out;
	
				p_caller->get_definition()->replace_statement(p_caller->get_definition()->get_body() , block);
			}
		}
	
		std::vector<SgForStatement *> p_for_stmts;
		SgFunctionDeclaration * p_kernel;
		SgFunctionDeclaration * p_caller;
		SgFunctionCallExp * p_kernel_call_site;
		
};

#endif /* __CUDA_TRAVERSAL_ */


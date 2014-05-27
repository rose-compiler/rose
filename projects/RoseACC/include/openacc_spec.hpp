
#ifndef __OPENACC_SPEC_HPP__
#define __OPENACC_SPEC_HPP__

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/iteration-mapper.hpp"
#include "KLT/Core/loop-mapper.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/mfb-klt.hpp"

#include "KLT/OpenACC/mfb-acc-ocl.hpp"
#include "KLT/OpenACC/dlx-openacc.hpp"

#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"
#include "MDCG/code-generator.hpp"

#include "MFB/Sage/variable-declaration.hpp"

#include <cassert>

#include "sage3basic.h"

typedef ::KLT::Language::OpenCL Language;
typedef ::KLT::Runtime::OpenACC Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

#endif /* __OPENACC_SPEC_HPP__ */


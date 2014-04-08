
#ifndef __OPENACC_SPEC_HPP__
#define __OPENACC_SPEC_HPP__

#include "KLT/loop-trees.hpp"
#include "KLT/data.hpp"

#include "KLT/iteration-mapper.hpp"
#include "KLT/loop-mapper.hpp"
#include "KLT/data-flow.hpp"
#include "KLT/cg-config.hpp"
#include "KLT/generator.hpp"
#include "KLT/kernel.hpp"
#include "KLT/mfb-klt.hpp"

#include "KLT/mfb-acc-ocl.hpp"

#include "KLT/dlx-openacc.hpp"

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


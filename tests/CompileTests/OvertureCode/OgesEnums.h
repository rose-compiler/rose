#ifndef OGESENUMS_H
#define OGESENUMS_H
const Oges::conjugateGradientPreconditioners none            = Oges::none;
const Oges::conjugateGradientPreconditioners diagonal        = Oges::diagonal;
const Oges::conjugateGradientPreconditioners incompleteLU    = Oges::incompleteLU;
const Oges::conjugateGradientPreconditioners SSOR            = Oges::SSOR;

const Oges::conjugateGradientTypes biConjugateGradient       = Oges::biConjugateGradient;
const Oges::conjugateGradientTypes biConjugateGradientSquared= Oges::biConjugateGradientSquared;
const Oges::conjugateGradientTypes GMRes                     = Oges::GMRes;
const Oges::conjugateGradientTypes CGStab                    = Oges::CGStab;

const Oges::solvers yale                                     = Oges::yale;
const Oges::solvers harwell                                  = Oges::harwell;
const Oges::solvers bcg                                      = Oges::bcg;
const Oges::solvers sor                                      = Oges::sor;
#endif

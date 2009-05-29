roseSrcDir=$1


if [ -z ${roseSrcDir}  ]
then
    echo "Usage:";
    echo "./generateMakefiles.sh <roseSourceDir>";
    exit
fi



srcdir=${roseSrcDir}/projects/roseQt/widgets
genScript="${roseSrcDir}/projects/roseQt/scripts/MakefileAmGenerator.pl"


cd ${srcdir}/AsmInstructionBar                  && ${genScript} > Makefile.am
cd ${srcdir}/AsmSourceMapper                    && ${genScript} > Makefile.am
cd ${srcdir}/AstBrowserWidget                   && ${genScript} > Makefile.am
cd ${srcdir}/AstGraphWidget                     && ${genScript} > Makefile.am
cd ${srcdir}/AstProcessing                      && ${genScript} > Makefile.am
cd ${srcdir}/BeautifiedAst                      && ${genScript} > Makefile.am
cd ${srcdir}/FlopCounter                        && ${genScript} > Makefile.am
cd ${srcdir}/KiviatView                         && ${genScript} > Makefile.am
cd ${srcdir}/MetricsKiviat                      && ${genScript} > Makefile.am
cd ${srcdir}/MetricsConfig                      && ${genScript} > Makefile.am
cd ${srcdir}/MetricFilter                       && ${genScript} > Makefile.am

cd ${srcdir}/NodeInfoWidget                     && ${genScript} > Makefile.am
cd ${srcdir}/PropertyTreeWidget                 && ${genScript} > Makefile.am
cd ${srcdir}/QtDesignerPlugins                  && ${genScript} > Makefile.am

cd ${srcdir}/RoseCodeEdit                       && ${genScript}  > Makefile.am
cd ${srcdir}/RoseCodeEdit/QCodeEdit             && ${genScript} headertarget QCodeEdit > Makefile.am
cd ${srcdir}/RoseCodeEdit/QCodeEdit/document    && ${genScript} headertarget QCodeEdit/document  > Makefile.am
cd ${srcdir}/RoseCodeEdit/QCodeEdit/qnfa        && ${genScript} headertarget QCodeEdit/qnfa      > Makefile.am
cd ${srcdir}/RoseCodeEdit/QCodeEdit/widgets     && ${genScript} headertarget QCodeEdit/widgets   > Makefile.am


cd ${srcdir}/RoseFileSelector                   && ${genScript} > Makefile.am
cd ${srcdir}/util                               && ${genScript} > Makefile.am
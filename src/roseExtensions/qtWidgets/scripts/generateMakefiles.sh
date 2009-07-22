SUBDIRS="AsmInstructionBar \
          AsmView \
          AstBrowserWidget \
          AstGraphWidget \
          AstProcessing \
          BeautifiedAst \
          FlopCounter \
          InstructionCountAnnotator \
          KiviatView \
          MetricFilter \
          MetricsConfig \
          MetricsKiviat \
          NodeInfoWidget \
          ProjectManager \
          PropertyTreeWidget \
          RoseCodeEdit \
          RoseFileSelector \
          TaskSystem \
          util \
          WidgetCreator"

         
QROSEDIRS=". document qnfa widgets"
            

# ------------------------------------------------------------------------------


roseSrcDir=$1


if [ -z ${roseSrcDir}  ]
then
    echo "Usage:";
    echo "./generateMakefiles.sh <roseSourceDir>";
    exit
fi


srcdir=${roseSrcDir}/src/roseExtensions/qtWidgets
genScript="${roseSrcDir}/src/roseExtensions/qtWidgets/scripts/MakefileAmGenerator.pl"


for folder in $SUBDIRS  
do
    echo "Writing Makefile to " ${folder}
    cd ${srcdir}/${folder} && ${genScript} > Makefile.am
done

   

for folder in $QROSEDIRS
do
    echo "Writing Makefile to " ${folder}
    cd ${srcdir}/RoseCodeEdit/QCodeEdit/${folder} && ${genScript} headertarget QCodeEdit > Makefile.am
done
     





/**
@mainpage Rose Qt Widgets

     \htmlonly <center> \endhtmlonly

    \c libRoseQtWidgets.so provides Qt-Widgets which can display internal datastructures of ROSE. \n
       They can be used like the native Qt-Widgets, even integration into Qt-Designer is possible. \n
       RoseQt is a GUI for ROSE which uses all these widgets and is able to manage and display an SgProject \n \n

     \htmlonly </center> \endhtmlonly


\section toc Table of Contents

    - \subpage widgets
    - \subpage tutorial_usage
    - \subpage additional_functionaliy
    - \subpage impl_guide
        - \subpage installation
        - \subpage methods_of_usage
        - \subpage autotools_setup


\section location_in_rose Location in ROSE distribution
    The project is split up into several directories:
    - Library with all widgets is located at \c src/roseExtensions/qtWidgets
    - Plugin library for Qt-Designer at \c projects/QtDesignerPlugins
    - Main Application  \c projects/RoseQt

   \image html overview.jpg "RoseQt GUI"


\section using_projects Projects that already use RoseQtWidgets
    There are several projects which already use the ROSE QtWidgets.
    You can look at them to learn how to integrate the Qt-Widgets in your own project.

    - RoseQt Gui    \c projects/RoseQt
    - Compass       \c projects/compass/tools/compass/gui2
    - RTED Debugger \c projects/RTED/CppRuntimeSystem/DebuggerQt

\author Martin Bauer  ( bauer_martin@gmx.de, bauer25@llnl.gov ) \n
        Thomas Heller ( heller9@llnl.gov )

*/



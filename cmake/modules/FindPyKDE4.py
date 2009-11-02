# By Simon Edwards <simon@simonzone.com>
# modified by Paul Giannaros <paul@giannaros.org> to add better PyKDE4
# sip directory finding
# This file is in the public domain.

import sys
import os
import PyKDE4.pykdeconfig
import PyQt4.pyqtconfig

if "_pkg_config" in dir(PyKDE4.pykdeconfig):
    _pkg_config = PyKDE4.pykdeconfig._pkg_config

    for varname in [
            'kde_version',
            'kde_version_extra',
            'kdebasedir',
            'kdeincdir',
            'kdelibdir',
            'libdir',
            'pykde_kde_sip_flags', 
            'pykde_mod_dir',
            'pykde_modules', 
            'pykde_sip_dir',
            'pykde_version',
            'pykde_version_str']:
        varvalue = _pkg_config[varname]
        if varname == 'pykde_sip_dir':
            d = os.path.join(_pkg_config[varname], 'PyKDE4')
            if os.path.exists(d):
                varvalue = d
        print("%s:%s\n" % (varname, varvalue))
    pykde_version_tag = ''
    in_t = False
    for item in _pkg_config['pykde_kde_sip_flags'].split():
        if item == "-t":
            in_t = True
        elif in_t:
            if item.startswith("KDE_"):
                pykde_version_tag = item
        else:
            in_t = False
    print("pykde_version_tag:%s" % pykde_version_tag)

else:
    sys.exit(1)

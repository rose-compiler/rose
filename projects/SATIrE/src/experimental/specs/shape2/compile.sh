#!/bin/bash

BUILDDIR=build
ANALYSIS=nnh99

CARRIER_MARKUP='{DFI}'
CARRIER_EXPANSION='ShapeGraphSetLifted'
ANALYSIS_MARKUP='{ANALYSIS}'
ANALYSIS_EXPANSION="$ANALYSIS"

dest=${BUILDDIR}/${ANALYSIS}


function cp_subst {
  if [ "$#" -ne "2" ]; then
    echo ERROR: cp_subst needs two arguments: source-file destination-file
    exit 1
  fi
  sed "$1" -e "s/${CARRIER_MARKUP}/${CARRIER_EXPANSION}/g" \
           -e "s/${ANALYSIS_MARKUP}/${ANALYSIS_EXPANSION}/g" > "$2"
}

function update_optla {
  # include common_*.optla into $(ANALYSIS).optla, expand DFI, remove prefix
	cpp -C -P "${ANALYSIS}/${ANALYSIS}.code" | \
  sed -e "s/${CARRIER_MARKUP}/${CARRIER_EXPANSION}/g" \
      -e "s/${ANALYSIS_MARKUP}/${ANALYSIS_EXPANSION}/g" | \
  grep -v "prefix *: *${ANALYSIS}" > "${dest}/${ANALYSIS}.optla"
}

function edit_main {
  cp_subst "nnh99_main.C" "${dest}/main.C"
}

function init {
  mkdir -p "${dest}"

  # copy common files
  cp attributes                      "${dest}"
  cp ShapeAnalyzerOptions.h          "${dest}"
  cp ShapeCommandLineParser.h        "${dest}"
  cp ShapeAnalysisDriver.h           "${dest}"
  cp ShapeImplementation.C           "${dest}"
  cp PagAttributes.C                 "${dest}"
  cp satire_legacy.h                 "${dest}"

  # copy per analysis files
  cp "${ANALYSIS}/Makefile.custom"   "${dest}"

  # with substitution
  cp_subst visualisation.h  "${dest}/visualisation.h"
  cp_subst visualisation.c  "${dest}/visualisation.c"

  update_optla
  pushd .; cd "${BUILDDIR}"; newanalysis "${ANALYSIS}"; popd
  edit_main
}


function compile {
  update_optla
  make -C "${dest}"
}


init
compile

## Process this file with automake to create Makefile.in

## Uncomment this line to compile Overture source file: xFDerivative.C
TESTCODES = overtureBug_01.C
## TESTCODES = overtureBug_01.C xFDerivative.C
## TESTCODES = overtureBug_01.C xFDerivative.C defect.C
## TESTCODES = 

# Build the list of object files
TEST_Objects = ${TESTCODES:.C=.o}

# Restrict to level 6 testing for the moment
# ROSE_FLAGS = -rose:test 6
ROSE_FLAGS =

TEST_TRANSLATOR = $(top_builddir)/tests/testTranslator $(ROSE_FLAGS)

## We don't have to add the A++ header files to the include path since we have placed
## them directly into the test code directory (testing only the compilation with ROSE
## and not the execution!)
## Include the path to A++ and the transformation specification
## INCLUDES = @Axx_INCLUDES@
INCLUDES = -I. -I$(srcdir) -I$(srcdir)/../A++Code

# libTestSource_a_SOURCES = $(TESTCODES)
# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@$(MAKE) $(TEST_Objects)

$(TEST_Objects): $(TESTCODES)
	$(TEST_TRANSLATOR) $(INCLUDES) -c $(srcdir)/$(@:.o=.C)

EXTRA_DIST = \
   AbstractException.h          GenericGrid.h                           ListOfReferenceCountedObjects.h        Regrid.h \
   AdvancingFront.h             GenericGridCollection.h                 Looping.H                              ReparameterizationTransform.h \
   AdvancingFrontParameters.h   GenericGridCollectionFunction.h                                                RestrictionMapping.h \
   AirfoilMapping.h             GenericGridCollectionOperators.h        Makefile.am                            RevolutionMapping.h \
   AliasedDPtr.H                GenericGridFunction.h                                                          RocketMapping.h \
   Annulus.h                    GenericGridMotion.h                                                            SPACE.H \
   Arena.H                      GenericGridMotionParameters.h           Makefile.overture                      SPACE_F.H \
   ArithFab.H                   GenericMappedGridOperators.h                                                   ShowFileReader.h \
   Array.H                      Geom.h                                  MappedGrid.h                           SimpleDPtr.H \
   ArrayLim.H                   GeomADTTuple3dInt.h                     MappedGridFiniteVolumeOperators.h      SlapEquationSolver.h \
   Assert.H                     GeometricADT.h                          MappedGridFunction.h                   SmoothedPolygon.h \
   AssertException.h            GeometricADT3dInt.h                     MappedGridOperators.h                  SparseRep.h \
   BArena.H                     GeometricADTExceptions.h                MappedGridOperatorsInclude.h           Sphere.h \
   BCTypes.h                    GraphicsParameters.h                    Mapping.h                              SplineMapping.h \
   BLVERSION.H                  GridCollection.h                        MappingBuilder.h                       Square.h \
   BaseFab.H                    GridCollectionFiniteVolumeOperators.h   MappingEnums.h                         SquareMapping.h \
   BodyDefinition.h             GridCollectionFunction.h                MappingInformation.h                   Stencil.h \
   Boolean.H                    GridCollectionOperators.h               MappingP.h                             StretchMapping.h \
   Bound.h                      GridFunction.h                          MappingProjectionParameters.h          StretchTransform.h \
   BoundaryAdjustment.h         GridFunctionParameters.h                MappingRC.h                            StretchedSquare.h \
   BoundaryConditionParameters.h HDF_DataBase.h                         MappingWS.h                            SweepMapping.h \
   BoundingBox.h                HarwellEquationSolver.h                 MappingsFromCAD.h                      TFIMapping.h \
   Box.H                        HyperbolicMapping.h                     MatrixMapping.h                        Triangle.h \
                                IgesReader.h                            MatrixTransform.h                      TriangleWrapper.h \
   BoxArray.H                   IndexArg.h                              MatrixTransformGridMotion.h            TridiagonalSolver.h \
   BoxAssoc.H                   IndexType.H                             MatrixTransformGridMotionParameters.h  TrimmedMapping.h \
   BoxDomain.H                  IntVect.H                               MatrixTransformMotionFunction.h        TrivialArray.h \
   BoxLib.H                     Integrate.h                             Misc.H                                  \
   BoxList.H                    Interpolant.h                           MultiFab.H                             Tuple.H \
   BoxMapping.h                 Interpolate.h                           MultigridEquationSolver.h              TwilightZoneWizard.h \
   CONSTANTS.H                  InterpolateParameters.h                 NTreeNode.h                            Ugen.h \
   CircleMapping.h              InterpolateRefinements.h                NTreeNode2GeomADTTuple3dInt.h          UnstructuredMapping.h \
   ComposeMapping.h             IntersectionMapping.h                   NTreeNodeExceptions.h                  UseCount.H \
   CompositeGrid.h              Inverse.h                               NameList.h                             Utility.H \
                                JoinMapping.h                           NormalMapping.h                        YaleEquationSolver.h \
   CompositeGridFiniteVolumeOperators.h  LineMapping.h                  NormedFab.H                            aString.H \
   CompositeGridFunction.h               List.H                         NurbsMapping.h                         axisDefs.h \
   CompositeGridHybridConnectivity.h     ListOfBoundingBox.h            OGFunction.h                           billsMergeMacro.h \
   CompositeGridOperators.h     ListOfCompositeGrid.h                   OGPolyFunction.h                       cgux2a.h \
   CompositeSurface.h           ListOfCompositeGridOperators.h          OGPulseFunction.h                      cgux4a.h \
   CompositeTopology.h          ListOfDoubleArray.h                     OGTrigFunction.h                       conversion.h \
   CrossSectionMapping.h        ListOfDoubleCompositeGridFunction.h     OGgetIndex.h                           davidsReal.h \
   CylinderMapping.h            ListOfDoubleDistributedArray.h          OVFortranDefine.h                      depend \
   DPtr.H                       ListOfDoubleGenericGridFunction.h       OVFortranDefine.h.in                   depend.bak \
   DataBaseAccessFunctions.h    ListOfDoubleGridCollectionFunction.h    OffsetShell.h                          derivativeDefinitions.h \
   DataBaseBuffer.h             ListOfDoubleMappedGridFunction.h        Ogen.h                                 display.h \
   DataFormats.h                ListOfDoubleSerialArray.h               Oges.h                                 doubleCompositeGridFunction.h \
   DataPointMapping.h           ListOfFloatArray.h                      OgesEnums.h                    doubleGenericGridCollectionFunction.h \
   DepthMapping.h               ListOfFloatCompositeGridFunction.h      OgesParameters.h                       doubleGenericGridFunction.h \
   DerivedFunctions.h           ListOfFloatDistributedArray.h           Ogmg.h                                 doubleGridCollectionFunction.h \
   DialogData.h                 ListOfFloatGenericGridFunction.h        OgmgParameters.h                       doubleMappedGridFunction.h \
   Display.h                    ListOfFloatGridCollectionFunction.h     Ogshow.h                               floatCompositeGridFunction.h \
   DisplayParameters.h          ListOfFloatMappedGridFunction.h         OrderedFab.H                   floatGenericGridCollectionFunction.h \
   DynamicMovingGrids.h         ListOfFloatSerialArray.h                Orientation.H                          floatGenericGridFunction.h \
   EllipticGridGenerator.h      ListOfGenericCompositeGridOperators.h   OrthographicTransform.h                floatGridCollectionFunction.h \
   EllipticTransform.h          ListOfGenericGrid.h                     Overture.h                             floatMappedGridFunction.h \
   EquationSolver.h             ListOfGenericGridCollection.h           OvertureBL.h                           intCompositeGridFunction.h \
   EquiDistribute.h             ListOfGenericGridCollectionOperators.h  OvertureDefine.h               intGenericGridCollectionFunction.h \
   ErrorEstimator.h             ListOfGenericMappedGridOperators.h      OvertureDefine.h.in                    intGenericGridFunction.h \
   FArrayBox.H                  ListOfGridCollection.h                  OvertureInit.h                         intGridCollectionFunction.h \
   FPC.H                        ListOfHDF_DataBaseRCData.h              OvertureTypes.h                        intMappedGridFunction.h \
   FabArray.H                   ListOfIntArray.h                        PArray.H                               interpolatePoints.h \
   FabConv.H                    ListOfIntCompositeGridFunction.h        PETScEquationSolver.h                  laplacian.h \
   Face.h                       ListOfIntDistributedArray.h             ParallelUtility.h                      loops.h \
   FilamentMapping.h            ListOfIntGenericGridFunction.h          ParmParse.H                            makeDebugPlots.h \
   FileOutput.h                 ListOfIntGridCollectionFunction.h       PlaneMapping.h                         mathutil.h \
   FilletMapping.h              ListOfIntMappedGridFunction.h           PlotStuff.h                            mogl.h \
   FortranIO.h                  ListOfIntSerialArray.h                  PlotStuffParameters.h                  ply.h \
   FourierOperators.h           ListOfIntegerArray.h                    Pointers.H                             plyFileInterface.h \
   Fraction.h                   ListOfListOfDoubleArray.h               PriorityQueue.h                        testUtils.h \
   GCMath.h                     ListOfListOfFloatArray.h                Projection.h                           tlist.h \
   GL_GraphicsInterface.h       ListOfListOfIntArray.h                  QuadTree.h                             triangle.h \
   GUIState.h                   ListOfListOfRealArray.h                 QuadraticMapping.h                     wdhdefs.h \
   GenericCollectionOperators.h    ListOfMappedGrid.h                   REAL.H                                 xD.h \
   GenericCompositeGridOperators.h ListOfMappedGridOperators.h          ReductionMapping.h                     xDC.h \
   GenericDataBase.h               ListOfMappingRC.h                    ReferenceCounting.h \
   GenericGraphicsInterface.h   ListOfRealArray.h                       ReferenceCountingList.h \
   AliasedDPtr.C                Array.C                                 DPtr.C \
   Face_inline.C                List.C                                  NormedFab.C \
   PArray.C                     SimpleDPtr.C                            tlist.C \
   ArithFab.C                   BaseFab.C                               FabArray.C \
   Geom_inline.C                ListOfReferenceCountedObjects.C         OrderedFab.C \
   Pointers.C                   Tuple.C \
   $(TESTCODES)

check-local:
	@echo "Tests for Overture examples."
	@$(MAKE) $(PASSING_TEST_Objects)
	@echo "**************************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/OvertureCode: make check rule complete (terminated normally) ******"
	@echo "**************************************************************************************************"

clean-local:
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show \
              *.C.pdf *.C.edg.pdf *.pdf *.int.C rose_directives_list.txt core
	rm -rf ti_files *.ii
























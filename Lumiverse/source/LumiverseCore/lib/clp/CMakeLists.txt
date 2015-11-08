INCLUDE (CheckIncludeFiles)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

SET (CLP_HAVE_CONFIG "")

IF(NOT WIN32)
  CHECK_INCLUDE_FILES(cfloat HAVE_CFLOAT)
  CHECK_INCLUDE_FILES(cieeefp HAVE_CIEEEFP)
  CHECK_INCLUDE_FILES(cmath HAVE_CMATH)
  CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
  CHECK_INCLUDE_FILES(float.h HAVE_FLOAT_H)
  CHECK_INCLUDE_FILES(ieeefp.h HAVE_IEEEFP_H)
  CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
  CHECK_INCLUDE_FILES(math.h HAVE_MATH_H)
  CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
  CHECK_INCLUDE_FILES(readline/readline.h HAVE_READLINE_READLINE_H)
  CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
  CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
  CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
  CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
  CHECK_INCLUDE_FILES(sys/stat.h HAVE_SYS_STAT_H)
  CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
  CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)

  SET (CLP_HAVE_CONFIG "#define HAVE_CONFIG_H")

  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

  CHECK_INCLUDE_FILES(stdint.h COINUTILSHAS_STDINT_H)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config_coinutils.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config_coinutils.h)
ENDIF (NOT WIN32)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/ClpConfig.h.in ${CMAKE_CURRENT_SOURCE_DIR}/ClpConfig.h)

add_library(clp
  Clp_C_Interface.cpp
  ClpCholeskyBase.cpp
  ClpCholeskyDense.cpp
  #ClpCholeskyMumps.cpp
  #ClpCholeskyUfl.cpp
  #ClpCholeskyWssmp.cpp
  #ClpCholeskyWssmpKKT.cpp
  ClpConfig.h
  ClpConstraint.cpp
  ClpConstraintLinear.cpp
  ClpConstraintQuadratic.cpp
  ClpDualRowDantzig.cpp
  ClpDualRowPivot.cpp
  ClpDualRowSteepest.cpp
  ClpDummyMatrix.cpp
  ClpDynamicExampleMatrix.cpp
  ClpDynamicMatrix.cpp
  ClpEventHandler.cpp
  ClpFactorization.cpp
  ClpGubDynamicMatrix.cpp
  ClpGubMatrix.cpp
  ClpHelperFunctions.cpp
  ClpInterior.cpp
  ClpLinearObjective.cpp
  ClpLsqr.cpp
  ClpMatrixBase.cpp
  ClpMessage.cpp
  ClpModel.cpp
  ClpNetworkBasis.cpp
  ClpNetworkMatrix.cpp
  ClpNode.cpp
  ClpNonLinearCost.cpp
  ClpObjective.cpp
  ClpPackedMatrix.cpp
  ClpParameters.hpp
  ClpPdco.cpp
  ClpPdcoBase.cpp
  ClpPlusMinusOneMatrix.cpp
  ClpPredictorCorrector.cpp
  ClpPresolve.cpp
  ClpPrimalColumnDantzig.cpp
  ClpPrimalColumnPivot.cpp
  ClpPrimalColumnSteepest.cpp
  ClpQuadraticObjective.cpp
  ClpSimplex.cpp
  ClpSimplexDual.cpp
  ClpSimplexNonlinear.cpp
  ClpSimplexOther.cpp
  ClpSimplexPrimal.cpp
  ClpSolve.cpp
  Idiot.cpp
  IdiSolve.cpp
  CoinAlloc.cpp
  CoinBuild.cpp
  CoinDenseFactorization.cpp
  CoinDenseVector.cpp
  CoinError.cpp
  CoinFactorization1.cpp
  CoinFactorization2.cpp
  CoinFactorization3.cpp
  CoinFactorization4.cpp
  CoinFileIO.cpp
  CoinFinite.cpp
  CoinIndexedVector.cpp
  CoinLpIO.cpp
  CoinMessage.cpp
  CoinMessageHandler.cpp
  CoinModel.cpp
  CoinModelUseful.cpp
  CoinModelUseful2.cpp
  CoinMpsIO.cpp
  CoinOslFactorization.cpp
  CoinOslFactorization2.cpp
  CoinOslFactorization3.cpp
  CoinPackedMatrix.cpp
  CoinPackedVector.cpp
  CoinPackedVectorBase.cpp
  CoinParam.cpp
  CoinParamUtils.cpp
  CoinPostsolveMatrix.cpp
  CoinPrePostsolveMatrix.cpp
  CoinPrePostsolveMatrix.cpp
  CoinPresolveDoubleton.cpp
  CoinPresolveDual.cpp
  CoinPresolveDupcol.cpp
  CoinPresolveEmpty.cpp
  CoinPresolveFixed.cpp
  CoinPresolveForcing.cpp
  CoinPresolveHelperFunctions.cpp
  CoinPresolveImpliedFree.cpp
  CoinPresolveIsolated.cpp
  CoinPresolveMatrix.cpp
  CoinPresolvePsdebug.cpp
  CoinPresolveSingleton.cpp
  CoinPresolveSubst.cpp
  CoinPresolveTighten.cpp
  CoinPresolveTripleton.cpp
  CoinPresolveUseless.cpp
  CoinPresolveZeros.cpp
  CoinSearchTree.cpp
  CoinShallowPackedVector.cpp
  CoinSimpFactorization.cpp
  CoinSnapshot.cpp
  CoinStructuredModel.cpp
  CoinWarmStartBasis.cpp
  CoinWarmStartDual.cpp
  CoinWarmStartPrimalDual.cpp
  CoinWarmStartVector.cpp
)

# Suppress warnings due to clp sloppiness
# Clang goes crazy otherwise
IF(APPLE)
    target_compile_options(clp PRIVATE -Wno-parentheses)
    target_compile_options(clp PRIVATE -Wno-extra-tokens)
    target_compile_options(clp PRIVATE -Wno-format-security)
    target_compile_options(clp PRIVATE -Wno-deprecated-register)
    target_compile_options(clp PRIVATE -Wno-constant-conversion)
    target_compile_options(clp PRIVATE -Wno-constant-logical-operand)
ENDIF(APPLE)

INSTALL(TARGETS clp ARCHIVE DESTINATION lib)

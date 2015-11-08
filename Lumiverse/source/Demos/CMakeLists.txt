#Demo build options
set (LumiverseDemos_BUILD_DEMO ON CACHE BOOL "Build demo application.")
set (LumiverseDemos_BUILD_SPEED_TEST ON CACHE BOOL "Build speed tester demo application")
#set (LumiverseDemos_BUILD_FEATURE_GENERATOR ON CACHE BOOL "Build feature generator/appearance transfer demo application")

IF (LumiverseDemos_BUILD_DEMO)
	add_subdirectory(LumiverseDemo)
	add_subdirectory(LMSBridgeDemo)
ENDIF(LumiverseDemos_BUILD_DEMO)

IF (LumiverseDemos_BUILD_SPEED_TEST)
	add_subdirectory(SpeedTest)
ENDIF(LumiverseDemos_BUILD_SPEED_TEST)

#IF (LumiverseDemos_BUILD_FEATURE_GENERATOR)
#	add_subdirectory(FeatureGenerator)
#ENDIF(LumiverseDemos_BUILD_FEATURE_GENERATOR)

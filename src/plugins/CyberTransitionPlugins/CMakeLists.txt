ADD_TRANSITION_PLUGIN(CyberTransitionPlugin ${CMAKE_CURRENT_LIST_DIR}/CyberTransitionPlugin.cpp)
target_include_directories(CyberTransitionPlugin PRIVATE ${PROJECT_SOURCE_DIR})

ADD_TRANSITION_PLUGIN(CyberDTransitionPlugin ${CMAKE_CURRENT_LIST_DIR}/CyberDTransitionPlugin.cpp)
target_include_directories(CyberDTransitionPlugin PRIVATE ${PROJECT_SOURCE_DIR})

ADD_TRANSITION_PLUGIN(CyberLDTransitionPlugin ${CMAKE_CURRENT_LIST_DIR}/CyberLDTransitionPlugin.cpp)
target_include_directories(CyberLDTransitionPlugin PRIVATE ${PROJECT_SOURCE_DIR})

ADD_TRANSITION_PLUGIN(CyberLDMTransitionPlugin ${CMAKE_CURRENT_LIST_DIR}/CyberLDMTransitionPlugin.cpp)
target_include_directories(CyberLDMTransitionPlugin PRIVATE ${PROJECT_SOURCE_DIR})

ADD_TRANSITION_PLUGIN(CyberRandDefTransitionPlugin ${CMAKE_CURRENT_LIST_DIR}/CyberRandDefTransitionPlugin.cpp)
target_include_directories(CyberRandDefTransitionPlugin PRIVATE ${PROJECT_SOURCE_DIR})

ADD_TRANSITION_PLUGIN(CyberSmartDefTransitionPlugin ${CMAKE_CURRENT_LIST_DIR}/CyberSmartDefTransitionPlugin.cpp)
target_include_directories(CyberSmartDefTransitionPlugin PRIVATE ${PROJECT_SOURCE_DIR})

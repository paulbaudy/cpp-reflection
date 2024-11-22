cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Reflection")
file(GLOB_RECURSE RF_FILES "Reflection/**.h")
file(GLOB_RECURSE CORE_FILES "Core/**.h")
add_executable (Reflection "Reflection.cpp" 
	
	${RF_FILES}
	${CORE_FILES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Reflection PROPERTY CXX_STANDARD 20)
endif()
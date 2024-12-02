add_library(suisha SHARED loop.cc)
target_compile_features(suisha PUBLIC cxx_std_20)
target_include_directories(suisha PUBLIC
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include>"
)

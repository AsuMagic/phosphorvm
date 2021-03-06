add_executable(
	${PROJECT_NAME}

	"bc/disasm.cpp"
	"bc/names.cpp"
	"vm/vm.cpp"
	"vm/instancemanager.cpp"
	"std/debug.cpp"
	"unpack/chunk/form.cpp"
	"unpack/mmap.cpp"

	"main.cpp"
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE

	".."
)

set(RELEASE_OPTIONS
	"-O3"
)

target_compile_options(
	${PROJECT_NAME}
	PRIVATE

	"-std=c++1z"
	"-Wall"
	"-Wextra"
	"-Wpedantic"
	"-Wno-missing-braces"
	"-g"
	$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>
)

target_link_libraries(
	${PROJECT_NAME}

	fmt
)

DPFix-NG v0.0.32 modular refactor

This is a source-layout refactor of v0.0.31-exp2. The working world-detail exp2
logic is preserved. Stable subsystems are now real C++ translation units:
	dpfixng/logging.h/.cpp
	dpfixng/config.h/.cpp
	dpfixng/main_exe.h/.cpp
	dpfixng/world_streaming.h/.cpp

The profiler and D3D9 hook implementation remain .inl files for now. They still
compile as part of asi_main.cpp, which minimizes risk while the reverse-engineering
code is evolving.

CMake
-----
Add the four new .cpp files to the existing DPFixNG target. A helper file is
provided as DPFixNG_sources.cmake. If your target name differs from DPFixNG, edit
that helper or add the files to target_sources manually.
No INI change is required. World.HighDetailDistanceScale=2 retains the working
exp2 behavior.

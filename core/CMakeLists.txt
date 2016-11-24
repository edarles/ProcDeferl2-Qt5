include_directories(core)
include_directories(core/Utils)
include_directories(core/Ocean)
include_directories(core/SPH)
include_directories(core/BreakingWave)
include_directories(core/HybridOcean)
include_directories(core/IHM)


SET(APP_SOURCES
			core/IHM/glWidget.cpp
			core/IHM/window.cpp
			core/IHM/main.cpp

			core/Ocean/GridOcean.cpp
			core/Ocean/WaveGroup.cpp
			core/Ocean/WaveGroupMP.cpp

			core/SPH/GridSPH.cpp
			core/SPH/Particle.cpp
			core/SPH/SPH.cpp
			core/SPH/SESPH.cpp
			core/SPH/WCSPH.cpp

			core/BreakingWave/GridBreaking.cpp
			core/BreakingWave/BreakingWave.cpp

			core/HybridOcean/Grid.cpp
			core/HybridOcean/HybridOcean.cpp

			core/Utils/shaders.cpp
)


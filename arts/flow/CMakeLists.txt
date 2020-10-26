
add_custom_command(
    OUTPUT artsflow.h artsflow.cc
    COMMAND mcopidl ${CMAKE_CURRENT_SOURCE_DIR}/artsflow.idl
    )

set(artsflow_source
    synth_play_impl.cc synthschedule.cc audiosubsys.cc pipebuffer.cc convert.cc
    synth_wave_sin_impl.cc synth_frequency_impl.cc synth_multi_add_impl.cc
    synth_add_impl.cc synth_mul_impl.cc synth_play_wav_impl.cc
    stdsynthmodule.cc cache.cc asyncschedule.cc bytestreamtoaudio_impl.cc
    stereovolumecontrol_impl.cc stereoeffectstack_impl.cc fft.c
    stereofftscope_impl.cc virtualports.cc bus.cc audiomanager_impl.cc
    synth_record_impl.cc resample.cc audioio.cc audioiooss.cc audioioalsa.cc
    audioionull.cc audioiolibaudioio.cc audioiosun.cc audioioaix.cc
    audioionas.cc audioioalsa9.cc cpuinfo.cc
    )

kde2_library(libartsflow
    SOURCES
        artsflow.cc
        ${artsflow_source}
    LIBS
        kde2::mcop
    VERSION
        3.0.0
    )


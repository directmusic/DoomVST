cmake_minimum_required(VERSION 3.15)
add_subdirectory(JUCE)
project(JUCEDoom VERSION 0.1.0)

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

juce_add_plugin(DoomPlugin
        COMPANY_NAME Direct
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        PLUGIN_MANUFACTURER_CODE Jldr
        PLUGIN_CODE Doom
        FORMATS AU VST3 Standalone
        PRODUCT_NAME "Doom")

set(DOOM_SRC "doomgeneric/doomgeneric")

target_sources(DoomPlugin
        PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
        ${DOOM_SRC}/am_map.c
        ${DOOM_SRC}/d_event.c
        ${DOOM_SRC}/d_items.c
        ${DOOM_SRC}/d_iwad.c
        ${DOOM_SRC}/d_loop.c
        ${DOOM_SRC}/d_main.c
        ${DOOM_SRC}/d_mode.c
        ${DOOM_SRC}/d_net.c
        ${DOOM_SRC}/doomdef.c
        ${DOOM_SRC}/doomgeneric.c
        ${DOOM_SRC}/doomstat.c
        ${DOOM_SRC}/dstrings.c
        ${DOOM_SRC}/dummy.c
        ${DOOM_SRC}/f_finale.c
        ${DOOM_SRC}/f_wipe.c
        ${DOOM_SRC}/g_game.c
        ${DOOM_SRC}/gusconf.c
        ${DOOM_SRC}/hu_lib.c
        ${DOOM_SRC}/hu_stuff.c
        ${DOOM_SRC}/i_cdmus.c
        ${DOOM_SRC}/i_endoom.c
        ${DOOM_SRC}/i_input.c
        ${DOOM_SRC}/i_joystick.c
        ${DOOM_SRC}/i_scale.c
        ${DOOM_SRC}/i_sound.c
        ${DOOM_SRC}/i_system.c
        ${DOOM_SRC}/i_timer.c
        ${DOOM_SRC}/i_video.c
        ${DOOM_SRC}/icon.c
        ${DOOM_SRC}/info.c
        ${DOOM_SRC}/m_argv.c
        ${DOOM_SRC}/m_bbox.c
        ${DOOM_SRC}/m_cheat.c
        ${DOOM_SRC}/m_config.c
        ${DOOM_SRC}/m_controls.c
        ${DOOM_SRC}/m_fixed.c
        ${DOOM_SRC}/m_menu.c
        ${DOOM_SRC}/m_misc.c
        ${DOOM_SRC}/m_random.c
        ${DOOM_SRC}/memio.c
        ${DOOM_SRC}/mus2mid.c
        ${DOOM_SRC}/p_ceilng.c
        ${DOOM_SRC}/p_doors.c
        ${DOOM_SRC}/p_enemy.c
        ${DOOM_SRC}/p_floor.c
        ${DOOM_SRC}/p_inter.c
        ${DOOM_SRC}/p_lights.c
        ${DOOM_SRC}/p_map.c
        ${DOOM_SRC}/p_maputl.c
        ${DOOM_SRC}/p_mobj.c
        ${DOOM_SRC}/p_plats.c
        ${DOOM_SRC}/p_pspr.c
        ${DOOM_SRC}/p_saveg.c
        ${DOOM_SRC}/p_setup.c
        ${DOOM_SRC}/p_sight.c
        ${DOOM_SRC}/p_spec.c
        ${DOOM_SRC}/p_switch.c
        ${DOOM_SRC}/p_telept.c
        ${DOOM_SRC}/p_tick.c
        ${DOOM_SRC}/p_user.c
        ${DOOM_SRC}/r_bsp.c
        ${DOOM_SRC}/r_data.c
        ${DOOM_SRC}/r_draw.c
        ${DOOM_SRC}/r_main.c
        ${DOOM_SRC}/r_plane.c
        ${DOOM_SRC}/r_segs.c
        ${DOOM_SRC}/r_sky.c
        ${DOOM_SRC}/r_things.c
        ${DOOM_SRC}/s_sound.c
        ${DOOM_SRC}/sha1.c
        ${DOOM_SRC}/sounds.c
        ${DOOM_SRC}/st_lib.c
        ${DOOM_SRC}/st_stuff.c
        ${DOOM_SRC}/statdump.c
        ${DOOM_SRC}/tables.c
        ${DOOM_SRC}/v_video.c
        ${DOOM_SRC}/w_checksum.c
        ${DOOM_SRC}/w_file.c
        ${DOOM_SRC}/w_file_stdc.c
        ${DOOM_SRC}/w_main.c
        ${DOOM_SRC}/w_wad.c
        ${DOOM_SRC}/wi_stuff.c
        ${DOOM_SRC}/z_zone.c
        )

target_compile_definitions(DoomPlugin
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)


target_link_libraries(DoomPlugin
        PRIVATE
        juce::juce_audio_utils
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

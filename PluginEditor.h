#pragma once

#include "PluginProcessor.h"
#include <thread>

//==============================================================================
class DoomWindow : public juce::AudioProcessorEditor,
                   public juce::KeyListener,
                   public juce::ComponentBoundsConstrainer,
                   public juce::AsyncUpdater{
public:
    explicit DoomWindow(AudioPluginAudioProcessor&);
    ~DoomWindow() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;
    bool keyStateChanged(bool isKeyDown, Component* originatingComponent) override;
    void modifierKeysChanged(const juce::ModifierKeys& modifiers) override;
    void handleAsyncUpdate() override;
private:
    bool keyboard_state[512];
    void key_state_handler(int);
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    juce::Image m_framebuffer;

    std::string m_who_cares;
    std::string m_iwad_flag;
    std::string m_file_path;
    std::array<char*, 3> m_args;
    std::unique_ptr<std::thread> m_doom_thread;
    std::unique_ptr<juce::FileChooser> m_file_chooser;

    std::atomic<bool> m_valid_file_selected;
    void wad_file_found(const std::string& path);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DoomWindow)
};

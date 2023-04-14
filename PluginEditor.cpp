#include "PluginEditor.h"
#include "PluginProcessor.h"

extern "C" {
#include "doomgeneric/doomgeneric/d_iwad.h"
#include "doomgeneric/doomgeneric/d_main.h"
#include "doomgeneric/doomgeneric/doomgeneric.h"
#include "doomgeneric/doomgeneric/doomkeys.h"
#include "doomgeneric/doomgeneric/m_argv.h"
}

#include <chrono>
#include <fstream>
#include <thread>

DoomWindow* editor_ptr = nullptr;

static bool validate_IWAD(const std::string& path) {
    char hopefully_this_says_iwad[4];
    std::fstream file_for_validation(path);

    if (!file_for_validation.is_open())
        return false;

    file_for_validation.read(hopefully_this_says_iwad, 4);

    if (hopefully_this_says_iwad[0] == 'I'
        && hopefully_this_says_iwad[1] == 'W'
        && hopefully_this_says_iwad[2] == 'A'
        && hopefully_this_says_iwad[3] == 'D') {
        // "Valid" file.
        file_for_validation.close();
        return true;
    } else {
        file_for_validation.close();
        return false;
    }
}

void DoomWindow::wad_file_found(const std::string& path) {
    if (!validate_IWAD(path)) {
        m_valid_file_selected = false;
        return;
    }

    m_file_path = path;
    m_valid_file_selected = true;
    m_args = { (char*)m_who_cares.c_str(), (char*)m_iwad_flag.c_str(), (char*)m_file_path.c_str() };
    m_doom_thread = std::make_unique<std::thread>(doomgeneric_Create, m_args.size(), m_args.data());
    m_doom_thread->detach();
}

//==============================================================================
DoomWindow::DoomWindow(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p) {
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addKeyListener(this);
    setSize(DOOMGENERIC_RESX, DOOMGENERIC_RESY);
    editor_ptr = this;

    setWantsKeyboardFocus(true);
    setResizable(true, false);
    setFixedAspectRatio(float(DOOMGENERIC_RESX) / float(DOOMGENERIC_RESY));
    setConstrainer(this);

    m_framebuffer = juce::Image(juce::Image::PixelFormat::ARGB, DOOMGENERIC_RESX, DOOMGENERIC_RESY, true);

    for (int i = 0; i < INT_MAX; i++) {
        keyboard_state[i] = false;
    }

    m_file_chooser = std::make_unique<juce::FileChooser>("Choose WAD File");
    auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    m_file_chooser->launchAsync(flags, [this](const juce::FileChooser& chooser) {
        juce::File wad_file(chooser.getResult());
        wad_file_found(wad_file.getFullPathName().toStdString());
    });

    m_who_cares = "@";
    m_iwad_flag = "-iwad";
}

DoomWindow::~DoomWindow() { }

void DG_Init() { }
void DG_DrawFrame() {
    // Not sure if this is the best way to make this thread safe.
    const juce::MessageManagerLock message_manager_lock;
    if (editor_ptr)
        editor_ptr->repaint();
}

void DG_SleepMs(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint32_t DG_GetTicksMs() {
    static auto started = std::chrono::high_resolution_clock::now();
    auto done = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count();

    return ms;
}

#define KEYQUEUE_SIZE 16
static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;
#define KCTRL 42069
#define KSHIFT 69420

static unsigned char convertToDoomKey(unsigned int key) {
    if (key == juce::KeyPress::leftKey)
        key = KEY_LEFTARROW;
    if (key == juce::KeyPress::rightKey)
        key = KEY_RIGHTARROW;
    if (key == juce::KeyPress::downKey)
        key = KEY_DOWNARROW;
    if (key == juce::KeyPress::upKey)
        key = KEY_UPARROW;
    if (key == juce::KeyPress::returnKey)
        key = KEY_ENTER;
    if (key == juce::KeyPress::spaceKey)
        key = KEY_USE;
    if (key == juce::KeyPress::escapeKey)
        key = KEY_ESCAPE;
    if (key == KSHIFT)
        key = KEY_RSHIFT;
    if (key == KCTRL)
        key = KEY_FIRE;

    key = tolower(key);
    return key;
}

static void addKeyToQueue(int pressed, int keyCode) {
    unsigned char key = convertToDoomKey(keyCode);

    unsigned short keyData = (pressed << 8) | key;

    s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

std::queue<unsigned int> key_queue;

bool DoomWindow::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) { }

void DoomWindow::modifierKeysChanged(const juce::ModifierKeys& modifiers) {
    {
        bool state = modifiers.isCtrlDown();
        if (state != keyboard_state[KCTRL]) {
            addKeyToQueue(state, KCTRL);
        }
        keyboard_state[KCTRL] = state;
    }
    {
        bool state = modifiers.isShiftDown();
        if (state != keyboard_state[KSHIFT]) {
            addKeyToQueue(state, KSHIFT);
        }
        keyboard_state[KSHIFT] = state;
    }
}

void DoomWindow::key_state_handler(const int key_code) {
    bool state = juce::KeyPress::isKeyCurrentlyDown(key_code);
    if (state != keyboard_state[key_code]) {
        addKeyToQueue(state, key_code);
    }
    keyboard_state[key_code] = state;
}

bool DoomWindow::keyStateChanged(bool isKeyDown, juce::Component* originatingComponent) {
    key_state_handler(juce::KeyPress::leftKey);
    key_state_handler(juce::KeyPress::rightKey);
    key_state_handler(juce::KeyPress::upKey);
    key_state_handler(juce::KeyPress::downKey);
    key_state_handler(juce::KeyPress::returnKey);
    key_state_handler(juce::KeyPress::spaceKey);
    key_state_handler(juce::KeyPress::escapeKey);
    for (int i = 65; i < 123; i++) {
        key_state_handler(i);
    }
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
        // key queue is empty
        return 0;
    } else {
        unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
        s_KeyQueueReadIndex++;
        s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

        *pressed = keyData >> 8;
        *doomKey = keyData & 0xFF;

        return 1;
    }

    return 0;
}

void DG_SetWindowTitle(const char* title) {
    (void)title;
}

//==============================================================================
void DoomWindow::paint(juce::Graphics& g) {
    if (m_valid_file_selected) {
        unsigned char img_test[DOOMGENERIC_RESX * DOOMGENERIC_RESY];
        for (int i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; i++) {
            img_test[i] = 255;
        }

        grabKeyboardFocus();
        juce::Image::BitmapData bitmap(m_framebuffer, juce::Image::BitmapData::readWrite);

        for (int i = 0; i < DOOMGENERIC_RESX; i++) {
            for (int j = 0; j < DOOMGENERIC_RESY; j++) {
                auto temp = DG_ScreenBuffer[j * DOOMGENERIC_RESX + i];

                auto red = static_cast<uint8_t>((temp >> 16) & 0xff);
                auto green = static_cast<uint8_t>((temp >> 8) & 0xff);
                auto blue = static_cast<uint8_t>(temp & 0xff);

                bitmap.setPixelColour(i, j, juce::Colour(red, green, blue, (uint8_t)255));
            }
        }
        g.fillAll(juce::Colours::black);
        g.drawImageWithin(m_framebuffer, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
    } else {
        g.setColour(juce::Colours::white);
        g.drawText("You did not select a valid WAD file. Please close the Plug-In and select a valid WAD file.", juce::Rectangle<int> { 0, 0, getWidth(), getHeight() }, juce::Justification::centred);
    }
}

void DoomWindow::resized() { }

#pragma once

#include "../Results/PresetConfig.h"

class InteractiveSession {
public:
    int Run() const;

private:
    static void PrintBanner();
    static int PromptRepeatCount();
    static Preset PromptPreset();
    static bool ConfirmExtreme();
};

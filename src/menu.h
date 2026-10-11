#pragma once

namespace Menu
{
    void Init();
    void Draw();
    void Toggle();
    bool IsOpen();

    bool GetBoxes();
    bool GetHealth();
    bool GetNames();
    bool GetDistance();
    float GetMaxDistance();
    float GetBoxThickness();
}
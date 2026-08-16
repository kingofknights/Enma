#pragma once

class Theme {
public:
    Theme() = delete;
    ~Theme() = delete;

    static auto SetupGui() -> void;
};

#pragma once

#include <filesystem>

struct ChangeNameModalWindow{
    ChangeNameModalWindow(const std::string& id) : id(id){}
    void Open(const std::filesystem::path& oldPath);
    void Show();
    bool IsFinished();
    std::string newName;
    bool finished;
    std::string id;
    bool mustOpen = false;
    std::filesystem::path oldNamePath;
};

void ShowAssetsMenu(const std::filesystem::path& root, ChangeNameModalWindow& modal);
#include <iostream>
#include <filesystem>
#include <fstream>
#include "json.hpp"
#include "core/Log.h"
#include "glm/glm.hpp"
#include "core/GUID.h"

int main(int argc, char** argv)
{
    nimo::Log::Initialize();
    std::filesystem::path filepath = "Assets/Scenes/NewScene.nscene";
    std::filesystem::path bakFilepath = "Assets/Scenes/NewScene.bak";

    std::cout << "Reading scene file " << filepath << std::endl;
    std::ifstream file(filepath.c_str());

    if (!file.good())
    {
        NIMO_INFO("Error opening file!: {}", strerror(errno));
        return -1;
    }

    nlohmann::ordered_json scene;
    scene << file;

    nlohmann::ordered_json sceneCopy(scene);

    NIMO_INFO("Multiplying mesh and point light entities...");
    for (auto& entity : scene["Entities"].items())
    {
        //NIMO_INFO("{}\n", entity.key());

        if (entity.value().contains("Mesh") || entity.value().contains("PointLight"))
        {
            auto Translation = glm::vec3((float)entity.value()["Transform"]["Translation"][0], (float)entity.value()["Transform"]["Translation"][1], (float)entity.value()["Transform"]["Translation"][2]);

            // Multiplies meshes and point lights
            glm::vec3 offset(2, 0, 1);
            int numOfCopies = 10;

            nlohmann::ordered_json entityCopy(entity.value());

            for (int i = -4; i < 5; ++i)
            {
                for (int j = -20; j < 0; ++j)
                {
                    nlohmann::ordered_json translation;
                    translation["Translation"] = { Translation.x + i * offset.x, Translation.y, Translation.z + j * offset.z };
                    nlohmann::ordered_json guid;
                    guid["GUID"] = nimo::GUID::Create().Str();

                    entityCopy.update(guid);
                    entityCopy["Transform"].update(translation);

                    sceneCopy["Entities"].push_back(entityCopy);
                }
            }
        }
    }

    file.close();

    if (std::rename(filepath.string().c_str(), bakFilepath.string().c_str()))
    {
        NIMO_INFO("Error renaming file: {} !!", strerror(errno));
        return -1;
    }

    std::ofstream outFile(filepath);
    outFile << sceneCopy.dump(1, '\t');
    file.close();

    NIMO_INFO("DONE!");
}
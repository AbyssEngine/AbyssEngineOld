#pragma once

#include "Abyss/Common/Scene.h"
#include "Abyss/DataTypes/DC6.h"
#include "OD2/Common/ButtonDefManager.h"
#include "OD2/Common/PaletteManager.h"
#include "OD2/Common/ResourcePaths.h"

#include <vector>

namespace OD2::Scenes::Credits {

struct CreditsLabelItem {
    Abyss::UI::Label label;
    bool isHeading{};
    bool available{};
    SDL_Point position{};
};

class Credits final : public Abyss::Common::Scene {
    void onExitClicked();
    static std::unique_ptr<CreditsLabelItem> getNewFontLabel(bool isHeading);
    void addNextItem();

    bool _doneWithCredits{};
    std::chrono::duration<double> _cycleTime{};
    int _cyclesUntilNextLine{};
    Abyss::DataTypes::DC6 _background = Abyss::DataTypes::DC6(Common::ResourcePaths::Credits::CreditsBackground, Common::GetPalette("Sky"));
    Abyss::UI::Button<Abyss::DataTypes::DC6> _btnSinglePlayer = Common::CreateButton("Medium", "EXIT", [this] { onExitClicked(); });
    std::vector<std::string> _creditLines{};
    std::vector<std::unique_ptr<CreditsLabelItem>> _creditLabels{};

  public:
    Credits();

    void update(std::chrono::duration<double> deltaTime) override;
    void processEvent(const SDL_Event &event) override;
    void render() override;
};

} // namespace OD2::Scenes::Credits

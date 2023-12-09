#include "Credits.h"
#include "Abyss/AbyssEngine.h"
#include "OD2/Scenes/MainMenu/MainMenu.h"

namespace OD2::Scenes::Credits {

Credits::Credits() {
    // TODO: Contributors list
    _creditLines = Abyss::AbyssEngine::getInstance().loadStringList(Common::ResourcePaths::Credits::CreditsText);

    // Magic janky conversion from UTF16 to UTF8
    // Really we're basically just control characters and 0x00 characters
    // This could be done better at some point
    for (auto &line : _creditLines) {
        for (int i = 0; i < line.size(); i += 2) {
            if (line[i] == -1) {
                i -= 1;
                continue;
            }
            if (line[i] != 0x00 && line[i] != -2) {
                continue;
            }
            line[i / 2] = line[i + 1];
        }
        line.resize(line.size() / 2);
        line.erase(line.find_last_not_of(" \n\r\t") + 1);
        line.erase(0, line.find_first_not_of(" \n\r\t"));
        std::erase(line, '\r');
        std::erase(line, '\n');
        std::erase(line, '\0');
    }
}

void Credits::update(const std::chrono::duration<double> deltaTime) {
    constexpr auto durationPerCycle = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::duration<float>(0.02f));
    _cycleTime += deltaTime;

    while (_cycleTime >= durationPerCycle) {
        _cycleTime -= durationPerCycle;
        _cyclesUntilNextLine -= 1;

        if (!_doneWithCredits && _cyclesUntilNextLine <= 0)
            addNextItem();

        for (auto &[label, _, available, position] : _creditLabels) {
            if (available)
                continue;

            if ((position.y -= 1) < -15)
                available = true;
        }
    }

    std::erase_if(_creditLabels, [](const auto &label) { return label.available; });
}

void Credits::processEvent(const SDL_Event &event) {}

void Credits::render() {
    _background.draw(0, 0, 0, 4, 3);

    for (auto &[label, _, available, position] : _creditLabels) {
        if (available) {
            continue;
        }
        label.draw(position.x, position.y);
    }

    _btnSinglePlayer.draw(33, 543);
}

void Credits::addNextItem() {
    if (_creditLines.empty()) {
        _doneWithCredits = true;
        return;
    }

    const auto text = _creditLines.front();
    _creditLines.erase(_creditLines.begin());

    if (text.empty()) {
        if (!_creditLines.empty() && _creditLines.front()[0] == '*') {
            _cyclesUntilNextLine = 38;
        }

        _cyclesUntilNextLine = 19;
        return;
    }

    const auto isHeading = text[0] == '*';
    auto isNextHeading = !_creditLines.empty() && !_creditLines.front().empty() && _creditLines.front()[0] == '*';
    const auto isNextSpace = !_creditLines.empty() && _creditLines.front().empty();
    auto isDoubled = false;

    _creditLabels.emplace_back(getNewFontLabel(isHeading));
    auto &label = _creditLabels.back().label;
    auto &labelRecord = _creditLabels.back();
    label.setText(isHeading ? text.substr(1) : text);
    int width, height;
    label.getSize(width, height);

    if (!isHeading && !isNextHeading && !isNextSpace) {
        isDoubled = true;
        labelRecord.position = {395 - width, 605};

        const auto text2 = _creditLines.front();
        _creditLines.erase(_creditLines.begin());

        isNextHeading = !_creditLines.empty() && !_creditLines.front().empty() && _creditLines.front()[0] == '*';
        _creditLabels.emplace_back(getNewFontLabel(isHeading));
        auto &label2 = _creditLabels.back().label;
        auto &labelRecord2 = _creditLabels.back();
        label2.setText(text2);
        labelRecord2.position = {405, 605};
    } else {
        labelRecord.position = {400 - width / 2, 605};
    }

    if (isHeading && isNextHeading)
        _cyclesUntilNextLine = 38;
    else if (isNextHeading)
        _cyclesUntilNextLine = isDoubled ? 38 : 57;
    else if (isHeading)
        _cyclesUntilNextLine = 38;
    else
        _cyclesUntilNextLine = 19;
}

CreditsLabelItem Credits::getNewFontLabel(const bool isHeading) {
    const auto labelColor = (isHeading) ? SDL_Color{0xff, 0x77, 0x77, 0xff} : SDL_Color{0xc7, 0xb3, 0x77, 0xff};
    return {Abyss::UI::Label(Common::GetFont("fontformal10"), labelColor, ""), isHeading, false, {0, 0}};
}

void Credits::onExitClicked() { Abyss::AbyssEngine::getInstance().setScene(std::make_unique<MainMenu::MainMenu>()); }

} // namespace OD2::Scenes::Credits

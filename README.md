# AbyssEngine Engine

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Patreon](https://img.shields.io/badge/dynamic/json?color=%23e85b46&label=Support%20us%20on%20Patreon&query=data.attributes.patron_count&suffix=%20patrons&url=https://www.patreon.com/api/campaigns/4762180)](https://www.patreon.com/bePatron?u=37261055)
[![Discord](https://img.shields.io/discord/515518620034662421?label=Discord&style=flat)](https://discord.gg/pRy8tdc)
[![Twitch Status](https://img.shields.io/twitch/status/essial?style=flat)](https://www.twitch.tv/essial)

Join us on IRC: [irc.libera.chat #opendiablo2](https://web.libera.chat/#opendiablo2)

----
AbyssEngine is a game engine designed to run games similar to 2000's style ARPGs such as Diablo II.
This repository provides the **core engine** as well as **OpenDiablo2**, an open-source re-implementation of Diablo II using the engine.

__*NOTE:*__ Running OpenDiablo2 requires you to already posses the **Diablo 2 + LOD** game files (MPQs) with 1.14d patch _or_ the *Diablo 2: Resurrected* game
files (CASCs).

## Installation

### Building from Source

1. Install [vcpkg](https://vcpkg.io/en/getting-started.html) (don't forget to `vcpkg integrate install` if you use
   Visual Studio!)
2. Either open the CMake project via Visual Studio, or use `-DCMAKE_TOOLCHAIN_FILE=...` option to cmake as described in
   vcpkg docs in your favorite IDE or shell.

Note for package maintainers: if all the C++ dependencies are found on the system already, vcpkg shouldn't be needed.

# dinputto8

dinputto8 is a dll module that is designed to improve compatibility in games using DirectInput 1-7 (dinput.dll) by converting all API calls to their equivalent DirectInput 8 (dinput8.dll) ones. This allows older games to be able to use newer tools and wrappers written for DirectInput 8.

### Installation

To install just download the latest binary release from the repository's [Release](https://github.com/elishacloud/dinputto8/releases) page and unzip the `dinput.dll` file into the folder of a DirectInput 1-7 program or game.

_Note: Do not attempt to overwrite any .dll in a Windows system directory as it is currently not supported and will not work._

### Uninstallation

Delete the `dinput.dll` files from the game's directory. You can also delete the associated log file, if there is one.

### Configuration

There are no configuration options for dinputto8.

### License
Copyright (C) 2019 Elisha Riedlinger

This software is provided 'as-is', without any express or implied warranty. In no event will the author(s) be held liable for any damages arising from the use of this software. Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

### Development
dinputto8 is written in C++ using Microsoft Visual Studio Community 2017.

Thanks for stopping by!

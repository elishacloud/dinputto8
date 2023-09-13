# dinputto8

dinputto8 is a dll module that is designed to improve compatibility in games using DirectInput 1-7 (dinput.dll) by converting all API calls to their equivalent DirectInput 8 (dinput8.dll) ones. This allows older games to be able to use newer tools and wrappers written for DirectInput 8.

### Installation

To install just download the latest binary release from the repository's [Release](https://github.com/elishacloud/dinputto8/releases) page and unzip the `dinput.dll` file into the folder of a DirectInput 1-7 program or game.

_Note: Do not attempt to overwrite any .dll in a Windows system directory as it is currently not supported and will not work._

### Uninstallation

Delete the `dinput.dll` files from the game's directory. You can also delete the associated log file, if there is one.

### Supported Games

 * 1NSANE
 * 8th Wonder of the World
 * Arcanum
 * Atlantis 3: The New World
 * B-17: Flying Fortress
 * Beyond Atlantis
 * Call To Power 2
 * Castle Adventure
 * Chris Sawyer's Locomotion
 * Colin McRae Rally 2.0
 * Commandos 2
 * Commandos 3
 * Conquest: Frontier Wars
 * Dark Reign
 * Digger XP
 * DOSBox
 * Dungeon Keeper 2
 * Enclave
 * FA-18E: Super Hornet
 * Gangsters: Organized Crime
 * Geneforge Saga 1-4
 * Hogs of War
 * Interstate 76
 * Interstate 82
 * Jazz Jackrabbit 2
 * Knight Rider
 * Knight Rider 2
 * Legacy of Kain: Soul Reaver
 * Lionheart: Legacy of the Crusader
 * MegaRace 3
 * Michelin Rally Masters: Race of Champions
 * Might and Magic IX
 * Moto Racer
 * Moto Racer 2
 * Myst III: Exile
 * NOX
 * Populous: The Beginning
 * Rayman 2: The Great Escape
 * Raymond 2
 * Requiem: Avenging Angel
 * Settlers 3
 * Shogo
 * Simon the Sorcerer 3D
 * Slave Zero
 * Star Wars Episode I: Racer
 * Swedish Touring Car Championship
 * Swedish Touring Car Championship 2
 * Uprising 2: Lead and Destroy
 * Uprising: Join or Die
 * V8 Challenge
 * Vampire: The Masquerade Redemption
 * Warrior Kings
 * Warrior Kings: Battles
 * X-COM: Interceptor

### Configuration

There are no configuration options for dinputto8.

### License
Copyright (C) 2022 Elisha Riedlinger

This software is provided 'as-is', without any express or implied warranty. In no event will the author(s) be held liable for any damages arising from the use of this software. Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

### Credits
dinputto8 uses code from some other projects. Below is a list of locations that source code was taken from:

 - [Aqrit's ddwrapper](http://bitpatch.com/ddwrapper.html): Includes code to read the ini config file.
 - [DDrawCompat](https://github.com/narzoul/DDrawCompat/): Includes code to for logging.
 - [noser-sandbox](https://code.google.com/archive/p/noser-sandbox/source/default/source) Includes code in EnumDevices() used to order the device list.  You will find the extracted code in the Rayman2InputFix_DirectInputA.cpp file from the Rayman2InputFix project.

### Development
dinputto8 is written in C++ using Microsoft Visual Studio Community 2017.

Thanks for stopping by!

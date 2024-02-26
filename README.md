## dtranslatebot
Open Source Discord Translation Bot

- Translate incoming channel messages to a Webhook
- Support configuration through slashcommands and JSON
- Cross-Platform

#### Build Dependencies
- CMake 3.16 or newer
- Compiler with C++17 Support
- [D++: A C++ Discord API Library for Bots](https://dpp.dev/)

#### Runtime Dependencies
- [LibreTranslate](https://libretranslate.com/)

#### Build dtranslatebot

```bash
git clone https://github.com/Syping/dtranslatebot
cmake -B dtranslatebot-build dtranslatebot
cmake --build dtranslatebot-build
sudo cmake --install dtranslatebot-build
```

##### Optional CMake flags
`-DWITH_BOOST=TRUE`  
`-DWITH_SYSTEMD=TRUE`

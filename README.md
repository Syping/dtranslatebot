## dtranslatebot
Open Source Discord Translation Bot

- Translate incoming channel messages to a Webhook
- Support configuration through slash commands and JSON
- Cross-Platform

#### Build Dependencies
- CMake 3.16 or newer
- Compiler with C++17 Support
- [D++: A C++ Discord API Library for Bots](https://dpp.dev/)

#### Supported Translation Engines
- [LibreTranslate](https://libretranslate.com/) (Default)
- [Lingva Translate](https://lingva.ml/)
- [Mozhi](https://codeberg.org/aryak/mozhi)
- [DeepL](https://deepl.com/) (Experimental)

#### Build dtranslatebot

```bash
git clone https://github.com/Syping/dtranslatebot
cmake -B dtranslatebot-build dtranslatebot
cmake --build dtranslatebot-build
sudo cmake --install dtranslatebot-build
```

##### Optional CMake flags
`-DWITH_BOOST=TRUE`  
`-DWITH_DPP_STATIC_BUNDLE=TRUE`  
`-DWITH_SYSTEMD=TRUE`

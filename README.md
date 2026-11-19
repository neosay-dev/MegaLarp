# MegaLarp

Открытый кроссплатформенный мод-меню (аналог MegaHack) для **Geometry Dash (2.207–2.209)** на базе **Geode SDK** и **Dear ImGui**.

Поддерживает **Windows (включая Steam/Proton на Arch Linux)**, **Android (32/64-bit)**, **macOS** и **iOS**.

## Возможности

### Gameplay
- **Noclip** + **Noclip Red Tint** + **Noclip Accuracy & Deaths HUD** (с обходом проверки `m_anticheatSpike` в 2.2)
- **Safe Mode** и **Auto Safe Mode** (автоматически защищает аккаунт от сохранения прогресса при включённых читах)
- **Speedhack** (0.05x – 5.0x) + синхронизация тона музыки (**Speedhack Audio Pitch**)
- **Practice Music Sync** и **Auto Practice Mode**
- **StartPos Switcher** (`Q` / `E` или кнопки `<` `>` в HUD)
- **Instant Restart**, **Jump Hack**, **Instant Complete**
- **No Respawn Blink**, **No Death Effect**, **No Mirror Effect**

### Visual & FPS
- **FPS Bypass** (30 – 360 FPS)
- **Show Hitboxes** и **Show Hitboxes on Death**
- **Hide Pause Menu**, **Hide Practice Buttons**, **Hide Attempts**
- **No Wave Pulse**, **No Glow**, **No Camera Shake**
- **Player Trail** (`Normal` / `Always On` / `Always Off`)
- **Rainbow Icon** с настройкой скорости

### Creator & Levels
- **Copy Hack** (копирование уровней без пароля)
- **Verify Hack** (выгрузка без верификации)
- **Object Limit Bypass**, **Custom Object Bypass**, **Slider Limit Bypass**
- **Level Edit** (кнопка редактора в паузе любого уровня)
- **Default Song Bypass**

### Global, QoL & UI
- **Unlock All Icons** (иконки и цвета локально)
- **No Transition** и **Fast Menu**
- **Confirm Exit** (подтверждение выхода из уровня)
- **Cheat Indicator** (индикатор статуса в левом верхнем углу: зелёный / жёлтый / оранжевый / красный)
- **FPS Counter**, **поиск по функциям**, **4 темы оформления**, **режим плавающих окон (как в MegaHack)** и **режим вкладок**
- **Редактор биндов** (по умолчанию: `Tab` — открыть/закрыть меню, `Q`/`E` — StartPos)
- **Плавающая кнопка `ML`** для Android / iOS

## Сборка

### Вариант 1: GitHub Actions (самый простой — собирает сразу под все платформы)
1. Залей репозиторий на GitHub.
2. Открой вкладку **Actions** → запусти workflow **Build MegaLarp** (или просто сделай `git push`).
3. Скачай готовый универсальный файл `.geode` из артефакта **MegaLarp-Universal** (или отдельно `MegaLarp-Win64` для Steam/Proton).

### Вариант 2: Локальная сборка на Arch Linux (для Windows / Proton)
1. Установи Geode CLI:
   ```bash
   cargo install geode-cli
   ```
2. Настрой SDK и кросс-компиляцию под Windows (`splat` / `xwin` + `clang-cl` + `lld`):
   ```bash
   geode sdk install-binaries
   geode sdk update
   ```
3. Собери мод:
   ```bash
   geode build
   ```

## Установка в Geometry Dash (Steam на Arch Linux через Proton)
1. Установи **Geode** в папку с Geometry Dash (через установщик Geode под Proton или вручную).
2. Скопируй собранный `megalarp.megalarp.geode` в:
   ```text
   ~/.local/share/Steam/steamapps/common/Geometry Dash/geode/mods/
   ```
3. Запусти игру и нажми **`Tab`**, чтобы открыть **MegaLarp**!

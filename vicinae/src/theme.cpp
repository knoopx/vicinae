#include "theme.hpp"
#include "template-engine/template-engine.hpp"
#include "timer.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "vicinae.hpp"
#include <QLinearGradient>
#include <filesystem>
#include <QStyleHints>
#include <stack>
#include <system_error>

namespace fs = std::filesystem;

// Helper function to get color with fallback - make inline for performance
inline static QColor getColorWithFallback(const QColor &primary, const QColor &fallback) {
  return primary.isValid() ? primary : fallback;
}

QColor ThemeInfo::resolveTint(SemanticColor tint) const {
  switch (tint) {
  // Basic palette
  case SemanticColor::Blue:
    return colors.blue;
  case SemanticColor::Green:
    return colors.green;
  case SemanticColor::Magenta:
    return colors.magenta;
  case SemanticColor::Orange:
    return colors.orange;
  case SemanticColor::Purple:
    return colors.purple;
  case SemanticColor::Red:
    return colors.red;
  case SemanticColor::Yellow:
    return colors.yellow;
  case SemanticColor::Cyan:
    return colors.cyan;

  // Text colors with fallbacks
  case SemanticColor::TextPrimary:
    return getColorWithFallback(colors.textPrimary, colors.text);
  case SemanticColor::TextSecondary:
    return getColorWithFallback(colors.textSecondary, colors.subtext);
  case SemanticColor::TextTertiary:
    return colors.textTertiary;
  case SemanticColor::TextDisabled:
    return colors.textDisabled;
  case SemanticColor::TextOnAccent:
    return colors.textOnAccent;
  case SemanticColor::TextError:
    return colors.textError;
  case SemanticColor::TextSuccess:
    return colors.textSuccess;
  case SemanticColor::TextWarning:
    return colors.textWarning;

  // Backgrounds
  case SemanticColor::MainBackground:
    return colors.mainBackground;
  case SemanticColor::MainHoverBackground:
    return colors.mainHoveredBackground;
  case SemanticColor::MainSelectedBackground:
    return colors.mainSelectedBackground;
  case SemanticColor::SecondaryBackground:
    return colors.secondaryBackground;
  case SemanticColor::TertiaryBackground:
    return colors.tertiaryBackground;

  // Button states - grouped by button type
  case SemanticColor::ButtonPrimary:
    return colors.buttonPrimary;
  case SemanticColor::ButtonPrimaryHover:
    return colors.buttonPrimaryHover;
  case SemanticColor::ButtonPrimaryPressed:
    return colors.buttonPrimaryPressed;
  case SemanticColor::ButtonPrimaryDisabled:
    return colors.buttonPrimaryDisabled;

  case SemanticColor::ButtonSecondary:
    return colors.buttonSecondary;
  case SemanticColor::ButtonSecondaryHover:
    return colors.buttonSecondaryHover;
  case SemanticColor::ButtonSecondaryPressed:
    return colors.buttonSecondaryPressed;
  case SemanticColor::ButtonSecondaryDisabled:
    return colors.buttonSecondaryDisabled;

  case SemanticColor::ButtonDestructive:
    return colors.buttonDestructive;
  case SemanticColor::ButtonDestructiveHover:
    return colors.buttonDestructiveHover;
  case SemanticColor::ButtonDestructivePressed:
    return colors.buttonDestructivePressed;
  case SemanticColor::ButtonDestructiveDisabled:
    return colors.buttonDestructiveDisabled;

  case SemanticColor::ButtonTertiary:
    return colors.buttonTertiary;
  case SemanticColor::ButtonTertiaryHover:
    return colors.buttonTertiaryHover;
  case SemanticColor::ButtonTertiaryPressed:
    return colors.buttonTertiaryPressed;
  case SemanticColor::ButtonTertiaryDisabled:
    return colors.buttonTertiaryDisabled;

  // Input states
  case SemanticColor::InputBackground:
    return colors.inputBackground;
  case SemanticColor::InputBorder:
    return colors.inputBorder;
  case SemanticColor::InputBorderFocus:
    return colors.inputBorderFocus;
  case SemanticColor::InputBorderError:
    return colors.inputBorderError;
  case SemanticColor::InputPlaceholder:
    return colors.inputPlaceholder;

  // Borders
  case SemanticColor::Border:
    return colors.border;
  case SemanticColor::BorderSubtle:
    return colors.borderSubtle;
  case SemanticColor::BorderStrong:
    return colors.borderStrong;
  case SemanticColor::Separator:
    return colors.separator;
  case SemanticColor::Shadow:
    return colors.shadow;

  // Status colors
  case SemanticColor::StatusBackground:
    return colors.statusBackground;
  case SemanticColor::StatusBorder:
    return colors.statusBackgroundBorder;
  case SemanticColor::StatusHover:
    return colors.statusBackgroundHover;

  case SemanticColor::ErrorBackground:
    return colors.errorBackground;
  case SemanticColor::ErrorBorder:
    return colors.errorBorder;
  case SemanticColor::SuccessBackground:
    return colors.successBackground;
  case SemanticColor::SuccessBorder:
    return colors.successBorder;
  case SemanticColor::WarningBackground:
    return colors.warningBackground;
  case SemanticColor::WarningBorder:
    return colors.warningBorder;

  // Interactive
  case SemanticColor::LinkDefault:
    return colors.linkDefault;
  case SemanticColor::LinkHover:
    return colors.linkHover;
  case SemanticColor::LinkVisited:
    return colors.linkVisited;

  // Special
  case SemanticColor::Focus:
    return colors.focus;
  case SemanticColor::Overlay:
    return colors.overlay;
  case SemanticColor::Tooltip:
    return colors.tooltip;
  case SemanticColor::TooltipText:
    return colors.tooltipText;

  default:
    return {};
  }
}

QColor ThemeInfo::adjustColorHSL(const QColor &base, int hueShift, float satMult, float lightMult) {
  // Input validation
  if (!base.isValid()) { return {}; }

  // Clamp multipliers to reasonable ranges to prevent overflow/underflow
  satMult = qBound(0.0f, satMult, 10.0f);
  lightMult = qBound(0.0f, lightMult, 10.0f);

  // Normalize hue shift to avoid excessive wrapping
  hueShift = hueShift % 360;

  auto hsl = base.toHsl();

  int newHue = (hsl.hue() + hueShift) % 360;
  if (newHue < 0) newHue += 360;

  int newSat = qBound(0, (int)(hsl.saturation() * satMult), 255);
  int newLight = qBound(0, (int)(hsl.lightness() * lightMult), 255);

  return QColor::fromHsl(newHue, newSat, newLight, hsl.alpha());
}

ThemeInfo ThemeInfo::fromParsed(const ParsedThemeData &scheme) {
  ThemeInfo info;

  // Set basic theme information
  info.id = scheme.id;
  info.name = scheme.name;
  info.appearance = scheme.appearance;
  info.icon = scheme.icon;
  info.description = scheme.description;

  // Copy basic palette colors
  info.colors.blue = scheme.palette.blue;
  info.colors.green = scheme.palette.green;
  info.colors.magenta = scheme.palette.magenta;
  info.colors.orange = scheme.palette.orange;
  info.colors.purple = scheme.palette.purple;
  info.colors.red = scheme.palette.red;
  info.colors.yellow = scheme.palette.yellow;
  info.colors.cyan = scheme.palette.cyan;

  // Assign all palette colors to ThemeInfo colors using helper function
  assignPaletteColors(info.colors, scheme.palette);

  // Set mainBackground from palette if not already set
  if (!info.colors.mainBackground.isValid()) { info.colors.mainBackground = scheme.palette.background; }

  bool isDark = (scheme.appearance == "dark");

  // Generate fallback colors for missing values
  generateFallbackColors(info.colors, scheme, isDark);

  // Generate derived colors using helper functions
  generateTextColors(info.colors, scheme.palette.foreground, isDark);

  // Generate button variants using helper functions
  generateButtonColors(info.colors, scheme.palette.blue, isDark);
  generateSecondaryButtonColors(info.colors, info.colors.mainBackground, isDark);
  generateDestructiveButtonColors(info.colors, scheme.palette.red, isDark);

  // Generate input colors and borders
  generateInputColors(info.colors, scheme, isDark);

  // Generate status colors using helper function
  generateStatusColors(info.colors, scheme.palette.red, scheme.palette.green, scheme.palette.orange,
                       scheme.palette.blue, isDark);

  // Generate link colors
  generateLinkColors(info.colors, scheme, isDark);

  // Generate special colors
  generateSpecialColors(info.colors, scheme, isDark);

  // Generate semantic text colors
  generateSemanticTextColors(info.colors, scheme.palette.green, scheme.palette.orange, isDark);

  return info;
}

void ThemeService::setTheme(const ThemeInfo &info) {
  m_theme = info;

  double mainInputSize = std::round(m_baseFontPointSize * 1.20);

  TemplateEngine engine;

  engine.setVar("FONT_SIZE", QString::number(m_baseFontPointSize));
  engine.setVar("INPUT_BORDER_COLOR", info.colors.border.name());
  engine.setVar("INPUT_FOCUS_BORDER_COLOR", info.colors.inputBorderFocus.name());
  engine.setVar("SEARCH_FONT_SIZE", QString::number(mainInputSize));

  /**
   * We try to not use stylesheets directly in most of the app, but some very high level
   * rules can help fix issues that would be hard to fix otherwise.
   */
  auto style = engine.build(R"(
  		QWidget {
			font-size: {FONT_SIZE}pt;
		}

		QLineEdit, QTextEdit, QPlainTextEdit {
			background-color: transparent;
			border: none;
 		}

		QWidget[form-input="true"] {
		    border: 2px solid {INPUT_BORDER_COLOR};
			border-radius: 5px;
		}

		QLineEdit[form-input="true"]:focus {
			border-color: {INPUT_FOCUS_BORDER_COLOR};
		}

		QPlainTextEdit[form-input="true"]:focus {
			border-color: {INPUT_FOCUS_BORDER_COLOR};
		}

		QLineEdit[search-input="true"] {
			font-size: {SEARCH_FONT_SIZE}pt;
		}

		QScrollArea,
		QScrollArea > QWidget,
		QScrollArea > QWidget > QWidget {
			background: transparent;
		}
		)");

  auto palette = QApplication::palette();

  palette.setBrush(QPalette::WindowText, info.colors.text);
  palette.setBrush(QPalette::Text, info.colors.text);
  palette.setBrush(QPalette::Link, info.colors.linkDefault);
  palette.setBrush(QPalette::LinkVisited, info.colors.linkVisited);

  QColor placeholderText = info.colors.subtext;

  placeholderText.setAlpha(200);

  OmniPainter painter;

  palette.setBrush(QPalette::PlaceholderText, placeholderText);
  palette.setBrush(QPalette::Highlight, painter.colorBrush(info.colors.blue));
  palette.setBrush(QPalette::HighlightedText, info.colors.text);

  QApplication::setPalette(palette);

  Timer timer;
  qApp->setStyleSheet(style);
  timer.time("Theme changed");

  emit themeChanged(info);
}

void ThemeService::registerBuiltinThemes() {
  for (const auto &scheme : loadColorSchemes()) {
    registerTheme(ThemeInfo::fromParsed(scheme));
  }
}

std::optional<ThemeInfo> ThemeService::findTheme(const QString &name) {
  auto it = std::ranges::find_if(m_themes, [&](auto &&theme) { return theme.id == name; });

  if (it == m_themes.end()) {
    QString normalized = QString("%1.json").arg(name);
    it = std::ranges::find_if(m_themes, [&](auto &&theme) { return theme.id == normalized; });
  }

  if (it == m_themes.end()) return {};

  return *it;
}

void ThemeService::upsertTheme(const ParsedThemeData &data) {
  auto info = ThemeInfo::fromParsed(data);
  auto it = std::ranges::find_if(m_themes, [&](const ThemeInfo &model) { return info.id == model.id; });

  if (it == m_themes.end()) {
    m_themes.emplace_back(info);
    return;
  }

  *it = ThemeInfo::fromParsed(data);
}

void ThemeService::scanThemeDirectories() {
  auto configThemes = Omnicast::configDir() / "themes";
  auto dataThemes = Omnicast::dataDir() / "themes";

  scanThemeDirectory(configThemes);
  scanThemeDirectory(dataThemes);

  for (const auto dir : Omnicast::xdgDataDirs()) {
    fs::path themeDir = dir / "vicinae" / "themes";
    std::error_code ec;

    if (!fs::is_directory(themeDir, ec)) continue;

    scanThemeDirectory(themeDir);
  }
}

void ThemeService::scanThemeDirectory(const std::filesystem::path &path) {
  std::error_code ec;
  std::stack<std::filesystem::path> dirs;

  dirs.push(path);

  while (!dirs.empty()) {
    auto dir = dirs.top();
    dirs.pop();
    auto it = std::filesystem::directory_iterator(dir, ec);

    for (const auto &entry : it) {
      if (entry.is_directory()) {
        dirs.push(entry.path());
        continue;
      }

      bool isJson = entry.path().string().ends_with(".json");

      if (!isJson) continue;

      QFile file(entry.path());

      if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Theme: failed to open" << entry.path() << "for reading";
        continue;
      }

      QJsonParseError error;
      auto json = QJsonDocument::fromJson(file.readAll(), &error);

      if (error.error != QJsonParseError::NoError) {
        qCritical() << "Failed to parse" << entry.path() << "as JSON: " << error.errorString();
        continue;
      }

      QJsonObject obj = json.object();
      ParsedThemeData theme;

      theme.id = QString::fromStdString(entry.path().filename().string());
      theme.appearance = obj.value("appearance").toString();
      theme.name = obj.value("name").toString();
      theme.description = obj.value("description").toString();

      if (theme.name.isEmpty()) {
        qCritical() << "Ignoring theme" << entry.path() << "=> missing name field";
        continue;
      }

      if (obj.contains("icon")) {
        QString rawIcon = obj.value("icon").toString();

        if (rawIcon.isEmpty()) { qWarning() << "'icon' field specified but empty"; }

        // assuming absolute path
        if (rawIcon.startsWith("/")) {
          theme.icon = rawIcon.toStdString();
        } else {
          theme.icon = dir / rawIcon.toStdString();
        }
      }

      auto colors = obj.value("palette").toObject();

      // Helper function to extract color values
      auto extractColor = [&](const QString &key) { return colors.value(key).toString(); };

      // Basic palette colors
      theme.palette.background = extractColor("background");
      theme.palette.foreground = extractColor("foreground");
      theme.palette.blue = extractColor("blue");
      theme.palette.green = extractColor("green");
      theme.palette.magenta = extractColor("magenta");
      theme.palette.orange = extractColor("orange");
      theme.palette.purple = extractColor("purple");
      theme.palette.red = extractColor("red");
      theme.palette.yellow = extractColor("yellow");
      theme.palette.cyan = extractColor("cyan");

      // Text colors
      theme.palette.textPrimary = extractColor("textPrimary");
      theme.palette.textSecondary = extractColor("textSecondary");
      theme.palette.textTertiary = extractColor("textTertiary");
      theme.palette.textDisabled = extractColor("textDisabled");
      theme.palette.textOnAccent = extractColor("textOnAccent");
      theme.palette.textError = extractColor("textError");
      theme.palette.textSuccess = extractColor("textSuccess");
      theme.palette.textWarning = extractColor("textWarning");

      // Background colors
      theme.palette.mainBackground = extractColor("mainBackground");
      theme.palette.mainHoverBackground = extractColor("mainHoverBackground");
      theme.palette.mainSelectedBackground = extractColor("mainSelectedBackground");
      theme.palette.secondaryBackground = extractColor("secondaryBackground");
      theme.palette.tertiaryBackground = extractColor("tertiaryBackground");

      // Button colors - grouped by button type
      theme.palette.buttonPrimary = extractColor("buttonPrimary");
      theme.palette.buttonPrimaryHover = extractColor("buttonPrimaryHover");
      theme.palette.buttonPrimaryPressed = extractColor("buttonPrimaryPressed");
      theme.palette.buttonPrimaryDisabled = extractColor("buttonPrimaryDisabled");

      theme.palette.buttonSecondary = extractColor("buttonSecondary");
      theme.palette.buttonSecondaryHover = extractColor("buttonSecondaryHover");
      theme.palette.buttonSecondaryPressed = extractColor("buttonSecondaryPressed");
      theme.palette.buttonSecondaryDisabled = extractColor("buttonSecondaryDisabled");

      theme.palette.buttonDestructive = extractColor("buttonDestructive");
      theme.palette.buttonDestructiveHover = extractColor("buttonDestructiveHover");
      theme.palette.buttonDestructivePressed = extractColor("buttonDestructivePressed");

      // Input colors
      theme.palette.inputBackground = extractColor("inputBackground");
      theme.palette.inputBorder = extractColor("inputBorder");
      theme.palette.inputBorderFocus = extractColor("inputBorderFocus");
      theme.palette.inputBorderError = extractColor("inputBorderError");
      theme.palette.inputPlaceholder = extractColor("inputPlaceholder");

      // UI element colors
      theme.palette.border = extractColor("border");
      theme.palette.borderSubtle = extractColor("borderSubtle");
      theme.palette.borderStrong = extractColor("borderStrong");
      theme.palette.separator = extractColor("separator");
      theme.palette.shadow = extractColor("shadow");

      // Status colors
      theme.palette.statusBackground = extractColor("statusBackground");
      theme.palette.statusBorder = extractColor("statusBorder");
      theme.palette.statusHover = extractColor("statusHover");

      theme.palette.errorBackground = extractColor("errorBackground");
      theme.palette.errorBorder = extractColor("errorBorder");
      theme.palette.successBackground = extractColor("successBackground");
      theme.palette.successBorder = extractColor("successBorder");
      theme.palette.warningBackground = extractColor("warningBackground");
      theme.palette.warningBorder = extractColor("warningBorder");

      // Interactive colors
      theme.palette.linkDefault = extractColor("linkDefault");
      theme.palette.linkHover = extractColor("linkHover");
      theme.palette.linkVisited = extractColor("linkVisited");

      // Special colors
      theme.palette.focus = extractColor("focus");
      theme.palette.overlay = extractColor("overlay");
      theme.palette.tooltip = extractColor("tooltip");
      theme.palette.tooltipText = extractColor("tooltipText");

      upsertTheme(theme);

      // use default
    }
  }
}

// Helper function to create a theme with common structure
static ParsedThemeData createBuiltinTheme(const QString &name, const QString &description, const QString &id,
                                          const QString &appearance, const QString &bg, const QString &fg,
                                          const QString &blue, const QString &green, const QString &magenta,
                                          const QString &orange, const QString &purple, const QString &red,
                                          const QString &yellow, const QString &cyan) {
  ParsedThemeData theme;
  theme.name = name;
  theme.description = description;
  theme.id = id;
  theme.appearance = appearance;
  theme.palette = ColorPalette{.background = bg,
                               .foreground = fg,
                               .blue = blue,
                               .green = green,
                               .magenta = magenta,
                               .orange = orange,
                               .purple = purple,
                               .red = red,
                               .yellow = yellow,
                               .cyan = cyan};
  return theme;
}

std::vector<ParsedThemeData> ThemeService::loadColorSchemes() const {
  std::vector<ParsedThemeData> schemes;
  schemes.reserve(2);

  // Light theme
  schemes.emplace_back(createBuiltinTheme("Vicinae Light", "Default Vicinae light palette", "vicinae-light",
                                          "light", "#F4F2EE", "#1A1A1A", "#1F6FEB", "#3A9C61", "#A48ED6",
                                          "#DA8A48", "#8374B7", "#C25C49", "#BFAE78", "#18A5B3"));

  // Dark theme
  schemes.emplace_back(createBuiltinTheme("Vicinae Dark", "Default Vicinae dark palette", "vicinae-dark",
                                          "dark", "#1A1A1A", "#E8E6E1", "#2F6FED", "#3A9C61", "#BC8CFF",
                                          "#F0883E", "#7267B0", "#B9543B", "#BFAE78", "#18A5B3"));

  return schemes;
}

ThemeService &ThemeService::instance() {
  static ThemeService _instance;

  return _instance;
}

double ThemeService::pointSize(TextSize size) const {
  switch (size) {
  case TextSize::TextRegular:
    return m_baseFontPointSize;
  case TextSize::TextTitle:
    return m_baseFontPointSize * 1.5;
  case TextSize::TextSmaller:
    return m_baseFontPointSize * 0.9;
  }

  return m_baseFontPointSize;
}

void ThemeService::setFontBasePointSize(double pointSize) { m_baseFontPointSize = pointSize; }

void ThemeService::reloadCurrentTheme() { setTheme(m_theme.id); }

std::optional<ThemeInfo> ThemeService::theme(const QString &name) const {
  for (const auto &info : m_themes) {
    if (info.name == name) { return info; }
  }

  return std::nullopt;
}

bool ThemeService::setTheme(const QString &name) {
  if (auto theme = findTheme(name)) {
    setTheme(*theme);
    return true;
  }

  return false;
}

const ThemeInfo &ThemeService::theme() const { return m_theme; }

ColorLike ThemeService::getTintColor(SemanticColor tint) const { return m_theme.resolveTint(tint); }

ThemeService::ThemeService() {
  registerBuiltinThemes();
  scanThemeDirectories();
  setTheme("vicinae-dark");
}

// Helper function implementations
void ThemeInfo::generateTextColors(Colors &colors, const QColor &baseText, bool isDark) {
  colors.textPrimary = baseText;
  colors.textSecondary = adjustColorHSL(baseText, 0, 1.0f, isDark ? 0.7f : 1.3f);
  colors.textTertiary = adjustColorHSL(baseText, 0, 1.0f, isDark ? 0.5f : 1.6f);
  colors.textDisabled = adjustColorHSL(baseText, 0, 0.5f, isDark ? 0.4f : 1.8f);
  colors.textOnAccent = isDark ? QColor("#ffffff") : QColor("#000000");
}

void ThemeInfo::generateButtonColors(Colors &colors, const QColor &primaryColor, bool isDark) {
  colors.buttonPrimary = primaryColor;
  colors.buttonPrimaryHover = adjustColorHSL(primaryColor, 0, 1.0f, isDark ? 1.2f : 0.9f);
  colors.buttonPrimaryPressed = adjustColorHSL(primaryColor, 0, 1.0f, isDark ? 1.4f : 0.8f);
  colors.buttonPrimaryDisabled = adjustColorHSL(primaryColor, 0, 0.3f, isDark ? 0.6f : 1.4f);
}

void ThemeInfo::generateSecondaryButtonColors(Colors &colors, const QColor &baseBackground, bool isDark) {
  colors.buttonSecondary = adjustColorHSL(baseBackground, 0, 1.0f, isDark ? 1.3f : 0.9f);
  colors.buttonSecondaryHover = adjustColorHSL(baseBackground, 0, 1.0f, isDark ? 1.5f : 0.8f);
  colors.buttonSecondaryPressed = adjustColorHSL(baseBackground, 0, 1.0f, isDark ? 1.7f : 0.7f);
  colors.buttonSecondaryDisabled = adjustColorHSL(baseBackground, 0, 0.3f, isDark ? 0.8f : 1.2f);
}

void ThemeInfo::generateDestructiveButtonColors(Colors &colors, const QColor &redColor, bool isDark) {
  colors.buttonDestructive = redColor;
  colors.buttonDestructiveHover = adjustColorHSL(redColor, 0, 1.0f, isDark ? 1.2f : 0.9f);
  colors.buttonDestructivePressed = adjustColorHSL(redColor, 0, 1.0f, isDark ? 1.4f : 0.8f);
}

void ThemeInfo::generateBorderVariations(Colors &colors, const QColor &baseBorder, bool isDark) {
  colors.borderSubtle = adjustColorHSL(baseBorder, 0, 0.5f, isDark ? 0.7f : 1.3f);
  colors.borderStrong = adjustColorHSL(baseBorder, 0, 1.2f, isDark ? 1.4f : 0.8f);
  colors.separator = adjustColorHSL(baseBorder, 0, 0.8f, isDark ? 0.8f : 1.2f);
  colors.shadow = adjustColorHSL(baseBorder, 0, 0.3f, isDark ? 0.3f : 1.8f);
}

void ThemeInfo::generateStatusColors(Colors &colors, const QColor &red, const QColor &green,
                                     const QColor &orange, const QColor &blue, bool isDark) {
  // Error states
  colors.errorBackground = adjustColorHSL(red, 0, 0.3f, isDark ? 0.2f : 1.9f);
  colors.errorBorder = adjustColorHSL(red, 0, 0.8f, isDark ? 0.8f : 1.2f);

  // Success states
  colors.successBackground = adjustColorHSL(green, 0, 0.3f, isDark ? 0.2f : 1.9f);
  colors.successBorder = adjustColorHSL(green, 0, 0.8f, isDark ? 0.8f : 1.2f);

  // Warning states
  colors.warningBackground = adjustColorHSL(orange, 0, 0.3f, isDark ? 0.2f : 1.9f);
  colors.warningBorder = adjustColorHSL(orange, 0, 0.8f, isDark ? 0.8f : 1.2f);

  // Status (using blue)
  colors.statusBackground = adjustColorHSL(blue, 0, 0.3f, isDark ? 0.2f : 1.9f);
  colors.statusBackgroundBorder = adjustColorHSL(blue, 0, 0.8f, isDark ? 0.8f : 1.2f);
  colors.statusBackgroundHover = adjustColorHSL(blue, 0, 0.4f, isDark ? 0.3f : 1.8f);
  colors.statusBackgroundLighter = adjustColorHSL(blue, 0, 0.2f, isDark ? 0.15f : 1.95f);
}

void ThemeInfo::generateSemanticTextColors(Colors &colors, const QColor &green, const QColor &orange,
                                           bool isDark) {
  colors.textSuccess = adjustColorHSL(green, 0, 1.0f, isDark ? 1.2f : 0.8f);
  colors.textWarning = adjustColorHSL(orange, 0, 1.0f, isDark ? 1.2f : 0.8f);
}

// Helper functions for fromParsed to break down the massive function
void ThemeInfo::assignPaletteColors(Colors &colors, const ColorPalette &palette) {
  // Assign all palette colors to ThemeInfo colors if they're valid
  if (palette.textPrimary.isValid()) colors.text = palette.textPrimary;
  if (palette.textSecondary.isValid()) colors.subtext = palette.textSecondary;
  if (palette.textTertiary.isValid()) colors.textTertiary = palette.textTertiary;
  if (palette.textDisabled.isValid()) colors.textDisabled = palette.textDisabled;
  if (palette.textOnAccent.isValid()) colors.textOnAccent = palette.textOnAccent;
  if (palette.textError.isValid()) colors.textError = palette.textError;
  if (palette.textSuccess.isValid()) colors.textSuccess = palette.textSuccess;
  if (palette.textWarning.isValid()) colors.textWarning = palette.textWarning;

  if (palette.mainBackground.isValid()) colors.mainBackground = palette.mainBackground;
  if (palette.mainHoverBackground.isValid()) colors.mainHoveredBackground = palette.mainHoverBackground;
  if (palette.mainSelectedBackground.isValid())
    colors.mainSelectedBackground = palette.mainSelectedBackground;
  if (palette.secondaryBackground.isValid()) colors.secondaryBackground = palette.secondaryBackground;
  if (palette.tertiaryBackground.isValid()) colors.tertiaryBackground = palette.tertiaryBackground;

  if (palette.buttonPrimary.isValid()) colors.buttonPrimary = palette.buttonPrimary;
  if (palette.buttonPrimaryHover.isValid()) colors.buttonPrimaryHover = palette.buttonPrimaryHover;
  if (palette.buttonPrimaryPressed.isValid()) colors.buttonPrimaryPressed = palette.buttonPrimaryPressed;
  if (palette.buttonPrimaryDisabled.isValid()) colors.buttonPrimaryDisabled = palette.buttonPrimaryDisabled;

  if (palette.buttonSecondary.isValid()) colors.buttonSecondary = palette.buttonSecondary;
  if (palette.buttonSecondaryHover.isValid()) colors.buttonSecondaryHover = palette.buttonSecondaryHover;
  if (palette.buttonSecondaryPressed.isValid())
    colors.buttonSecondaryPressed = palette.buttonSecondaryPressed;
  if (palette.buttonSecondaryDisabled.isValid())
    colors.buttonSecondaryDisabled = palette.buttonSecondaryDisabled;

  if (palette.buttonDestructive.isValid()) colors.buttonDestructive = palette.buttonDestructive;
  if (palette.buttonDestructiveHover.isValid())
    colors.buttonDestructiveHover = palette.buttonDestructiveHover;
  if (palette.buttonDestructivePressed.isValid())
    colors.buttonDestructivePressed = palette.buttonDestructivePressed;

  if (palette.inputBackground.isValid()) colors.inputBackground = palette.inputBackground;
  if (palette.inputBorder.isValid()) colors.inputBorder = palette.inputBorder;
  if (palette.inputBorderFocus.isValid()) colors.inputBorderFocus = palette.inputBorderFocus;
  if (palette.inputBorderError.isValid()) colors.inputBorderError = palette.inputBorderError;
  if (palette.inputPlaceholder.isValid()) colors.inputPlaceholder = palette.inputPlaceholder;

  if (palette.border.isValid()) colors.border = palette.border;
  if (palette.borderSubtle.isValid()) colors.borderSubtle = palette.borderSubtle;
  if (palette.borderStrong.isValid()) colors.borderStrong = palette.borderStrong;
  if (palette.separator.isValid()) colors.separator = palette.separator;
  if (palette.shadow.isValid()) colors.shadow = palette.shadow;

  if (palette.statusBackground.isValid()) colors.statusBackground = palette.statusBackground;
  if (palette.statusBorder.isValid()) colors.statusBackgroundBorder = palette.statusBorder;
  if (palette.statusHover.isValid()) colors.statusBackgroundHover = palette.statusHover;

  if (palette.errorBackground.isValid()) colors.errorBackground = palette.errorBackground;
  if (palette.errorBorder.isValid()) colors.errorBorder = palette.errorBorder;
  if (palette.successBackground.isValid()) colors.successBackground = palette.successBackground;
  if (palette.successBorder.isValid()) colors.successBorder = palette.successBorder;
  if (palette.warningBackground.isValid()) colors.warningBackground = palette.warningBackground;
  if (palette.warningBorder.isValid()) colors.warningBorder = palette.warningBorder;

  if (palette.linkDefault.isValid()) colors.linkDefault = palette.linkDefault;
  if (palette.linkHover.isValid()) colors.linkHover = palette.linkHover;
  if (palette.linkVisited.isValid()) colors.linkVisited = palette.linkVisited;

  if (palette.focus.isValid()) colors.focus = palette.focus;
  if (palette.overlay.isValid()) colors.overlay = palette.overlay;
  if (palette.tooltip.isValid()) colors.tooltip = palette.tooltip;
  if (palette.tooltipText.isValid()) colors.tooltipText = palette.tooltipText;
}

void ThemeInfo::generateFallbackColors(Colors &colors, const ParsedThemeData &scheme, bool isDark) {
  // Base colors - only set if not provided in palette
  if (!colors.mainBackground.isValid()) colors.mainBackground = scheme.palette.background;
  if (!colors.border.isValid()) {
    colors.border = adjustColorHSL(colors.mainBackground, 0, isDark ? 0.5f : 0.6f, isDark ? 1.8f : 0.75f);
  }
  if (!colors.mainSelectedBackground.isValid()) {
    colors.mainSelectedBackground =
        adjustColorHSL(colors.mainBackground, 0, isDark ? 1.1f : 1.2f, isDark ? 1.4f : 0.9f);
  }
  if (!colors.mainHoveredBackground.isValid()) {
    colors.mainHoveredBackground =
        adjustColorHSL(colors.mainBackground, 0, isDark ? 1.0f : 1.1f, isDark ? 1.3f : 0.95f);
  }
  if (!colors.statusBackground.isValid()) {
    colors.statusBackground =
        adjustColorHSL(colors.mainBackground, 0, isDark ? 0.8f : 0.9f, isDark ? 1.3f : 0.92f);
  }
  if (!colors.statusBackgroundLighter.isValid()) {
    colors.statusBackgroundLighter =
        adjustColorHSL(colors.statusBackground, 0, isDark ? 0.9f : 0.8f, isDark ? 1.2f : 0.96f);
  }
  if (!colors.statusBackgroundHover.isValid()) {
    colors.statusBackgroundHover =
        adjustColorHSL(colors.statusBackground, 0, isDark ? 1.0f : 1.1f, isDark ? 1.1f : 0.88f);
  }
  if (!colors.statusBackgroundBorder.isValid()) {
    colors.statusBackgroundBorder =
        adjustColorHSL(colors.statusBackground, 0, isDark ? 0.6f : 0.7f, isDark ? 0.6f : 0.8f);
  }
  if (!colors.text.isValid()) colors.text = scheme.palette.foreground;
  if (!colors.subtext.isValid()) {
    colors.subtext = adjustColorHSL(scheme.palette.foreground, 0, isDark ? 0.8f : 0.7f, isDark ? 0.7f : 1.4f);
  }

  // Background levels - only generate if not provided
  if (!colors.secondaryBackground.isValid()) {
    colors.secondaryBackground =
        adjustColorHSL(colors.mainBackground, 0, isDark ? 0.9f : 0.8f, isDark ? 1.2f : 0.95f);
  }
  if (!colors.tertiaryBackground.isValid()) {
    colors.tertiaryBackground =
        adjustColorHSL(colors.mainBackground, 0, isDark ? 0.8f : 0.9f, isDark ? 0.8f : 1.05f);
  }
}

void ThemeInfo::generateInputColors(Colors &colors, const ParsedThemeData &scheme, bool isDark) {
  // Input and border variations - only generate if not provided
  if (!colors.inputBackground.isValid()) {
    if (isDark) {
      colors.inputBackground = adjustColorHSL(colors.mainBackground, 0, 0.7f, 1.1f);
    } else {
      colors.inputBackground = QColor("#FFFFFF");
    }
  }
  if (!colors.inputBorder.isValid()) {
    colors.inputBorder = adjustColorHSL(colors.mainBackground, 0, isDark ? 0.6f : 0.5f, isDark ? 1.5f : 0.7f);
  }
  if (!colors.inputBorderFocus.isValid()) colors.inputBorderFocus = scheme.palette.blue;
  if (!colors.inputBorderError.isValid()) colors.inputBorderError = scheme.palette.red;
  if (!colors.inputPlaceholder.isValid()) {
    colors.inputPlaceholder = adjustColorHSL(scheme.palette.foreground, 0, 0.5f, isDark ? 0.6f : 1.5f);
  }

  // Generate border variations and shadow
  if (!colors.borderSubtle.isValid() || !colors.borderStrong.isValid() || !colors.separator.isValid() ||
      !colors.shadow.isValid()) {
    generateBorderVariations(colors, colors.border, isDark);
  }
  if (!colors.shadow.isValid()) { colors.shadow = QColor(0, 0, 0, isDark ? 80 : 40); }
}

void ThemeInfo::generateLinkColors(Colors &colors, const ParsedThemeData &scheme, bool isDark) {
  // Links - only generate if not provided
  if (!colors.linkDefault.isValid()) {
    colors.linkDefault = adjustColorHSL(scheme.palette.blue, 0, isDark ? 1.0f : 1.1f, isDark ? 1.3f : 0.8f);
  }
  if (!colors.linkHover.isValid()) {
    colors.linkHover = adjustColorHSL(scheme.palette.blue, 0, isDark ? 1.2f : 1.3f, isDark ? 1.5f : 0.7f);
  }
  if (!colors.linkVisited.isValid()) {
    colors.linkVisited = adjustColorHSL(scheme.palette.purple, 0, isDark ? 1.0f : 1.1f, isDark ? 1.2f : 0.8f);
  }
}

void ThemeInfo::generateSpecialColors(Colors &colors, const ParsedThemeData &scheme, bool isDark) {
  // Special elements - only generate if not provided
  if (!colors.focus.isValid()) colors.focus = scheme.palette.blue;
  if (!colors.overlay.isValid()) { colors.overlay = QColor(0, 0, 0, isDark ? 120 : 80); }
  if (!colors.tooltip.isValid()) {
    colors.tooltip = adjustColorHSL(colors.mainBackground, 0, isDark ? 0.8f : 0.7f, isDark ? 2.0f : 0.2f);
  }
  if (!colors.tooltipText.isValid()) {
    colors.tooltipText = isDark ? scheme.palette.foreground : QColor("#FFFFFF");
  }
}

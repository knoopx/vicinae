#include "theme.hpp"
#include "template-engine/template-engine.hpp"
#include "timer.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "vicinae.hpp"
#include <QLinearGradient>
#include <filesystem>
#include <QStyleHints>
#include <system_error>

namespace fs = std::filesystem;

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

    // Text colors
  case SemanticColor::TextPrimary:
    return colors.textPrimary.isValid() ? colors.textPrimary : colors.text;
  case SemanticColor::TextSecondary:
    return colors.textSecondary.isValid() ? colors.textSecondary : colors.subtext;
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

  // Button states
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
    break;
  }

  return {};
}

QColor ThemeInfo::adjustColorHSL(const QColor &base, int hueShift, float satMult, float lightMult) {
  auto hsl = base.toHsl();

  int newHue = (hsl.hue() + hueShift) % 360;
  if (newHue < 0) newHue += 360;

  int newSat = qBound(0, (int)(hsl.saturation() * satMult), 255);
  int newLight = qBound(0, (int)(hsl.lightness() * lightMult), 255);

  return QColor::fromHsl(newHue, newSat, newLight, hsl.alpha());
}

ThemeInfo ThemeInfo::fromParsed(const ParsedThemeData &scheme) {
  ThemeInfo info;

  // IMPORTANT: most of the semantic colors derived from the palette have been generated
  // but are not used yet, only the main ones are.
  // Eventually we will shit toward using more meaningful semantic colors for particular elements.

  info.id = scheme.id;
  info.name = scheme.name;
  info.appearance = scheme.appearance;
  info.icon = scheme.icon;
  info.description = scheme.description;
  info.colors.blue = scheme.palette.blue;
  info.colors.green = scheme.palette.green;
  info.colors.magenta = scheme.palette.magenta;
  info.colors.orange = scheme.palette.orange;
  info.colors.purple = scheme.palette.purple;
  info.colors.red = scheme.palette.red;
  info.colors.yellow = scheme.palette.yellow;
  info.colors.cyan = scheme.palette.cyan;

  // Assign all palette colors to ThemeInfo colors
  if (scheme.palette.textPrimary.isValid()) info.colors.text = scheme.palette.textPrimary;
  if (scheme.palette.textSecondary.isValid()) info.colors.subtext = scheme.palette.textSecondary;
  if (scheme.palette.textTertiary.isValid()) info.colors.textTertiary = scheme.palette.textTertiary;
  if (scheme.palette.textDisabled.isValid()) info.colors.textDisabled = scheme.palette.textDisabled;
  if (scheme.palette.textOnAccent.isValid()) info.colors.textOnAccent = scheme.palette.textOnAccent;
  if (scheme.palette.textError.isValid()) info.colors.textError = scheme.palette.textError;
  if (scheme.palette.textSuccess.isValid()) info.colors.textSuccess = scheme.palette.textSuccess;
  if (scheme.palette.textWarning.isValid()) info.colors.textWarning = scheme.palette.textWarning;

  if (scheme.palette.mainBackground.isValid()) info.colors.mainBackground = scheme.palette.mainBackground;
  if (scheme.palette.mainHoverBackground.isValid())
    info.colors.mainHoveredBackground = scheme.palette.mainHoverBackground;
  if (scheme.palette.mainSelectedBackground.isValid())
    info.colors.mainSelectedBackground = scheme.palette.mainSelectedBackground;
  if (scheme.palette.secondaryBackground.isValid())
    info.colors.secondaryBackground = scheme.palette.secondaryBackground;
  if (scheme.palette.tertiaryBackground.isValid())
    info.colors.tertiaryBackground = scheme.palette.tertiaryBackground;

  if (scheme.palette.buttonPrimary.isValid()) info.colors.buttonPrimary = scheme.palette.buttonPrimary;
  if (scheme.palette.buttonPrimaryHover.isValid())
    info.colors.buttonPrimaryHover = scheme.palette.buttonPrimaryHover;
  if (scheme.palette.buttonPrimaryPressed.isValid())
    info.colors.buttonPrimaryPressed = scheme.palette.buttonPrimaryPressed;
  if (scheme.palette.buttonPrimaryDisabled.isValid())
    info.colors.buttonPrimaryDisabled = scheme.palette.buttonPrimaryDisabled;

  if (scheme.palette.buttonSecondary.isValid()) info.colors.buttonSecondary = scheme.palette.buttonSecondary;
  if (scheme.palette.buttonSecondaryHover.isValid())
    info.colors.buttonSecondaryHover = scheme.palette.buttonSecondaryHover;
  if (scheme.palette.buttonSecondaryPressed.isValid())
    info.colors.buttonSecondaryPressed = scheme.palette.buttonSecondaryPressed;
  if (scheme.palette.buttonSecondaryDisabled.isValid())
    info.colors.buttonSecondaryDisabled = scheme.palette.buttonSecondaryDisabled;

  if (scheme.palette.buttonDestructive.isValid())
    info.colors.buttonDestructive = scheme.palette.buttonDestructive;
  if (scheme.palette.buttonDestructiveHover.isValid())
    info.colors.buttonDestructiveHover = scheme.palette.buttonDestructiveHover;
  if (scheme.palette.buttonDestructivePressed.isValid())
    info.colors.buttonDestructivePressed = scheme.palette.buttonDestructivePressed;

  if (scheme.palette.inputBackground.isValid()) info.colors.inputBackground = scheme.palette.inputBackground;
  if (scheme.palette.inputBorder.isValid()) info.colors.inputBorder = scheme.palette.inputBorder;
  if (scheme.palette.inputBorderFocus.isValid())
    info.colors.inputBorderFocus = scheme.palette.inputBorderFocus;
  if (scheme.palette.inputBorderError.isValid())
    info.colors.inputBorderError = scheme.palette.inputBorderError;
  if (scheme.palette.inputPlaceholder.isValid())
    info.colors.inputPlaceholder = scheme.palette.inputPlaceholder;

  if (scheme.palette.border.isValid()) info.colors.border = scheme.palette.border;
  if (scheme.palette.borderSubtle.isValid()) info.colors.borderSubtle = scheme.palette.borderSubtle;
  if (scheme.palette.borderStrong.isValid()) info.colors.borderStrong = scheme.palette.borderStrong;
  if (scheme.palette.separator.isValid()) info.colors.separator = scheme.palette.separator;
  if (scheme.palette.shadow.isValid()) info.colors.shadow = scheme.palette.shadow;

  if (scheme.palette.statusBackground.isValid())
    info.colors.statusBackground = scheme.palette.statusBackground;
  if (scheme.palette.statusBorder.isValid()) info.colors.statusBackgroundBorder = scheme.palette.statusBorder;
  if (scheme.palette.statusHover.isValid()) info.colors.statusBackgroundHover = scheme.palette.statusHover;

  if (scheme.palette.errorBackground.isValid()) info.colors.errorBackground = scheme.palette.errorBackground;
  if (scheme.palette.errorBorder.isValid()) info.colors.errorBorder = scheme.palette.errorBorder;
  if (scheme.palette.successBackground.isValid())
    info.colors.successBackground = scheme.palette.successBackground;
  if (scheme.palette.successBorder.isValid()) info.colors.successBorder = scheme.palette.successBorder;
  if (scheme.palette.warningBackground.isValid())
    info.colors.warningBackground = scheme.palette.warningBackground;
  if (scheme.palette.warningBorder.isValid()) info.colors.warningBorder = scheme.palette.warningBorder;

  if (scheme.palette.linkDefault.isValid()) info.colors.linkDefault = scheme.palette.linkDefault;
  if (scheme.palette.linkHover.isValid()) info.colors.linkHover = scheme.palette.linkHover;
  if (scheme.palette.linkVisited.isValid()) info.colors.linkVisited = scheme.palette.linkVisited;

  if (scheme.palette.focus.isValid()) info.colors.focus = scheme.palette.focus;
  if (scheme.palette.overlay.isValid()) info.colors.overlay = scheme.palette.overlay;
  if (scheme.palette.tooltip.isValid()) info.colors.tooltip = scheme.palette.tooltip;
  if (scheme.palette.tooltipText.isValid()) info.colors.tooltipText = scheme.palette.tooltipText;

  // Set mainBackground from palette if not already set
  if (!info.colors.mainBackground.isValid()) { info.colors.mainBackground = scheme.palette.background; }

  if (scheme.appearance == "dark") {
    // EXISTING COLORS (your current code) - only set if not provided in palette
    if (!info.colors.mainBackground.isValid()) info.colors.mainBackground = scheme.palette.background;
    if (!info.colors.border.isValid())
      info.colors.border = adjustColorHSL(info.colors.mainBackground, 0, 0.5f, 1.8f);
    if (!info.colors.mainSelectedBackground.isValid())
      info.colors.mainSelectedBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.1f, 1.4f);
    if (!info.colors.mainHoveredBackground.isValid())
      info.colors.mainHoveredBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.0f, 1.3f);
    if (!info.colors.statusBackground.isValid())
      info.colors.statusBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 1.3f);
    if (!info.colors.statusBackgroundLighter.isValid())
      info.colors.statusBackgroundLighter = adjustColorHSL(info.colors.statusBackground, 0, 0.9f, 1.2f);
    if (!info.colors.statusBackgroundHover.isValid())
      info.colors.statusBackgroundHover = adjustColorHSL(info.colors.statusBackground, 0, 1.0f, 1.1f);
    if (!info.colors.statusBackgroundBorder.isValid())
      info.colors.statusBackgroundBorder = adjustColorHSL(info.colors.statusBackground, 0, 0.6f, 0.6f);
    if (!info.colors.text.isValid()) info.colors.text = scheme.palette.foreground;
    if (!info.colors.subtext.isValid())
      info.colors.subtext = adjustColorHSL(scheme.palette.foreground, 0, 0.8f, 0.7f);

    // NEW TEXT COLORS - only generate if not provided
    if (!info.colors.textTertiary.isValid())
      info.colors.textTertiary = adjustColorHSL(scheme.palette.foreground, 0, 0.6f, 0.5f);
    if (!info.colors.textDisabled.isValid())
      info.colors.textDisabled = adjustColorHSL(scheme.palette.foreground, 0, 0.3f, 0.4f);
    if (!info.colors.textOnAccent.isValid()) info.colors.textOnAccent = QColor("#FFFFFF");

    // NEW BACKGROUND LEVELS - only generate if not provided
    if (!info.colors.secondaryBackground.isValid())
      info.colors.secondaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.9f, 1.2f);
    if (!info.colors.tertiaryBackground.isValid())
      info.colors.tertiaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 0.8f);

    // PRIMARY BUTTONS - only generate if not provided
    if (!info.colors.buttonPrimary.isValid()) info.colors.buttonPrimary = scheme.palette.blue;
    if (!info.colors.buttonPrimaryHover.isValid())
      info.colors.buttonPrimaryHover = adjustColorHSL(scheme.palette.blue, 0, 1.1f, 1.2f);
    if (!info.colors.buttonPrimaryPressed.isValid())
      info.colors.buttonPrimaryPressed = adjustColorHSL(scheme.palette.blue, 0, 1.2f, 0.8f);
    if (!info.colors.buttonPrimaryDisabled.isValid())
      info.colors.buttonPrimaryDisabled = adjustColorHSL(scheme.palette.blue, 0, 0.3f, 0.6f);

    // SECONDARY BUTTONS - only generate if not provided
    if (!info.colors.buttonSecondary.isValid())
      info.colors.buttonSecondary = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 1.6f);
    if (!info.colors.buttonSecondaryHover.isValid())
      info.colors.buttonSecondaryHover = adjustColorHSL(info.colors.buttonSecondary, 0, 1.0f, 1.2f);
    if (!info.colors.buttonSecondaryPressed.isValid())
      info.colors.buttonSecondaryPressed = adjustColorHSL(info.colors.buttonSecondary, 0, 1.1f, 0.9f);
    if (!info.colors.buttonSecondaryDisabled.isValid())
      info.colors.buttonSecondaryDisabled = adjustColorHSL(info.colors.buttonSecondary, 0, 0.5f, 0.7f);

    // DESTRUCTIVE BUTTONS - only generate if not provided
    if (!info.colors.buttonDestructive.isValid()) info.colors.buttonDestructive = scheme.palette.red;
    if (!info.colors.buttonDestructiveHover.isValid())
      info.colors.buttonDestructiveHover = adjustColorHSL(scheme.palette.red, 0, 1.1f, 1.2f);
    if (!info.colors.buttonDestructivePressed.isValid())
      info.colors.buttonDestructivePressed = adjustColorHSL(scheme.palette.red, 0, 1.2f, 0.8f);

    // INPUT STATES - only generate if not provided
    if (!info.colors.inputBackground.isValid())
      info.colors.inputBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.7f, 1.1f);
    if (!info.colors.inputBorder.isValid())
      info.colors.inputBorder = adjustColorHSL(info.colors.mainBackground, 0, 0.6f, 1.5f);
    if (!info.colors.inputBorderFocus.isValid()) info.colors.inputBorderFocus = scheme.palette.blue;
    if (!info.colors.inputBorderError.isValid()) info.colors.inputBorderError = scheme.palette.red;
    if (!info.colors.inputPlaceholder.isValid())
      info.colors.inputPlaceholder = adjustColorHSL(scheme.palette.foreground, 0, 0.5f, 0.6f);

    // BORDER VARIATIONS - only generate if not provided
    if (!info.colors.borderSubtle.isValid())
      info.colors.borderSubtle = adjustColorHSL(info.colors.border, 0, 0.7f, 0.8f);
    if (!info.colors.borderStrong.isValid())
      info.colors.borderStrong = adjustColorHSL(info.colors.border, 0, 1.3f, 1.3f);
    if (!info.colors.separator.isValid())
      info.colors.separator = adjustColorHSL(info.colors.border, 0, 0.5f, 1.0f);
    if (!info.colors.shadow.isValid()) info.colors.shadow = QColor(0, 0, 0, 80);

    // STATUS BACKGROUNDS - only generate if not provided
    if (!info.colors.errorBackground.isValid())
      info.colors.errorBackground = adjustColorHSL(scheme.palette.red, 0, 0.6f, 1.8f);
    if (!info.colors.errorBorder.isValid())
      info.colors.errorBorder = adjustColorHSL(scheme.palette.red, 0, 0.8f, 1.4f);
    if (!info.colors.successBackground.isValid())
      info.colors.successBackground = adjustColorHSL(scheme.palette.green, 0, 0.6f, 1.8f);
    if (!info.colors.successBorder.isValid())
      info.colors.successBorder = adjustColorHSL(scheme.palette.green, 0, 0.8f, 1.4f);
    if (!info.colors.warningBackground.isValid())
      info.colors.warningBackground = adjustColorHSL(scheme.palette.orange, 0, 0.6f, 1.8f);
    if (!info.colors.warningBorder.isValid())
      info.colors.warningBorder = adjustColorHSL(scheme.palette.orange, 0, 0.8f, 1.4f);

    // LINKS - only generate if not provided
    if (!info.colors.linkDefault.isValid())
      info.colors.linkDefault = adjustColorHSL(scheme.palette.blue, 0, 1.0f, 1.3f);
    if (!info.colors.linkHover.isValid())
      info.colors.linkHover = adjustColorHSL(scheme.palette.blue, 0, 1.2f, 1.5f);
    if (!info.colors.linkVisited.isValid())
      info.colors.linkVisited = adjustColorHSL(scheme.palette.purple, 0, 1.0f, 1.2f);

    // SPECIAL ELEMENTS - only generate if not provided
    if (!info.colors.focus.isValid()) info.colors.focus = scheme.palette.blue;
    if (!info.colors.overlay.isValid()) info.colors.overlay = QColor(0, 0, 0, 120);
    if (!info.colors.tooltip.isValid())
      info.colors.tooltip = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 2.0f);
    if (!info.colors.tooltipText.isValid()) info.colors.tooltipText = scheme.palette.foreground;
    // SEMANTIC TEXT COLORS - only generate if not provided
    if (!info.colors.textSuccess.isValid()) {
      info.colors.textSuccess = adjustColorHSL(scheme.palette.green, 0, 1.1f, 1.2f);
      // ^ Slightly brighter and more saturated green for success text
    }
    if (!info.colors.textWarning.isValid()) {
      info.colors.textWarning = adjustColorHSL(scheme.palette.orange, 0, 1.1f, 1.2f);
      // ^ Slightly brighter and more saturated orange for warning text
    }

  } else {
    // LIGHT THEME - Similar logic but inverted lightness relationships

    // EXISTING COLORS (your current code) - only set if not provided in palette
    if (!info.colors.mainBackground.isValid()) info.colors.mainBackground = scheme.palette.background;
    if (!info.colors.border.isValid())
      info.colors.border = adjustColorHSL(info.colors.mainBackground, 0, 0.6f, 0.75f);
    if (!info.colors.mainSelectedBackground.isValid())
      info.colors.mainSelectedBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.2f, 0.9f);
    if (!info.colors.mainHoveredBackground.isValid())
      info.colors.mainHoveredBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.1f, 0.95f);
    if (!info.colors.statusBackground.isValid())
      info.colors.statusBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.9f, 0.92f);
    if (!info.colors.statusBackgroundLighter.isValid())
      info.colors.statusBackgroundLighter = adjustColorHSL(info.colors.statusBackground, 0, 0.8f, 0.96f);
    if (!info.colors.statusBackgroundHover.isValid())
      info.colors.statusBackgroundHover = adjustColorHSL(info.colors.statusBackground, 0, 1.1f, 0.88f);
    if (!info.colors.statusBackgroundBorder.isValid())
      info.colors.statusBackgroundBorder = adjustColorHSL(info.colors.statusBackground, 0, 0.7f, 0.8f);
    if (!info.colors.text.isValid()) info.colors.text = scheme.palette.foreground;
    if (!info.colors.subtext.isValid())
      info.colors.subtext = adjustColorHSL(scheme.palette.foreground, 0, 0.7f, 1.4f);

    // NEW TEXT COLORS - only generate if not provided
    if (!info.colors.textTertiary.isValid())
      info.colors.textTertiary = adjustColorHSL(scheme.palette.foreground, 0, 0.6f, 1.6f);
    if (!info.colors.textDisabled.isValid())
      info.colors.textDisabled = adjustColorHSL(scheme.palette.foreground, 0, 0.4f, 1.8f);
    if (!info.colors.textOnAccent.isValid()) info.colors.textOnAccent = QColor("#FFFFFF");

    // NEW BACKGROUND LEVELS - only generate if not provided
    if (!info.colors.secondaryBackground.isValid())
      info.colors.secondaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 0.95f);
    if (!info.colors.tertiaryBackground.isValid())
      info.colors.tertiaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.9f, 1.05f);

    // PRIMARY BUTTONS - only generate if not provided
    if (!info.colors.buttonPrimary.isValid()) info.colors.buttonPrimary = scheme.palette.blue;
    if (!info.colors.buttonPrimaryHover.isValid())
      info.colors.buttonPrimaryHover = adjustColorHSL(scheme.palette.blue, 0, 1.1f, 0.9f);
    if (!info.colors.buttonPrimaryPressed.isValid())
      info.colors.buttonPrimaryPressed = adjustColorHSL(scheme.palette.blue, 0, 1.2f, 0.8f);
    if (!info.colors.buttonPrimaryDisabled.isValid())
      info.colors.buttonPrimaryDisabled = adjustColorHSL(scheme.palette.blue, 0, 0.3f, 1.5f);

    // SECONDARY BUTTONS - only generate if not provided
    if (!info.colors.buttonSecondary.isValid())
      info.colors.buttonSecondary = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 0.85f);
    if (!info.colors.buttonSecondaryHover.isValid())
      info.colors.buttonSecondaryHover = adjustColorHSL(info.colors.buttonSecondary, 0, 1.0f, 0.8f);
    if (!info.colors.buttonSecondaryPressed.isValid())
      info.colors.buttonSecondaryPressed = adjustColorHSL(info.colors.buttonSecondary, 0, 1.1f, 0.75f);
    if (!info.colors.buttonSecondaryDisabled.isValid())
      info.colors.buttonSecondaryDisabled = adjustColorHSL(info.colors.buttonSecondary, 0, 0.5f, 1.2f);

    // DESTRUCTIVE BUTTONS - only generate if not provided
    if (!info.colors.buttonDestructive.isValid()) info.colors.buttonDestructive = scheme.palette.red;
    if (!info.colors.buttonDestructiveHover.isValid())
      info.colors.buttonDestructiveHover = adjustColorHSL(scheme.palette.red, 0, 1.1f, 0.9f);
    if (!info.colors.buttonDestructivePressed.isValid())
      info.colors.buttonDestructivePressed = adjustColorHSL(scheme.palette.red, 0, 1.2f, 0.8f);

    // INPUT STATES - only generate if not provided
    if (!info.colors.inputBackground.isValid()) info.colors.inputBackground = QColor("#FFFFFF");
    if (!info.colors.inputBorder.isValid())
      info.colors.inputBorder = adjustColorHSL(info.colors.mainBackground, 0, 0.5f, 0.7f);
    if (!info.colors.inputBorderFocus.isValid()) info.colors.inputBorderFocus = scheme.palette.blue;
    if (!info.colors.inputBorderError.isValid()) info.colors.inputBorderError = scheme.palette.red;
    if (!info.colors.inputPlaceholder.isValid())
      info.colors.inputPlaceholder = adjustColorHSL(scheme.palette.foreground, 0, 0.5f, 1.5f);

    // BORDER VARIATIONS - only generate if not provided
    if (!info.colors.borderSubtle.isValid())
      info.colors.borderSubtle = adjustColorHSL(info.colors.border, 0, 0.7f, 1.2f);
    if (!info.colors.borderStrong.isValid())
      info.colors.borderStrong = adjustColorHSL(info.colors.border, 0, 1.2f, 0.6f);
    if (!info.colors.separator.isValid())
      info.colors.separator = adjustColorHSL(info.colors.border, 0, 0.6f, 1.0f);
    if (!info.colors.shadow.isValid()) info.colors.shadow = QColor(0, 0, 0, 40);

    // STATUS BACKGROUNDS - only generate if not provided
    if (!info.colors.errorBackground.isValid())
      info.colors.errorBackground = adjustColorHSL(scheme.palette.red, 0, 0.3f, 1.8f);
    if (!info.colors.errorBorder.isValid())
      info.colors.errorBorder = adjustColorHSL(scheme.palette.red, 0, 0.7f, 1.2f);
    if (!info.colors.successBackground.isValid())
      info.colors.successBackground = adjustColorHSL(scheme.palette.green, 0, 0.3f, 1.8f);
    if (!info.colors.successBorder.isValid())
      info.colors.successBorder = adjustColorHSL(scheme.palette.green, 0, 0.7f, 1.2f);
    if (!info.colors.warningBackground.isValid())
      info.colors.warningBackground = adjustColorHSL(scheme.palette.orange, 0, 0.3f, 1.8f);
    if (!info.colors.warningBorder.isValid())
      info.colors.warningBorder = adjustColorHSL(scheme.palette.orange, 0, 0.7f, 1.2f);

    // LINKS - only generate if not provided
    if (!info.colors.linkDefault.isValid())
      info.colors.linkDefault = adjustColorHSL(scheme.palette.blue, 0, 1.1f, 0.8f);
    if (!info.colors.linkHover.isValid())
      info.colors.linkHover = adjustColorHSL(scheme.palette.blue, 0, 1.3f, 0.7f);
    if (!info.colors.linkVisited.isValid())
      info.colors.linkVisited = adjustColorHSL(scheme.palette.purple, 0, 1.1f, 0.8f);

    // SPECIAL ELEMENTS - only generate if not provided
    if (!info.colors.focus.isValid()) info.colors.focus = scheme.palette.blue;
    if (!info.colors.overlay.isValid()) info.colors.overlay = QColor(0, 0, 0, 80);
    if (!info.colors.tooltip.isValid())
      info.colors.tooltip = adjustColorHSL(info.colors.mainBackground, 0, 0.7f, 0.2f);
    if (!info.colors.tooltipText.isValid()) info.colors.tooltipText = QColor("#FFFFFF");

    // SEMANTIC TEXT COLORS - only generate if not provided
    if (!info.colors.textSuccess.isValid()) {
      info.colors.textSuccess = adjustColorHSL(scheme.palette.green, 0, 1.1f, 0.8f);
      // ^ Slightly darker and more saturated green for success text in light theme
    }
    if (!info.colors.textWarning.isValid()) {
      info.colors.textWarning = adjustColorHSL(scheme.palette.orange, 0, 1.1f, 0.8f);
      // ^ Slightly darker and more saturated orange for warning text in light theme
    }
    if (info.colors.textSuccess.isValid()) {
      info.colors.textSuccess = adjustColorHSL(scheme.palette.green, 0, 1.1f, 0.8f);
      // ^ Slightly darker and more saturated green for success text in light theme
    }
    if (info.colors.textWarning.isValid()) {
      info.colors.textWarning = adjustColorHSL(scheme.palette.orange, 0, 1.1f, 0.8f);
      // ^ Slightly darker and more saturated orange for warning text in light theme
    }
  }

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

      // TODO: use default value for missing colors
      theme.palette.background = colors.value("background").toString();
      theme.palette.foreground = colors.value("foreground").toString();
      theme.palette.blue = colors.value("blue").toString();
      theme.palette.green = colors.value("green").toString();
      theme.palette.magenta = colors.value("magenta").toString();
      theme.palette.orange = colors.value("orange").toString();
      theme.palette.purple = colors.value("purple").toString();
      theme.palette.red = colors.value("red").toString();
      theme.palette.yellow = colors.value("yellow").toString();
      theme.palette.cyan = colors.value("cyan").toString();

      // Text colors
      theme.palette.textPrimary = colors.value("textPrimary").toString();
      theme.palette.textSecondary = colors.value("textSecondary").toString();
      theme.palette.textTertiary = colors.value("textTertiary").toString();
      theme.palette.textDisabled = colors.value("textDisabled").toString();
      theme.palette.textOnAccent = colors.value("textOnAccent").toString();
      theme.palette.textError = colors.value("textError").toString();
      theme.palette.textSuccess = colors.value("textSuccess").toString();
      theme.palette.textWarning = colors.value("textWarning").toString();

      // Background colors
      theme.palette.mainBackground = colors.value("mainBackground").toString();
      theme.palette.mainHoverBackground = colors.value("mainHoverBackground").toString();
      theme.palette.mainSelectedBackground = colors.value("mainSelectedBackground").toString();
      theme.palette.secondaryBackground = colors.value("secondaryBackground").toString();
      theme.palette.tertiaryBackground = colors.value("tertiaryBackground").toString();

      // Button colors
      theme.palette.buttonPrimary = colors.value("buttonPrimary").toString();
      theme.palette.buttonPrimaryHover = colors.value("buttonPrimaryHover").toString();
      theme.palette.buttonPrimaryPressed = colors.value("buttonPrimaryPressed").toString();
      theme.palette.buttonPrimaryDisabled = colors.value("buttonPrimaryDisabled").toString();

      theme.palette.buttonSecondary = colors.value("buttonSecondary").toString();
      theme.palette.buttonSecondaryHover = colors.value("buttonSecondaryHover").toString();
      theme.palette.buttonSecondaryPressed = colors.value("buttonSecondaryPressed").toString();
      theme.palette.buttonSecondaryDisabled = colors.value("buttonSecondaryDisabled").toString();

      theme.palette.buttonDestructive = colors.value("buttonDestructive").toString();
      theme.palette.buttonDestructiveHover = colors.value("buttonDestructiveHover").toString();
      theme.palette.buttonDestructivePressed = colors.value("buttonDestructivePressed").toString();

      // Input colors
      theme.palette.inputBackground = colors.value("inputBackground").toString();
      theme.palette.inputBorder = colors.value("inputBorder").toString();
      theme.palette.inputBorderFocus = colors.value("inputBorderFocus").toString();
      theme.palette.inputBorderError = colors.value("inputBorderError").toString();
      theme.palette.inputPlaceholder = colors.value("inputPlaceholder").toString();

      // UI element colors
      theme.palette.border = colors.value("border").toString();
      theme.palette.borderSubtle = colors.value("borderSubtle").toString();
      theme.palette.borderStrong = colors.value("borderStrong").toString();
      theme.palette.separator = colors.value("separator").toString();
      theme.palette.shadow = colors.value("shadow").toString();

      // Status colors
      theme.palette.statusBackground = colors.value("statusBackground").toString();
      theme.palette.statusBorder = colors.value("statusBorder").toString();
      theme.palette.statusHover = colors.value("statusHover").toString();

      theme.palette.errorBackground = colors.value("errorBackground").toString();
      theme.palette.errorBorder = colors.value("errorBorder").toString();
      theme.palette.successBackground = colors.value("successBackground").toString();
      theme.palette.successBorder = colors.value("successBorder").toString();
      theme.palette.warningBackground = colors.value("warningBackground").toString();
      theme.palette.warningBorder = colors.value("warningBorder").toString();

      // Interactive colors
      theme.palette.linkDefault = colors.value("linkDefault").toString();
      theme.palette.linkHover = colors.value("linkHover").toString();
      theme.palette.linkVisited = colors.value("linkVisited").toString();

      // Special colors
      theme.palette.focus = colors.value("focus").toString();
      theme.palette.overlay = colors.value("overlay").toString();
      theme.palette.tooltip = colors.value("tooltip").toString();
      theme.palette.tooltipText = colors.value("tooltipText").toString();

      upsertTheme(theme);

      // use default
    }
  }
}

std::vector<ParsedThemeData> ThemeService::loadColorSchemes() const {
  std::vector<ParsedThemeData> schemes;

  schemes.reserve(2);

  ParsedThemeData lightTheme;

  lightTheme.name = "Vicinae Light";
  lightTheme.description = "Default Vicinae light palette";
  lightTheme.id = "vicinae-light";
  lightTheme.appearance = "light";
  lightTheme.palette = ColorPalette{.background = "#F4F2EE",
                                    .foreground = "#1A1A1A",
                                    .blue = "#1F6FEB",
                                    .green = "#3A9C61",
                                    .magenta = "#A48ED6",
                                    .orange = "#DA8A48",
                                    .purple = "#8374B7",
                                    .red = "#C25C49",
                                    .yellow = "#BFAE78",
                                    .cyan = "#18A5B3"};

  schemes.emplace_back(lightTheme);

  ParsedThemeData darkTheme;

  darkTheme.name = "Vicinae Dark";
  darkTheme.description = "Default Vicinae dark palette";
  darkTheme.id = "vicinae-dark";
  darkTheme.appearance = "dark";
  darkTheme.palette = ColorPalette{.background = "#1A1A1A",
                                   .foreground = "#E8E6E1",
                                   .blue = "#2F6FED",
                                   .green = "#3A9C61",
                                   .magenta = "#BC8CFF",
                                   .orange = "#F0883E",
                                   .purple = "#7267B0",
                                   .red = "#B9543B",
                                   .yellow = "#BFAE78",
                                   .cyan = "#18A5B3"};
  schemes.emplace_back(darkTheme);

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

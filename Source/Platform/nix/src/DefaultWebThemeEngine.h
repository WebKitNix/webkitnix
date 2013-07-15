#ifndef DefaultWebThemeEngine_h
#define DefaultWebThemeEngine_h

#include "public/ThemeEngine.h"

namespace Nix {

class DefaultWebThemeEngine : public ThemeEngine {
public:
    // Text selection colors.
    virtual Color activeSelectionBackgroundColor() const;
    virtual Color activeSelectionForegroundColor() const;
    virtual Color inactiveSelectionBackgroundColor() const;
    virtual Color inactiveSelectionForegroundColor() const;

    // List box selection colors
    virtual Color activeListBoxSelectionBackgroundColor() const;
    virtual Color activeListBoxSelectionForegroundColor() const;
    virtual Color inactiveListBoxSelectionBackgroundColor() const;
    virtual Color inactiveListBoxSelectionForegroundColor() const;

    virtual Color activeTextSearchHighlightColor() const;
    virtual Color inactiveTextSearchHighlightColor() const;

    virtual Color focusRingColor() const;

    virtual Color tapHighlightColor() const;

    virtual void paintButton(Canvas*, State, const Rect&, const ButtonExtraParams&) const;
    virtual void paintTextField(Canvas*, State, const Rect&) const;
    virtual void paintTextArea(Canvas*, State, const Rect&) const;
    virtual Size getCheckboxSize() const;
    virtual void paintCheckbox(Canvas*, State, const Rect&, const ButtonExtraParams&) const;
    virtual Size getRadioSize() const;
    virtual void paintRadio(Canvas*, State, const Rect&, const ButtonExtraParams&) const;
    virtual void paintMenuList(Canvas *, State, const Rect &) const;
    virtual void getMenuListPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const;
    virtual Size getProgressBarSize() const;
    virtual void paintProgressBar(Canvas*, State, const Rect&, const ProgressBarExtraParams&) const;
    virtual double getAnimationRepeatIntervalForProgressBar() const;
    virtual double getAnimationDurationForProgressBar() const;
    virtual void paintInnerSpinButton(Canvas *, State, const Rect &, const InnerSpinButtonExtraParams&) const;
    virtual void getInnerSpinButtonPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const;
    virtual void paintMeter(Canvas*, State, const Rect&, const MeterExtraParams&) const;
    virtual void paintSliderTrack(Canvas*, State, const Rect&) const;
    virtual void paintSliderThumb(Canvas*, State, const Rect&) const;
};

}
#endif

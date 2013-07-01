#ifndef DefaultWebThemeEngine_h
#define DefaultWebThemeEngine_h

#include "public/WebThemeEngine.h"

namespace WebKit {

class DefaultWebThemeEngine : public WebThemeEngine {
public:
    // Text selection colors.
    virtual WebColor activeSelectionBackgroundColor() const;
    virtual WebColor activeSelectionForegroundColor() const;
    virtual WebColor inactiveSelectionBackgroundColor() const;
    virtual WebColor inactiveSelectionForegroundColor() const;

    // List box selection colors
    virtual WebColor activeListBoxSelectionBackgroundColor() const;
    virtual WebColor activeListBoxSelectionForegroundColor() const;
    virtual WebColor inactiveListBoxSelectionBackgroundColor() const;
    virtual WebColor inactiveListBoxSelectionForegroundColor() const;

    virtual WebColor activeTextSearchHighlightColor() const;
    virtual WebColor inactiveTextSearchHighlightColor() const;

    virtual WebColor focusRingColor() const;

    virtual WebColor tapHighlightColor() const;

    virtual void paintButton(WebCanvas*, State, const WebRect&, const ButtonExtraParams&) const;
    virtual void paintTextField(WebCanvas*, State, const WebRect&) const;
    virtual void paintTextArea(WebCanvas*, State, const WebRect&) const;
    virtual WebSize getCheckboxSize() const;
    virtual void paintCheckbox(WebCanvas*, State, const WebRect&, const ButtonExtraParams&) const;
    virtual WebSize getRadioSize() const;
    virtual void paintRadio(WebCanvas*, State, const WebRect&, const ButtonExtraParams&) const;
    virtual void paintMenuList(WebCanvas *, State, const WebRect &) const;
    virtual void getMenuListPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const;
    virtual WebSize getProgressBarSize() const;
    virtual void paintProgressBar(WebCanvas*, State, const WebRect&, const ProgressBarExtraParams&) const;
    virtual double getAnimationRepeatIntervalForProgressBar() const;
    virtual double getAnimationDurationForProgressBar() const;
    virtual void paintInnerSpinButton(WebCanvas *, State, const WebRect &, const InnerSpinButtonExtraParams&) const;
    virtual void getInnerSpinButtonPadding(int& paddingTop, int& paddingLeft, int& paddingBottom, int& paddingRight) const;
    virtual void paintMeter(WebCanvas*, State, const WebRect&, const MeterExtraParams&) const;
    virtual void paintSliderTrack(WebCanvas*, State, const WebRect&) const;
    virtual void paintSliderThumb(WebCanvas*, State, const WebRect&) const;
};

}
#endif

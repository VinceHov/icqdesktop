#include "stdafx.h"

#include "../../cache/themes/themes.h"
#include "../../controls/CommonStyle.h"
#include "../../controls/DpiAwareImage.h"
#include "../../controls/TextEditEx.h"
#include "../../fonts.h"
#include "../../theme_settings.h"

#include "../../utils/log/log.h"
#include "../../utils/Text2DocConverter.h"
#include "../../utils/utils.h"

#include "ChatEventInfo.h"
#include "MessageStyle.h"

#include "ChatEventItem.h"

namespace Ui
{

    namespace
    {
        qint32 getBubbleHorPadding();

        qint32 getIconLeftPadding();

        qint32 getTextHorPadding();

        qint32 getTextVertOffset();

        int32_t getWidgetFontSize();

        qint32 getWidgetMinHeight();

        qint32 getIconRightPadding();
    }

    ChatEventItem::~ChatEventItem()
    {
    }

    ChatEventItem::ChatEventItem(const HistoryControl::ChatEventInfoSptr& _eventInfo, const qint64 _id)
        : HistoryControlPageItem(nullptr)
        , TextWidget_(nullptr)
        , EventInfo_(_eventInfo)
        , height_(0)
        , id_(_id)
    {
        Icon_ = std::make_unique<DpiAwareImage>(EventInfo_->loadEventIcon(getWidgetMinHeight()));
    }

    ChatEventItem::ChatEventItem(QWidget* _parent, const HistoryControl::ChatEventInfoSptr& _eventInfo, const qint64 _id)
        : HistoryControlPageItem(_parent)
        , TextWidget_(nullptr)
        , EventInfo_(_eventInfo)
        , height_(0)
        , id_(_id)
    {
        assert(EventInfo_);

        TextWidget_ = new TextEditEx(
            this,
            Fonts::appFont(getWidgetFontSize()),
            CommonStyle::getColor(CommonStyle::Color::TEXT_SECONDARY),
            false,
            false
            );

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        TextWidget_->setFrameStyle(QFrame::NoFrame);
        TextWidget_->setContextMenuPolicy(Qt::NoContextMenu);
        TextWidget_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        TextWidget_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        TextWidget_->setOpenLinks(true);
        TextWidget_->setOpenExternalLinks(true);
        TextWidget_->setWordWrapMode(QTextOption::WordWrap);
        TextWidget_->setFocusPolicy(Qt::NoFocus);
        TextWidget_->setStyleSheet(qsl("background: transparent"));
        TextWidget_->document()->setDocumentMargin(0);

        const auto &message = EventInfo_->formatEventText();
        Logic::Text4Edit(message, *TextWidget_, Logic::Text2DocHtmlMode::Escape, true, true);

        TextWidget_->setAlignment(Qt::AlignHCenter);
        TextWidget_->show();

        Icon_ = std::make_unique<DpiAwareImage>(EventInfo_->loadEventIcon(getWidgetMinHeight()));

        updateTheme();
    }

    QString ChatEventItem::formatRecentsText() const
    {
        return EventInfo_->formatEventText();
    }

    void ChatEventItem::setLastStatus(LastStatus _lastStatus)
    {
        if (getLastStatus() != _lastStatus)
        {
            assert(_lastStatus == LastStatus::None);
            HistoryControlPageItem::setLastStatus(_lastStatus);
            updateGeometry();
            update();
        }
    }

    int32_t ChatEventItem::evaluateFullIconWidth()
    {
        if (Icon_->isNull())
        {
            return 0;
        }

        auto fullIconWidth = getIconLeftPadding();
        fullIconWidth += Icon_->width();
        fullIconWidth += getIconRightPadding();

        return fullIconWidth;
    }

    int32_t ChatEventItem::evaluateTextHeight(const int32_t _textWidth)
    {
        assert(_textWidth > 0);

        auto &document = *TextWidget_->document();

        if (TextWidget_->width() != _textWidth)
        {
            TextWidget_->setFixedWidth(_textWidth);
            document.setTextWidth(_textWidth);
        }

        return TextWidget_->getTextSize().height();
    }

    int32_t ChatEventItem::evaluateTextWidth(const int32_t _widgetWidth)
    {
        assert(_widgetWidth > 0);

        const auto maxBubbleWidth = (
            _widgetWidth -
            getBubbleHorPadding() -
            getBubbleHorPadding()
            );

        const auto maxBubbleContentWidth = (
            maxBubbleWidth -
            getTextHorPadding() -
            getTextHorPadding()
            );

        const auto maxTextWidth = (
            maxBubbleContentWidth -
            evaluateFullIconWidth()
            );

        auto &document = *TextWidget_->document();

        if (TextWidget_->width() != maxTextWidth)
        {
            TextWidget_->setFixedWidth(maxTextWidth);
            document.setTextWidth(maxTextWidth);
        }

        const auto idealWidth = document.idealWidth();

        const auto widthInfidelityFix = (
            QFontMetrics(document.defaultFont()).averageCharWidth() * 2
            );

        const auto fixedWidth = (idealWidth + widthInfidelityFix);

        return fixedWidth;
    }

    void ChatEventItem::updateTheme()
    {
        const auto theme = get_qt_theme_settings()->themeForContact(getAimid());
        const auto textColor = theme->chat_event_.text_color_;

        auto palette = TextWidget_->palette();

        const auto textColorChanged = (
            palette.color(QPalette::Text) != textColor
            );
        if (!textColorChanged)
        {
            return;
        }

        palette.setBrush(QPalette::Background, QBrush(Qt::transparent));
        palette.setColor(QPalette::Text, textColor);
        TextWidget_->setPalette(palette);

        TextWidget_->document()->setDefaultFont(
            Fonts::appFont(
                getWidgetFontSize(),
                Fonts::FontWeight::Medium));
    }

    void ChatEventItem::paintEvent(QPaintEvent*)
    {
        assert(!BubbleRect_.isEmpty());

        QPainter p(this);

        p.setRenderHint(QPainter::Antialiasing);
        p.setRenderHint(QPainter::SmoothPixmapTransform);
        p.setRenderHint(QPainter::TextAntialiasing);

        if (!Icon_->isNull())
        {
            const auto iconX = (BubbleRect_.left() + getIconLeftPadding());

            const auto iconY = (height() - Icon_->height()) / 2;

            Icon_->draw(p, iconX, iconY);
        }

        updateTheme();
    }

    QSize ChatEventItem::sizeHint() const
    {
        return QSize(0, height_);
    }

    void ChatEventItem::resizeEvent(QResizeEvent* _event)
    {
        HistoryControlPageItem::resizeEvent(_event);

        const auto &newSize = _event->size();
        const auto newWidth = newSize.width();

        // setup the text control and get it dimensions
        const auto textWidth = evaluateTextWidth(newWidth);
        const auto textHeight = evaluateTextHeight(textWidth);
        TextWidget_->setFixedSize(textWidth, textHeight);

        // evaluate bubble width

        auto bubbleWidth = 0;

        if (Icon_->isNull())
        {
            bubbleWidth += getTextHorPadding();
        }
        else
        {
            bubbleWidth += evaluateFullIconWidth();
        }

        bubbleWidth += textWidth + getTextHorPadding();

        // evaluate bubble height

        const auto bubbleHeight = textHeight + getTextVertOffset() + getTextVertOffset();

        BubbleRect_ = QRect(0, 0, bubbleWidth, bubbleHeight);
        BubbleRect_.moveCenter(QRect(0, 0, newWidth, bubbleHeight).center());

        const auto topPadding = MessageStyle::getTopMargin(hasTopMargin());
        BubbleRect_.moveTop(topPadding);

        // setup geometry

        height_ = bubbleHeight + topPadding;

        auto textWidgetLeft = BubbleRect_.left();

        if (Icon_->isNull())
        {
            textWidgetLeft += getTextHorPadding();
        }
        else
        {
            textWidgetLeft += evaluateFullIconWidth();
        }

        TextWidget_->move(
            textWidgetLeft,
            BubbleRect_.top() + getTextVertOffset()
            );
    }



    void ChatEventItem::clearSelection()
    {
        TextWidget_->clearSelection();
    }


    qint64 ChatEventItem::getId() const
    {
        return id_;
    }

    void ChatEventItem::setQuoteSelection()
    {
        /// TODO-quote
        assert(0);
    }

    namespace
    {
        qint32 getBubbleHorPadding()
        {
            return Utils::scale_value(12);
        }

        qint32 getTextHorPadding()
        {
            return Utils::scale_value(12);
        }

        qint32 getTextVertOffset()
        {
            return Utils::scale_value(6);
        }

        int32_t getWidgetFontSize()
        {
            return (Utils::scale_value(12));
        }

        qint32 getWidgetMinHeight()
        {
            return Utils::scale_value(22);
        }

        qint32 getIconRightPadding()
        {
            return Utils::scale_value(4);
        }

        qint32 getIconLeftPadding()
        {
            return Utils::scale_value(6);
        }

    }

};

//
//  VideoFrameMacos.h
//  ICQ
//
//  Created by IceSeer on 04/12/15.
//  Copyright © 2015 Mail.RU. All rights reserved.
//

#ifndef VideoFrameMacos_h
#define VideoFrameMacos_h

#include "../VideoFrame.h"

namespace platform_macos {
    
    void setPanelAttachedAsChild(bool attach, QWidget& parent, QWidget& child);
    void setAspectRatioForWindow(QWidget& wnd, float aspectRatio);
    bool windowIsOverlapped(QWidget* frame);
    void setWindowPosition(QWidget& widget, const QWidget& parent, const bool top);
    void moveAboveParentWindow(QWidget& parent, QWidget& child);
    
    // @return true, if iTunes was paused.
    bool pauseiTunes();
    void playiTunes();
    
    void fadeIn(QWidget* wnd);
    void fadeOut(QWidget* wnd);
    
    class GraphicsPanelMacos {
    public:
        static platform_specific::GraphicsPanel* create(QWidget* parent, std::vector<QWidget*>& panels);
    };
    
    
    // This class use Cocoa delegates and
    // notificate us about start/finish fullscreen animation.
    class FullScreenNotificaton : public QObject
    {
        Q_OBJECT
    public:
        FullScreenNotificaton (QWidget& parentWindow);
        virtual ~FullScreenNotificaton ();
        
    Q_SIGNALS:
        
        void fullscreenAnimationStart();
        void fullscreenAnimationFinish();
        void activeSpaceDidChange();
    protected:
        
        // Cocoa delegate
        void* _delegate;
        QWidget& _parentWindow;
    };
    
}

#endif /* VideoFrameMacos_h */

#pragma once

#include "../types/images.h"

namespace Previewer
{
    class ImageLoader
        : public QObject
    {
        Q_OBJECT
    public:
        ImageLoader(const QString& _aimId, const Data::Image& _image);
        ImageLoader(const QString& _aimId, const Data::Image& _image, const QString& _localPath);

        void load();
        void cancelLoading() const;

        enum class State
        {
            ReadyToLoad,
            Loading,
            Success,
            Error
        };

        State getState() const;

        QString getFileName();

        const QPixmap& getPixmap() const;
        const QString& getLocalFileName() const;

    signals:
        void imageLoaded();
        void imageLoadingError();

        void metainfoLoaded();

    public slots:
        void onCancelLoading();
        void onTryAgain();

    private slots:
        void onSharedFileDownloaded(qint64 _seq, QString _url, QString _local);
        void onSharedFileDownloadError(qint64 _seq, QString _url, qint32 _errorCode);

        void onImageDownloaded(qint64 _seq, QString _url, QPixmap _image, QString _local);
        void onImageDownloadError(qint64 _seq, QString _url);

    private:
        void start();
        void setState(State _newState);

    private:
        const QString aimId_;

        const Data::Image image_;

        qint64 seq_;
        State state_;

        QPixmap pixmap_;
        QString localFileName_;
    };
}

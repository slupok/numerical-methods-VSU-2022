#ifndef LOADERTHREAD_H
#define LOADERTHREAD_H

#include "loader.h"

#include <QThread>

class LoaderThread : public QThread {
  Q_OBJECT
public:
  LoaderThread() = default;
  LoaderThread(const QString &path);

  void load();

  const QVector<QPointF> &getVertices() const;

  const QString &getPath() const;
  void setPath(const QString &path);

signals:
  void loading();
  void loaded();
  void failed();

protected:
  void run() override;

private:
  Loader mLoader;
};

#endif // LOADERTHREAD_H

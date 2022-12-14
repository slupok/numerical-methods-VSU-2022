#include "loaderthread.h"

LoaderThread::LoaderThread(const QString &path) : mLoader(path) {}

void LoaderThread::load() { this->run(); }

const QVector<QPointF> &LoaderThread::getVertices() const {
  return mLoader.getVertices();
}

const QString &LoaderThread::getPath() const { return mLoader.getPath(); }

void LoaderThread::setPath(const QString &path) { mLoader.setPath(path); }

void LoaderThread::run() {
  emit loading();

  if (mLoader.load()) {
    emit loaded();
  } else {
    emit failed();
  }
}

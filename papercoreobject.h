#ifndef PAPERCOREOBJECT_H
#define PAPERCOREOBJECT_H

#include <QObject>
#include <QGeoCoordinate>
#include <QStringList>
#include <QDateTime>

class PaperCoreObjectPrivate;
class PaperCoreObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int paperId READ paperId WRITE setPaper NOTIFY paperIdChanged)
    Q_PROPERTY(QString uuid READ uuid NOTIFY uuidChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QDateTime create READ create WRITE setCreate NOTIFY createChanged)
    Q_PROPERTY(QDateTime modified READ modified WRITE setModified NOTIFY modifiedChanged)
    Q_PROPERTY(QGeoCoordinate location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(int group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(int activity READ activity WRITE setActivity NOTIFY activityChanged)
    Q_PROPERTY(QString weather READ weather WRITE setWeather NOTIFY weatherChanged)
    Q_PROPERTY(int temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(QStringList files READ files NOTIFY filesChanged)
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)

public:
    PaperCoreObject(QObject *parent = 0);
    ~PaperCoreObject();

    int paperId() const;
    void setPaper(int id);

    QString uuid() const;

    void setTitle(const QString &title);
    QString title() const;

    void setText(const QString &txt);
    QString text() const;

    void setCreate(const QDateTime &create);
    QDateTime create() const;

    void setModified(const QDateTime &modified);
    QDateTime modified() const;

    void setLocation(const QGeoCoordinate &location);
    QGeoCoordinate location() const;

    void setGroup(int group);
    int group() const;

    void setActivity(int activity);
    int activity() const;

    void setWeather(const QString &weather);
    QString weather() const;

    void setTemperature(int temperature);
    int temperature() const;

    QStringList files() const;

    void setType(int type);
    int type() const;

public slots:
    void addFile(const QString &file);
    void removeFile(const QString &file);
    void replaceFile(const QString &oldFile, const QString &newFile);

    void reset();
    void refresh();

    void save(bool force = true);

signals:
    void paperIdChanged();
    void textChanged();
    void uuidChanged();
    void titleChanged();
    void createChanged();
    void modifiedChanged();
    void locationChanged();
    void groupChanged();
    void activityChanged();
    void weatherChanged();
    void temperatureChanged();
    void filesChanged();
    void typeChanged();

private slots:
    void paperUuidChanged(int id);
    void paperTitleChanged(int id);
    void paperTextChanged(int id);
    void paperCreateChanged(int id);
    void paperModifiedChanged(int id);
    void paperLocationChanged(int id);
    void paperGroupChanged(int id);
    void paperActivityChanged(int id);
    void paperWeatherChanged(int id);
    void paperTemperatureChanged(int id);
    void paperFilesChanged(int id);
    void paperTypeChanged(int id);

private:
    void reset_prv();
    void emit_all();

private:
    PaperCoreObjectPrivate *p;
};

#endif // PAPERCOREOBJECT_H

#define SAVE_CHANGES save(false);

#include "papercoreobject.h"
#include "database.h"
#include "papyrus.h"

#include <QTimer>
#include <QDebug>

class PaperCoreObjectPrivate
{
public:
    int paperId;
    Database *db;

    PaperClass data;
    QStringList files;

    QTimer *saveTimer;
};

PaperCoreObject::PaperCoreObject(QObject *parent) :
    QObject(parent)
{
    p = new PaperCoreObjectPrivate;
    p->paperId = -1;
    p->db = Papyrus::database();

    p->saveTimer = new QTimer(this);
    p->saveTimer->setInterval(1000);
    p->saveTimer->setSingleShot(true);

    connect(p->saveTimer, SIGNAL(timeout()), SLOT(save()));

    connect( p->db, SIGNAL(paperUuidChanged(int))        ,SLOT(paperUuidChanged(int))        );
    connect( p->db, SIGNAL(paperTitleChanged(int))       ,SLOT(paperTitleChanged(int))       );
    connect( p->db, SIGNAL(paperTextChanged(int))        ,SLOT(paperTextChanged(int))        );
    connect( p->db, SIGNAL(paperCreateChanged(int))      ,SLOT(paperCreateChanged(int))      );
    connect( p->db, SIGNAL(paperModifiedChanged(int))    ,SLOT(paperModifiedChanged(int))    );
    connect( p->db, SIGNAL(paperLocationChanged(int))    ,SLOT(paperLocationChanged(int))    );
    connect( p->db, SIGNAL(paperGroupChanged(int))       ,SLOT(paperGroupChanged(int))       );
    connect( p->db, SIGNAL(paperActivityChanged(int))    ,SLOT(paperActivityChanged(int))    );
    connect( p->db, SIGNAL(paperWeatherChanged(int))     ,SLOT(paperWeatherChanged(int))     );
    connect( p->db, SIGNAL(paperTemperatureChanged(int)) ,SLOT(paperTemperatureChanged(int)) );
    connect( p->db, SIGNAL(paperFilesChanged(int))       ,SLOT(paperFilesChanged(int))       );
    connect( p->db, SIGNAL(paperTypeChanged(int))        ,SLOT(paperTypeChanged(int))        );
}

int PaperCoreObject::paperId() const
{
    return p->paperId;
}

QString PaperCoreObject::uuid() const
{
    return p->data.uuid;
}

void PaperCoreObject::setTitle(const QString &title)
{
    if(p->data.title == title)
        return;

    p->data.title = title;
    emit titleChanged();

    SAVE_CHANGES;
}

QString PaperCoreObject::title() const
{
    return p->data.title;
}

void PaperCoreObject::setText(const QString &txt)
{
    if(p->data.text == txt)
        return;

    p->data.text = txt;
    emit textChanged();

    SAVE_CHANGES;
}

QString PaperCoreObject::text() const
{
    return p->data.text;
}

void PaperCoreObject::setCreate(const QDateTime &create)
{
    if(p->data.create == create)
        return;

    p->data.create = create;
    emit createChanged();

    SAVE_CHANGES;
}

QDateTime PaperCoreObject::create() const
{
    return p->data.create;
}

void PaperCoreObject::setModified(const QDateTime &modified)
{
    if(p->data.modified == modified)
        return;

    p->data.modified = modified;
    emit modifiedChanged();

    SAVE_CHANGES;
}

QDateTime PaperCoreObject::modified() const
{
    return p->data.modified;
}

void PaperCoreObject::setLocation(const QGeoCoordinate &location)
{
    if(p->data.location == location)
        return;

    p->data.location = location;
    emit locationChanged();

    SAVE_CHANGES;
}

QGeoCoordinate PaperCoreObject::location() const
{
    return p->data.location;
}

void PaperCoreObject::setGroup(int group)
{
    if(p->data.group == group)
        return;

    p->data.group = group;
    emit groupChanged();

    SAVE_CHANGES;
}

int PaperCoreObject::group() const
{
    return p->data.group;
}

void PaperCoreObject::setActivity(int activity)
{
    if(p->data.activity == activity)
        return;

    p->data.activity = activity;
    emit activityChanged();

    SAVE_CHANGES;
}

int PaperCoreObject::activity() const
{
    return p->data.activity;
}

void PaperCoreObject::setWeather(const QString &weather)
{
    if(p->data.weather == weather)
        return;

    p->data.weather = weather;
    emit weatherChanged();

    SAVE_CHANGES;
}

QString PaperCoreObject::weather() const
{
    return p->data.weather;
}

void PaperCoreObject::setTemperature(int temperature)
{
    if(p->data.temperature == temperature)
        return;

    p->data.temperature = temperature;
    emit temperatureChanged();

    SAVE_CHANGES;
}

int PaperCoreObject::temperature() const
{
    return p->data.temperature;
}

QStringList PaperCoreObject::files() const
{
    return p->files;
}

void PaperCoreObject::setType(int type)
{
    if(p->data.type == type)
        return;

    p->data.type = static_cast<Enums::PaperType>(type);
    emit typeChanged();

    SAVE_CHANGES;
}

int PaperCoreObject::type() const
{
    return p->data.type;
}

void PaperCoreObject::addFile(const QString &file)
{
    if(p->files.contains(file))
        return;

    p->files << file;
    save();
    p->db->addFileToPaper(p->paperId, file);
    save();
}

void PaperCoreObject::removeFile(const QString &file)
{
    if(!p->files.contains(file))
        return;

    p->db->removeFileFromPaper(p->paperId, file);
}

void PaperCoreObject::replaceFile(const QString &oldFile, const QString &newFile)
{
    if(p->files.contains(newFile))
        return;
    if(!p->files.contains(oldFile))
        return;

    const qint64 date = p->db->fileTime(p->paperId,oldFile);
    p->db->removeFileFromPaper(p->paperId,oldFile);
    p->db->addCustomFileToPaper(p->paperId,newFile,date);
}

void PaperCoreObject::setPaper(int id)
{
    if(p->paperId == id)
        return;

    p->paperId = id;

    emit paperIdChanged();
    refresh();
}

void PaperCoreObject::reset()
{
    reset_prv();
    emit_all();
}

void PaperCoreObject::refresh()
{
    const int paperId = p->paperId;

    reset_prv();

    p->paperId = paperId;
    p->data = p->db->getPaper(p->paperId);
    p->files = p->db->paperFiles(p->paperId);

    emit textChanged();
    emit uuidChanged();
    emit titleChanged();
    emit createChanged();
    emit modifiedChanged();
    emit locationChanged();
    emit groupChanged();
    emit activityChanged();
    emit weatherChanged();
    emit temperatureChanged();
    emit filesChanged();
    emit typeChanged();
}

void PaperCoreObject::save(bool force)
{
    p->saveTimer->stop();

    if(force)
    {
        if(p->paperId == -1)
        {
            if(p->data.title.isEmpty() && p->data.text.isEmpty() && p->files.isEmpty() )
                return;

            p->paperId = p->db->createPaper();
            const PaperClass &pcls = p->db->getPaper(p->paperId);
            p->data.id = p->paperId;
            p->data.create = pcls.create;
            p->data.uuid = pcls.uuid;

            emit paperIdChanged();
        }

        p->db->savePaper(p->data);
    }
    else
        p->saveTimer->start();
}

void PaperCoreObject::reset_prv()
{
    p->paperId = -1;
    p->data = PaperClass();
    p->files.clear();
}

void PaperCoreObject::emit_all()
{
    emit paperIdChanged();
    emit textChanged();
    emit uuidChanged();
    emit titleChanged();
    emit createChanged();
    emit modifiedChanged();
    emit locationChanged();
    emit groupChanged();
    emit activityChanged();
    emit weatherChanged();
    emit temperatureChanged();
    emit filesChanged();
    emit typeChanged();
}

void PaperCoreObject::paperUuidChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.uuid = p->db->paperUuid(id);
    emit uuidChanged();
}

void PaperCoreObject::paperTitleChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.title = p->db->paperTitle(id);
    emit titleChanged();
}

void PaperCoreObject::paperTextChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.text = p->db->paperText(id);
    emit textChanged();
}

void PaperCoreObject::paperCreateChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.create = p->db->paperCreatedDate(id);
    emit createChanged();
}

void PaperCoreObject::paperModifiedChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.modified = p->db->paperModifiedDate(id);
    emit modifiedChanged();
}

void PaperCoreObject::paperLocationChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.location = p->db->paperLocation(id);
    emit locationChanged();
}

void PaperCoreObject::paperGroupChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.group = p->db->paperGroup(id);
    emit groupChanged();
}

void PaperCoreObject::paperActivityChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.activity = p->db->paperActivity(id);
    emit activityChanged();
}

void PaperCoreObject::paperWeatherChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.weather = p->db->paperWeather(id);
    emit weatherChanged();
}

void PaperCoreObject::paperTemperatureChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.temperature = p->db->paperTemperature(id);
    emit temperatureChanged();
}

void PaperCoreObject::paperFilesChanged(int id)
{
    if(p->paperId != id)
        return;

    p->files = p->db->paperFiles(id);
    emit filesChanged();
}

void PaperCoreObject::paperTypeChanged(int id)
{
    if(p->paperId != id)
        return;

    p->data.type = static_cast<Enums::PaperType>(p->db->paperType(id));
    emit typeChanged();
}

PaperCoreObject::~PaperCoreObject()
{
    delete p;
}


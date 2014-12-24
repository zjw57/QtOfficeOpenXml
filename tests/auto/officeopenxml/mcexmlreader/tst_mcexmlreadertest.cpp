#include <QString>
#include <QtTest>
#include <QDebug>
#include <QCoreApplication>
#include "mcexmlstreamreader.h"

using namespace QtOfficeOpenXml;

class MceXmlReaderTest : public QObject
{
    Q_OBJECT

public:
    MceXmlReaderTest();

private Q_SLOTS:
    void testProcessingIgnorableAttribute();
};

MceXmlReaderTest::MceXmlReaderTest()
{
}

void MceXmlReaderTest::testProcessingIgnorableAttribute()
{
    QFile file(SRCDIR"data/processing_ignorable_attribute.xml");
    const QString v1("http://schemas.openxmlformats.org/Circles/v1");
    const QString v2("http://schemas.openxmlformats.org/Circles/v2");
    const QString v3("http://schemas.openxmlformats.org/Circles/v3");

    //error will occur if the consumer doesn't support v1.
    {
        file.open(QFile::ReadOnly);
        Mce::XmlStreamReader reader(&file);
        //reader.addMceCurrentNamespace(v1);
        reader.readNextStartElement(); //Circles start
        QVERIFY(reader.tokenType() == QXmlStreamReader::Invalid);
        QVERIFY(reader.error() == QXmlStreamReader::CustomError);
        file.close();
    }

    //Both v2 and v3 attributes will be ignored when consumer only support v1.
    {
        file.open(QFile::ReadOnly);
        Mce::XmlStreamReader reader(&file);
        reader.addMceCurrentNamespace(v1);
        reader.readNextStartElement(); //Circles start

        reader.readNextStartElement(); //first Circle start
        QXmlStreamAttributes attributes = reader.attributes();
        QCOMPARE(attributes.value("Center").toString(), QString("0,0"));
        QVERIFY(!attributes.hasAttribute(v2, "Opacity"));
        QVERIFY(!attributes.hasAttribute(v3, "Luminance"));
        reader.readNextStartElement(); //first Circle end

        reader.readNextStartElement(); //second Circle start
        attributes = reader.attributes();
        QCOMPARE(attributes.value("Center").toString(), QString("25,0"));
        QVERIFY(!attributes.hasAttribute(v2, "Opacity"));
        QVERIFY(!attributes.hasAttribute(v3, "Luminance"));
        reader.readNextStartElement(); //second Circle end
        file.close();
    }

    //only v3 attributes will be ignored when consumer support v1 and v2
    {
        file.open(QFile::ReadOnly);
        Mce::XmlStreamReader reader(&file);
        reader.addMceCurrentNamespace(v1);
        reader.addMceCurrentNamespace(v2);
        reader.readNextStartElement(); //Circles start

        reader.readNextStartElement(); //first Circle start
        reader.readNextStartElement(); //first Circle end

        reader.readNextStartElement(); //second Circle start
        QXmlStreamAttributes attributes = reader.attributes();
        attributes = reader.attributes();
        QCOMPARE(attributes.value("Center").toString(), QString("25,0"));
        QCOMPARE(attributes.value(v2, "Opacity").toString(), QString("0.5"));
        QVERIFY(!attributes.hasAttribute(v3, "Luminance"));
        reader.readNextStartElement(); //second Circle end
        file.close();
    }

    //no attribute will be ignored when consumer support v1, v2 and v3
    {
        file.open(QFile::ReadOnly);
        Mce::XmlStreamReader reader(&file);
        reader.setMceCurrentNamespaces(QSet<QString>()<<v1<<v2<<v3);
        reader.readNextStartElement(); //Circles start

        reader.readNextStartElement(); //first Circle start
        reader.readNextStartElement(); //first Circle end

        reader.readNextStartElement(); //second Circle start
        QXmlStreamAttributes attributes = reader.attributes();
        attributes = reader.attributes();
        QCOMPARE(attributes.value("Center").toString(), QString("25,0"));
        QCOMPARE(attributes.value(v2, "Opacity").toString(), QString("0.5"));
        QCOMPARE(attributes.value(v3, "Luminance").toString(), QString("13"));
        reader.readNextStartElement(); //second Circle end
        file.close();
    }
}

QTEST_MAIN(MceXmlReaderTest)

#include "tst_mcexmlreadertest.moc"

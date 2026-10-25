#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QNetworkInformation>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include "DriftController.h"

// Write debug messages to a log file since -mwindows suppresses console output.
static QFile s_logFile;

static void logMessage(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    Q_UNUSED(type)
    Q_UNUSED(ctx)
    if (s_logFile.isOpen()) {
        QTextStream ts(&s_logFile);
        ts << QDateTime::currentDateTime().toString(Qt::ISODateWithMs)
           << " [" << type << "] " << msg << "\n";
        ts.flush();
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName(QStringLiteral("Drift"));
    app.setApplicationName(QStringLiteral("Drift"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));
    // Complements the exe resource icon (taskbar/title bar icon)
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/drift.ico")));

    // Setup log file in the app data directory
    const QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    s_logFile.setFileName(logDir + QStringLiteral("/drift.log"));
    if (s_logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qInstallMessageHandler(logMessage);
    }

    qDebug() << "Drift starting...";
    qDebug() << "Qt version:" << qVersion();
    qDebug() << "Log directory:" << logDir;
    qDebug() << "AppDataLocation:" << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Initialize network information for offline detection
    QNetworkInformation::loadDefaultBackend();
    qDebug() << "Network backend loaded";

    // Create the controller facade
    auto *controller = new drift::DriftController(&app);
    controller->initialize();
    qDebug() << "Controller initialized";

    // QML engine
    QQmlApplicationEngine engine;
    qDebug() << "QML engine created";

    // Connect to QML loading errors
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() {
            qCritical() << "QML object creation failed!";
            QCoreApplication::exit(-1);
        });

    // Expose the controller as initial properties
    QVariantMap props;
    props[QStringLiteral("controller")] = QVariant::fromValue(controller);
    engine.setInitialProperties(props);
    qDebug() << "Initial properties set:" << props.keys();

    // Load the main QML file
    const QUrl qmlUrl(QStringLiteral("qrc:/qt/qml/Drift/qml/Main.qml"));
    qDebug() << "Loading QML from:" << qmlUrl.toString();

    engine.load(qmlUrl);

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "No root objects created! QML loading failed.";
        qCritical() << "Ensure the QML module is correctly deployed.";
        return -1;
    }

    qDebug() << "QML loaded successfully, entering event loop."
             << "Root objects:" << engine.rootObjects().size();

    return app.exec();
}

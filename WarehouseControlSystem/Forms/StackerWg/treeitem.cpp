/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "treeitem.h"
#include "opcuamodel.h"
#include <QOpcUaClient>
#include <QMetaEnum>
#include <QPixmap>

QT_BEGIN_NAMESPACE

const int numberOfDisplayColumns = 7; // NodeId, Value, NodeClass, DataType, BrowseName, DisplayName, Description

TreeItem::TreeItem(OpcUaModel *model) : QObject(nullptr)
  , mModel(model)
{
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, TreeItem *parent) : QObject(parent)
  , mOpcNode(node)
  , mModel(model)
  , mParentItem(parent)
{
    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::browseFinished, this, &TreeItem::browseFinished);

    if (!mOpcNode->readAttributes( QOpcUa::NodeAttribute::Value
                            | QOpcUa::NodeAttribute::NodeClass
                            | QOpcUa::NodeAttribute::Description
                            | QOpcUa::NodeAttribute::DataType
                            | QOpcUa::NodeAttribute::BrowseName
                            | QOpcUa::NodeAttribute::DisplayName
                            ))
    {
        qWarning() << "Reading attributes" << mOpcNode->nodeId() << "failed";
    }

}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent) : TreeItem(node, model, parent)
{
    mNodeBrowseName = browsingData.browseName().name();
    mNodeClass = browsingData.nodeClass();
    mNodeId = browsingData.targetNodeId().nodeId();
    mNodeDisplayName = browsingData.displayName().text();
   // qDebug()<<"treeitem:"<<mNodeBrowseName<<mNodeClass<<mNodeId<<mNodeDisplayName <<;
}

TreeItem::~TreeItem()
{
    qDeleteAll(mChildItems);
}

TreeItem *TreeItem::child(int row)
{
    if (row >= mChildItems.size())
        qCritical() << "TreeItem in row" << row << "does not exist.";
    return mChildItems[row];
}

int TreeItem::childIndex(const TreeItem *child) const
{
    return mChildItems.indexOf(const_cast<TreeItem *>(child));
}

int TreeItem::childCount()
{
    startBrowsing();
    return mChildItems.size();
}

int TreeItem::columnCount() const
{
    return numberOfDisplayColumns;
}

QVariant TreeItem::data(int column)
{
    if (column == 0) {
        return mNodeBrowseName;
    } else if (column == 1) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const auto type = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        const auto value = mOpcNode->attribute(QOpcUa::NodeAttribute::Value);
        return variantToString(value, type);
    } else if (column == 2) {
        QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
        QString name = metaEnum.valueToKey((uint)mNodeClass);
        return name + " (" + QString::number((uint)mNodeClass) + ")";
    } else if (column == 3) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const QString typeId = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        auto enumEntry = QOpcUa::namespace0IdFromNodeId(typeId);
        QString name;
        if (enumEntry == QOpcUa::NodeIds::Namespace0::Unknown)
            return typeId;
        return QOpcUa::namespace0IdName(enumEntry) + " (" + typeId + ")";
    } else if (column == 4) {
        return mNodeId;
    } else if (column == 5) {
        return mNodeDisplayName;
    } else if (column == 6) {
        if (!mAttributesReady)
            return tr("Loading ...");

        return mOpcNode->attribute(QOpcUa::NodeAttribute::Description).value<QOpcUa::QLocalizedText>().text();
    }
    return QVariant();
}

int TreeItem::row() const
{
    if (!mParentItem)
        return 0;
    return mParentItem->childIndex(this);
}

TreeItem *TreeItem::parentItem()
{
    return mParentItem;
}

void TreeItem::appendChild(TreeItem *child)
{
    if (!child)
        return;

    if (!hasChildNodeItem(child->mNodeId)) {
        mChildItems.append(child);
        mChildNodeIds.insert(child->mNodeId);
    } else {
        child->deleteLater();
    }
}

QPixmap TreeItem::icon(int column) const
{
    if (column != 0 || !mOpcNode)
        return QPixmap();

    QColor c;

    switch (mNodeClass) {
    case QOpcUa::NodeClass::Object:
        c = Qt::darkGreen;
        break;
    case QOpcUa::NodeClass::Variable:
        c = Qt::darkBlue;
        break;
    case QOpcUa::NodeClass::Method:
        c = Qt::darkRed;
        break;
    default:
        c = Qt::gray;
    }

    QPixmap p(10,10);
    p.fill(c);
    return p;
}

bool TreeItem::hasChildNodeItem(const QString &nodeId) const
{
    return mChildNodeIds.contains(nodeId);
}

void TreeItem::startBrowsing()
{
    if (mBrowseStarted)
        return;

    if (!mOpcNode->browseChildren())
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "failed";
    else
        mBrowseStarted = true;
}

void TreeItem::handleAttributes(QOpcUa::NodeAttributes attr)
{
    if (attr & QOpcUa::NodeAttribute::NodeClass)
        mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
    if (attr & QOpcUa::NodeAttribute::BrowseName)
        mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUa::QQualifiedName>().name();
    if (attr & QOpcUa::NodeAttribute::DisplayName)
        mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUa::QLocalizedText>().text();

    mAttributesReady = true;
   // qDebug()<<"mNodeClass:"<<mNodeClass<<"mNodeBrowseName:"<<mNodeBrowseName<<"mNodeDisplayName:"<<mNodeDisplayName;
    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

void TreeItem::browseFinished(QVector<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "finally failed:" << statusCode;
        return;
    }

    auto index = mModel->createIndex(row(), 0, this);

    for (const auto &item : children) {
        if (hasChildNodeItem(item.targetNodeId().nodeId()))
            continue;

        auto node = mModel->opcUaClient()->node(item.targetNodeId());
        if (!node) {
            qWarning() << "Failed to instantiate node:" << item.targetNodeId().nodeId();
            continue;
        }

        mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size() + 1);
        appendChild(new TreeItem(node, mModel, item, this));
        mModel->endInsertRows();
    }

    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

QString TreeItem::variantToString(const QVariant &value, const QString &typeNodeId) const
{
    if (value.type() == QVariant::List) {
        const auto list = value.toList();
        QStringList concat;

        for (auto it : list)
            concat.append(variantToString(it, typeNodeId));

        return concat.join("\n");
    }

    if (typeNodeId == QLatin1String("ns=0;i=19")) { // StatusCode
        const char *name = QMetaEnum::fromType<QOpcUa::UaStatusCode>().valueToKey(value.toInt());
        if (!name)
            return QLatin1String("Unknown StatusCode");
        else
            return QString(name);
    }
    if (typeNodeId == QLatin1String("ns=0;i=2")) // Char
        return QString::number(value.toInt());
    else if (typeNodeId == QLatin1String("ns=0;i=3")) // SChar
        return QString::number(value.toUInt());
    else if (typeNodeId == QLatin1String("ns=0;i=4")) // Int16
        return QString::number(value.toInt());
    else if (typeNodeId == QLatin1String("ns=0;i=5")) // UInt16
        return QString::number(value.toUInt());
    else if (value.type() == QVariant::ByteArray)
        return QLatin1String("0x") + value.toByteArray().toHex();
    else if (value.type() == QVariant::DateTime)
        return value.toDateTime().toString(Qt::ISODate);
    else if (value.canConvert<QOpcUa::QQualifiedName>()) {
        const auto name = value.value<QOpcUa::QQualifiedName>();
        return QStringLiteral("[NamespaceIndex: %1, Name: \"%2\"]").arg(name.namespaceIndex()).arg(name.name());
    } else if (value.canConvert<QOpcUa::QLocalizedText>()) {
        const auto text = value.value<QOpcUa::QLocalizedText>();
        return localizedTextToString(text);
    } else if (value.canConvert<QOpcUa::QRange>()) {
        const auto range = value.value<QOpcUa::QRange>();
        return rangeToString(range);
    } else if (value.canConvert<QOpcUa::QComplexNumber>()) {
        const auto complex = value.value<QOpcUa::QComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    } else if (value.canConvert<QOpcUa::QDoubleComplexNumber>()) {
        const auto complex = value.value<QOpcUa::QDoubleComplexNumber>();
        return QStringLiteral("[Real: %1, Imaginary: %2]").arg(complex.real()).arg(complex.imaginary());
    } else if (value.canConvert<QOpcUa::QXValue>()) {
        const auto xv = value.value<QOpcUa::QXValue>();
        return QStringLiteral("[X: %1, Value: %2]").arg(xv.x()).arg(xv.value());
    } else if (value.canConvert<QOpcUa::QEUInformation>()) {
        const auto info = value.value<QOpcUa::QEUInformation>();
        return euInformationToString(info);
    } else if (value.canConvert<QOpcUa::QAxisInformation>()) {
        const auto info = value.value<QOpcUa::QAxisInformation>();
        return QStringLiteral("[EUInformation: %1, EURange: %2, Title: %3 , AxisScaleType: %4, AxisSteps: %5]").arg(
                    euInformationToString(info.engineeringUnits())).arg(rangeToString(info.eURange())).arg(localizedTextToString(info.title())).arg(
                        info.axisScaleType() == QOpcUa::AxisScale::Linear ? "Linear" : (info.axisScaleType() == QOpcUa::AxisScale::Ln) ? "Ln" : "Log").arg(
                        numberArrayToString(info.axisSteps()));
    } else if (value.canConvert<QOpcUa::QExpandedNodeId>()) {
        const auto id = value.value<QOpcUa::QExpandedNodeId>();
        return QStringLiteral("[NodeId: \"%1\", ServerIndex: \"%2\", NamespaceUri: \"%3\"]").arg(
                    id.nodeId()).arg(id.serverIndex()).arg(id.namespaceUri());
    } else if (value.canConvert<QOpcUa::QArgument>()) {
        const auto a = value.value<QOpcUa::QArgument>();

        return QStringLiteral("[Name: \"%1\", DataType: \"%2\", ValueRank: \"%3\", ArrayDimensions: %4, Description: %5]").arg(
                    a.name()).arg(a.dataTypeId()).arg(a.valueRank()).arg(numberArrayToString(a.arrayDimensions())).arg(
                    localizedTextToString(a.description()));
    } else if (value.canConvert<QOpcUa::QExtensionObject>()) {
        const auto obj = value.value<QOpcUa::QExtensionObject>();
        return QStringLiteral("[TypeId: \"%1\", Encoding: %2, Body: 0x%3]").arg(obj.encodingTypeId()).arg(
                    obj.encoding() == QOpcUa::QExtensionObject::Encoding::NoBody ?
                        "NoBody" : (obj.encoding() == QOpcUa::QExtensionObject::Encoding::ByteString ?
                            "ByteString" : "XML")).arg(obj.encodedBody().isEmpty() ? "0" : QString(obj.encodedBody().toHex()));
    }

    if (value.canConvert<QString>())
        return value.toString();

    return QString();
}

QString TreeItem::localizedTextToString(const QOpcUa::QLocalizedText &text) const
{
    return QStringLiteral("[Locale: \"%1\", Text: \"%2\"]").arg(text.locale()).arg(text.text());
}

QString TreeItem::rangeToString(const QOpcUa::QRange &range) const
{
    return QStringLiteral("[Low: %1, High: %2]").arg(range.low()).arg(range.high());
}

QString TreeItem::euInformationToString(const QOpcUa::QEUInformation &info) const
{
    return QStringLiteral("[UnitId: %1, NamespaceUri: \"%2\", DisplayName: %3, Description: %4]").arg(info.unitId()).arg(
                info.namespaceUri()).arg(localizedTextToString(info.displayName())).arg(localizedTextToString(info.description()));
}

QT_END_NAMESPACE

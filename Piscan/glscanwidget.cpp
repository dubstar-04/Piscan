/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtWidgets>
#include <QtOpenGL>

#include <math.h>

#include "glscanwidget.h"

#include <QMouseEvent>
#include <QTime>

#include <math.h>
#include <stdlib.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLScanWidget::GLScanWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    //QTime midnight(0, 0, 0);
    //qsrand(midnight.secsTo(QTime::currentTime()));

    //logo = 0;
    xRot = -75 * 16;;
    yRot = 0;
    zRot = 0;
    scaling = 1.0;

    qtGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
    qtPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);

    setAutoFillBackground(false);
    createGradient();
    setMinimumSize(200, 200);
    //setWindowTitle(tr("Overpainting a Scene"));
}

GLScanWidget::~GLScanWidget()
{
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLScanWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot)
        xRot = angle;
}

void GLScanWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot)
        yRot = angle;
}

void GLScanWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot)
        zRot = angle;
}

void GLScanWidget::initializeGL()
{
    glEnable(GL_MULTISAMPLE);

    //logo = new QtLogo(this);
    //logo->setColor(qtGreen.dark());
}

void GLScanWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLScanWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    //int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        //setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        //setXRotation(xRot + 8 * dy);
        //setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();

    update();
}

void GLScanWidget::wheelEvent(QWheelEvent *event){

    double numDegrees = -event->delta()/8.0;
    double numSteps = numDegrees / 15.0;
    scaling *= pow(1.25, numSteps);
    if (scaling > 3){
        scaling = 3;
    }else if (scaling < 0.25){
        scaling = 0.25;
    }
    //qDebug() << "scaling: " << QString::number(scaling);
    update();
}

void GLScanWidget::paintEvent(QPaintEvent *event)
{
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    qglClearColor(Qt::white);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glEnable(GL_MULTISAMPLE);
    //static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    setupViewport(width(), height());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(zRot / 16.0, 0.0, 0.0, 1.0);
    glScalef(scaling, scaling, scaling);

    //logo->draw();
    //paintGL();

    drawGL();

    glShadeModel(GL_FLAT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();



    QPainter painter(this);
    //drawBackground(&painter);
    drawInstructions(&painter);
    painter.end();
}

void GLScanWidget::drawGL()
{
    //Draw Z Axis
    glLineWidth(1.5);
    qglColor(Qt::red);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 200);
    glEnd();
    //renderText(0, 0, 260, "z");
    //Draw X Axis
    qglColor(Qt::green);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(125, 0, 0);
    glEnd();
    //Draw X Axis
    qglColor(Qt::blue);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 125, 0);
    glEnd();


    qglColor(Qt::lightGray);
    int sections = 36;
    GLfloat radius = 125;
    GLfloat twoPi = 2.0 * 3.14159;

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.0, 0.0, -1.5); //center of triangles

    for(int i = 0; i <= sections; i++) {

        glVertex3f(radius*cos(i*twoPi / sections),
                   radius*sin(i*twoPi / sections), -1.5);
    }

    glEnd();




 //Draw Cloud

        if (cloud != NULL)
        {

        if(cloud->cloudSize() != 0 ){

        qglColor(Qt::black);

        glBegin(GL_POINTS);
        for(int i=0; i < cloud->cloudSize(); i++){
            glVertex3f( cloud->X(i), cloud->Y(i), (cloud->Z(i)) + abs(cloud->getZmin()));
            //qDebug() << cloud.X(i);
        }
        glEnd();

        qglColor(Qt::red);

        glBegin(GL_POINTS);
        for(int i=0; i < cloud->errorSize(); i++){
            glVertex3f( cloud->errorX(i), cloud->errorY(i), (cloud->errorZ(i)) + abs(cloud->getZmin()));
            //qDebug() << cloud.X(i);
        }
        glEnd();


        }

    }


}

void GLScanWidget::drawBackground(QPainter *painter)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(gradient);
    painter->drawRect(rect());
}

void GLScanWidget::createGradient()
{
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setCenter(0.45, 0.50);
    gradient.setFocalPoint(0.40, 0.45);
    gradient.setColorAt(0.0, QColor(105, 146, 182));
    gradient.setColorAt(0.4, QColor(81, 113, 150));
    gradient.setColorAt(0.8, QColor(16, 56, 121));
}

void GLScanWidget::resizeGL(int width, int height)
{
    setupViewport(width, height);
}

void GLScanWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    //createBubbles(20 - bubbles.count());
}

QSize GLScanWidget::sizeHint() const
{
    return QSize(400, 400);
}



void GLScanWidget::setupViewport(int width, int height)
{


    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-width/2, +width/2, -height/4, +height/4*3, -width, width);
#endif
    glMatrixMode(GL_MODELVIEW);

}

void GLScanWidget::drawInstructions(QPainter *painter)
{
    QString text = tr("Click and drag with the left mouse button "
                      "to rotate the point cloud.");
    QFontMetrics metrics = QFontMetrics(font());
    int border = qMax(4, metrics.leading());

    QRect rect = metrics.boundingRect(0, 0, width() - 2*border, int(height()*0.125),
                                      Qt::AlignCenter | Qt::TextWordWrap, text);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
                     QColor(0, 0, 0, 127));
    painter->setPen(Qt::white);
    painter->fillRect(QRect(0, 0, width(), rect.height() + 2*border),
                      QColor(0, 0, 0, 127));
    painter->drawText((width() - rect.width())/2, border,
                      rect.width(), rect.height(),
                      Qt::AlignCenter | Qt::TextWordWrap, text);
}



void GLScanWidget::setCloud(pointCloud *cloudptr)
{
    cloud = cloudptr;
    //qDebug() << "Point Cloud Set";
    update();

}






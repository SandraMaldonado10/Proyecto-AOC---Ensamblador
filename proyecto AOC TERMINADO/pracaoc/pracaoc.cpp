/***************************************************************************
 *   Copyright (C) 2024 by pilar                                           *
 *   pilarb@unex.es                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "pracaoc.h"


pracAOC::pracAOC(): QWidget()
{
	mainWin=new QMainForm();
	mainWin->setupUi(this);

    mainWin->buttonPrev->setVisible(false);
    mainWin->buttonSig->setVisible(false);

    for(int y=0; y<CONTAINERH; y++)
        for(int x=0; x<CONTAINERW; x++)
            if((CONTAINERH-y)<=4  || (CONTAINERW-x)<=4)
                container[y*CONTAINERW+x] = 1;
            else
                container[y*CONTAINERW+x] = 0;


    assignColors();

    frameFigs[0] = mainWin->frameFigure1;
    frameFigs[1] = mainWin->frameFigure2;
    frameFigs[2] = mainWin->frameFigure3;
    areaFigs[0] = mainWin->figureArea1;
    areaFigs[1] = mainWin->figureArea2;
    areaFigs[2] = mainWin->figureArea3;

    generateNewFigures();

    start = true;
    gameOver = false;
    points = 0;


    connect(mainWin->buttonComenzar, SIGNAL(clicked()), this, SLOT(comenzar_parar()));
    connect(mainWin->groupBoxPruebas, SIGNAL(clicked(bool)), this, SLOT(set_modo_pruebas(bool)));
    connect(mainWin->buttonVaciar, SIGNAL(clicked()), this, SLOT(prueba_vaciar()));
    connect(mainWin->buttonBorrarCeldas, SIGNAL(clicked()), this, SLOT(prueba_borrar_celdas()));
    connect(mainWin->buttonLineasCompletas, SIGNAL(clicked()), this, SLOT(prueba_lineas_completas()));
    connect(mainWin->buttonPosiblJugada, SIGNAL(clicked()), this, SLOT(prueba_posible_jugada()));
    connect(mainWin->buttonInsertarF, SIGNAL(clicked()), this, SLOT(prueba_insertar_figura()));
    connect(mainWin->buttonBorrarF, SIGNAL(clicked()), this, SLOT(prueba_borrar_figura()));
    connect(mainWin->buttonHuecoLibre, SIGNAL(clicked()), this, SLOT(prueba_hueco_libre()));
    connect(mainWin->buttonPrev, SIGNAL(clicked()), this, SLOT(prueba_selecciona_anterior()));
    connect(mainWin->buttonSig, SIGNAL(clicked()), this, SLOT(prueba_selecciona_siguiente()));
    connect(mainWin->checkBoxColorFijo, SIGNAL(toggled(bool)), this, SLOT(cambia_color_fijo_variable(bool)));
    connect(mainWin->comboBoxColores, SIGNAL(currentIndexChanged(int)), SLOT(cambia_color(int)));

}


pracAOC::~pracAOC()
{
}


void pracAOC::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

    //Draw container
    int xI, yI, w, h;

    xI = mainWin->containerArea->x();
    yI = mainWin->containerArea->y();
    w = mainWin->containerArea->width();
    h = mainWin->containerArea->height();

    painter.fillRect(xI,yI,w,h,Qt::white);

    for(int y=0; y<CONTAINERH-4; y++)
        for(int x=0, xP=0; x<CONTAINERW-4; x++, xP++)
        {
            int idColor = container[y*CONTAINERW+x];
            if(gameOver)
                painter.setOpacity(0.2);
            else
                painter.setOpacity(1.);
            painter.fillRect(xI+xP*SQUARESIZEC, yI+y*SQUARESIZEC, SQUARESIZEC, SQUARESIZEC, colors[idColor*2]);
            painter.fillRect(xI+xP*SQUARESIZEC+SQUAREMARGIN, yI+y*SQUARESIZEC+SQUAREMARGIN, SQUARESIZEC-SQUAREMARGIN*2, SQUARESIZEC-SQUAREMARGIN*2, colors[idColor*2+1]);
            if(container[y*CONTAINERW+x]>0)
                painter.drawRect(xI+xP*SQUARESIZEC, yI+y*SQUARESIZEC, SQUARESIZEC, SQUARESIZEC);
            if(mainWin->groupBoxPruebas->isChecked() and celdasMarcadas[y*CONTAINERW+x]>0)
            {
                painter.setPen(QPen ( QBrush ( QColor(200,0,0) ), 2));
                painter.drawLine(xI+xP*SQUARESIZEC, yI+y*SQUARESIZEC, xI+(xP+1)*SQUARESIZEC, yI+(y+1)*SQUARESIZEC);
                if(celdasMarcadas[y*CONTAINERW+x]>1)
                {
                    painter.drawLine(xI+(xP+1)*SQUARESIZEC, yI+y*SQUARESIZEC, xI+xP*SQUARESIZEC, yI+(y+1)*SQUARESIZEC);
                    if(celdasMarcadas[y*CONTAINERW+x]>2)
                    {
                        painter.drawLine(xI+(xP+0.5)*SQUARESIZEC, yI+y*SQUARESIZEC, xI+(xP+0.5)*SQUARESIZEC, yI+(y+1)*SQUARESIZEC);
                        if(celdasMarcadas[y*CONTAINERW+x]>3)
                            painter.drawLine(xI+xP*SQUARESIZEC, yI+(y+0.5)*SQUARESIZEC, xI+(xP+1)*SQUARESIZEC, yI+(y+0.5)*SQUARESIZEC);
                    }
                }
                painter.setPen(QPen ( QBrush ( Qt::black ), 1));
            }
        }

    if(mainWin->groupBoxPruebas->isChecked())
    {
        int x_selarea = (x_seleccionada)*SQUARESIZEC;
        int y_selarea = y_seleccionada*SQUARESIZEC;
        int w_selarea = SQUARESIZEC*4;
        int h_selarea = SQUARESIZEC*4;
        if(x_selarea+w_selarea >= mainWin->containerArea->width())
            w_selarea = mainWin->containerArea->width()-x_selarea;
        if(y_selarea+h_selarea >= mainWin->containerArea->height())
            h_selarea = mainWin->containerArea->height()-y_selarea;

        painter.setPen(QPen ( QBrush ( color_selarea ), 2));
        painter.drawRect(xI+x_selarea, yI+y_selarea, w_selarea, h_selarea);
        painter.setPen(QPen ( QBrush ( Qt::black ), 1));

    }


    //Draw next figures
    for(int i=0; i<3; i++)
    {
        QPoint p = areaFigs[i]->pos();
        xI = frameFigs[i]->mapToParent(p).x();
        yI = frameFigs[i]->mapToParent(p).y();
        w = areaFigs[i]->width();
        h = areaFigs[i]->height();

        if(gameOver or (mainWin->groupBoxPruebas->isChecked() and i>0))
            painter.setOpacity(0.2);
        else
            painter.setOpacity(1.);

        painter.fillRect(xI,yI,w,h,Qt::white);

        if(not emptyFig[i] or (i==0 and mainWin->groupBoxPruebas->isChecked()))
        {
            for(int y=0; y<4; y++)
                for(int x=0; x<4; x++)
                {

                    painter.fillRect(xI+x*SQUARESIZEF, yI+y*SQUARESIZEF, SQUARESIZEF, SQUARESIZEF, colors[currentFig[i][y][x]*2]);
                    painter.fillRect(xI+x*SQUARESIZEF+SQUAREMARGIN, yI+y*SQUARESIZEF+SQUAREMARGIN, SQUARESIZEF-SQUAREMARGIN*2, SQUARESIZEF-SQUAREMARGIN*2, colors[currentFig[i][y][x]*2+1]);
                    if(currentFig[i][y][x]>0)
                        painter.drawRect(xI+x*SQUARESIZEF, yI+y*SQUARESIZEF, SQUARESIZEF, SQUARESIZEF);
                }
        }
    }

    if(gameOver)
    {
        xI = mainWin->containerArea->x();
        yI = mainWin->containerArea->y();

        painter.setOpacity(0.5);
        painter.setPen(Qt::black);
        painter.setFont(QFont("Times", 32, QFont::Bold));
        painter.drawText(QPointF(xI+3*SQUARESIZEC, yI+6*SQUARESIZEC), "GAME OVER");
    }


    //Draw selected figure
    if(selection)
    {
        QPoint pI= mousePSelected;
        if(lastPosSelected.x()>=0 and lastPosSelected.y()>=0)
            pI = normalizeCoordinates(mousePSelected);
        xI = pI.x();
        yI = pI.y();

        painter.setOpacity(0.5);

        for(int y=0; y<4; y++)
            for(int x=0; x<4; x++)
            {

                if(selectedFig[y][x]>0)
                {
                    painter.fillRect(xI+x*SQUARESIZEC, yI+y*SQUARESIZEC, SQUARESIZEC, SQUARESIZEC, Qt::darkGray);
                    painter.fillRect(xI+x*SQUARESIZEC+SQUAREMARGIN, yI+y*SQUARESIZEC+SQUAREMARGIN, SQUARESIZEC-SQUAREMARGIN*2, SQUARESIZEC-SQUAREMARGIN*2, Qt::gray);
                    painter.drawRect(xI+x*SQUARESIZEC, yI+y*SQUARESIZEC, SQUARESIZEC, SQUARESIZEC);
                }
            }

    }

    painter.setOpacity(1);
    painter.setPen(color_posibleJ);
    painter.setBrush(color_posibleJ);
    QPoint pPosJ = mainWin->groupBoxPruebas->pos() + mainWin->framePosibleJ->pos();
    painter.fillRect(pPosJ.x(), pPosJ.y(), mainWin->framePosibleJ->width(), mainWin->framePosibleJ->height(), color_posibleJ);

    mainWin->lcdNumber->display(points);
 	
}

void pracAOC::assignColors()
{
    colors[0]=QColor(255,255,255);
    colors[1]=QColor(255,255,255);
    colors[2]=QColor(0,0,155);
    colors[3]=QColor(0,0,255);
    colors[4]=QColor(0,155,0);
    colors[5]=QColor(0,255,0);
    colors[6]=QColor(155,0,0);
    colors[7]=QColor(255,0,0);
    colors[8]=QColor(0,155,155);
    colors[9]=QColor(0,255,255);
    colors[10]=QColor(155,155,0);
    colors[11]=QColor(255,255,0);
    colors[12]=QColor(155,0,155);
    colors[13]=QColor(255,0,255);
    colors[14]=QColor(255,100,0);
    colors[15]=QColor(255,165,0);

}


void pracAOC::generateNewFigures()
{
    for(int i=0; i<3; i++)
    {
        generateRandomFig(currentFig[i]);
        emptyFig[i] = false;

    }
}

void pracAOC::generateRandomFig(char fig[4][4])
{
    int color;

    if(mainWin->groupBoxPruebas->isChecked() and mainWin->checkBoxColorFijo->isChecked())
        color = mainWin->comboBoxColores->currentIndex()+1;
    else
        color = QRandomGenerator::global()->generate() % 7 + 1;

    int totalBlocks = QRandomGenerator::global()->generate() % 5+2;
    int lastIniPos = 0;
    int lastNBlocks= 1;
    int nBlocks = 0;
    for(int y=0; y<4; y++)
    {
        int nCBlocks = QRandomGenerator::global()->generate() % 4+1;
        int iniPos = QRandomGenerator::global()->generate() % lastNBlocks + lastIniPos;
        int nBlocksRow = 0;
        for(int x=0; x<4; x++)
        {
            if(x>=iniPos and nBlocksRow<nCBlocks and nBlocks<totalBlocks)
            {
                fig[y][x] = color;
                nBlocks++;
                nBlocksRow++;
            }
            else
                fig[y][x] = 0;
        }
        lastIniPos = iniPos;
        lastNBlocks = nCBlocks;
    }

}

void pracAOC::generateFigsSequence()
{
    nFigsInSequence = 0;
    for(int i=0; i<NFIGSINSEQUENCE; i++)
    {
        generateRandomFig(figureSequence[i]);
        nFigsInSequence++;
    }
    indexSequence = 0;
    memcpy(currentFig[0], figureSequence[0], 16);

}

void pracAOC::comenzar_parar()
{
    start = !start;

    if(start)
    {
        mainWin->buttonComenzar->setText(QString("Parar"));
        gameOver = false;
        blockblast::vaciaContenedor(container, CONTAINERW, CONTAINERH);
        generateNewFigures();
        points = 0;
    }
    else
    {
        mainWin->buttonComenzar->setText(QString("Comenzar"));
        gameOver = true;
    }
    update();

}

void pracAOC::cambia_color_fijo_variable(bool fijo)
{
    mainWin->comboBoxColores->setEnabled(fijo);
    generateFigsSequence();
    update();
}

void pracAOC::cambia_color(int color)
{
    Q_UNUSED(color);
    generateFigsSequence();
    update();
}

void pracAOC::set_modo_pruebas(bool iniciar)
{
    if(iniciar)
    {
        mainWin->comboBoxColores->setEnabled(mainWin->checkBoxColorFijo->isChecked());
        generateFigsSequence();
    }
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();

}

void pracAOC::prueba_vaciar()
{
    blockblast::vaciaContenedor(container, CONTAINERW, CONTAINERH);
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();
}

void pracAOC::prueba_borrar_celdas()
{
    points += blockblast::borraCeldasMarcadas(container, CONTAINERW, CONTAINERH, celdasMarcadas);
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();
}

void pracAOC::prueba_lineas_completas()
{
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    blockblast::marcaLineasCompletas(container, CONTAINERW, CONTAINERH, celdasMarcadas);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();

}

void pracAOC::prueba_posible_jugada()
{
    if(blockblast::posibleJugada(container, CONTAINERW, CONTAINERH, currentFig[0]))
        color_posibleJ = Qt::green;
    else
        color_posibleJ = Qt::red;
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    color_selarea = Qt::gray;
    update();
}

void pracAOC::prueba_insertar_figura()
{
    blockblast::insertaPieza(container, CONTAINERW, currentFig[0], x_seleccionada, y_seleccionada);
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();
}

void pracAOC::prueba_borrar_figura()
{
    blockblast::borraPieza(container, CONTAINERW, currentFig[0], x_seleccionada, y_seleccionada);
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();
}

void pracAOC::prueba_hueco_libre()
{
    if(blockblast::huecoLibre(container, CONTAINERW, currentFig[0], x_seleccionada, y_seleccionada))
        color_selarea = Qt::green;
    else
        color_selarea = Qt::red;
    blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
    color_posibleJ = Qt::white;
    update();
}

void pracAOC::prueba_selecciona_anterior()
{
    indexSequence--;
    if(indexSequence<0)
        indexSequence = nFigsInSequence-1;
    memcpy(currentFig[0], figureSequence[indexSequence], 16);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();

}

void pracAOC::prueba_selecciona_siguiente()
{
    indexSequence = (indexSequence+1)%nFigsInSequence;
    memcpy(currentFig[0], figureSequence[indexSequence], 16);
    color_selarea = Qt::gray;
    color_posibleJ = Qt::white;
    update();

}

QPoint pracAOC::normalizeCoordinates(QPoint pView)
{
    QPoint pNorm;
    QPoint pFrame = mainWin->containerArea->mapFrom(this, pView);
    QPoint pContainer = viewToContainer(pFrame);
    pContainer.setX((int) rint(pContainer.x()*SQUARESIZEC));
    pContainer.setY((int) rint(pContainer.y()*SQUARESIZEC));
    pNorm = mainWin->containerArea->mapTo(this, pContainer);

    return pNorm;
}

QPoint pracAOC::viewToContainer(QPoint pView)
{
    QPoint pContainer;
    pContainer.setX((int) rint(pView.x()/SQUARESIZEC));
    pContainer.setY((int) rint(pView.y()/SQUARESIZEC));

    return pContainer;
}

void pracAOC::mousePressEvent ( QMouseEvent *e )
{
    if ( not gameOver and e->button() == Qt::LeftButton )
    {
        QPoint p(e->x(), e->y());
        QPoint pFrame = mainWin->containerArea->mapFrom(this, p);
        mousePSelected = p;

        if(QRect(0,0, mainWin->containerArea->width(), mainWin->containerArea->height()).contains(pFrame))
        {
            QPoint pSelect = viewToContainer(pFrame);
            x_seleccionada = pSelect.x();
            y_seleccionada = pSelect.y();
            color_selarea = Qt::gray;
            update();
        }
        else
        {
            if(not mainWin->groupBoxPruebas->isChecked())
            {
                idSelectedFig = -1;
                for(int i=0; i<3 && idSelectedFig<0; i++)
                {
                    if(not emptyFig[i])
                    {
                        QPoint pFig = areaFigs[i]->mapFrom(this, p);
                        if(QRect(0,0, areaFigs[i]->width(), areaFigs[i]->height()).contains(pFig))
                            idSelectedFig = i;
                    }
                }
                if(idSelectedFig>=0)
                {
                    lastPosSelected = QPoint(-1,-1);
                    selection = true;
                    memcpy(selectedFig, currentFig[idSelectedFig], 16);
                }
            }
        }
    }
}

void pracAOC::mouseMoveEvent(QMouseEvent *e)
{
    if ( selection )
    {
        QPoint p(e->x(), e->y());
        QPoint pFrame = mainWin->containerArea->mapFrom(this, p);
        mousePSelected = p;

        if(QRect(0,0, mainWin->containerArea->width(), mainWin->containerArea->height()).contains(pFrame))
        {
            emptyFig[idSelectedFig] = true;
            QPoint pContainer = viewToContainer(pFrame);
            int xAct = lastPosSelected.x();
            int yAct = lastPosSelected.y();
            int xSig = pContainer.x();
            int ySig = pContainer.y();
            if(xAct>=0 and yAct>=0)
                blockblast::borraPieza(container, CONTAINERW, selectedFig, xAct, yAct);
            if(blockblast::huecoLibre(container, CONTAINERW, selectedFig, xSig, ySig))
            {
                blockblast::insertaPieza(container, CONTAINERW, selectedFig, xSig, ySig);
                lastPosSelected.setX(xSig);
                lastPosSelected.setY(ySig);
            }
            else
                lastPosSelected = QPoint(-1,-1);
        }
        else
        {
            int xAct = lastPosSelected.x();
            int yAct = lastPosSelected.y();

            if(xAct>=0 and yAct>=0)
            {
                lastPosSelected = QPoint(-1,-1);
                blockblast::borraPieza(container, CONTAINERW, selectedFig, xAct, yAct);        }
            }
    }
    update();
}

void pracAOC::mouseReleaseEvent(QMouseEvent *e)
{
    if(selection and e->button() == Qt::LeftButton)
    {
        if(lastPosSelected.x()>=0)
        {
            blockblast::insertaPieza(container, CONTAINERW, selectedFig, lastPosSelected.x(), lastPosSelected.y());
            blockblast::vaciaContenedor(celdasMarcadas, CONTAINERW, CONTAINERH);
            blockblast::marcaLineasCompletas(container, CONTAINERW, CONTAINERH, celdasMarcadas);
            int nPoints = blockblast::borraCeldasMarcadas(container, CONTAINERW, CONTAINERH, celdasMarcadas);
            points += nPoints;
            if(emptyFig[0] and emptyFig[1] and emptyFig[2])
                generateNewFigures();
            checkGameOver();
        }
        else
        {
            emptyFig[idSelectedFig] = false;
        }
    }
    update();
    selection = false;
}

void pracAOC::checkGameOver()
{
    gameOver = true;
    for(int i=0; i<3 and gameOver; i++)
    {
        if(not emptyFig[i])
            if(blockblast::posibleJugada(container, CONTAINERW, CONTAINERH, currentFig[i]))
                gameOver = false;
    }
    if(gameOver)
        comenzar_parar();
    update();
}

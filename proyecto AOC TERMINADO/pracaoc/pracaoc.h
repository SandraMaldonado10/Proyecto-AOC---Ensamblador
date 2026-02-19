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


#ifndef PRACAOC_H
#define PRACAOC_H

#include <ui_mainForm.h>
#include <QtCore>
#include <QtWidgets/QLabel>
#include <QPainter>
#include <QtWidgets/QFileDialog>
#include <QMouseEvent>
#include <iostream>
#include "blockblast.h"
#include <QtWidgets/QColorDialog>
#include <QRandomGenerator>


#define SQUARESIZEC 40
#define SQUARESIZEF 30
#define SQUAREMARGIN 5
#define CONTAINERW 16
#define CONTAINERH 16
#define NCOLORS 16
#define NFIGSINSEQUENCE 20

typedef char FigureType[4][4];

using namespace Ui;

class pracAOC:public QWidget
{
    Q_OBJECT

public:
    pracAOC();
    ~pracAOC();
		
		

private:

		QMainForm * mainWin;

        bool gameOver;

        char container[CONTAINERH*CONTAINERW], celdasMarcadas[CONTAINERH*CONTAINERW];

        QColor colors[NCOLORS];

        FigureType currentFig[3];
        QFrame * frameFigs[3], * areaFigs[3];
        FigureType selectedFig;
        int idSelectedFig;
        bool emptyFig[3];

        bool selection;

        QPoint lastPosSelected, mousePSelected;

        int xFig, yFig;

        FigureType figureSequence[NFIGSINSEQUENCE];
        int nFigsInSequence, indexSequence;

        int giro_seleccionado=0;
        int x_seleccionada=0, y_seleccionada=0;
        QColor color_selarea = Qt::gray;
        QColor color_posibleJ = Qt::white;

        int points;
        bool start;

        int nFigures, nNewFigures;
        char newRow[CONTAINERW];

        void assignColors();
        void generateNewFigures();
        void generateRandomFig(char fig[4][4]);
        void generateFigsSequence();
        QPoint normalizeCoordinates(QPoint pView);
        QPoint viewToContainer(QPoint pview);
        void checkGameOver();

public slots:

		void paintEvent(QPaintEvent *);
        void cambia_color_fijo_variable(bool);
        void cambia_color(int);
        void comenzar_parar();
        void set_modo_pruebas(bool);
        void prueba_vaciar();
        void prueba_borrar_celdas();
        void prueba_lineas_completas();
        void prueba_posible_jugada();
        void prueba_insertar_figura();
        void prueba_borrar_figura();
        void prueba_hueco_libre();
        void prueba_selecciona_anterior();
        void prueba_selecciona_siguiente();
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);


};




#endif

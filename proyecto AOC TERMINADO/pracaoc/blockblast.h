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

#ifndef BLOCKBLAST_H
#define BLOCKBLAST_H

#include <QtCore>

namespace blockblast{
    extern void vaciaContenedor(char * container, int w, int h);
    extern bool huecoLibre(char * container, int w, char figure[4][4], int c, int f);
    extern void insertaPieza(char * container, int w, char figure[4][4], int c, int f);
    extern void borraPieza(char * container, int w, char figure[4][4], int c, int f);
    extern void marcaLineasCompletas(char * container, int w, int h, char * celdasMarcadas);
    extern int borraCeldasMarcadas(char * container, int w, int h, char * celdasMarcadas);
    extern bool posibleJugada(char * container, int w, int h, char figure[4][4]);

}

#endif

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

#include "blockblast.h"

//#define WITH_SSE
// ESTE ES EL QUE HAY QUE HACER, LA 1º OPCIÓN
// ESTO ES UN EJEMPLO PARA HACER LOS SIGUIENTES ALGORITMOS -> FIJARSE EN ESTE
void blockblast::vaciaContenedor(char * container, int w, int h)
{
#ifndef WITH_SSE
    asm volatile(

        // W= ancho ; H= alto
        // W = FILA     H = COLUMNA

        // SE RECOMIENDA NO USAR LOOP, NI DOS LOOPS ANIDADOS
        // %rcx <- h-4
        // rsi = container
        "mov %0, %%rsi;" // ES MUY IMPORTANTE PONER EL ; PORQUE LUEGO NO NOS PONE DONDE ESTÁ EL ERROR
        // rbx = w = rax
        "mov %1, %%eax;"
        "mov %%eax, %%ebx;"
        "sub $4, %%ebx;" // w-4
        // rcx = h
        "mov $0, %%rcx;"
        "mov %2, %%ecx;"
        "sub $4, %%ecx;" // Nos pasan un puntero a char -> h-4
        "BVaciarF:"
        // %si:
            "mov $0, %%edi;" // Se elige edi porque hay que comparar con un registro de enteros
            "BVaciarC:"
                "cmp %%ebx, %%edi;" // si (c<w-4)
                "jge sigVaciar;"
                "movb $0, (%%rsi);" // [dirContainer] = 0
            "sigVaciar:"
                "inc %%rsi;" // dirContainer ++
                "inc %%edi;" //c++
                "cmp %%eax, %%edi;" // c<w
                "jl BVaciarC;"
            "loop BVaciarF;" // Se incrementa el rcx hasta que llegue al límite que es h-4

        // INDICAR LOS REGISTROS QUE USEMOS (IMPORTANTISIMO):
        :
        : "m" (container), "m" (w), "m" (h) // m = %0; w = %1; h  = %2
        : "%rax", "%rbx", "%rcx", "%rsi", "%rdi", "memory"

    );
#else
    asm volatile(
        // Por simplificación, asumimos w=16.
        // Dicho valor es el utilizado como anchura en la definición de "container"

        "mov %0, %%rsi \n\t" // ACORDARSEs DE PONER LOS 2 % PORQUE LUEGO NO VOY A SABER DONDE ESTÁ EL ERROR DE COMPILACIÓN
        "mov $0, %%rcx;"
        "mov %2, %%ecx;"
        "sub $4, %%rcx;"

        "pxor %%xmm0, %%xmm0;"
        "mov $0xFFFFFFFF, %%eax;"
        "pinsrd $3, %%eax, %%xmm0;"

        "BVaciarF:"
            "movdqu (%%rsi), %%xmm1;"
            "pand %%xmm0, %%xmm1;"
            "movdqu %%xmm1, (%%rsi);"

            "add $16, %%rsi;"
            "loop BVaciarF;"

        : //
        : "m" (container), "m" (w), "m" (h) // OPERANDOS DE ENTRADA
        : "%rax","%rcx","%rsi","%xmm0","%xmm1","memory" // LA LETRA DE ANTES, INDICA SI ES DESDE MEMORIA O REGISTRO (M=memoria; R=registro)
        // %0      %1     %2
        // %0 = container
        // %1 = w
        // %2 = h
    );

#endif
}

// * -> 64 bits -> %rax, %rsi, %r8...
// int (l) -> 32 bits -> %eax, %esi, %r8d...
// char, bool (b) ->  8 bits -> %al, %ah, %r8b...


//                               contenedor   ancho  figura(4*4)->los que salen a la derecha que son 3
//                                                                    columna   fila
bool blockblast::huecoLibre(char * container, int w, char figure[4][4], int c, int f)
{
 bool hayHueco=false;

#ifndef WITH_SSE  // ------------> SI NO ESTÁ DEFINIDO WITH SEE, HAY LO SIGUIENTE, SINO LO QUE HAY EN EL #ELSE
    asm volatile(
        ";"

       "movb $1, %0;" //hayHueco
       "mov %1, %%rsi;" //dirContainer = container = rsi
       "mov %3, %%rdi;" //dirFigure = figure = rdi
       "mov %5, %%r8d;" //fAct = f = r8d
       "mov %2, %%r9d;" //w = r9d
       "mov %4, %%r10d;" //c = r10d

             "mov $0, %%r11d;" // df = r11d
             "jmp condicionHayHueco1;"
             "cuerpoHayHueco1:;"
                // posCelda = ebx
                "mov %%r9d, %%eax;" // Para conservar el valor de w
                "imul %%r8d, %%eax;" // eax = fAct * w
                "mov %%r10d, %%ebx;" // Para conservar el valor de c
                "add %%eax, %%ebx;" // ebx = fAct * w + c -> posCelda

                "mov $0, %%r12d;" // dc = r12d
                "jmp condicionHuecoLibre2;"
                "cuerpoHuecoLibre2:"
                    "cmpb $0, (%%rdi);" // [dirFigure] != 0
                    "je noEntraIF;"
                    "movsx %%ebx, %%rbx;" // Para pasarlo a 64 bits
                    "cmpb $0, (%%rsi, %%rbx);"
                    "je noEntraIF;"

                    "movb $0, %0;" // hayHueco = falso

                    "noEntraIF:;"
                        "inc %%ebx;" //posCelda++
                        "inc  %%rdi;" //dirFigure++

                    "inc %%r12d;" //dc++
                "condicionHuecoLibre2:;"
                    "cmp $4, %%r12d;" //dc < 4
                    "jge seSale2;"
                    "cmpb $1, %0;" //hayHueco = true
                    "je cuerpoHuecoLibre2;"
                    "jmp seSale2;"

             "condicionHayHueco1:;"
                "cmp $4, %%r11d;" // df < 4
                "jge seSale1;"
                "cmpb $1, %0;" // hayHueco  =  true
                "je cuerpoHayHueco1;"
                "jmp seSale1;"

             "seSale2:;"
                "inc %%r8d;" // fAct++

             "inc %%r11d;" // df++
             "jmp condicionHayHueco1;"

             "seSale1:;"

        : "=m" (hayHueco) // -> hayHueco %0
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        //        %1         %2         %3          %4        %5
        : "%rsi", "%rdi", "%rax", "%rbx", "%r9", "%r10", "%r11", "%r12", "memory"

    );
#else
    asm volatile(
        ";"


        : "=m" (hayHueco)
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );


#endif

    return hayHueco;
}

void blockblast::insertaPieza(char * container, int w, char figure[4][4], int c, int f)
{

#ifndef WITH_SSE
    asm volatile(
        ";"

        "mov %0, %%rsi;" // dirContainer = container = rsi
        "mov %2, %%rdi;" // dirFigure = figure = rdi
        "mov %4, %%r8d;" // fAct = f = r8d
        "mov %1, %%r11d;" //w = r11d
        "mov %3, %%r10d;" //c = r10d

        "mov $0, %%r9d;" //df = 0 --> r9d
        "jmp condicionInsertar1;"
        "cuerpoInsertar1:;"
                // posCelda = ebx
                "mov %%r11d, %%eax;" // Para conservar el valor de w
                "imul %%r8d, %%eax;" // eax = fAct * w
                "mov %%r10d, %%ebx;" // Para conservar el valor de c
                "add %%eax, %%ebx;" // ebx = fAct * w + c -> posCelda

                "mov $0, %%r12d;" // dc = 0 -> r12d
                "jmp condicionInsertar2;"
                "cuerpoInsertar2:;"
                    "cmpb $0, (%%rdi);" // [dirFigure] = 0
                    "je noEntraIFInsertar2;"
                    //entraIFInsertar2:
                    "mov (%%rdi), %%r14b;" // FIGURE ES DE 8 BITS!!
                    "mov %%ebx, %%r13d;" // Pasar el posCelda a otro registro para no perderlo -> r13d
                    "movsx %%r13d, %%r13;" // Se pasa a 64 bits
                    "mov %%r14b, (%%rsi, %%r13);" // [dirContainer + posCelda] = [dirFigure]
                    "noEntraIFInsertar2:;"
                        "inc %%ebx;" // posCelda++
                        "inc %%rdi;" //  dirFigure++

                    "inc %%r12d;" // dc++
                "condicionInsertar2:;"
                    "cmp $4, %%r12d;" // dc < 4
                    "jl cuerpoInsertar2;"
                    "inc %%r8d;" // fAct++

                "inc %%r9d;" //df++
        "condicionInsertar1:;"
                "cmp $4, %%r9d;"  // df < 4
                "jl cuerpoInsertar1;"

        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "%rsi", "%rdi", "%rax", "%rbx", "%r8", "%r9",  "%r10", "%r11", "%r12", "%r13", "%r14", "memory"
          // container = %0, w = %1, figure = %2,  c = %3, f = %4

    );

#else

    asm volatile(
        ";"

        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );

#endif
}

void blockblast::borraPieza(char * container, int w, char figure[4][4], int c, int f)
{

#ifndef WITH_SSE
    asm volatile(
        ";"

        "mov %0, %%rsi;" // dirContainer = container = rsi
        "mov %2, %%rdi;" // dirFigure = figure = rdi
        "mov %4, %%r8d;" // fAct = f
        "mov %1, %%r11d;" // w
        "mov %3, %%r10d;" // c

        "mov $0, %%r9d;" // df = 0 -> r9d
        "jmp condicionBorrar1;"
        "cuerpoBorrar1:;"
                // posCelda = ebx
                "mov %%r11d, %%eax;" // Para conservar el valor de w
                "imul %%r8d, %%eax;" // eax = fAct * w
                "mov %%r10d, %%ebx;" // Para conservar el valor de c
                "add %%eax, %%ebx;" // ebx = fAct * w + c -> posCelda

                "mov $0, %%r12d;" // dc= 0 -> r12d
                "jmp condicionBorrar2;"
                "cuerpoBorrar2:;"
                    "mov (%%rdi), %%r13b;" // Pasar figure a 8 bits!!!
                    "cmp $0, %%r13b;" // [dirFigure] != 0
                    "je noEntraIFBorrar;"
                    "movsx %%ebx, %%rbx;" // Pasar a 64 bits
                    "movb $0, (%%rsi, %%rbx);" // [dirContainer+posCelda] = 0

                    "noEntraIFBorrar:;"
                        "inc %%ebx;" //posCelda++
                        "inc %%rdi;" // dirFigure++

                    "inc %%r12d;" // dc++
                "condicionBorrar2:;"
                        "cmp $4, %%r12d;" // dc < 4
                        "jl cuerpoBorrar2;"
                        "inc %%r8d;" // fAct++

                "inc %%r9d;" // df++
        "condicionBorrar1:;"
                "cmp $4, %%r9d;" //df < 4
                "jl cuerpoBorrar1;"



        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "%rsi", "%rdi",  "%rax", "%rbx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "memory"

        // container = %0, w = %1, figure = %2, c = %3, f = %4

    );
#else
    asm volatile(
        ";"


        :
        : "m" (container), "m" (w), "m" (figure), "m" (c), "m" (f)
        : "memory"

    );


#endif

}

// Compueba si no hay lineas verticales u horizontales
void blockblast::marcaLineasCompletas(char * container, int w, int h, char * celdasMarcadas)
{
    bool completa = false;
    bool mismoColor = false;

    asm volatile(
        ";"

        // ------------  LINEAS HORIZONTALES  ----------------:
        "mov %2, %%rsi;" // dirContainer = container = rsi
        "mov %5, %%rdi;" // dirCeldasMarcadas = celdasMarcadas = rdi

        "mov %4, %%eax;" // h = eax -> valor original
        "mov %3, %%r9d;" // w = r9d -> valor original

        "mov %%eax, %%ebx;" // Lo pasamos a ebx para guardar el original
        "sub $4, %%ebx;" // h-4 -> ebx
        "mov %%r9d, %%ecx;" // Lo pasamos a ecx para guardar el original
        "sub $4, %%ecx;" // w-4 -> ecx

        "mov $0, %%r8d;" // f = 0 -> r8d
        "jmp condicionLineas1;"
        "cuerpoLineas1:;"
                // posCelda = edx
                "mov %%r9d, %%edx;"
                "imul %%r8d, %%edx;" // edx = f * w -> posCelda
                "movb $1, %0;" // completa = cierto
                "movsx %%edx, %%rdx;" //  Pasar posCelda a 64 bits
                "mov (%%rsi, %%rdx), %%r10b;" // color = [dirContainer + posCelda] -> r10b -> SE GUARDA EN UNO DE 8 BITS PORQUE SON TIPO CHAR (1 BYTE)
                "movb $1, %1;" // mismoColor = true

                "mov $0, %%r11d;" // c = 0 -> r11d
                "jmp condicionLineas2;"
                "cuerpoLineas2:;"
                    "cmpb $0, (%%rsi, %%rdx);" // [dirContainer + posCelda] = 0
                    "jne noEntraIFLineas2;"
                    "movb $0, %0;" // completa = falso
                    "jmp continaIFLineas2;"

                    "noEntraIFLineas2:;"
                        "cmp %%r10b, (%%rsi, %%rdx);" //[dirContainer + posCelda] != color
                        "je continaIFLineas2;"
                        "movb $0, %1;" // mismoColor = falso

                    "continaIFLineas2:;"
                        "inc %%edx;" //posCelda++

                    "inc %%r11d;" // c++
                    "jmp condicionLineas2;"

                "noLineas2:;"

                    "cmpb $1, %0;" // Si (completa)
                    "jne continuaFORLineas1;"
                    "cmpb $1, %1;" // Si (mismoColor)
                    "jne noEntraIFMismoColor;"
                    "mov $2, %%r12b;" // marca = 2 -> r12b -> TIPO CHAR (8 BITS)
                    "jmp continuaIFCompleta;"

                    "noEntraIFMismoColor:;"
                        "mov $1, %%r12b;" // marca = 1

                    "continuaIFCompleta:;"
                        "xor %%edx, %%edx;" // Poner a 0 posCelda por si acaso antes de hacerlo de nuevo
                        "mov %%r9d, %%edx;"
                        "imul %%r8d, %%edx;" // edx = f * w -> posCelda
                        "mov $0, %%r11d;" // c = 0
                        "jmp condicionLineas3;"
                        "cuerpoLineas3:;"
                            "movsx %%edx, %%rdx;" // Pasar posCelda a 64 bits --> NO SE SI HACE FALTA
                            "mov (%%rdi, %%rdx), %%r13b;" // r13b = [dirCeldasMarcadas+posCelda]
                            "mov %%r12b, %%r14b;" // Pasar marca a r14d para guardar su valor antes de modificarlo
                            "add %%r13b, %%r14b;" // [dirCeldasMarcadas+posCelda] = [dirCeldasMarcadas+posCelda]+ marca -> r14b
                            "mov %%r14b, (%%rdi, %%rdx);" // [dirCeldasMarcadas+posCelda] = [dirCeldasMarcadas+posCelda]+ marca
                            "inc %%edx;" // posCelda++

                        "inc %%r11d;" // c++
                        "condicionLineas3:;"
                            "cmp %%ecx, %%r11d;" // c<w-4
                            "jl cuerpoLineas3;"
                            "jmp continuaFORLineas1;"


                "condicionLineas2:;"
                    "cmp %%ecx, %%r11d;" // c < w-4
                    "jge noLineas2;" // si la Primera condicion no se cumple se sale
                    "cmpb $1, %0;" // completa = true
                    "je cuerpoLineas2;"
                    "jmp noLineas2;"


               "continuaFORLineas1:;" // SI NO ENTRA EN EL IF DE COMPLETA

               "inc %%r8d;" //  f++
        "condicionLineas1:;"
                "cmp %%ebx, %%r8d;" // f < h-4
                "jl cuerpoLineas1;"

        // ------------  LINEAS VERTICALES  ----------------:

        "mov $0, %%r11d;" // c = 0
        "jmp condicionLineas4;"
        "cuerpoLineas4:;"
                "mov %%r11d, %%edx;" // posCelda = c --> ESTO NO ES EDX O RDX
                "movb $1, %0;" // completa = cierto
                "movsx %%edx, %%rdx;"
                "xor %%r10, %%r10;"
                "mov (%%rsi, %%rdx), %%r10b;" // color = [dirContainer+posCelda] -> r10b -> COLOR ES UN TIPO CHAR (8 BITS)
                "movb $1, %1;" // mismoColor = cierto

                "mov $0, %%r8d;" // f = 0
                "jmp condicionLineas5;"
                "cuerpoLineas5:;"
                    "cmpb $0, (%%rsi, %%rdx);" //  [dirContainer+posCelda] == 0
                    "jne noEntraIF1Lineas5;"
                    "movb $0, %0;" // completa = falso
                    "jmp continuaIF1Lineas5;"

                    "noEntraIF1Lineas5:;"
                        "cmp %%r10b, (%%rsi, %%rdx);" //  [dirContainer+posCelda] != color
                        "je continuaIF1Lineas5;"
                        "movb $0, %1;" // mismoColor = falso

                    "continuaIF1Lineas5:;"
                        "movsx %%r9d, %%r9;" // Pasamos w a 64 bits
                        "add %%r9, %%rdx;" // posCelda = posCelda  + w -> rdx

                    "inc %%r8d;" // f++
                    "jmp condicionLineas5;"

        "seSaleFORLineas5:;"
                "cmpb $1, %0;" // si (completa)
                "jne continuaFORLineas4;"
                "cmpb $1, %1;" // si (mismoColor)
                "jne noEntraIFMismoColor2;"
                    "mov $2, %%r12b;" // marca = 2
                    "jmp continuarIFMismoColor2;"

                "noEntraIFMismoColor2:;"
                    "mov $1, %%r12b;" // marca = 1

                "continuarIFMismoColor2:;"
                    "movsx %%r11d, %%r11;" // Pasar c a 64 bits
                    "mov %%r11, %%rdx;" // posCelda = c
                    "mov $0, %%r8d;" // f = 0
                    "jmp condicionLineas6;"
                    "cuerpoLineas6:;"
                        "mov (%%rdi, %%rdx), %%r13b;" // r13b = [dirCeldasMarcadas+posCelda]
                        "mov %%r12b, %%r14b;" // Pasar marca a r14d para guardar su valor antes de modificarlo
                        "add %%r13b, %%r14b;" // [dirCeldasMarcadas+posCelda] = [dirCeldasMarcadas+posCelda]+ marca -> r14b
                        "mov %%r14b, (%%rdi, %%rdx);" // [dirCeldasMarcadas+posCelda] = [dirCeldasMarcadas+posCelda]+ marca
                        "movsx %%r9d, %%r9;" // Pasamos w a 64 bits
                        "add %%r9, %%rdx;" // posCelda = posCelda  + w -> rdx

                        "inc %%r8d;" // f++

                    "condicionLineas6:;"
                        "cmp %%ebx, %%r8d;" // f < h-4
                        "jl cuerpoLineas6;"
                        "jmp continuaFORLineas4;"

                "condicionLineas5:;"
                    "cmp %%ebx, %%r8d;" //f < h-4
                    "jge seSaleFORLineas5;"
                    "cmpb $1, %0;" // completa = true
                    "je cuerpoLineas5;"
                    "jmp seSaleFORLineas5;"

                "continuaFORLineas4:;"

                "inc %%r11d;" //c++
        "condicionLineas4:;"
                "cmp %%ecx, %%r11d;" // c<w-4
                "jl cuerpoLineas4;"

        : "=m" (completa), "=m" (mismoColor) // -> completa = %0  ;  mismoColor =  %1
        : "m" (container), "m" (w), "m" (h), "m" (celdasMarcadas)
        : "%rsi", "%rdi", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "memory"
                // container = %2, w = %3, h = %4, celdasMarcadas = %5

    );

}

int blockblast::borraCeldasMarcadas(char * container, int w, int h, char * celdasMarcadas)
{
    int puntuacion = 0;

    asm volatile(
        ";"

        //"movl $0, %0;" // puntuación = 0
        "mov %0,  %%r8d;" // puntuación -> r8d
        "mov %1, %%rsi;" // dirContainer = container
        "mov %4, %%rdi;" // dirCeldasMarcadas = celdasMarcadas

        "mov %3, %%r10d;" // Valor original de h -> r10d
        "mov %%r10d, %%eax;" // Pasar h a eax para no perder el valor original -> eax
        "sub $4, %%eax;" // h-4
        "mov %2, %%r11d;" // Valor original de w -> r11d
        "mov %%r11d, %%edx;" // Pasar w a edx para no perder el valor original -> edx
        "sub $4, %%edx;" // w-4

        "mov $0, %%r9d;" // f = 0 -> r9d
        "jmp condicionBorrarCeldas1;"
        "cuerpoBorrarCeldas1:;"
            "mov %%r11d, %%ebx;" // ebx = w -> Pasar el valor a otro registro para no perderlo
            "imul %%r9d, %%ebx;"  // posCelda = f*w -> ebx -> posCelda

            "mov $0, %%r12d;" // c = 0 -> r12d
            "jmp condicionBorrarCeldas2;"
            "cuerpoBorrarCeldas2:;"
                "movsx %%ebx, %%rbx;" // Pasar posCelda a 64 bits -> rbx
                "cmpb $0, (%%rdi, %%rbx);" //  [dirCeldasMarcadas  + posCelda] > 0
                "jle saleIFBorrarCeldas;"
                "movb $0, (%%rsi, %%rbx);" // [dirContainer+posCelda] = 0
                "addb (%%rdi, %%rbx), %%r8b;" // puntuación = [dirCeldasMarcadas+posCelda]+ puntuación -> r8b -> SOLO SE USA LA PARTE AJA POR LO QUE METERLO EN EL B

                "saleIFBorrarCeldas:;"
                    "inc %%ebx;" // posCelda++

                "inc %%r12d;" // c++
            "condicionBorrarCeldas2:;"
                "cmp %%edx, %%r12d;" // c < w-4
                "jl cuerpoBorrarCeldas2;"

            "inc %%r9d;" // f++
        "condicionBorrarCeldas1:;"
                "cmp %%eax, %%r9d;" // f < h-4
                "jl cuerpoBorrarCeldas1;"

                // RETURN:
                "movl %%r8d, %0;"

        : "=m" (puntuacion) // HAY QUE DEVOLVER LA NUEVA PUNTUACIÓN
        : "m" (container), "m" (w), "m" (h), "m" (celdasMarcadas)
        : "%rsi", "%rdi", "%rax", "%rbx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "memory"
          // puntuacion = %0, container = %1, w = %2, h = %3, celdasMarcadas = %4

    );

    return puntuacion;
}

// COMPROBAR SI SE PUEDE INSERTAR LA FIGURA EN CUALQUIER SITIO
bool blockblast::posibleJugada(char * container, int w, int h, char figure[4][4])
{
    bool posibleJ = true;
    bool hayHueco  = false;

    asm volatile(
        ";"

        "movb $0, %0;"  //  posibleJ = falso
        //"movb $0, %1;"  //  hayHueco = falso
        "mov %2, %%rsi;" // dirContainer = container -> rsi
        "mov %4, %%r9d;" //  h ->  r9d  ; Valor original de h
        "mov %3, %%r10d;" // w -> r10d  ; Valor original de w

        "mov %%r9d, %%eax;" // Pasar valor de h a eax para poder modificarlo
        "sub $4, %%eax;" // eax = h - 4
        "mov %%r10d, %%ebx;" // Pasar valor de w a ebx para poder modificarlo
        "sub $4, %%ebx;" // ebx = w - 4

        "mov $0, %%r8d;" //  f = 0 -> r8d
        "jmp condicionJugada1;"
        "cuerpoJugada1:;"

                "mov $0, %%r11d;" // c = 0 -> r11d
                "jmp condicionJugada2;"
                "cuerpoJugada2:;"
                    "movb $1, %1;" // hayHueco = true
                    "mov %%r8d, %%r12d;" // fAct = f -> fAct = r12d
                    "mov %5,  %%rcx;" // dirFigure = figure -> rcx

                    "mov $0, %%r13d;" // df = 0
                    "jmp condicionJugada3;"
                    "cuerpoJugada3:;"
                        // posCelda = r15d
                        "mov %%r10d, %%r14d;" // Para conservar el valor de w
                        "imul %%r12d, %%r14d;" // r14d = fAct * w
                        "mov %%r11d, %%r15d;" // Para conservar el valor de c
                        "add %%r14d, %%r15d;" // r15d = fAct * w + c -> posCelda

                        "mov $0, %%edx;" // dc = 0 -> edx
                        "jmp condicionJugada4;"
                        "cuerpoJugada4:;"
                            "cmpb $0, (%%rcx);" // [dirFigure] != 0
                            "je noEntraIF1Jugada;"
                            "movsx %%r15d, %%r15;" // Pasar posCelda a 64 bits
                            "cmpb $0, (%%rsi, %%r15);" // [dirContainer + posCelda] != 0
                            "je noEntraIF1Jugada;"
                            "movb $0, %1;" // hayHueco = false

                            "noEntraIF1Jugada:;"
                                "inc %%r15;" // posCelda ++
                                "inc %%rcx;" //  dirFigure ++

                            "inc %%edx;"// dc ++
                        "condicionJugada4:;"
                            "cmp $4, %%edx;" // dc < 4
                            "jge salirJugada4;"
                            "cmpb $1, %1;" // hayHueco = true
                            "je cuerpoJugada4;"

                            "salirJugada4:;"
                                "inc %%r12d;" // fAct ++

                        "inc %%r13d;" // df++
                    "condicionJugada3:;"
                        "cmp $4, %%r13d;" // df < 4
                        "jge salirJugada3;"
                        "cmpb $1, %1;" // hayHueco = true
                        "je cuerpoJugada3;"

                        "salirJugada3:;"

                    "cmpb $1, %1;" // si (hayHueco)
                    "jne continuarJugada2;"
                    "movb $1, %0;" // posibleJ = cierto

                "continuarJugada2:;"
                    "inc %%r11d;" // c++
                "condicionJugada2:;"
                    "cmp %%ebx, %%r11d;" // c < w-4
                    "jge salirJugada2;"
                    "cmpb $0, %0;" // no posibleJ
                    "je cuerpoJugada2;"

                "salirJugada2:;"
                "inc %%r8d;" // f++
        "condicionJugada1:;"
                "cmp %%eax, %%r8d;" // f < h-4
                "jge salirJugada1;"
                "cmpb $0, %0;" // no posibleJ
                "je cuerpoJugada1;"

                "salirJugada1:;"


        : "=m" (posibleJ), "=m" (hayHueco)
        : "m" (container), "m" (w), "m" (h), "m" (figure)
        : "%rsi", "%rdi", "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", "memory"
          // posibleJ = %0, hayHueco = %1, container = %2, w = %3, h = %4, figure = %5

    );

    return posibleJ;
}

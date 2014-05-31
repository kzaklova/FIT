/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id:$
 */

#ifndef Student_H
#define Student_H

/******************************************************************************
 * Includes
 */

#include "render.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Studentsky renderer, do ktereho muzete dopisovat svuj kod
 */

/* Jadro vaseho rendereru */
typedef struct S_StudentRenderer
{
    /* Kopie default rendereru
     * Typicky trik jak implementovat "dedicnost" znamou z C++
     * Ukazatel na strukturu lze totiz pretypovat na ukazatel
     * na prvni prvek struktury a se strukturou S_StudentRenderer
     * tak pracovat jako s S_Renderer... */
    S_Renderer  base;

    /* Zde uz muzete doplnovat svuj kod dle libosti */
    /* ??? */   

} S_StudentRenderer;


/******************************************************************************
 * Nasledujici fce budete nejspis muset re-implementovat podle sveho
 */

/* Funkce vytvori vas renderer a nainicializuje jej */
S_Renderer * studrenCreate();

/* Funkce vyhodnoti Phonguv osvetlovaci model pro zadany bod
 * a vraci barvu, kterou se bude kreslit do frame bufferu
 * point - souradnice bodu v prostoru
 * normal - normala k povrchu v danem bode */
S_RGBA studrenPhongReflectance(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal);

/* Nova fce pro rasterizace trojuhelniku do frame bufferu
 * vcetne interpolace z souradnice a prace se z-bufferem
 * a interpolaci normaly pro Phonguv osvetlovaci model
 * v1, v2, v3 - ukazatele na vrcholy trojuhelniku ve 3D pred projekci
 * n1, n2, n3 - ukazatele na normaly ve vrcholech ve 3D pred projekci
 * x1, y1, ... - vrcholy trojuhelniku po projekci do roviny obrazovky */
void studrenDrawTriangle(S_Renderer *pRenderer,
                         S_Coords *v1, S_Coords *v2, S_Coords *v3,
                         S_Coords *n1, S_Coords *n2, S_Coords *n3,
                         int x1, int y1,
                         int x2, int y2,
                         int x3, int y3
                         );

/* Vykresli i-ty trojuhelnik modelu
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku */
void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i);


#ifdef __cplusplus
}
#endif

#endif /* Student_H */

/*****************************************************************************/
/*****************************************************************************/

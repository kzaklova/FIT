/******************************************************************************
 * Projekt - Zaklady pocitacove grafiky - IZG
 * spanel@fit.vutbr.cz
 *
 * $Id: student.c 217 2012-02-28 17:10:06Z spanel $
 */

#include "student.h"
#include "transform.h"

#include <memory.h>


/*****************************************************************************
 * Funkce vytvori vas renderer a nainicializuje jej */

S_Renderer * studrenCreate()
{
    S_StudentRenderer * renderer = (S_StudentRenderer *)malloc(sizeof(S_StudentRenderer));
    IZG_CHECK(renderer, "Cannot allocate enough memory");

    /* inicializace default rendereru */
    renInit(&renderer->base);

    /* nastaveni ukazatelu na vase upravene funkce */
    renderer->base.projectTriangleFunc = studrenProjectTriangle;

    return (S_Renderer *)renderer;
}

/****************************************************************************
 * Funkce vyhodnoti Phonguv osvetlovaci model pro zadany bod
 * a vraci barvu, kterou se bude kreslit do frame bufferu
 * point - souradnice bodu v prostoru
 * normal - normala k povrchu v danem bode */

S_RGBA studrenPhongReflectance(S_Renderer *pRenderer, const S_Coords *point, const S_Coords *normal)
{
    S_Coords    lvec, cvec, rvec;
    double      r, g, b, lscal, cscal;
    S_RGBA      color;

    IZG_ASSERT(pRenderer && point && normal);

    /* vektor ke zdroji svetla */
    lvec = makeCoords(pRenderer->light_position.x - point->x,
                      pRenderer->light_position.y - point->y,
                      pRenderer->light_position.z - point->z);

    coordsNormalize(&lvec);

    /* ambientni cast */
    r = pRenderer->light_ambient.red * pRenderer->mat_ambient.red;
    g = pRenderer->light_ambient.green * pRenderer->mat_ambient.green;
    b = pRenderer->light_ambient.blue * pRenderer->mat_ambient.blue;


    lscal = lvec.x * normal->x + lvec.y * normal->y + lvec.z * normal->z; // intenzita dopadajiciho svetla
    /* difuzni cast */
    if( lscal > 0 )
    {
        r += lscal * pRenderer->light_diffuse.red * pRenderer->mat_diffuse.red;
        g += lscal * pRenderer->light_diffuse.green * pRenderer->mat_diffuse.green;
        b += lscal * pRenderer->light_diffuse.blue * pRenderer->mat_diffuse.blue;
    }

    cvec = makeCoords(0,0,-1); // vektor kamery
    rvec = makeCoords((2*lscal*normal->x)-lvec.x, (2*lscal*normal->y)-lvec.y, (2*lscal*normal->z)-lvec.z);
    cscal = cvec.x*rvec.x + cvec.y*rvec.y + cvec.z*rvec.z; // svetlo, co jde do kamery
    /* spekularni cast */
    if( cscal > 0 )
    {
        r += cscal * pRenderer->light_specular.red * pRenderer->mat_specular.red;
        g += cscal * pRenderer->light_specular.green * pRenderer->mat_specular.green;
        b += cscal * pRenderer->light_specular.blue * pRenderer->mat_specular.blue;
    }
    
    /* saturace osvetleni*/
    r = MIN(1, r);
    g = MIN(1, g);
    b = MIN(1, b);

    /* kreslici barva */
    color.red = ROUND2BYTE(255 * r);
    color.green = ROUND2BYTE(255 * g);
    color.blue = ROUND2BYTE(255 * b);
    return color;
}

/****************************************************************************
 * Nova fce pro rasterizace trojuhelniku do frame bufferu
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
                         )
{
    int xmax, xmin, ymax, ymin;
    S_Coords point, norm;
    int x, y, bod1, bod2, bod3;
    float p1, p2, p3;

    xmax = MAX(x1, x2);
    xmax = MAX(xmax, x3);
    xmax = MIN(xmax, pRenderer->frame_w-1);

    ymax = MAX(y1, y2);
    ymax = MAX(ymax, y3);
    ymax = MIN(ymax, pRenderer->frame_h-1);

    xmin = MIN(x1, x2);
    xmin = MIN(xmin, x3);
    xmin = MAX(xmin, 0);

    ymin = MIN(y1, y2);
    ymin = MIN(ymin, y3);
    ymin = MAX(ymin, 0);

    for(x = xmin; x <= xmax; x++) {
        for(y = ymin; y <= ymax; y++) {
            bod1 = (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);
            if (((y2 - y1) == 0 && (x2 - x1) > 0) || (y2 - y1) < 0)
                bod1 += 1;
            bod2 = (x - x2) * (y3 - y2) - (y - y2) * (x3 - x2);
            if (((y3 - y2) == 0 && (x3 - x2) > 0) || (y3 - y2) < 0)
                bod2 += 1;
            bod3 = (x - x3) * (y1 - y3) - (y - y3) * (x1 - x3);
            if (((y1 - y3) == 0 && (x1 - x3) > 0) || (y1 - y3) < 0)
                bod3 += 1;

            if (bod1 > 0 && bod2 > 0 && bod3 > 0) {
                p1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / (float)((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3)); 
                p2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / (float)((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3)); 
                p3 = 1.0 - p1 - p2;

                point = makeCoords(v1->x * p1 + v2->x * p2 + v3->x * p3, v1->y * p1 + v2->y * p2 + v3->y * p3, v1->z * p1 + v2->z * p2 + v3->z * p3);
                norm = makeCoords(n1->x * p1 + n2->x * p2 + n3->x * p3, n1->y * p1 + n2->y * p2 + n3->y * p3, n1->z * p1 + n2->z * p2 + n3->z * p3);

                if (point.z < DEPTH(pRenderer, x, y)) {
                    DEPTH(pRenderer, x, y) = point.z;
                    PIXEL(pRenderer, x, y) = studrenPhongReflectance(pRenderer, &point, &norm);
                }
            }
        }
    }
}

/*****************************************************************************
 * Vykresli i-ty trojuhelnik modelu
 * Pred vykreslenim aplikuje na vrcholy a normaly trojuhelniku
 * aktualne nastavene transformacni matice!
 * i - index trojuhelniku */

void studrenProjectTriangle(S_Renderer *pRenderer, S_Model *pModel, int i)
{
    S_Coords    aa, bb, cc;             /* souradnice vrcholu po transformaci ve 3D pred projekci */
    S_Coords    nn;                     /* normala trojuhelniku po transformaci */
    S_Coords    na, nb, nc;             /* souradnice vrcholu trojuhelnika */
    int         u1, v1, u2, v2, u3, v3; /* souradnice vrcholu po projekci do roviny obrazovky */
    S_Triangle  * triangle;

    IZG_ASSERT(pRenderer && pModel && i >= 0 && i < trivecSize(pModel->triangles));

    /* z modelu si vytahneme trojuhelnik */
    triangle = trivecGetPtr(pModel->triangles, i);

    /* transformace vrcholu matici model */
    trTransformVertex(&aa, cvecGetPtr(pModel->vertices, triangle->v[0]));
    trTransformVertex(&bb, cvecGetPtr(pModel->vertices, triangle->v[1]));
    trTransformVertex(&cc, cvecGetPtr(pModel->vertices, triangle->v[2]));

    /* promitneme vrcholy trojuhelniku na obrazovku */
    trProjectVertex(&u1, &v1, &aa);
    trProjectVertex(&u2, &v2, &bb);
    trProjectVertex(&u3, &v3, &cc);

    /* transformace normaly trojuhelniku matici model */
    trTransformVector(&nn, cvecGetPtr(pModel->trinormals, triangle->n));
    trTransformVector(&na, cvecGetPtr(pModel->normals, triangle->v[0]));
    trTransformVector(&nb, cvecGetPtr(pModel->normals, triangle->v[1]));
    trTransformVector(&nc, cvecGetPtr(pModel->normals, triangle->v[2]));
    
    /* normalizace normaly */
    coordsNormalize(&nn);
    coordsNormalize(&na);
    coordsNormalize(&nb);
    coordsNormalize(&nc);

    /* je troj. privraceny ke kamere, tudiz viditelny? */
    if( !renCalcVisibility(pRenderer, &aa, &nn) )
    {
        /* odvracene troj. vubec nekreslime */
        return;
    }

    /* rasterizace celeho trojuhelniku */
    studrenDrawTriangle(pRenderer,
                    &aa, &bb, &cc,
                    &na, &nb, &nc,
            u1, v1, u2, v2, u3, v3);
}

/*****************************************************************************
 *****************************************************************************/

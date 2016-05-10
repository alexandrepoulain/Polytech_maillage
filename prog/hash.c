#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // you need to add -lm for link edition
#include "mesh.h"
#include "hash.h"
#include "distanceMeshFunctions.h"

#define KA 7
#define KB 11
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

unsigned char idir[5]     = {0,1,2,0,1};

int hashHedge(pMesh mesh, Hedge *tab)
{
  pHedge ph;
  pTria  pt;
  int i,j,k,i1,i2,min,max;
  
  int hnxt = mesh->np + 1;
  int hsize = mesh->np;
  int key;
  int adj,control=2;
  
  
  for(i=1; i<=mesh->nt;i++)
  {
    pt = &mesh->tria[i];
    if ( !pt->v[0] )  continue;
    
    for(j=0;j<3;j++)
    {       i1 = idir[j+1];
      i2 = idir[j+2];
      min = MIN(pt->v[i1], pt->v[i2]);
      max = MAX(pt->v[i1], pt->v[i2]);
      /* compute key */
      key = (KA*min +KB*max)%hsize + 1;
      /* insert */
      adj = 3*i+j;
      
      
      if (tab[key].adj1 == 0) // si tab[key] est visité pour la premiere fois
      {
        tab[key].ia = min;
        tab[key].ib = max;
        tab[key].adj1 = adj;
      }
      
      else { // tab[key].adj1 a déjà été rempli
        control =2 ;
        do {
          if ( (tab[key].ia == min ) && (tab[key].ib == max)  )
          { tab[key].adj2 = adj; control =1 ;}
          
          key = tab[key].nxt;
        }
        while(( tab[key].nxt) && ( control==2 ) );
        
        
        if(control==2) {
          tab[hnxt].ia = min;
          tab[hnxt].ib = max;
          tab[hnxt].adj1 = adj;
          tab[key].nxt = hnxt;
          hnxt++;
        }
      }
    }
  }
  
  
//  /* test debugging */
//  /* On cherche les trois voisins du triangle 9629 */
//  pt = &mesh->tria[9629];
//  for(j=0;j<3;j++)
//  {  i1 = idir[j+1];
//    i2 = idir[j+2];
//    fprintf(stdout,"  -- j %d  i1 %d  i2 %d \n",j,i1,i2);
//    min = MIN(pt->v[i1], pt->v[i2]);
//    max = MAX(pt->v[i1], pt->v[i2]);
//    /* compute key */
//    key = (KA*min +KB*max)%hsize + 1;
//    /* insert */
//   
//     
//    do {
//      if ( (tab[key].ia == min) &&  (tab[key].ib == max) )
//
//    fprintf(stdout,"  -- ad1 %d  adj2 %d  \n",tab[key].adj1/3,tab[key].adj2/3);
//
//       key = tab[key].nxt;
//        
//    }
//    
//    while(tab[key].nxt);
//  
//  }
  
  
  return 0;
  
}


// Calculate neighbour triangles of a given triangle (only at edges -> max 3 triangles)
int setAdj(pMesh mesh, Hedge * tab)
{
  pTria  pt;
  Hedge  *ph;
	int i,j,i1,i2,min,max,adj;
	int key,control;
	int hsize = mesh->np; // gives first indice disponible in the tableau

	// 3 arrêt per triangle
	mesh->adja = (int *)calloc(3*mesh->nt+1,sizeof(int));

	for(i=1; i<=mesh->nt; i++)
	{
    pt = &mesh->tria[i];
    if ( !pt->v[0] )  continue;
    
		for(j=0; j<3; j++)
    {
      i1 = idir[j+1];
      i2 = idir[j+2];
      min = MIN(pt->v[i1], pt->v[i2]);
      max = MAX(pt->v[i1], pt->v[i2]);
			key = (KA*min+KB*max)%hsize+1;
			
      // On parcourt la chaine de tab jusqu'à ce que l'objet correspondant à l'arrete soit trouvé
      do{
       if ( (tab[key].ia == min) &&  (tab[key].ib == max) )
        { adj = 3*i+j;
          if( (tab[key].adj2) == adj)  mesh->adja[3*(i-1)+1+j] = tab[key].adj1;
          else   mesh->adja[3*(i-1)+1+j] = tab[key].adj2;
          break;
        }
        key = tab[key].nxt;
      }
      while(tab[key].nxt);
    }
  }
  
  /* Test debugging: on cherche les trois voisins du triangle 34 */
  int iadr,iel, *adja;
  iadr = (34-1)*3 + 1;
  adja = &mesh->adja[iadr];
  
  iel = (adja[0]) / 3 ;
  printf("Tria = %d\n", iel);
  iel = (adja[1]) / 3 ;
  printf("Tria = %d\n", iel);
  iel = (adja[2] ) / 3  ;
  printf("Tria = %d\n", iel);
  
  return 0;
}


int localiseTriangleBruteForce(pMesh mesh, pPoint point){
	int i,j;
	for(i=1; i<mesh->nt; i++){
		for(j=0; j<3; j++){
			if (point->c[0] == mesh->point[mesh->tria[i].v[j]].c[0])
			{
				return i;
			}
		}
	}
	return 0;
}

// calculates coordinates barycentric u, v, 1-u-v
void baryCoord(pMesh mesh, int triangle, pPoint p, double cb[3])
{
    Point a = mesh->point[ mesh->tria[triangle].v[0] ];
    Point b = mesh->point[ mesh->tria[triangle].v[1] ];
    Point c = mesh->point[ mesh->tria[triangle].v[2] ];

    Point v0, v1, v2;

    v0.c[0] = b.c[0] - a.c[0];
    v0.c[1] = b.c[1] - a.c[1];
    v0.c[2] = b.c[2] - a.c[2];

    v1.c[0] = c.c[0] - a.c[0];
    v1.c[1] = c.c[1] - a.c[1];
    v1.c[2] = c.c[2] - a.c[2];

    v2.c[0] = p->c[0] - a.c[0];
    v2.c[1] = p->c[1] - a.c[1];
    v2.c[2] = p->c[2] - a.c[2];

    double d00, d01, d11, d20, d21, denom;

    d00 = dotProduct3D(v0, v0);
    d01 = dotProduct3D(v0, v1);
    d11 = dotProduct3D(v1, v1);
    d20 = dotProduct3D(v2, v0);
    d21 = dotProduct3D(v2, v1);
    denom = d00 * d11 - d01 * d01;

    cb[0] = (d11 * d20 - d01 * d21) / denom; // u
    cb[1] = (d00 * d21 - d01 * d20) / denom; // v
    cb[2] = (1. - cb[0] - cb[1]); // 1 - u - v

}

// find triangle who includes the point p
int locelt(pMesh mesh, int startTriangle, pPoint p, double cb[3])
{
	// distPointToTriangle(pMesh mesh, pTria tria, pPoint P0)
	int triaRef = startTriangle;
	pTria t = &mesh->tria[triaRef];
	printf("Tria = %d\n", t->v[0]);
	double distOld = 1e20;
	double distCurrent;
	int refClosest = 0;
	int i, it = 0;

	while(1)
	{
		baryCoord(mesh, triaRef, p, cb);
		if (cb[0] >= 0 && cb[1] >= 0 && cb[2] >= 0)
		{
			return triaRef;
		} else {
			for(i=0; i<3; i++)
			{

				t = &mesh->tria[mesh->adja[3*(triaRef-1) + 1 + i]];
				assert(mesh->adja[3*(triaRef-1) + 1 + i] <= 3*mesh->nt);
				printf("Triangle numero %d\n", mesh->adja[3*(triaRef-1) + 1 + i]);
				distCurrent = distPointToTriangle(mesh, t, p);
				
				if (distCurrent < distOld)
				{
					distOld = distCurrent;
					triaRef = mesh->adja[3*(triaRef-1) + 1 + i];
					break;
				}
				if (i == 2)
				{
					return -2; 
					// c'est à dire que parmis les voisins d'un triangle, il n'a pas trouvé un voisin qui est plus proche
					// donc s'il continue il va s'éloigner du traingle à chercher
				}
			}
		}
		it++;
		if (it > 1000)
		{
			return -1;
		}
	}
}









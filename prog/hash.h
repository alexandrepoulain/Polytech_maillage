#ifndef HASH_H
#define HASH_H

/*
Hedge struct doc:
na et nb les extrémités de l'arête

ia = min(na,nb)
ib = max(na,nb)

Ces objects Hedge sont rangés dans un tableau global

key = (KA*min(na,nb)+KB*max(na,nb))%hsize
KA = 7, KB = 11, hsize = mesh->np
*/

typedef struct {
	int ia, ib, adj1, adj2, nxt;
} Hedge;
typedef Hedge * pHedge;

int hashHedge(pMesh mesh, Hedge * tab);

int setAdj(pMesh mesh, Hedge * tab);

int localiseTriangleBruteForce(pMesh mesh, pPoint point);

void baryCoord(pMesh mesh, int triangle, pPoint p, double cb[3]);

int locelt(pMesh mesh, int startTriangle, pPoint p, double cb[3]);


#endif

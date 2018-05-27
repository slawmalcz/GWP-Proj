#pragma once

namespace Core
{
	// vertexArray - jednowymiarowa tablica zawierajaca wartosci opisujace pozycje kolejnych wierzcholkow w jednym ciagu (x1, y1, z1, w1, x2, y2, z2, w2, ...)
	// numVertices - liczba wierzcholkow do narysowania
	// elementSize - liczba wartosci opisujacych pojedynczy wierzcholek (np. 3 gdy wierzcholek opisany jest trojka (x, y, z))
	void DrawVertexArray(const float * vertexArray, int numVertices, int elementSize);

	// indexArray - jednowymiarowa tablica zawierajaca indeksy wierzcholkow kolejnych trojkatow w jednym ciagu (t1_i1, t1_i2, t1_i3, t2_i1, t2_i2, t2_i3, ...)
	// numIndexes - liczba indeksow w tablicy indexArray
	void DrawVertexArrayIndexed(const float * vertexArray, const int * indexArray, int numIndexes, int elementSize);
}
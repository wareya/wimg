//#include "dct.c"
#include "dct.c"
#include <stdio.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

uint8_t fix(int16_t in)
{
    if(in < 0)
        return 0;
    if(in > 255)
        return 255;
    return in;
}

int main()
{
    FILE * bmp = fopen("testimage.bmp", "r+b");
    fseek(bmp, 0xA, SEEK_SET);
    int where;
    fread(&where, 4, 1, bmp);
    
    rgb_t rgb_data[8][8];
    fseek(bmp, where, SEEK_SET);
    fread(rgb_data, 3, 8*8, bmp);
    
    int16_t Y[8][8]; // 0~1020
    int16_t G[8][8]; // 0~1020
    int16_t O[8][8]; // 0~1020
    
    for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
    {
        Y[x][y] = rgb_data[x][y].r
                + rgb_data[x][y].g* 2
                + rgb_data[x][y].b;
        G[x][y] = rgb_data[x][y].r*-1
                + rgb_data[x][y].g* 2
                + rgb_data[x][y].b*-1
                +510;
        O[x][y] = rgb_data[x][y].r* 2
                + rgb_data[x][y].g* 0
                + rgb_data[x][y].b*-2
                +510;
    }
    
    double L[8][8];
    double C[8][8];
    double M[8][8];
    
    dct(Y, L);
    dct(G, C);
    dct(O, M);
    
    quantize(L);
    quantize(C);
    quantize(M);
    
    idct(L, Y);
    idct(C, G);
    idct(M, O);
    
    for(int x = 0; x < 8; x++)
    for(int y = 0; y < 8; y++)
    {
        int16_t temp = Y[x][y] - (G[x][y]-510);
        rgb_data[x][y].r = fix((temp + (O[x][y]-510))/4);
        rgb_data[x][y].g = fix((Y[x][y] + (G[x][y]-510))/4);
        rgb_data[x][y].b = fix((temp - (O[x][y]-510))/4);
        //rgb_data[x][y].r = Y[x][y]/4;
        //rgb_data[x][y].g = G[x][y]/4;
        //rgb_data[x][y].b = O[x][y]/4;
        // uncomment above for debugging raw YGO values
    }
    
    fseek(bmp, where, SEEK_SET);
    fwrite(rgb_data, 3, 8*8, bmp);
    fflush(bmp);
    fclose(bmp);
}
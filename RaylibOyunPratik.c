#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include <time.h>

#define SATIRLAR 25
#define SUTUNLAR 20
#define DOKU_SAYISI 10
#define KARE_BOYUTU 40
#define OYUNCU_BOYUTU 35 

#define EKRAN_GENISLIK (SUTUNLAR * KARE_BOYUTU)
#define EKRAN_YUKSEKLIK (SATIRLAR * KARE_BOYUTU)

#define MAX_DUSEN_KARE 25
#define MAX_SKOR_SAYISI 5

#define OYUNCU_HIZI 5.0f
#define YER_CEKIMI 0.8f
#define ZIPLAMA -16.0f

#define KAYIT_DOSYASI "rekor.dat"

typedef struct Oyuncu
{
    Rectangle alan;
    bool yerdemi;
    float dikeyhiz;
    int dokuId;
} Oyuncu;

typedef struct DusenBlok
{
    int sutun;
    float y;
    bool aktif;
    int dokuId;
} DusenBlok;

typedef struct Oyun
{
    int skor;
    int enyuksekskor[MAX_SKOR_SAYISI];
    int zorluksayaci;
    bool oyunbitti;
    float dusmehizi;
    int olusturmasayaci;
    Music arkaplanmuzik;
    Sound olumsesi;
    Sound ziplamasesi;
    Sound taskirilmasesi;
} Oyun;

Oyun oyunverileri;
bool izgara[SATIRLAR][SUTUNLAR];		//kareli sistemde 1/0 kontrolü
int izgaraDokuId[SATIRLAR][SUTUNLAR];	//kareli sistemde doku kontrolü

Oyuncu oyuncu;
DusenBlok dusenbloklar[MAX_DUSEN_KARE];

Texture2D arkaplanDokusu;				//
Texture2D oyuncuDokusu;					//doku yapılarının tanımlanması
Texture2D blokDokulari[DOKU_SAYISI];	//


void RekorYukle(Oyun *oyun)
{
    FILE *dosya = fopen(KAYIT_DOSYASI, "rb");
    if(dosya != NULL)							
    {
        fread(oyun->enyuksekskor, sizeof(int), MAX_SKOR_SAYISI, dosya);	//dosyanın okunması
        fclose(dosya);
    }
    else
    {
        for(int i=0; i<MAX_SKOR_SAYISI; i++) oyun->enyuksekskor[i] = 0;	//en yüksek 5 skorlara 0 yazar
    }
}

void RekorKaydet(Oyun *oyun)
{
    FILE *dosya = fopen(KAYIT_DOSYASI, "wb");
    if(dosya != NULL)
    {
        fwrite(oyun->enyuksekskor, sizeof(int), MAX_SKOR_SAYISI, dosya); //en yüksek skoru yazar
        fclose(dosya);
    }
}

void SkorlariSirala(Oyun *oyun)						//bubble sort yöntemi ile en yüksek 5 skorun sıralanması
{
    for(int i=0; i < MAX_SKOR_SAYISI - 1; i++)
    {
        for(int j=0; j < MAX_SKOR_SAYISI - i - 1; j++)
        {
            if(oyun->enyuksekskor[j] < oyun->enyuksekskor[j+1])
            {
                int temp = oyun->enyuksekskor[j];
                oyun->enyuksekskor[j] = oyun->enyuksekskor[j+1];
                oyun->enyuksekskor[j+1] = temp;
            }
        }    
    }
}

void BloklariOlustur()
{
    int blokadeti = GetRandomValue(1,4); //rastgele oluşacak blok sayısı belirlenir
    for(int k=0; k < blokadeti; k++)
    {
        for(int i=0; i < MAX_DUSEN_KARE; i++)
        {
            if(!dusenbloklar[i].aktif)									//bloğu pencerenin dışında oluşturuyor
            {
                dusenbloklar[i].aktif = true;
                dusenbloklar[i].sutun = GetRandomValue(0, SUTUNLAR - 1);
                dusenbloklar[i].y = -KARE_BOYUTU;
                dusenbloklar[i].dokuId = GetRandomValue(0, DOKU_SAYISI - 1);
                break;
            }
        }
    }
}

void SatirlariGuncelle(Oyun *oyun)
{
    for(int y=SATIRLAR - 1; y >= 0; y--)
    {
        bool satirdolu = true;
        for(int x=0; x < SUTUNLAR; x++)			
        {
            if(!izgara[y][x])					//ızgara boşsa satirdolu değil
            {
                satirdolu = false;
                break;
            }
        }
        
        if(satirdolu)						//satır doluysa dolu satırı siler
        {
            oyun->skor += 30;
            PlaySound(oyun->taskirilmasesi);
            
            for(int satir=y; satir > 0; satir--)
            {
                for(int sutun=0; sutun < SUTUNLAR; sutun++) 		
                {													//bir üst satır aşağı kaydırılır
                    izgara[satir][sutun] = izgara[satir-1][sutun];
                    izgaraDokuId[satir][sutun] = izgaraDokuId[satir-1][sutun];
                }
            }
            
            for (int sutun = 0; sutun < SUTUNLAR; sutun++)
            {														// en üst satırı siler
                izgara[0][sutun] = false;
                izgaraDokuId[0][sutun] = 0;
            }
            y++; 
        }        
    }
}

void OyuncuFizigiGuncelle(Oyuncu *oyuncu, Oyun *oyun)
{
    if(IsKeyDown(KEY_RIGHT))										
    {
        oyuncu->alan.x += OYUNCU_HIZI;									// sağ ok tuşuna basıldığında karakter sağa gider
        if(oyuncu->alan.x + oyuncu->alan.width > EKRAN_GENISLIK) oyuncu->alan.x = EKRAN_GENISLIK - oyuncu->alan.width;	//karakter ekranın dışına çıkması engellenir
        
        int izgarax = (int)((oyuncu->alan.x + oyuncu->alan.width) / KARE_BOYUTU);
        int izgaray1 = (int)(oyuncu->alan.y / KARE_BOYUTU);
        int izgaray2 = (int)((oyuncu->alan.y + oyuncu->alan.height -1) / KARE_BOYUTU);
        
        if(izgarax < SUTUNLAR && (izgara[izgaray1][izgarax] || izgara[izgaray2][izgarax]))		// karakter blokların içine girmesi engellenir
        {
            oyuncu->alan.x = (izgarax * KARE_BOYUTU) - oyuncu->alan.width;
        }
    }
    
    if(IsKeyDown(KEY_LEFT))											
    {																	
        oyuncu->alan.x -= OYUNCU_HIZI;								// sol ok tuşuna basıldığında karakter sağa gider
        if(oyuncu->alan.x < 0) oyuncu->alan.x = 0;					//karakter ekranın dışına çıkması engellenir
        
        int izgarax = (int)(oyuncu->alan.x / KARE_BOYUTU);
        int izgaray1 = (int)(oyuncu->alan.y / KARE_BOYUTU);
        int izgaray2 = (int)((oyuncu->alan.y + oyuncu->alan.height - 1) / KARE_BOYUTU);
        
        if(izgarax >= 0 && (izgara[izgaray1][izgarax] || izgara[izgaray2][izgarax]))	// karakter blokların içine girmesi engellenir
        {
            oyuncu->alan.x = (izgarax + 1) * KARE_BOYUTU;
        }
    }    
    
    oyuncu->dikeyhiz += YER_CEKIMI;					// karakteri yerçekimi uygulanır
    oyuncu->alan.y += oyuncu->dikeyhiz;					
    oyuncu->yerdemi = false;
    
    if(oyuncu->alan.y + oyuncu->alan.height >= EKRAN_YUKSEKLIK) //karakterin ekranın altına düşmesi engellenir
    {
        oyuncu->alan.y = EKRAN_YUKSEKLIK - oyuncu->alan.height;	
        oyuncu->dikeyhiz = 0;
        oyuncu->yerdemi = true;        
    }
    
    int kareninsolu = (int)(oyuncu->alan.x / KARE_BOYUTU);
    int kareninsagi = (int)((oyuncu->alan.x + oyuncu->alan.width - 1) / KARE_BOYUTU);
    int kareninalti = (int)((oyuncu->alan.y + oyuncu->alan.height) / KARE_BOYUTU);
    
    if(kareninalti < SATIRLAR && (izgara[kareninalti][kareninsagi] || izgara[kareninalti][kareninsolu])) //karakterin blokların üstünde durmasını sağlar
    {
        float cakisma = (oyuncu->alan.y + oyuncu->alan.height) - kareninalti * KARE_BOYUTU;
        if(cakisma < 15 && oyuncu->dikeyhiz > 0)
        {
            oyuncu->alan.y = kareninalti * KARE_BOYUTU - oyuncu->alan.height;
            oyuncu->yerdemi = true;
            oyuncu->dikeyhiz = 0;
        }
    }
    
    if(IsKeyPressed(KEY_UP) && oyuncu->yerdemi)					//oyuncuyu zıplaması sağlanır
    {
        PlaySound(oyun->ziplamasesi);							//zıplama sesi çalınır
        oyuncu->dikeyhiz = ZIPLAMA;
    }
}

void OyunuSifirla(Oyun *oyun, Oyuncu *oyuncu)
{    
    for(int y=SATIRLAR-1; y >= 0; y--)
    {
        for(int x=0; x < SUTUNLAR; x++)
        {
            izgara[y][x] = false;				//tüm ızgara sistemi sıfırlanır
            izgaraDokuId[y][x] = 0;
        }
    }
    
    oyuncu->alan = (Rectangle){ EKRAN_GENISLIK / 2 - 15, EKRAN_YUKSEKLIK - 60, OYUNCU_BOYUTU, OYUNCU_BOYUTU };	//oyuncunun konumu sıfırlanır
    oyuncu->dikeyhiz = 0;
    oyuncu->yerdemi = true;
    
    for(int i=0; i < MAX_DUSEN_KARE; i++)	//düşen bloklar sıfırlanır
    {
        dusenbloklar[i].aktif = false;
    }
    oyun->skor = 0;							//
    oyun->oyunbitti = false;				//
    oyun->dusmehizi = 5.0f;					//oyun verileri sıfırlanır
    oyun->olusturmasayaci = 0;    			//
    oyun->zorluksayaci = 0;					//
}

void OyunuGuncelle(Oyun *oyun, Oyuncu *oyuncu)
{
    if(!oyun->oyunbitti)
    {
    	oyun->zorluksayaci++;
        if(oyun->zorluksayaci > 60)
        {								// her 60 karede düşen blokların hızı arttırılır
            oyun->dusmehizi += 0.1f;
            oyun->zorluksayaci = 0;
        }
        oyun->olusturmasayaci++;
        int mevcutolusturmasayaci = 45 - (int)(oyun->dusmehizi * 2);	
        if(mevcutolusturmasayaci < 10) mevcutolusturmasayaci = 10;
																		        
        if(oyun->olusturmasayaci > mevcutolusturmasayaci)			//oluşacak blokları sınırlar
        {
            BloklariOlustur();
            oyun->olusturmasayaci = 0;
        }
        
        OyuncuFizigiGuncelle(oyuncu, oyun);
        
        for(int i=0; i < MAX_DUSEN_KARE; i++)
        {
            if(!dusenbloklar[i].aktif) continue;    
            dusenbloklar[i].y += oyun->dusmehizi;			//düşen blokları düşme hızı kadar arttır
            
            int izgaray = (int) ((dusenbloklar[i].y + KARE_BOYUTU) / KARE_BOYUTU);		//bloğun yatay konumunu ızgara sistemine dönüştürür
            int izgarax = dusenbloklar[i].sutun;
            bool yereoturdumu = false;
            
            if(izgaray >= SATIRLAR)						//blok ekranın altına değiyorsa sabitler
            {
                izgaray = SATIRLAR - 1;
                yereoturdumu = true;
            }
            else if(izgaray >= 0 && izgara[izgaray][izgarax])	//blok başka bir bloğun üstüne değiyorsa sabitler
            {
                izgaray = izgaray - 1;
                yereoturdumu = true;
            }
            
            	Rectangle blokalani = {
                izgarax * KARE_BOYUTU,
                dusenbloklar[i].y,
                KARE_BOYUTU,
                KARE_BOYUTU
            };
            
            if(CheckCollisionRecs(oyuncu->alan, blokalani))		//oyuncunun düşen bloğa çarpması kontrol edilir
            {
                PlaySound(oyun->olumsesi);
                
                oyun->oyunbitti = true;										
                if(oyun->skor > oyun->enyuksekskor[MAX_SKOR_SAYISI - 1])
                {															//skor en yüksek 5 skor içine giriyorsa kaydeder
                    oyun->enyuksekskor[MAX_SKOR_SAYISI - 1] = oyun->skor;	
                    SkorlariSirala(oyun);
                    RekorKaydet(oyun);

                }
            }
            
            if(yereoturdumu)
            {									//bloklar en üste ulaşırsa oyun biter
                if(izgaray < 0)
                {    
                    PlaySound(oyun->olumsesi);
                    
                    oyun->oyunbitti = true;
                    if(oyun->skor > oyun->enyuksekskor[MAX_SKOR_SAYISI - 1])
                    {
                        oyun->enyuksekskor[MAX_SKOR_SAYISI - 1] = oyun->skor;
                        SkorlariSirala(oyun);
                        RekorKaydet(oyun);
                    }
                }
                else
                {
                    izgara[izgaray][izgarax] = true;
                    izgaraDokuId[izgaray][izgarax] = dusenbloklar[i].dokuId;
                    dusenbloklar[i].aktif = false;
                    oyun->skor++;
                    SatirlariGuncelle(oyun);
                }
            }
        }
    }
    else
    {
        if(IsKeyPressed(KEY_ENTER)) OyunuSifirla(oyun, oyuncu);				// oyunu sıfırlar
    }
}

void OyunuCiz(Oyun *oyun, Oyuncu *oyuncu)
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        if(!oyun->oyunbitti)
        {
            Rectangle kaynakBG = { 0.0f, 0.0f, (float)arkaplanDokusu.width, (float)arkaplanDokusu.height };
            Rectangle hedefBG = { 0.0f, 0.0f, (float)EKRAN_GENISLIK, (float)EKRAN_YUKSEKLIK };
            Vector2 originBG = { 0, 0 };
            DrawTexturePro(arkaplanDokusu, kaynakBG, hedefBG, originBG, 0.0f, WHITE);							//arkaplanı çizer

            for(int y=0; y < SATIRLAR; y++)
            {												// yerde duran blokların çizer
                for(int x=0; x < SUTUNLAR; x++)
                {
                    if(izgara[y][x])
                    {
                        int tur = izgaraDokuId[y][x];
                        Rectangle kaynak = { 0.0f, 0.0f, (float)blokDokulari[tur].width, (float)blokDokulari[tur].height };
                        Rectangle hedef = { x * KARE_BOYUTU, y * KARE_BOYUTU, KARE_BOYUTU, KARE_BOYUTU };
                        Vector2 origin = { 0, 0 };
                        DrawTexturePro(blokDokulari[tur], kaynak, hedef, origin, 0.0f, WHITE);
                    }
                }
            }
            
            for(int i=0; i < MAX_DUSEN_KARE; i++)
            {												//havadaki blokları çizer
                if(dusenbloklar[i].aktif)
                {
                    int tur = dusenbloklar[i].dokuId;
                    Rectangle kaynak = { 0.0f, 0.0f, (float)blokDokulari[tur].width, (float)blokDokulari[tur].height };
                    Rectangle hedef = { dusenbloklar[i].sutun * KARE_BOYUTU, dusenbloklar[i].y, KARE_BOYUTU, KARE_BOYUTU };
                    Vector2 origin = { 0, 0 };
                    DrawTexturePro(blokDokulari[tur], kaynak, hedef, origin, 0.0f, WHITE);
                }
            }

            Rectangle kaynakOyuncu = { 0.0f, 0.0f, (float)oyuncuDokusu.width, (float)oyuncuDokusu.height }; //oyuncuyu çizer
            Rectangle hedefOyuncu = oyuncu->alan;
            Vector2 origin = { 0.0f, 0.0f };
            DrawTexturePro(oyuncuDokusu, kaynakOyuncu, hedefOyuncu, origin, 0.0f, WHITE);
            
            DrawText(TextFormat("Skor : %04i", oyun->skor), 20, 20, 20, YELLOW);						//
            DrawText(TextFormat("En Yüksek Skor: %04i", oyun->enyuksekskor[0]), 20, 40, 20, RED);		//sol üstteki skorları çizer
            DrawText(TextFormat("Blok Hizi : %.1f", oyun->dusmehizi), 20, 60, 20, GOLD);				//
        }
        else
        {
            Rectangle kaynakBG = { 0, 0, arkaplanDokusu.width, arkaplanDokusu.height };							//ölüm menüsü çizer
            Rectangle hedefBG = { 0, 0, EKRAN_GENISLIK, EKRAN_YUKSEKLIK };
            Vector2 originBG = { 0, 0 };
            DrawTexturePro(arkaplanDokusu, kaynakBG, hedefBG, originBG, 0.0f, GRAY);
            
            DrawRectangle(EKRAN_GENISLIK / 2 - 200, EKRAN_YUKSEKLIK / 2 - 250, 400, 500, Fade(BLACK, 0.6f));
            DrawRectangleLines(EKRAN_GENISLIK / 2 - 200, EKRAN_YUKSEKLIK / 2 - 250, 400, 500, GOLD);
            
            DrawText("Kaybettin!", EKRAN_GENISLIK/2 - 100, EKRAN_YUKSEKLIK/2 - 220, 40, RED);							
            DrawText(TextFormat("Skorunuz: %i", oyun->skor), EKRAN_GENISLIK/2 - 70, EKRAN_YUKSEKLIK/2 - 170, 20, GRAY);
            
            DrawText("Skor Siralamalari:", EKRAN_GENISLIK/2 - 120, EKRAN_YUKSEKLIK/2 - 120, 30, GOLD);
            
            for(int i = 0; i < MAX_SKOR_SAYISI; i++)
            {																					//sıralanmış en büyük beş skor çizilir
                DrawText(TextFormat("%d. %04i", i+1, oyun->enyuksekskor[i]), EKRAN_GENISLIK/2 - 40, EKRAN_YUKSEKLIK/2 - 80 + (i * 30), 25, WHITE);
            }
            
            DrawText("[ENTER] Yeniden Oyna", EKRAN_GENISLIK/2 - 110, EKRAN_YUKSEKLIK/2 + 100, 20, LIGHTGRAY);
        }
    EndDrawing();
}

int main(int argc, char *argv[])
{
    SetRandomSeed(time(0));                     
    InitWindow(EKRAN_GENISLIK, EKRAN_YUKSEKLIK, "Kup yagmuru");
    InitAudioDevice();
    
    arkaplanDokusu = LoadTexture("textures/arkaplan.png");
    oyuncuDokusu = LoadTexture("textures/oyuncu.png");
    for(int i=0; i < DOKU_SAYISI; i++)
    {
        blokDokulari[i] = LoadTexture(TextFormat("textures/blok_%d.png", i)); 
    }
    
    RekorYukle(&oyunverileri);
    
    oyunverileri.arkaplanmuzik = LoadMusicStream("Sounds/ArkaPlanMuzik.mp3");
    oyunverileri.olumsesi = LoadSound("Sounds/OlumSesi.mp3");
    oyunverileri.ziplamasesi = LoadSound("Sounds/ZiplamaSesi.mp3");
    oyunverileri.taskirilmasesi = LoadSound("Sounds/TasKirilmaSesi.mp3");
    PlayMusicStream(oyunverileri.arkaplanmuzik);
    
    SetTargetFPS(60);
    OyunuSifirla(&oyunverileri, &oyuncu);
    
    while(!WindowShouldClose())
    {
        UpdateMusicStream(oyunverileri.arkaplanmuzik);
        OyunuGuncelle(&oyunverileri, &oyuncu);        
        OyunuCiz(&oyunverileri, &oyuncu);
    }
    
    UnloadMusicStream(oyunverileri.arkaplanmuzik);
    UnloadSound(oyunverileri.taskirilmasesi);
    UnloadSound(oyunverileri.ziplamasesi);
    UnloadSound(oyunverileri.olumsesi);
    
    UnloadTexture(oyuncuDokusu);
    for(int i=0; i < DOKU_SAYISI; i++)
    {
        UnloadTexture(blokDokulari[i]);
    }
    UnloadTexture(arkaplanDokusu);
    
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
*Ad-Soyad-Numara

Alperen Karadana 25253053
Alp Şahintürk 23253045


*Oyun Fikri;

   Oyunumuzun adı "Blok Yağmuru". Oyunda yukarıdan rastgele sütunlara düşen
   bloklardan kaçmaya çalışan bir kaşif bulunmaktadır. Kaşifimiz yıkılmaya yüz tutmuş tapınaktan düşen 
   tuğlalardan ve taşlardan sıyrılarak hayatta kalmaya çalışır. 
   Oyunun zorluğu zamanla artar, düşen taşların hızı yükselir.
   Oyuncu bir bloğa çarptığında oyun sona erer.


*Oyunun Derlenmesi ve Çalıştırılması;

Derleme ve çalıştırma adımları:
- Embarcadero Dev-C++ 6.3 açılır.
- Proje TDM-GCC 64-bit Release derleyicisi ile açılır.
- Raylib kütüphanesi projeye eklenmiş olmalıdır.
- "Compile & Run"(F11) butonuna basılarak oyun başlatılır.


*Zorunlu İşaretçi (Pointer) Kullanımı;

- Oyuncu ve oyun verileri fonksiyonlara işaretçi ile gönderilmiştir.
    Örnek;
    void OyunuGuncelle(Oyun *oyun, Oyuncu *oyuncu)
    void OyuncuFizigiGuncelle(Oyuncu *oyuncu, Oyun *oyun)

- Bu sayede fonksiyonlar oyun ve oyuncu verilerini doğrudan değiştirebilmekte,
  bellek kullanımı daha verimli hale gelmektedir.


*Yüksek Skor Sistemi ve Dosya Formatı;

- En yüksek 5 skor "rekor.dat" adlı dosyada kaydediliyor.
- Dosya ikili (binary) formatta kaydedilmektedir.
- En yüksek 5 skor, int dizisi halinde dosyaya yazılmaktadır.
- Dosyada her satırda bir sayı bulunmamaktadır, veriler ikili formatta ardışık olarak saklanmaktadır.
- Oyun açıldığında skorlar dosyadan okunur, oyun bitince gerekirse güncellenir.


*Ekipteki Görev Paylaşımı;

- Alperen Karadana:
  * Ana oyun mekanikleri.
  * Ses efektleri, arkaplan müziği eklenmesi.
  * Skor ve En yüksek skor sıralanması(Bubble Sort).

- Alp Şahintürk:
  * Oyun türünün ve fikrinin belirlenmesi.
  * Ana oyun mekanikleri.
  * Görsel dokuların, arkaplan manzarasının eklenmesi.
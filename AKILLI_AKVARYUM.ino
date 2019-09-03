//----------KÜTÜPHANELER----------
#include <LiquidCrystal_I2C.h>       //ekran Kütüphanesi
#include <Wire.h>                    //I2C Kütüphanesi

#include <DallasTemperature.h>       //DS18B20 Kütüphanesi
#include <OneWire.h>

#include <EEPROM.h>                  //Kalıcı Hafıza için Eprom kütüphanesi
#include <DS1307.h>                  //Gerçek zaman saati için DS1307 Kütüphanesi

LiquidCrystal_I2C ekran(0x27, 16, 2);  //ekran ayarları
DS1307 rtc(SDA, SCL);                //DS1307 ayarları

#define DS18B20 12              //DS18B20 Veri kablosu Arduino 12 numaralı pine
OneWire oneWire(DS18B20);       //Herhangi bir OneWire cihazıyla iletişim kurmak için bir oneWire kurulumu
DallasTemperature musur(&oneWire); // oneWire referansımızı Dallas Temperature iletişimi.

unsigned long time; // Delay yerine zaman korumalarını milis komutu ile yapıyoruz

//----------ÖZEL KARAKTERLER----------
byte dr [8] = {12, 18, 18, 12, 0, 0, 0, 0}; //Derece sembolü
byte y  [8] = {4, 14, 21, 4, 4, 4, 4, 4};   //Yukarı ok sembolü
byte a  [8] = {4, 4, 4, 4, 21, 14, 4, 0};   //Aşağı ok sembolü
byte r  [8] = {8, 4, 2, 31, 2, 4, 8, 0};    //Sağ ok sembolü
byte l  [8] = {2, 4, 8, 31, 8, 4, 2, 0};    //Sol ol sembolü

//----------ARDUİNO PİN TANIMLAMALARI----------
const int trig = 2;  //HC-SR04 ultrasonik sensör trig pini Arduino 2 nolu pine
const int echo = 4;  //HC-SR04 ultrasonik sensör echo pini Arduino 4 nolu pine
int isitici = 3;     //Isıtıcı Arduino 3 nolu pine
int buzzer = 5;      //Buzzer 5 nolu pine
int yukari = 7;      //Yukarı butonu Arduino 7 nolu pine
int asagi = 8;       //Aşağı butonu Arduino 8 nolu pine
int giris = 9;       //Giris butonu Arduino 9 nolu pine
int arti = 10;       //Artı butonu Arduino 10 nolu pine
int eksi = 11;       //Eksi butonu Arduino 11 nolu pine
int led = 13;        //Led Arduino 13 nolu pine
int sb = A0;         //Su boşaltma motoru Arduino A0 nolu pine
int sd = A1;         //Su doldurma motoru Arduino A1 nolu pine
int yemmotoru = A2;  //Yem motoru Arduino A2 nolu pine
int fan = A3;        //Fan Arduino A3 nolu pine

//----------DEĞİŞKENLER----------
int secim = 0; //Seçim isimli bir değişken tanımlıyoruz
int ayar = 0; //Hangi sekmeyi ayarlayacağımızı seçmek için değişken tanımlıyoruz.
int otomanyem = 0; //otomanyem yem değişkenini tanımlıyoruz.
int motor = 0;
int otomansb = 0;


int setDER = EEPROM.read(0); //setDER değişkenini Epromun 0.adresine yazıyoruz
int setSEV = EEPROM.read(1); //setSEV değişkenini Epromun 1.adresine yazıyoruz
int bs = EEPROM.read(2);     //bs değişkenini Epromun 2.adresine yazıyoruz
int hysISI = EEPROM.read(3); //hysISI değişkenini Epromun 3.adresine yazıyoruz
int setISI = EEPROM.read(4); //setISI değişkenini Epromun 4.adresine yazıyoruz
int ayd = EEPROM.read(5);    //ayd değişkenini Epromun 5.adresine yazıyoruz
int yem1s = EEPROM.read(6);  //yem1s saat değişkenini Epromun 6.adresine yazıyoruz
int yem2s = EEPROM.read(7);  //yem2s saat değişkenini Epromun 7.adresine yazıyoruz
int yem1d = EEPROM.read(8);  //yem1d saat değişkenini Epromun 8.adresine yazıyoruz
int yem2d = EEPROM.read(9);  //yem2d saat değişkenini Epromun 9.adresine yazıyoruz
int otomansd = EEPROM.read(10);//otomansd sd değişkenini Epromun 10.adresine yazıyoruz

int sure;         //sure isimli değişken tanımlıyoruz
int hamseviye;    //seviye isimli değişken tanımlıyoruz
int gercekseviye; //gercekseviye isimli değişken tanımlıyoruz
int hamlux;//mod isimli değişken tanımlıyoruz
int lux;       //lux isimli değişken tanımlıyoruz
int derece = 0;   //derece isimli değişken tanımlıyoruz ve başlangıç değerini "0" olarak atıyoruz.
int alm = 0;        //Alarmları ekrana yazmak için değişken tanımlıyoruz.
Time t;           //rtc için t isimli değişken tanımlıyoruz
int mod;


void setup() {

  ekran.begin();   //ekranı başlatıyoruz
  musur.begin(); //DS18B20 ısı sensörünü başlatıyoruz

  rtc.begin();   //DS1307 başlatıyoruz
  rtc.halt(false);
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);

  pinMode(asagi, INPUT_PULLUP); //Giriş ve çıkışları tanımlıyoruz
  pinMode(yukari, INPUT_PULLUP);
  pinMode(giris, INPUT_PULLUP);
  pinMode(arti, INPUT_PULLUP);
  pinMode(eksi, INPUT_PULLUP);
  pinMode(sb, OUTPUT);
  pinMode(sd, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(led, OUTPUT);
  pinMode(yemmotoru, OUTPUT);
  pinMode(isitici, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(buzzer, OUTPUT);

  ekran.print("AKILLI AKVARYUM"); //Açılış ekranı
  ekran.setCursor(3, 1);
  ekran.print("DOGA DOSTU");
  delay(3000);
  ekran.clear();

  menusecimi(); //Menü seçimi için değişken tanımlıyoruz
}
void loop() {

  altmenusecimi(); //Loop döngüleri için değişkenler tanımlıyoruz
  ekranayaz();
  okuma();
  saat();
  aydinlatma();
  yemleme();
  isikontrol();
  supompa();
  alarm();


  //-------------MENÜ BUTON KONTROL-----------
  if (!digitalRead(asagi)) {
    secim ++;
    menusecimi();
    while (!digitalRead(asagi));
  }
  time = millis();
  if (!digitalRead(yukari)) {
    secim --;
    menusecimi();
    while (!digitalRead(yukari));

    if (secim <= 0) {
      secim = 0;
    }
  }
  time = millis();
}

void menusecimi() {

  switch (secim) {

    case 0:
      mod = 0;
      ekran.clear();
      break;

    case 1:
      ekran.clear();
      ekran.print("ANLIK DEGERLERi" );
      ekran.setCursor(4, 1);
      ekran.print("iZLEME");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.createChar(2, a);
      ekran.setCursor(15, 1);
      ekran.write(2);
      break;

    case 2:
      mod = 2;
      ekran.clear();
      ekran.print(">SU SEViYESi" );
      ekran.setCursor(0, 1);
      ekran.print(" SU ISISI");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.createChar(2, a);
      ekran.setCursor(15, 1);
      ekran.write(2);
      break;

    case 3:
      ekran.clear();
      ekran.print(" SU SEViYESi");
      ekran.setCursor(0, 1);
      ekran.print(">SU ISISI");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.createChar(2, a);
      ekran.setCursor(15, 1);
      ekran.write(2);
      break;

    case 4:
      ekran.clear();
      ekran.print(">BALIK SAYISI");
      ekran.setCursor(0, 1);
      ekran.print(" AYDINLATMA");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.createChar(2, a);
      ekran.setCursor(15, 1);
      ekran.write(2);
      break;

    case 5:
      ekran.clear();
      ekran.print(" BALIK SAYISI");
      ekran.setCursor(0, 1);
      ekran.print(">AYDINLATMA");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.createChar(2, a);
      ekran.setCursor(15, 1);
      ekran.write(2);
      break;

    case 6:
      ekran.clear();
      ekran.print(">BAKIM");
      ekran.setCursor(0, 1);
      ekran.print(" YEMLEME");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.createChar(2, a);
      ekran.setCursor(15, 1);
      ekran.write(2);
      break;

    case 7:
      ekran.clear();
      ekran.print(" BAKIM");
      ekran.setCursor(0, 1);
      ekran.print(">YEMLEME");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.createChar(2, a);
      ekran.setCursor(15, 1);
      ekran.write(2);
      break;

    case 8:
      ekran.clear();
      ekran.setCursor(0, 0);
      ekran.print(">ALARMLAR");
      ekran.createChar(1, y);
      ekran.setCursor(15, 0);
      ekran.write(1);
      break;

    case 9:
      secim = 8;
      break;
  }
}
void altmenusecimi () {

  //----------ANLIK DEĞERLERİ İZLEME----------

  if (!digitalRead(giris) && secim == 1)
  {
    mod = 1;
    ekran.clear();
    ekran.print("SEViYE" );
    ekran.setCursor(7, 0);
    ekran.print("ISI");
    ekran.setCursor(12, 0);
    ekran.print("LUX");
    ekran.createChar(1, dr);
    ekran.setCursor(9, 1);
    ekran.write(1);
    ekran.setCursor(10, 1);
    ekran.print("C");
    while (!digitalRead(giris));
  }
  //----------SU SEVİYESİ----------

  if (!digitalRead(giris) && secim == 2)
  {
    ayar ++;
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("SU SEViYESI");

    ekran.setCursor(0, 1);
    ekran.print("Der:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(0));

    ekran.setCursor(9, 1);
    ekran.print("Set:");
    ekran.setCursor(13, 1);
    ekran.print(EEPROM.read(1));

    ekran.setCursor(15, 0);
    ekran.print(EEPROM.read(10));

    while (!digitalRead(giris));

    if (ayar > 3) {
      ayar = 1;
    }

    if (ayar == 1) {
      ekran.createChar(1, l);
      ekran.setCursor(7, 1);
      ekran.write(1);
    }
    if (ayar == 2)
    {
      ekran.createChar(1, r);
      ekran.setCursor(7, 1);
      ekran.write(1);
    }
    if (ayar == 3)
    {
      ekran.createChar(1, y);
      ekran.setCursor(15, 1);
      ekran.write(1);
    }
  }

  if (!digitalRead(arti) && secim == 2 && ayar == 1 )
  {
    setDER ++;
    if (setDER > 255 ) {
      setDER = 1;
    }
    time = millis();
    EEPROM.write(0, setDER);
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("SU SEViYESI");

    ekran.setCursor(0, 1);
    ekran.print("Der:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(0));

    ekran.setCursor(9, 1);
    ekran.print("Set:");
    ekran.setCursor(13, 1);
    ekran.print(EEPROM.read(1));

    ekran.setCursor(15, 0);
    ekran.print(EEPROM.read(10));

    ekran.createChar(1, l);
    ekran.setCursor(7, 1);
    ekran.write(1);
  }
  else  if (!digitalRead(eksi) && secim == 2 && ayar == 1 )
  {
    setDER --;
    if (setDER < 1 ) {
      setDER = 255;
    }
    time = millis();
    EEPROM.write(0, setDER);
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("SU SEViYESI");

    ekran.setCursor(0, 1);
    ekran.print("Der:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(0));

    ekran.setCursor(9, 1);
    ekran.print("Set:");
    ekran.setCursor(13, 1);
    ekran.print(EEPROM.read(1));

    ekran.setCursor(15, 0);
    ekran.print(EEPROM.read(10));

    ekran.createChar(1, l);
    ekran.setCursor(7, 1);
    ekran.write(1);
  }

  if (!digitalRead(arti) && secim == 2 && ayar == 2 )
  {
    setSEV ++;
    if (setSEV > 255 ) {
      setSEV = 1;
    }
    time = millis();
    EEPROM.write(1, setSEV);
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("SU SEViYESI");

    ekran.setCursor(0, 1);
    ekran.print("Der:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(0));

    ekran.setCursor(9, 1);
    ekran.print("Set:");
    ekran.setCursor(13, 1);
    ekran.print(EEPROM.read(1));

    ekran.setCursor(15, 0);
    ekran.print(EEPROM.read(10));

    ekran.createChar(1, r);
    ekran.setCursor(7, 1);
    ekran.write(1);
  }
  else if (!digitalRead(eksi) && secim == 2 && ayar == 2 )
  {
    setSEV --;
    if (setSEV < 1 ) {
      setSEV = 255;
    }
    time = millis();
    EEPROM.write(1, setSEV);
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("SU SEViYESI");

    ekran.setCursor(0, 1);
    ekran.print("Der:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(0));

    ekran.setCursor(9, 1);
    ekran.print("Set:");
    ekran.setCursor(13, 1);
    ekran.print(EEPROM.read(1));

    ekran.setCursor(15, 0);
    ekran.print(EEPROM.read(10));

    ekran.createChar(1, r);
    ekran.setCursor(7, 1);
    ekran.write(1);
  }
  if (!digitalRead(arti) && secim == 2 && ayar == 3 )
  {
    otomansd ++;
    if (otomansd > 1)
    {
      otomansd = 0;
    }
    time = millis();
    EEPROM.write(10, otomansd);
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("SU SEViYESI");

    ekran.setCursor(0, 1);
    ekran.print("Der:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(0));

    ekran.setCursor(9, 1);
    ekran.print("Set:");
    ekran.setCursor(13, 1);
    ekran.print(EEPROM.read(1));

    ekran.setCursor(15, 0);
    ekran.print(otomansd);

    while (!digitalRead(arti));

    ekran.createChar(1, y);
    ekran.setCursor(15, 1);
    ekran.write(1);
  }
  //----------SU ISISI----------
  if (!digitalRead(giris) && secim == 3)
  {
    ayar ++;
    ekran.clear();
    ekran.setCursor(4, 0);
    ekran.print("SU ISISI");

    ekran.setCursor(0, 1);
    ekran.print("Set:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(4));
    ekran.createChar(1, dr);
    ekran.setCursor(6, 1);
    ekran.write(1);
    ekran.setCursor(7, 1);
    ekran.print("C");

    ekran.setCursor(10, 1);
    ekran.print("Hys:");
    ekran.setCursor(14, 1);
    ekran.print(EEPROM.read(3));

    while (!digitalRead(giris));

    if (ayar > 2) {
      ayar = 1;
    }

    if (ayar == 1) {
      ekran.createChar(5, l);
      ekran.setCursor(8, 1);
      ekran.write(5);
    }
    else
    {
      ekran.createChar(6, r);
      ekran.setCursor(8, 1);
      ekran.write(6);
    }
  }

  if (!digitalRead(arti) && secim == 3 && ayar == 1 )
  {
    setISI ++;
    if (setISI > 35 ) {
      setISI = 25;
    }
    time = millis();
    EEPROM.write(4, setISI);
    ekran.clear();
    ekran.setCursor(4, 0);
    ekran.print("SU ISISI");
    ekran.setCursor(0, 1);
    ekran.print("Set:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(4));
    ekran.createChar(1, dr);
    ekran.setCursor(6, 1);
    ekran.write(1);
    ekran.setCursor(7, 1);
    ekran.print("C");
    ekran.setCursor(10, 1);
    ekran.print("Hys:");
    ekran.setCursor(14, 1);
    ekran.print(EEPROM.read(3));

    ekran.createChar(5, l);
    ekran.setCursor(8, 1);
    ekran.write(5);
  }
  else if (!digitalRead(eksi) && secim == 3 && ayar == 1 )
  {
    setISI --;
    if (setISI < 25 ) {
      setISI = 35;
    }
    time = millis();
    EEPROM.write(4, setISI);
    ekran.clear();
    ekran.setCursor(4, 0);
    ekran.print("SU ISISI");
    ekran.setCursor(0, 1);
    ekran.print("Set:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(4));
    ekran.createChar(1, dr);
    ekran.setCursor(6, 1);
    ekran.write(1);
    ekran.setCursor(7, 1);
    ekran.print("C");
    ekran.setCursor(10, 1);
    ekran.print("Hys:");
    ekran.setCursor(14, 1);
    ekran.print(EEPROM.read(3));

    ekran.createChar(5, l);
    ekran.setCursor(8, 1);
    ekran.write(5);
  }

  if (!digitalRead(arti) && secim == 3 && ayar == 2 )
  {
    hysISI ++;
    if (hysISI > 10 ) {
      hysISI = 1;
    }
    time = millis();
    EEPROM.write(3, hysISI);
    ekran.clear();
    ekran.setCursor(4, 0);
    ekran.print("SU ISISI");
    ekran.setCursor(0, 1);
    ekran.print("Set:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(4));
    ekran.createChar(1, dr);
    ekran.setCursor(6, 1);
    ekran.write(1);
    ekran.setCursor(7, 1);
    ekran.print("C");
    ekran.setCursor(10, 1);
    ekran.print("Hys:");
    ekran.setCursor(14, 1);
    ekran.print(EEPROM.read(3));

    ekran.createChar(6, r);
    ekran.setCursor(8, 1);
    ekran.write(6);
  }
  else  if (!digitalRead(eksi) && secim == 3 && ayar == 2 )
  {
    hysISI --;
    if (hysISI < 1 ) {
      hysISI = 10;
    }
    time = millis();
    EEPROM.write(3, hysISI);
    ekran.clear();
    ekran.setCursor(4, 0);
    ekran.print("SU ISISI");
    ekran.setCursor(0, 1);
    ekran.print("Set:");
    ekran.setCursor(4, 1);
    ekran.print(EEPROM.read(4));
    ekran.createChar(1, dr);
    ekran.setCursor(6, 1);
    ekran.write(1);
    ekran.setCursor(7, 1);
    ekran.print("C");
    ekran.setCursor(10, 1);
    ekran.print("Hys:");
    ekran.setCursor(14, 1);
    ekran.print(EEPROM.read(3));

    ekran.createChar(6, r);
    ekran.setCursor(8, 1);
    ekran.write(6);
  }

  //----------BALIK SAYISI----------

  if (!digitalRead(giris) && secim == 4)
  {
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("BALIK SAYISI");
    ekran.setCursor(0, 1);
    ekran.print("BS : ");
    ekran.setCursor(5, 1);
    ekran.print(EEPROM.read(2));
    ekran.setCursor(8, 1);
    ekran.print("ADET");

    while (!digitalRead(giris));
  }
  if (!digitalRead(arti) && secim == 4)
  {
    bs ++;
    if (bs > 25 ) {
      bs = 0;
    }
    time = millis();
    EEPROM.write(2, bs);
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("BALIK SAYISI");
    ekran.setCursor(0, 1);
    ekran.print("BS : ");
    ekran.setCursor(5, 1);
    ekran.print(EEPROM.read(2));
    ekran.setCursor(8, 1);
    ekran.print("ADET");

  }
  else  if (!digitalRead(eksi) && secim == 4 )
  {
    bs --;
    if (bs < 0 ) {
      bs = 25;
    }
    time = millis();
    EEPROM.write(2, bs);
    ekran.clear();
    ekran.setCursor(2, 0);
    ekran.print("BALIK SAYISI");
    ekran.setCursor(0, 1);
    ekran.print("BS : ");
    ekran.setCursor(5, 1);
    ekran.print(EEPROM.read(2));
    ekran.setCursor(8, 1);
    ekran.print("ADET");

  }
  //----------AYDINLATMA----------

  if (!digitalRead(giris) && secim == 5)
  {
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("AYDINLATMA");
    ekran.setCursor(0, 1);
    ekran.print("LUX : ");
    ekran.setCursor(6, 1);
    ekran.print(ayd);
    ekran.setCursor(10, 1);
    ekran.print("Lux");

    while (!digitalRead(giris));
  }
  if (!digitalRead(arti) && secim == 5)
  {
    ayd ++;
    if (ayd > 255 ) {
      ayd = 0;
    }
    time = millis();
    EEPROM.write(5, ayd);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("AYDINLATMA");
    ekran.setCursor(0, 1);
    ekran.print("LUX : ");
    ekran.setCursor(6, 1);
    ekran.print(EEPROM.read(5));
    ekran.setCursor(10, 1);
    ekran.print("Lux");
  }

  else if (!digitalRead(eksi) && secim == 5)
  {
    ayd --;
    if (ayd < 0 ) {
      ayd = 255;
    }
    time = millis();
    EEPROM.write(5, ayd);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("AYDINLATMA");
    ekran.setCursor(0, 1);
    ekran.print("LUX : ");
    ekran.setCursor(6, 1);
    ekran.print(EEPROM.read(5));
    ekran.setCursor(10, 1);
    ekran.print("Lux");

  }
  //----------BAKIM----------

  if (!digitalRead(giris) && secim == 6)
  {
    ekran.clear();
    ekran.setCursor(5, 0);
    ekran.print("BAKIM");
    ekran.setCursor(0, 1);
    ekran.print("SB: ");
    ekran.setCursor(4, 1);
    ekran.print("OFF");
    ekran.setCursor(8, 1);
    ekran.print("SD:");
    ekran.setCursor(12, 1);
    ekran.print("OFF");

    while (!digitalRead(giris));
  }
  if (!digitalRead(arti) && secim == 6) {

    otomansb ++;
    if (otomansb > 1) {
      otomansb = 0;
    }
    if (otomansb == 1) {
      ekran.clear();
      ekran.setCursor(5, 0);
      ekran.print("BAKIM");
      ekran.setCursor(0, 1);
      ekran.print("SB: ");
      ekran.setCursor(4, 1);
      ekran.print("ON");
      ekran.setCursor(8, 1);
      ekran.print("SD:");
      ekran.setCursor(11, 1);
      ekran.print("IPTAL");
    }
    else
    {
      ekran.clear();
      ekran.setCursor(5, 0);
      ekran.print("BAKIM");
      ekran.setCursor(0, 1);
      ekran.print("SB: ");
      ekran.setCursor(4, 1);
      ekran.print("OFF");
      ekran.setCursor(8, 1);
      ekran.print("SD: ");
      ekran.setCursor(12, 1);
      ekran.print("OFF");
    }
    while (!digitalRead(arti));
  }

  if (!digitalRead(eksi) && secim == 6 && otomansd == 0) {
    digitalWrite(sd, !digitalRead(sd));
    digitalWrite(sb, LOW);

    if (digitalRead(sd) == HIGH) {
      ekran.clear();
      ekran.setCursor(5, 0);
      ekran.print("BAKIM");
      ekran.setCursor(0, 1);
      ekran.print("SB:");
      ekran.setCursor(3, 1);
      ekran.print("IPTAL");
      ekran.setCursor(8, 1);
      ekran.print("SD:");
      ekran.setCursor(12, 1);
      ekran.print("ON");
    }
    else
    {
      ekran.clear();
      ekran.setCursor(5, 0);
      ekran.print("BAKIM");
      ekran.setCursor(0, 1);
      ekran.print("SB: ");
      ekran.setCursor(4, 1);
      ekran.print("OFF");
      ekran.setCursor(8, 1);
      ekran.print("SD: ");
      ekran.setCursor(12, 1);
      ekran.print("OFF");
    }
    while (!digitalRead(eksi));
  }
  //----------YEMLEME AYARLARI----------

  if (!digitalRead(giris) && secim == 7)
  {
    ayar++;
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    while (!digitalRead(giris));

    if (ayar > 5) {
      ayar = 1;
    }
    if (ayar == 1) {
      ekran.createChar(1, r);
      ekran.setCursor(0, 0);
      ekran.write(1);
      ekran.setCursor(14, 1);
      ekran.print(otomanyem);
    }
    if (ayar == 2) {
      ekran.createChar(1, l);
      ekran.setCursor(15, 0);
      ekran.write(1);
      ekran.setCursor(14, 1);
      ekran.print(otomanyem);
    }
    if (ayar == 3) {
      ekran.createChar(1, r);
      ekran.setCursor(0, 1);
      ekran.write(1);
      ekran.setCursor(14, 1);
      ekran.print(otomanyem);
    }
    if (ayar == 4) {
      ekran.createChar(1, l);
      ekran.setCursor(13, 1);
      ekran.write(1);
      ekran.setCursor(14, 1);
      ekran.print(otomanyem);
    }
    if (ayar == 5) {
      ekran.createChar(1, a);
      ekran.setCursor(14, 0);
      ekran.write(1);
      ekran.setCursor(14, 1);
      ekran.print(otomanyem);
    }
  }

  if (!digitalRead(arti) && secim == 7 && ayar == 1 )
  {
    yem1s ++;
    if (yem1s > 23 ) {
      yem1s = 0;
    }
    time = millis();
    EEPROM.write(6, yem1s);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, r);
    ekran.setCursor(0, 0);
    ekran.write(1);
  }
  if (!digitalRead(eksi) && secim == 7 && ayar == 1 )
  {
    yem1s --;
    if (yem1s < 0 ) {
      yem1s = 23;
    }
    time = millis();
    EEPROM.write(6, yem1s);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, r);
    ekran.setCursor(0, 0);
    ekran.write(1);
  }
  if (!digitalRead(arti) && secim == 7 && ayar == 2 )
  {
    yem1d ++;
    if (yem1d > 59 ) {
      yem1d = 0;
    }
    time = millis();
    EEPROM.write(8, yem1d);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, l);
    ekran.setCursor(15, 0);
    ekran.write(1);
  }
  if (!digitalRead(eksi) && secim == 7 && ayar == 2 )
  {
    yem1d --;
    if (yem1d < 0 ) {
      yem1d = 59;
    }
    time = millis();
    EEPROM.write(8, yem1d);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, l);
    ekran.setCursor(15, 0);
    ekran.write(1);
  }
  if (!digitalRead(arti) && secim == 7 && ayar == 3 )
  {
    yem2s ++;
    if (yem2s > 23 ) {
      yem2s = 0;
    }
    time = millis();
    EEPROM.write(7, yem2s);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, r);
    ekran.setCursor(0, 1);
    ekran.write(1);
  }
  if (!digitalRead(eksi) && secim == 7 && ayar == 3 )
  {
    yem2s --;
    if (yem2s < 0 ) {
      yem2s = 23;
    }
    time = millis();
    EEPROM.write(7, yem2s);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, r);
    ekran.setCursor(0, 1);
    ekran.write(1);
  }
  if (!digitalRead(arti) && secim == 7 && ayar == 4 )
  {
    yem2d ++;
    if (yem2d > 59 ) {
      yem2d = 0;
    }
    time = millis();
    EEPROM.write(9, yem2d);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, l);
    ekran.setCursor(15, 1);
    ekran.write(1);
  }
  if (!digitalRead(eksi) && secim == 7 && ayar == 4 )
  {
    yem2d --;
    if (yem2d < 0 ) {
      yem2d = 59;
    }
    time = millis();
    EEPROM.write(9, yem2d);
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("YEM1:");
    ekran.setCursor(8, 0);
    ekran.print(EEPROM.read(6));
    ekran.setCursor(10, 0);
    ekran.print(":");
    ekran.setCursor(11, 0);
    ekran.print(EEPROM.read(8));
    ekran.setCursor(3, 1);
    ekran.print("YEM2:");
    ekran.setCursor(8, 1);
    ekran.print(EEPROM.read(7));
    ekran.setCursor(10, 1);
    ekran.print(":");
    ekran.setCursor(11, 1);
    ekran.print(EEPROM.read(9));

    ekran.createChar(1, l);
    ekran.setCursor(15, 1);
    ekran.write(1);
  }
  if (!digitalRead(arti) && secim == 7 && ayar == 5 )
  {
    otomanyem ++;
    if (otomanyem > 1)
    {
      otomanyem = 0;
    }
    time = millis();
    ekran.setCursor(14, 1);
    ekran.print(otomanyem);

    while (!digitalRead(arti));
  }
  //----------ALARMLAR----------
  if (!digitalRead(giris) && secim == 8)

    if (alm == 0 && secim == 8)
    {
      ekran.clear();
      ekran.setCursor(1, 0);
      ekran.print("SISTEM NORMAL");
    }

  if (alm == 1 && secim == 8)
  {
    ekran.clear();
    ekran.setCursor(2, 1);
    ekran.print("ISI YUKSEK");
  }

  if (alm == 2 && secim == 8)
  {
    ekran.clear();
    ekran.setCursor(2, 1);
    ekran.print("ISI DUSUK");
  }

  if (alm == 3 && secim == 8)
  {
    ekran.clear();
    ekran.setCursor(3, 0);
    ekran.print("SU YUKSEK");
  }

  if (alm == 4 && secim == 8)
  {
    ekran.clear();
    ekran.setCursor(4, 0);
    ekran.print("SU AZALDI");
  }
}
//----------OKUMA----------
void okuma () {
  //SU SEVİYESİ OKUMA
  {
    digitalWrite(trig, HIGH);
    delayMicroseconds(2);
    digitalWrite(trig, LOW);
    sure = pulseIn(echo, HIGH);
    hamseviye = (sure / 2) / 28.5;
    gercekseviye = setDER - hamseviye;
  }
  //ISI OKUMA
  {
    musur.requestTemperatures();
    derece = musur.getTempCByIndex(0);
  }
  //LUX OKUMA
  {
    hamlux = analogRead(A6);
    lux = map(hamlux, 0, 1023, 0, 255);
  }
}
//----------ANLIK DEĞERLERİ EKRANA YAZ----------
void ekranayaz () {
  if (mod == 1) {
    ekran.setCursor(0, 1);
    ekran.print(gercekseviye);
    ekran.print("CM ");

    ekran.setCursor(7, 1);
    ekran.print(derece);

    ekran.setCursor(12, 1);
    ekran.print(lux);
    ekran.print(" ");
  }
}

//----------GERÇEK ZAMAN SAATİ----------
void saat() {
  t = rtc.getTime();
  if ((mod == 0) && (secim == 0)) {
    ekran.setCursor(4, 1);
    ekran.print(rtc.getTimeStr());
    ekran.setCursor(3, 0);
    ekran.print(rtc.getDateStr());
    ekran.createChar(1, a);
    ekran.setCursor(15, 1);
    ekran.write(1);
  }
}
//--------------------ÇIKIŞLAR--------------------

//----------AYDINLATMA----------
void aydinlatma () {

  if (lux >= EEPROM.read(5))
    digitalWrite(led, LOW);
  else
    digitalWrite(led, HIGH);
}
//----------YEM MOTORU ÇALIŞTIRMA----------
void yemleme () {

  if (yem1s == (t.hour) && yem1d == (t.min) || yem2s == (t.hour) && yem2d == (t.min) || otomanyem == 1)
  {
    motor ++;
  }
  else
  {
    motor = 0;
  }

  switch (motor) {

    case 0:
      digitalWrite(yemmotoru, LOW);
      break;

    case 1:
      digitalWrite(yemmotoru, HIGH);
      delay (bs * 100);
      digitalWrite(yemmotoru, LOW);
      otomanyem = 0;
      break;
  }
}
//----------ISITICI VE FAN KONTROL----------
void isikontrol () {

  if (derece >= setISI + hysISI) {
    digitalWrite(fan, HIGH);
  }
  if (derece <= setISI) {
    digitalWrite(fan, LOW);
  }
  if (derece <= setISI - hysISI) {
    digitalWrite(isitici, HIGH);
  }
  if (derece >= setISI) {
    digitalWrite(isitici, LOW);
  }

}
//----------SU POMPALARI----------
void supompa () {

  if (gercekseviye < setSEV && secim != 6 && otomansd == 1 && otomansb == 0)
  {
    digitalWrite(sd, HIGH);
    delay(5000);
    digitalWrite(sd, LOW);
    delay(2);
  }

  else if (gercekseviye >= setSEV || secim == 6 || otomansd == 0)
  {
    digitalWrite(sd, LOW);
  }

  if (gercekseviye > 5 && secim == 6 && otomansb == 1 && otomansd == 0)
  {
    digitalWrite(sb, HIGH);
    delay(5000);
    digitalWrite(sb, LOW);
    delay(2);
  }
  else if (gercekseviye <= 5 || secim != 6 || otomansb == 0)
  {
    digitalWrite(sb, LOW);
    otomansb = 0;
  }
}
//----------ALARMLAR----------
void alarm() {

  if (derece >= setISI + hysISI + 5)
  {
    alm = 1;
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else if (derece <= setISI - hysISI - 5)
  {
    alm = 2;
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else if (gercekseviye >= setSEV + 10)
  {
    alm = 3;
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else if (gercekseviye <= 0)
  {
    alm = 4;
    digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    delay(100);
  }
  else
  {
    alm = 0;
    digitalWrite(buzzer, LOW);
  }
}

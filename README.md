# 📡 ESPNowTNC
[🇺🇸 English Version](README-en.md)

ESPNowTNC è un **TNC (Terminal Node Controller)** basato sul protocollo **KISS** che utilizza **ESP-NOW**

## 📜 Protocollo KISS
**KISS** (Keep It Simple, Stupid) è un protocollo minimale per incapsulare trame AX.25 (o altri dati) su collegamenti seriali (tipicamente RS232, USB o UART).  
È ampiamente utilizzato in contesti **radioamatoriali** e permette una comunicazione semplice tra un computer e un TNC (Terminal Node Controller).

[KISS - Wikipedia](https://en.wikipedia.org/wiki/KISS_(amateur_radio_protocol))

## 🌐 Perché un TNC KISS su ESP-NOW?
I TNC KISS tradizionalmente vengono connessi a una ricetrasmittente che opera su frequenze radioamatoriali.

Questo progetto, invece, sfrutta **ESP-NOW**, una tecnologia di comunicazione wireless **basata su 802.11**, integrata nei chip **ESP32**, che opera nella **banda ISM a 2.4 GHz** (la stessa utilizzata da WiFi e Bluetooth).  
Può quindi risultare molto utile anche durante lo **sviluppo e il debug di software** che deve comunicare con un TNC.

[ESPNow](https://www.espressif.com/en/solutions/low-power-solutions/esp-now)

> [!NOTE]
> Questo progetto funge **esclusivamente** da TNC KISS:  
> - non è un sistema MESH, non gestisce inoltri, ritrasmissioni o instradamento dei pacchetti.  
> - si comporta in modo **completamente trasparente**, proprio come farebbe una radio analogica collegata a un TNC tradizionale.

## 🛠️ Compilazione e caricamento
Questo progetto è composto da un unico file che può essere compilato e caricato con facilità tramite l'**Arduino IDE**.

>[!NOTE]
> Attualmente è stato compilato e testato con successo solo sulla piattaforma NodeMCU-32S. Non è garantito il funzionamento su altre schede.

> [!WARNING]
>La stampa di debug via seriale è **attiva di default** (`#define DEBUG true`) e lo sarà fino a che il software non avrà raggiunto una versione stabile.
>Se desideri utilizzare il dispositivo in un'applicazione reale, è consigliato disattivarla per evitare >interferenze con i dati KISS.  
>Alcuni software KISS sono **tolleranti** alla stampa extra, ma non è garantito.

## 📦 Funzionalità attuali
- Incapsulamento e disincapsulamento KISS  
- Trasmissione e ricezione di pacchetti frammentati via ESP-NOW  
- Supporto parametri CSMA (Persistence `P`, SlotTime)  

## 🔮 Funzionalità future
- 🔧 **Comandi KISS custom** per:
  - Modificare il **baudrate** della porta seriale
  - Modificare il **canale radio ESP-NOW**

## ⚠️ DISCLAIMER ⚠️
Questo progetto è **sperimentale**, in **sviluppo attivo** e può contenere bug o subire modifiche che rompono la compatibilità con versioni precedenti.  

**Non è adatto per applicazioni critiche** o in cui siano richieste affidabilità o sicurezza elevata.

Sebbene **ESP-NOW** utilizzi la banda **ISM a 2.4 GHz**, accessibile liberamente senza licenza, è fondamentale::
- **Non interferire** con comunicazioni radioamatoriali.
- **Non connettere** il dispositivo a reti o apparecchiature che trasmettono su frequenze riservate ai radioamatori, a meno di essere **legalmente autorizzati** (radioamatori con licenza).

Inoltre il sistema **non implementa l'autenticazione**: se prevedi un uso in ambito radioamatoriale, valuta l’aggiunta di **meccanismi di accesso controllato** per evitare abusi.

L'autore **non è responsabile** per danni diretti, indiretti, incidentali o consequenziali derivanti dall'uso, dal malfunzionamento o dall’abuso del software, né per eventuali violazioni normative da parte dell’utente.
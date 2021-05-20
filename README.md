# projekt_nawadnianie
System podlewający oraz nadzorujący warunki panujące w otoczeniu rośliny oparty na własnoręcznie zaprojektowanym mikrokontrolerze wzorowanym na Arduino Uno oraz pompie perystaltycznej. 

Główne założenia, etapy realizacji projektu:
<ul>
  <li>Baza danych do łatwego zbierania odczytów czujników, zaplanowanych godzin podlewania oraz listy roślin</li>
  <li>Pompa perystaltyczna wydrukowana na drukarce 3D</li>
  <li>Mikrokontroler posiadający tylko potrzebne w tym projekcie moduły w celu optymalizacji poboru prądu</li>
  <li>Aplikacja na telefon oraz strona internetowa umożliwiające łatwe planowanie dat podlewania oraz podejrzenie mierzonych parametrów</li>
  <li>Podlewanie odpowiednią ilością wody w zależności od zmierzonej wilgotności. 
    Mikrokontroler może przełożyć operację ze względu na brak potrzeby uzupełnienia wody w doniczce</li>
  </ul>


<h1>Schemat koncepcyjny układu:
<img src="schemat_prototyp_naszego_UKLADU.bmp" alt="drawing" width="1000"/>

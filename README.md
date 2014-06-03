STM32F4_FruitKeyboard
=====================

Ten projekt wzorowany był na projekcie "Makey Makey" który w Jun 13, 2012 został sfinansowany na popularnym serwisie KickStarter opierającym się na crowdfundingu. Ideą tego projektu było pozwolenie użytkownikowi na stworzenie kontrolera do gier i nie tylko za pomocą niemal wszystkiego co przewodzi prąd. Użytkownik kontrolera musi cały czas "być podłączony" do uziemienia, przy czym, aby aktywować któryś przycisk kontrolera, dla przykładu strzałkę w górę, musi dotknąć odpowiedniego wyprowadzenia poprzez coś co przewodzi prąd.

Oryginalny projekt był wykonany z użyciem mikrokontrolera ATMega32u4, naszym zadaniem natomiast było wykorzystanie zestawu uruchomieniowego STM32F4 Discovery.

Środowisko i kompilator
-----------------------

Ten projekt wymaga środowiska [Atollic TrueStudio](http://www.atollic.com/index.php/download/truestudio-for-arm) - oraz kompilatora [GNU Tools for Arm](https://launchpad.net/gcc-arm-embedded). 


Sterowniki dla STM32F4 Discovery
--------------------------------

Ostatnią rzeczą, którą potrzeba do uruchomienia naszego projektu będą sterowniki do wbudowanego w zestaw uruchomieniowy programatora [ST-LINK/V2 in-circuit debugger/programmer](http://www.st.com/web/catalog/tools/FM146/CL1984/SC724/SS1677/PF251168). 


Uruchamianie projektu
---------------------

* Zaimportować projekt w Atollic TrueStudio, zaznaczyć plik main.c oraz kliknąć przycisk Build
* Podłączyć oryginalną płytkę rozszerzeń Fruit Keyboard od dołu zestawu uruchomieniowego STM32F4 Discovery do prawego rzędu goldpinów P2
* Podłączyć zestaw uruchomieniowy STM32F4 Discovery przez Mini-USB do USB komputera, kliknąć przycisk Debug w celu przejścia do trybu debugowania
* Podłączyć również dodatkowy kabel Micro-USB do USB w komputerze oraz kliknąć przycisk Resume

Działanie
---------
Podłączyć użytkownika do wyprowadzenia oznaczonego jako uziemienie, podłączyć owoce do wyprowadzeń kontrolera za pomocą przewodów, włączyć ulubioną grę, skonfigurować używane klawisze, dotykać owoców w celu aktywowania przycisków

Istnieje możliwość wyboru "czułości urządzenia" poprzez wciśnięcie przycisku na zestawie uruchomieniowym STM32F4 Discovery, wraz z kolejnymi kliknięciami przycisku zmienia się czułość urządzenia - każdy z trybów zapala osobną diodę

# LLCS-OPDRACHT: Thread-safe Inventory 

_Door: Lucas Hoogerbrugge 01/20/2026_ 

## Inleiding 

In document bevindt zich de documentatie over de opdracht thread safe inventory. Hier doorloop ik beknopt welke stappen ik heb doorlopen tijdens het maken van dit project. 

## Inhoud 

 - Inleiding
 - Opdracht
 - GitHub
 - Uitwerking
 - Startpunt
 - Threadpool
 - Collectie klasse ‘ConcurrentInventory’
 - Blackboard
 - Std::future
 - Test(s)
 - Test 1
 - Test 2
 - Test 3
 - Test 4
 - Conclusie



## Opdracht 

De C++ collection classes zijn standaard niet thread-safe. 

Maak een nieuwe class, ConcurrentInventory die gebruik maakt van locking om wél thread-safe te zijn. (Als interne datastructuur kun je wel std::vector gebruiken) 

Bewijs dat deze nu wel thread-safe is: voeg vanuit twee verschillende threads elementen toe aan de vector, en check de resultaten. 

Lever het project als een **ZIP** in: 

- Code (geen build artifacts) 

- GitHub/GitLab link wenselijk (niet verplicht) 

- Toelichting van je uitwerking (1 A4) 

## GitHub 

<u>https://github.com/HyruleTrash/HKU_LLCS_Herkansing/tree/opdracht-thread-safeinventory</u> 

## Uitwerking 

Onder dit kopje beschrijf ik mijn uitwerking. 

### Startpunt 

Als eerst ging ik kijken naar de twee relevante opdrachten over threading. Namelijk de oefening threadpools, en deze opdracht threadsafe inventory. 

Hier zag ik al snel dat de oefening best een uitdaging kon zijn, en dus heb ik deze laten zitten omdat de deadline al dichtbij was. En ging ik dus gelijk aan de slag met de inventory opdracht. 

Ik nam daarnaast wel de threadpool implementatie met me mee die ik deze oefening gezien had, deze hielp mij met mijn begrip op threading in c++. 

Daarna pakte ik mijn collision crisis herkansing erbij omdat ik geen start punt meer klaar had liggen. Hieruit heb ik toen de collision game gehaald, en heb ik de profiler getweaked (zodat hij niet in de weg zat). 

Dit liet mij achter met een cleanslate, waar bij ik makkelijk mijn opdracht en threadpool in kon stoppen. 

### Threadpool 

<u>https://github.com/mtrebi/thread-pool</u> 

Dit was de threadpool te vinden in de andere oefening. Het implementeren hiervan was redelijk simpel. 

Ik heb de header files toegevoegd aan mijn include folder. En de files toegevoegd aan mijn CMake file, (na eerst kijken hoe de repo dat doet, voor de zekerheid) 

Hierna heb ik deze toegevoegd aan mijn applicatie klasse. Al wordt deze uiteindelijk niet gebruikt omdat ik de collectie klasse voor deze opdracht standalone wilde maken. 

### Collectie klasse ‘ConcurrentInventory’ 

Tijdens het maken van de ConcurrentInventory begon ik eerst met het definiëren van de grote vijf. Al moest ik wel opnieuw opzoeken welke dat ook alweer waren. Uiteindelijk na het implementeren van dit merkte ik dat dit niet helemaal nodig was om te gebruiken voor de opdracht. Maar het was fijn om dit te her onderzoeken. 

Daarna begon ik met het uitschrijven van een paar simpele functies, via hun naam. En probeerde ik te bedenken wat de beste optie was voor een inventory. Dat ook bruikbaar is in een game context. 

#### Blackboard 

Ik kwam al snel uit op een Blackboard klasse. Deze ben ik namelijk momenteel aan het gebruiken in mijn valorisatie project. En werkt perfect voor een collectie aan items. Die eigenlijk nog geen definitie hebben. 

In mijn implementatie werkt het bijna hetzelfde als een dictionary. Alles heeft een unieke key. Maar uniekheid bestaat alleen uit de naam(key) en het type item. 

Als gebruiker van de inventory kan je namelijk een boolean opslaan onder “Zwaard”. En dat zou dan kunnen dicteren dat de speler een zwaard heeft. Terwijl potions een getal kan zijn. 

Ik wilde niet twee instances van hetzelfde hebben in de collectie. Zoals 2x bool “Zwaard”, omdat ik dacht dat dit verwarrend kon zijn. Wanneer iemand twee zwaarden in de inventory wil hebben dan is het of beter om dit een getal te maken. Of om ze te definiëren als twee verschillende zwaarden “MasterSword” en “DemonSword” bijvoorbeeld. 

#### Std::future 

Uiteindelijk zal de toepassing van een threadpool niet nodig zijn om threadsafe te zijn. Het enige wat daarvoor nodig zou zijn, was een paar locks. 

Maar ik voelde dat dat een saaie implementatie zou zijn. En ik had al de threadpool geïmplementeerd! 

Daarom heb ik de keuze gemaakt om de functies std::future te laten teruggeven. Dit zorgt ervoor dat een dure zoek functie apart gedaan wordt op een andere thread. En zal de main-thread alleen wachten als zij ‘.get()’ aanroepen. 

### Test(s) 

Uiteindelijk heb ik 4 tests geschreven. 

Deze zouden moeten testen of mijn implementatie met locks wel werken. 

#### Test 1 

In test 1 laat ik meerdere threads eenzelfde item uit de collectie verwijderen. Deze faalt de test als meerdere threads eenzelfde item weten te pakken terwijl er maar 1 te vinden is. 
<hr>
Test 1: 20 users try to take Excalibur at the exact same time 

PASS: Exactly 1 user claimed Excalibur (Others received false upon retrieval) 
<hr>

#### Test 2 

In test 2 laat ik meerdere threads te gelijk, items toevoegen aan de collectie. Deze faalt als er een hoeveelheid wordt terug gelezen dat incorrect is. 
<hr>
Test 2: 100 goblins stashing unique items at the same time... 

PASS: All 100 pouches safely added and read. 
<hr>

#### Test 3 

In deze test laat ik twee threads tegelijk dezelfde item toevoegen. Terwijl er maar een mogelijk mag zijn door mijn uniekheid logica. 

Dus deze faalt als er had toevoegen meerdere keren lukt. 
<hr>
Test 3: Two users add 'Dragon_Egg' at the same time 

PASS: Duplicate key rejected 
<hr>

#### Test 4 

Test 4 is relatief simpel, in deze test laat ik heel veel items toegevoegd worden, en gelezen worden. Om te kijken of de collectie crasht of in deadlock terecht komt. 
<hr>
Test 4: Read/Write stress test PASS: Survived: 4198 Writes 33724 Reads 
<hr>

## Conclusie 

Te zien aan de console output onder deze kop tekst, zijn alle tests geslaagd. En is de collectie succesvol geïmplementeerd. 

In de toekomst als ik verder zou gaan met deze code, zou ik eventueel de grote vijf willen na lopen en testen. 

Daarnaast zou ik misschien kijken of een interface misschien slim is in plaats van een Type. Om de collectie simpeler te maken, en niet alles toe te laten. 
<hr/>
C:\Users\Lucas\Documents\School\HKU\Lowlevel-CompSciHerkansing\cmake-build-debug\bin\ThreadingAssignmentRedoLLCS.exe

Initializing main application,

Creating:

window,

Loopdata,

and program clock.



Registering main game update loop to app loop.

Registering main game draw call to app.

Registering Profiler update loop to app loop.

Registering Profiler draw call to app.



STARTING THREADSAFE INVENTORY TEST

========================================



Test 1: 20 users try to take Excalibur at the exact same time

PASS: Exactly 1 user claimed Excalibur (Others received false upon retrieval)



Test 2: 100 goblins stashing unique items at the same time...

PASS: All 100 pouches safely added and read.



Test 3: Two users add 'Dragon_Egg' at the same time

PASS: Duplicate key rejected



Test 4: Read/Write stress test

PASS:

 Survived:
 
  4198 Writes
  
  33724 Reads

Without a crash or deadlock encounter


========================================

ALL THREAD SAFETY TESTS COMPLETED

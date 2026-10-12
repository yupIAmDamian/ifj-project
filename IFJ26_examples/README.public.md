# Ukázkové programy IFJ26

V tomto archivu najdete několik ukázkových programů v jazyce IFJ26. V adresáři `basic/` jsou ukázky
pro základní zadání, v adresáři `extensions/` je pak jeden příklad pro každé rozšíření. Programy lze
spustit také ve Scale 3.8.4 s využitím souboru (knihovny) [ifj26.scala](ifj26.scala).

Soubory `.in` a `.out` obsahují ukázkový standardní vstup a očekávaný standardní výstup.
Prázdný soubor `.in` znamená, že žádný vstup není potřeba.

V příkladech pro základní zadání jsou k dispozici také soubory `.ifjcode`, které implementují stejný 
algoritmus v IFJcode26. 

| IFJ26 | Protějšek v IFJcode26 | Požadovaná rozšíření | Ukázka |
| --- | --- | --- | --- |
| [basic/factorial.scala](basic/factorial.scala) | [factorial.ifjcode](basic/factorial.ifjcode) | Žádná | Iterativní a rekurzivní faktoriál, vstup, `val`/`var`, podmínky, cykly, explicitní a implicitní `return`. Vstup je omezen na 0–12, aby nedošlo k přetečení faktoriálu; neplatný celočíselný vstup se (podle specifikace) nastaví na nulu. |
| [basic/strings.scala](basic/strings.scala) | [strings.ifjcode](basic/strings.ifjcode) | Žádná | Operace s řetězci a aritmetické operace. **Pozor**, odpovídající kód v IFJcode26 není kompletní (viz komentáře v souboru). |
| [funexp.scala](extensions/funexp.scala) | – | FUNEXP | Vnořená volání ve výrazech a argumentech. |
| [boolthen.scala](extensions/boolthen.scala) | – | BOOLTHEN | Booleovský vstup, hodnoty a návratové typy, negace, zkrácené vyhodnocování AND/OR, `else if` a `if` bez `else`. |
| [control.scala](extensions/control.scala) | – | CONTROL, BOOLTHEN | Podmíněné výrazy, inkluzivní a exkluzivní rozsahy, záporný krok, více stráží a sčítání vybraných hodnot. |
| [valmem.scala](extensions/valmem.scala) | – | VALMEM | `.toString`, `.toInt`, `.floor`, `.length` a řetězené členy. |
| [algebdt.scala](extensions/algebdt.scala) | – | ALGEBDT | Součinové typy, sjednocení typů, vzory a stráže v `match`. |
| [interpolateds.scala](extensions/interpolateds.scala) | – | INTERPOLATEDS | `$id`, `${expression}`, `$$`, víceřádkový interpolovaný řetězec. |
| [lambdas.scala](extensions/lambdas.scala) | – | LAMBDAS, FUNEXP | Funkce vyššího řádu (funkce jako hodnoty), typované lambda výrazy. |
| [lists.scala](extensions/lists.scala) | – | LISTS, CONTROL, LAMBDAS, FUNEXP, BOOLTHEN | Konstrukce seznamu, převod z rozsahu, zřetězení, výřezy, indexování, head/tail, formátování, filtrování, take/dropWhile a iterace se stráží. |
| [extkw.scala](extensions/extkw.scala) | –| EXTKW | Rozšiřující metoda bez parametrů, metoda s prázdným seznamem parametrů a metoda s argumentem. |
| [paramlists.scala](extensions/paramlists.scala) | – | PARAMLISTS | Funkce bez parametrů, více seznamů parametrů a prázdný seznam parametrů. |

## Spuštění

```sh
# Spuštění programu IFJ26 ve Scale
scala run ifj26.scala basic/factorial.scala <basic/factorial.in

# Spuštění programu IFJcode26 v námi dodaném interpretu (viz e-learning)
./ic26int-linux-x86_64 basic/factorial.ifjcode <basic/factorial.in

# Příklad spuštění s využitím vašeho překladače
./vas-prekladac <basic/factorial.scala >/tmp/factorial.ifjcode
./ic26int-linux-x86_64 /tmp/factorial.ifjcode <basic/factorial.in
```

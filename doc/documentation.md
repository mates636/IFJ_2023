
![](logofit.png)

# Dokumentace

## Implementace překladače imperativního jazyka IFJ23

**Tým xpries01, varianta 1**

- Martin Priessnitz (xpries01)  
- Mikuláš Uřídil (xuridi01)  
- Dominik Horák (xfedor20)  
- Kristýna Fedorková (xhorak79)  

*Bez implementace rozšíření*

# Práce v týmu

## Rozdělení práce mezi členy týmu

### Martin Priessnitz

- Návrh automatu pro lexikální analýzu
- Implementace lexikálního analyzátoru
- Návrh LL gramatiky a LL tabulky
- Implementace syntaktického analyzátoru
- Implementace sémantického analyzátoru
- Testování
- Dokumentace

### Mikuláš Uřídil

- Návrh tabulky symbolů
- Implementace tabulky symbolů
- Implementace zásobníku na vyhodnocení výrazů
- Návrh precedenční tabulky
- Implementace syntaktického analyzátoru
- Implementace sémantického analyzátoru

### Dominik Horák

*(Zde může být specifikace úkolů pro Dominika Horáka, pokud jsou dostupné.)*

### Kristýna Fedorková

*(Zde může být specifikace úkolů pro Kristýnu Fedorkovou, pokud jsou dostupné.)*

# Spolupráce

Po založení týmu jsme se domluvili na první schůzce ve škole, kde jsme projednali strukturu překladače a okamžitě rozdělili úkoly mezi členy týmu. Další setkání probíhala prostřednictvím aplikace Discord, kde jsme měli vytvořený vlastní server. Veškeré dotazy jsme řešili a diskutovali přes tuto platformu. Dále jsme vytvořili repozitář na GitHubu, kde jsme ukládali veškeré provedené změny.

# Implementace překladače

## Lexikální analýza

Lexikální analyzátor byl implementován v souboru `scanner.c` (.h). Použili jsme konečný stavový automat pro implementaci tohoto analyzátoru. Hlavní funkcí, která obsahuje vnitřní logiku, je funkce `get_token()`. Tato funkce načítá jednotlivé znaky ze standardního vstupu pomocí cyklu `while`, uvnitř něhož se nachází přepínač `switch` s jednotlivými stavy automatu.

Po přechodu automatu do konečného stavu se vytvoří token pomocí funkce `init_token()`, a automat pokračuje ve čtení dalšího znaku, dokud není na vstupu dosaženo konce souboru (`EOF`). Jednotlivé znaky tokenu se ukládají do pole buffer pomocí funkce `add_char()`.

Mezi další úkoly lexikálního analyzátoru patří také redukce nepodstatných částí, například komentářů. Datový typ `token_t` je implementován v souboru `scanner.h` a reprezentuje token načtený lexikálním analyzátorem. V jeho struktuře se nachází typ tokenu a data; typ tokenu je určen konečným automatem na základě pravidel.

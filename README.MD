### Užduotis


- Client programa:
    - Programa kaip argumentą priima komandą (pvz ./client "ls -la /etc" ).
    - Gautą komandą TCP socket'ų pagalba  siunčia į Server programą.
    - Programa laukia kol Server programa atsiųs atsakymą.
    - Gavus atsakymą išveda jį naudotojui į komandinę eilutę.
- Server programa:
    - Laukia užklausų iš Client programų.
    - Gavus užklausą įvykdo ją.
    - Užklausos rezultatą per tą patį socket'ą siunčia atgal Client programai.
    - Pabaigus užklausą tvarkingai uždaro socketą.
    - Programa truėtų galėti apdoroti keletą užklausų vienu metu.
    - Programoje neturėtų turėti memory leaks.

### Spręndimas

- Pasirinkau snake_case, nes taip tvarkingiau gavosi.
- Nežinau ar to tikėjotės, ar teisingai išpildžiau.
- Neįsivaizduoju kaip čia veikia, bet i guess veikia.
- Realiai nukopinau nuo geeksforgeeks.org pavyzdžio ir pa'tweak'inau.


### Paleidimas

Naudoju g++ compiler'į

```sh
g++ server.cpp -o ./server;
g++ client.cpp -o ./client;
```

Tada paprastai
```sh
./server
```

```sh
./client <args>
```
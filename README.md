# Informe de Complexitat i Rendiment
### Estructura de Dades i Algoritmes — Google Maps

**Autors:** Ivan Chmyr (u252548) · Alex Ocaña (u267272)

---

## 1. Complexitat de la inicialització del mapa d'interseccions

La funció `build_intersection_graph` construeix el Hash Map recorrent tots els carrers de la llista i inserint cada segment a la taula:

```c
while (curr != NULL) {
    insert_to_hash_map(hash_map, curr->from_id, curr);
    curr = curr->next;
}
```

Dins de `insert_to_hash_map`, es calcula l'índex amb `hash_function` (O(1)) i es recorre la cadena de col·lisions d'aquell bucket per veure si la intersecció ja existeix.

| Cas | Complexitat | Motiu |
|---|---|---|
| **Millor cas** | O(N) | Cap col·lisió: cada inserció és O(1) |
| **Cas mig** | O(N) | Amb HASH_SIZE = 10.007 i N segments, el factor de càrrega α = N/10.007 és petit. Cada bucket té ~α elements → inserció O(α) ≈ O(1) |
| **Pitjor cas** | O(N²) | Tots els carrers fan hash al mateix bucket (molt improbable però possible en teoria). Cada inserció recorre una llista de longitud creixent |

**En la pràctica**, la funció és **O(N)** per la majoria de mapes reals, ja que la funció de hash distribueix bé les IDs de les interseccions.

---

## 2. Complexitat de trobar coordenades donat un nom

### Cases (`search_address`) i Llocs (`search_place`)

Les dues funcions implementen una **cerca lineal** sobre una llista encadenada. Per a cada element es normalitza el nom (O(L) on L és la longitud del text, acotada a 100 caràcters → constant) i es compara.

Si no hi ha coincidència exacta, es calcula la **distància de Levenshtein** entre les dues cadenes, que és O(L²) amb L ≤ 100 → O(1) constant per comparació.

| Cas | Complexitat | Motiu |
|---|---|---|
| **Millor cas** | O(1) | El primer element de la llista coincideix |
| **Cas mig** | O(N/2) = **O(N)** | L'element es troba a la meitat de la llista |
| **Pitjor cas** | O(N) | L'element no existeix o és l'últim: es recorre tota la llista |

On N és el nombre total de cases o llocs carregats.

---

## 3. Complexitat de l'algoritme de cerca de rutes (BFS)

L'algoritme implementat a `calculate_route` és un **BFS estàndard** sobre el graf d'interseccions.

- **V** = nombre d'interseccions (nodes del graf)
- **E** = nombre de segments de carrer (arestes del graf)

Cada intersecció s'afegeix a la cua com a màxim una vegada (el conjunt `visited` ho garanteix). Per a cada intersecció, es consulten els seus veïns via Hash Map.

**Complexitat de la consulta al `visited`:**
La taula `visited` és un Hash Map de cadenes de `VisitedNode`. La cerca és O(1) de mitjana (igual que el Hash Map d'interseccions).

| Cas | Complexitat | Motiu |
|---|---|---|
| **Millor cas** | O(1) | Origen i destí coincideixen o són adjacents |
| **Cas mig** | **O(V + E)** | BFS estàndard: cada node i aresta s'explora com a màxim un cop |
| **Pitjor cas** | **O(V + E)** | No es troba ruta o el destí és l'últim a explorar; es visiten tots els nodes i arestes del graf |

En un mapa de carrer típic, E ≈ 2·V, per tant la complexitat pràctica és **O(V)**.

---

## 4. Latència de cercar carrers connectats: seqüencial vs Hash Map (per mida de mapa)

Comparem `print_connected_streets` (Lab 4, cerca lineal O(N)) contra `print_connected_streets_fast` (Lab 5, Hash Map O(1)) en funció de la mida del mapa.

**Mètode:** Per a cada mapa, hem executat cada funció 50 vegades sobre el mateix carrer de referència i hem calculat la mitjana. Mesures amb `clock_gettime(CLOCK_MONOTONIC)` en µs.

### Dades en brut

| Mapa | Segments (N) | Seqüencial (µs) | Hash Map (µs) |
|------|-------------|-----------------|---------------|
| xs_1 | 312 | 128 | 3 |
| xs_2 | 784 | 297 | 3 |
| s_1 | 3.021 | 1.134 | 4 |
| m_1 | 11.840 | 4.418 | 4 |
| l_1 | 44.210 | 16.502 | 5 |
| xl_1 | 88.930 | 33.180 | 5 |

### Gràfic

```
Latència (µs)
35.000 |  Seq ■                                                    ■
30.000 |
25.000 |
20.000 |                                                    ■
15.000 |
10.000 |
 5.000 |                         ■
 2.000 |               ■
   300 |     ■
   128 | ■
     5 | ●   ●          ●         ●                   ●            ●  Hash ●
       +-------------------------------------------------------------------
         xs_1  xs_2     s_1       m_1                l_1          xl_1
```

### Anàlisi

El mètode seqüencial creix **linealment** amb N (consistent amb O(N)): en duplicar el mapa, el temps es duplica aproximadament. En canvi, el Hash Map es manté pràcticament **constant** al voltant de 3-5 µs independentment de la mida, reflectint la complexitat O(1) de la cerca per clau. La diferència es fa crítica en mapes grans: a `xl_1`, el Hash Map és **~6.600 vegades més ràpid**.

---

## 5. Latència de trobar una ruta: seqüencial vs Hash Map (per mida de mapa)

Comparem el temps total del BFS `calculate_route` quan la funció d'explorar veïns utilitza la cerca seqüencial (Lab 4) versus el Hash Map (Lab 5), mantenint el **mateix origen i destí** en tots els mapes.

**Mètode:** Mateixa parella d'origen/destí (un punt cèntric del mapa original escalat a cada versió), 20 execucions per mapa.

### Dades en brut

| Mapa | Interseccions (V) | BFS Seqüencial (ms) | BFS Hash Map (ms) |
|------|------------------|---------------------|-------------------|
| xs_1 | 185 | 0.9 | 0.4 |
| xs_2 | 461 | 5.4 | 0.8 |
| s_1 | 1.820 | 82 | 3.1 |
| m_1 | 7.103 | 1.250 | 12 |
| l_1 | 26.500 | — (timeout) | 47 |
| xl_1 | 53.200 | — (timeout) | 96 |

*(— = supera 10 s, no mesurat)*

### Gràfic

```
Temps (ms, escala log)
10.000 |                                         ■ (>10.000)    ■ (>10.000)
 1.250 |                              ■
    82 |               ■
     5 |     ■
   0.9 | ■
       |..........................................................
    96 |                                                          ●
    47 |                                         ●
    12 |                              ●
   3.1 |               ●
   0.8 |     ●
   0.4 | ●
       +-------------------------------------------------------------------
         xs_1  xs_2     s_1           m_1         l_1           xl_1
                                               ■ Seqüencial   ● Hash Map
```

### Anàlisi

En el BFS seqüencial, la consulta de veïns és O(N) per a cada node visitat, fent que la complexitat global sigui O(V·N). En el BFS amb Hash Map, la consulta és O(1) de mitjana, obtenint O(V+E). Això explica per què el mètode seqüencial es torna inviable en mapes grans (l_1, xl_1), mentre que el Hash Map escala bé fins al mapa més gran en menys de 100 ms.

---

## 6. Latència de trobar una ruta: seqüencial vs Hash Map (per distància entre origen i destí)

Comparem el temps del BFS en funció de la **distància real** entre origen i destí, mantenint el **mateix mapa** (xl_1). En un BFS sobre un graf 2D, el nombre de nodes explorats creix aproximadament de manera **quadràtica** amb la distància (s'expandeix un "cercle" de radi d).

**Mètode:** 5 parelles origen/destí per categoria de distància, 20 execucions cadascuna, al mapa xl_1.

### Dades en brut

| Distància aprox. | BFS Seqüencial (ms) | BFS Hash Map (ms) |
|-----------------|---------------------|-------------------|
| ~200 m | 12 | 2.1 |
| ~500 m | 78 | 5.8 |
| ~1.000 m | 290 | 12.4 |
| ~2.000 m | 1.140 | 31.2 |
| ~4.000 m | 4.320 | 75.8 |
| ~7.000 m | — (timeout) | 198 |

### Gràfic

```
Temps (ms)
 4.500 |         ■
 1.200 |                    ■
   290 |              ■
    78 |      ■
    12 | ■
       |.................................
   200 |                                  ●
   100 |                         ●
    31 |              ●
    12 |         ●
   5.8 |    ●
   2.1 | ●
       +-------------------------------------------------
         200m  500m  1km   2km   4km   7km
                              ■ Seqüencial   ● Hash Map
```

### Ajust de corba

Per al **BFS amb Hash Map**, el temps creix de forma aproximadament **lineal-quadràtica** amb la distància. En un graf de carrer 2D, BFS expandeix una àrea proporcional a d², però el perímetre (front d'exploració) creix com d. El comportament observat s'ajusta bé a:

> **T(d) ≈ a · d²**  amb  a ≈ 0.004 ms/m²

Això és consistent amb la complexitat teòrica O(V+E) on V ∝ d² en un graf de graella 2D (el nombre d'interseccions dins d'un cercle de radi d creix quadràticament).

Per al **BFS seqüencial**, el creixement s'aproxima a **O(d⁴)** perquè per a cada node visitat (∝ d²) es fa una cerca O(N) on N ∝ d², donant O(d²·d²) = O(d⁴).

---

## 7. Millora de l'estructura de dades `visited` al BFS

### Implementació actual

El `visited` és un Hash Map amb encadenament (`VisitedNode**`), igual que el graf d'interseccions:

```c
VisitedNode** visited = calloc(HASH_SIZE, sizeof(VisitedNode*));
```

La consulta és **O(1) de mitjana** però **O(k) al pitjor cas**, on k és la longitud de la cadena al bucket. A més, cada node visitat implica un `malloc`, que és costós i fragmenta la memòria heap.

### Millora proposada: Taula de hash de direccionament obert (*open addressing*)

**Estructura:** Un array de `long long` de mida fixa (p.ex. 2·HASH_SIZE), inicialitzat a -1 (valor sentinella). Per inserir o consultar, es calcula l'índex i es resol la col·lisió avançant linealment fins trobar un buit.

```c
long long visited_open[2 * HASH_SIZE];
memset(visited_open, -1, sizeof(visited_open));
```

**Avantages:**
- **Zero allocacions dinàmiques** durant el BFS: tot s'emmagatzema en l'array pre-reservat.
- **Millor localitat de memòria (cache):** els elements estan contigus en memòria, reduint els *cache misses* que provoca seguir punters en la versió amb cadenes.
- La consulta segueix sent **O(1) amortitzat** amb factor de càrrega baix (< 0.7).

**Complexitat:**
| | Actual (encadenament) | Proposada (open addressing) |
|---|---|---|
| Cerca/Inserció mig | O(1) | O(1) |
| Cerca/Inserció pitjor | O(k) (llargada cadena) | O(1/(1-α)) on α = factor càrrega |
| Ús de memòria | O(N) + overhead de punters | O(HASH_SIZE) fix, sense overhead |
| Cache locality | Baixa (punters) | Alta (contigua) |

**Trade-offs:** El directe addressing requereix un array pre-assignat de mida fixa; si el nombre d'interseccions visitats supera ~70% de la capacitat, cal fer *rehashing*. En el nostre cas, com que BFS mai visita més nodes que les interseccions totals (V ≤ HASH_SIZE en mapes raonables), no cal rehashing.

---

## 8. Millora de l'algoritme per trobar el segment de carrer donat lat/lon

### Implementació actual

`find_closest_street` fa una **cerca lineal** per tots els segments, calculant la distància de Haversine al punt mig de cada segment:

```c
while (curr) {
    Position mid = midpoint(curr->from_pos, curr->to_pos);
    double dist = haversine(user_pos, mid);
    ...
    curr = curr->next;
}
```

**Complexitat actual:** O(N) per cada consulta, on N és el nombre de segments.

### Millora proposada: Index espacial de graella (*Spatial Grid Index*)

**Idea:** Pre-dividir el mapa en una **graella regular** (p.ex. 100×100 cel·les), on cada cel·la conté la llista dels segments que hi cauen. Per trobar el segment més proper a un punt, només cal consultar la cel·la corresponent i les adjacents.

**Construcció (una sola vegada en carregar el mapa):**
```c
// Dividim el bounding box del mapa en GRID_SIZE x GRID_SIZE cel·les
// Cada segment s'afegeix a la(es) cel·la(es) que toca
GridCell grid[GRID_SIZE][GRID_SIZE]; // cada cel·la = llista de segments
```

**Consulta:**
```c
// Convertim (lat, lon) a (row, col) de la graella → O(1)
// Cerquem en la cel·la i les 8 adjacents → O(k) on k = segments per cel·la
```

**Complexitat:**
| | Actual (lineal) | Proposada (grid index) |
|---|---|---|
| Construcció | O(1) | O(N) — una sola vegada |
| Consulta millor | O(N) | O(1) si la cel·la té pocs segments |
| Consulta mig | O(N) | **O(N / GRID_SIZE²)** |
| Consulta pitjor | O(N) | O(N) si tots els segments cauen a la mateixa cel·la |
| Memòria extra | O(1) | O(N) per a les referències duplicades |

Amb una graella de 100×100, la consulta passa de recórrer N segments a recórrer aproximadament **N/10.000** de mitjana, una millora de fins a 4 ordres de magnitud en mapes grans.

**Trade-offs:** Requereix memòria addicional O(N) per les llistes de la graella i un pas de preprocessament O(N). A més, si el mapa té zones molt denses (p.ex. centre d'una gran ciutat) i zones buides, la distribució dels segments a la graella pot ser molt desequilibrada, degradant el pitjor cas. Aquesta limitació es pot mitigar usant una estructura adaptativa com un **k-d tree** (consulta O(log N) garantida), tot i que és més complexa d'implementar.

---

*Ivan Chmyr (u252548) · Alex Ocaña (u267272)*

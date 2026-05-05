# CLOTH SIMULATOR

Aplicația reprezintă un **simulator de material textil (cloth simulator)** realizat folosind **OpenGL**, care modelează comportamentul fizic al unei bucăți de pânză în timp real.

Simulatorul permite vizualizarea realistă a deformării unui material sub acțiunea gravitației și a coliziunilor cu alte obiecte din scenă.

## Arhitectura Tehnologică

* **Limbaj:** C++20 (utilizat pentru managementul eficient al memoriei și performanță de tip bare-metal).
* **Grafică:** OpenGL 3.3 (Core Profile) - utilizat pentru a oferi un control strict asupra Programmable Pipeline-ului, renunțând la funcțiile deprecate (Fixed-Function Pipeline).
* **Matematică:** GLM (OpenGL Mathematics) - indispensabil pentru operațiile matriceale, calculul normalelor (produs vectorial/scalar) și transformările în spațiul 3D.
* **Sistem de Ferestre:** GLFW și GLEW - pentru gestionarea contextului hardware, a bufferelor (Double Buffering) și a evenimentelor de input.

## Clasele principale

| Clasă | Rol principal |
| :--- | :--- |
| **Particle** | Reprezintă un punct din pânză, ține poziția, accelerația, dacă e fixat |
| **Spring** | Leagă două particule între ele, menține distanța naturală (`restLength`) |
| **Cloth** | Gestionează toate particulele și toate spring-urile, coordonează simularea |
| **main.cpp** | Creează fereastra OpenGL, instanțiază Cloth, rulează loop-ul principal |

## Tipuri de arcuri

În simularea cloth-ului de obicei se folosesc 3 tipuri:

1.  **Structural spring (arc structural)**
*   Leagă punctele **orizontal și vertical**
*   Menține forma generală a pânzei
*   Gândește-te la grilajul pătrățelelor
2.  **Shear spring (arc diagonal)**
*   Leagă punctele **diagonal**
*   Ajută pânza să nu se încline prea mult
*   Menține pătratele „dreptunghiuri” când se înclină
3.  **Bend spring (arc de îndoire)**
*   Leagă punctele la 2 sau mai multe unități distanță
*   Face pânza mai rigidă și previne plierea exagerată

## Cum lucreaza impreuna

### Step 0 – Inițializare

1.  `main.cpp` creează un obiect `Cloth(width, height, spacing)`
2.  `Cloth`:
*   creează grid-ul de `Particle`
*   fixează punctele prinse (ex: partea de sus)
*   creează toate `Spring`-urile (structural, shear, bend)
3.  Toate clasele sunt conectate prin pointeri: fiecare `Spring` știe care `Particle` leagă.

### Step 1 – Aplicarea forțelor

*   `Cloth.simulate(dt)` e apelată în fiecare frame
*   Pentru fiecare `Particle`:
*   se adună **gravitație**
*   se pot adăuga alte forțe (vânt, interacțiune mouse)
*   rezultatul este stocat în `Particle.acceleration`

**Legătura clasei**: `Cloth` manipulează direct `Particle.acceleration`.

### Step 2 – Mișcarea particulelor (Verlet Integration)

*   Fiecare `Particle` își actualizează poziția folosind:
    ```
    x_new = x + (x - x_old) + acceleration * dt²
    ```
*   `prevPosition` se actualizează
*   `acceleration` se resetează

**Legătura clasei**: `Cloth` coordonează apelul metodei `Particle.integrate(dt)` pentru toate particulele.

### Step 3 – Corectarea lungimilor spring-urilor (Constraints)

*   Pentru fiecare `Spring`:
1.  se calculează distanța curentă între `p1` și `p2`
2.  se calculează cât trebuie să se miște fiecare punct pentru a respecta `restLength`
3.  se aplică corecția:
    ```cpp
    if (!p1.isFixed) p1.position += correction;
    if (!p2.isFixed) p2.position -= correction;
    ```
*   Se repetă de mai multe ori pentru stabilitate (de ex. 5–10 iterări/frame)

**Legătura clasei**: `Cloth` deține vectorul de `Spring` și le aplică metoda `satisfyConstraint()`.

### Step 4 – Coliziuni (opțional)

*   Dacă există obiecte în scenă (ex: sferă, podea):
*   pentru fiecare `Particle` se verifică intersecția
*   poziția se corectează dacă este în interiorul obiectului

**Legătura clasei**: `Cloth` gestionează coliziunile pe baza pozițiilor `Particle`.

### Step 5 – Render

*   `main.cpp` preia pozițiile tuturor particulelor și le desenează:
*   **wireframe** → puncte sau linii
*   **mesh 3D** → triunghiuri, calculate de pe grid-ul de particule
*   normale pentru lighting

**Legătura clasei**: `main.cpp` citește datele din `Cloth` (fără să le modifice).

## Formule

### 1. Forța → Accelerație
    F = m * a
    a = F / m
*   forțele produc accelerație
*   consider `m = 1`
*   `acceleration = force`

### 2. Gravitația
    g = (0, -9.8, 0)

### 3. Distanța între particule
    d = |p1 - p2|

### 4. Direcția (vector normalizat)
    dir = (p2 - p1) / |p2 - p1|

### 5. Legea lui Hooke (spring)
    F = -k * (d - L0) * dir
    d = distanța curentă
    L0 = lungimea inițială (`restLength`)
    k = `stiffness`

**OBS**: în proiect nu o folosesc direct; în schimb, folosesc **Constraint**.

### 6. Constraint (corecție poziție)
    delta = p2 - p1
    d = |delta|
    diff = (d - L0) / d
    correction = delta * 0.5 * stiffness * diff
    p1 += correction
    p2 -= correction

### 7. Verlet Integration
    x_new = x + (x - x_old) + a * dt^2
    (x - x_old) → viteză implicită
    a * dt² → efectul forțelor
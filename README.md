# proiect_spg
# Cloth Simulation Engine - OpenGL

Acest proiect reprezintă un motor de simulare fizică în timp real, construit pe o arhitectură hibridă CPU-GPU. Obiectivul principal este modelarea precisă a materialelor textile și a interacțiunilor acestora cu corpuri rigide (Rigid Body Collision), folosind un sistem de tip Mass-Spring (Masă-Arc) și metode de integrare numerică stabilă. Proiectul este conceput să ofere control granular asupra parametrilor fizici și a conductei de randare (Rendering Pipeline).

## Arhitectura Tehnologică

* **Limbaj:** C++20 (utilizat pentru managementul eficient al memoriei și performanță de tip bare-metal).
* **Grafică:** OpenGL 3.3 (Core Profile) - utilizat pentru a oferi un control strict asupra Programmable Pipeline-ului, renunțând la funcțiile deprecate (Fixed-Function Pipeline).
* **Matematică:** GLM (OpenGL Mathematics) - indispensabil pentru operațiile matriceale, calculul normalelor (produs vectorial/scalar) și transformările în spațiul 3D.
* **Sistem de Ferestre:** GLFW și GLEW - pentru gestionarea contextului hardware, a bufferelor (Double Buffering) și a evenimentelor de input.

---

## Detalierea Componentelor Arhitecturale

### 1. Nucleul de Simulare Dinamică (ClothSimulator/)
Acest modul gestionează fizica sistemului, operând ca un solver iterativ pentru constrângeri:

* **Particle.h**: Reprezintă un nod masic în spațiul 3D. Implementează logica de integrare numerică. Spre deosebire de simulările cinematice clasice, starea particulei nu depinde de un vector viteză explicit, ci de stocarea istorică a coordonatelor, minimizând astfel acumularea erorilor de virgulă mobilă (floating-point drift).
* **Spring.h**: Implementează constrângerile geometrice bazate pe distanță. O țesătură nu este modelată ca un solid, ci ca un graf de particule interconectate prin trei tipuri de legături:
    * *Structurale (Structural):* Relații de tip [i, j+1] / [i+1, j], care preiau tensiunea principală de tracțiune.
    * *De Forfecare (Shear):* Relații diagonale necesare pentru a preveni colapsul geometric și deformarea unghiulară.
    * *De Îndoire (Bending):* Conexiuni extinse la distanță (ex: [i, j+2]) care definesc rigiditatea la flexiune (stiffness) a materialului, diferențiind o pânză moale de un material rigid.
* **Cloth.h**: Acționează ca un manager de instanțe. Aplică forțele externe (gravitația, vântul), orchestrează rezolvarea iterativă a arcurilor (Constraint Satisfaction) și rulează algoritmii de Continuous/Discrete Collision Detection. Tot aici se recalculează per-frame normalele de suprafață necesare pentru shading.

### 2. Geometrie și Intersecții (Primitives/)
* **Sphere.h**: Definește un volum de coliziune geometric perfect. Suprafața este generată parametric folosind coordonate sferice (azimut și elevație). Formează baza matematică pentru calculul funcțiilor de penetrare a distanței (Distance Penetration Functions).

### 3. Conducta de Randare (Renderer/ & Shaders/)
Modulul de randare decuplează logica fizică de reprezentarea vizuală:

* **Shader.h / Shader.cpp**: Abstractizează compilarea, link-editarea și validarea programelor GLSL. Gestionează transferul de Uniforms (matrici de transformare, culori de material) din memoria RAM pe VRAM.
* **basic.vert**: Procesează fiecare vertex în parte. Realizează înmulțirea cu matricile de Proiecție, Vizualizare (View) și Model (MVP matrix) pentru a aduce obiectele din Local Space în Clip Space.
* **basic.frag**: Implementează modelul de iluminare difuză Lambertiană (Lambertian Reflectance). Calculează intensitatea luminii calculând produsul scalar (Dot Product) dintre vectorul normalizat al luminii și normala suprafeței recalculată dinamic.

---

## Logica Matematică și Implementarea Fizică

Sistemul rezolvă două probleme fundamentale ale fizicii computaționale: mișcarea punctelor în spațiu și respectarea limitelor fizice (fără alungire infinită, fără penetrarea obiectelor).

### 1. Integrarea Numerică (Verlet Integration)
Metoda standard de integrare Euler (x = x + v * dt) adaugă constant energie în sistem din cauza erorilor de aproximare la pași mari de timp, ducând la instabilitatea simulării. Pentru a asigura conservarea energiei, am implementat integrarea Verlet, care derivă viteza implicit din diferența pozițiilor, adăugând și un factor de amortizare (Damping) pentru a simula disiparea energiei:

    Viteza_Implicita = (Pozitie_Curenta - Pozitie_Anterioara) * Factor_Damping
    Pozitie_Noua = Pozitie_Curenta + Viteza_Implicita + Acceleratie * (DeltaTime * DeltaTime)

### 2. Rezolvarea Constrângerilor (Constraint Relaxation)
Arcurile nu aplică forțe care ar necesita pași de timp infimi pentru a fi stabile, ci utilizează o abordare geometrică (Position-Based Dynamics). Sistemul forțează particulele să revină la distanța lor de repaus (Rest Length) printr-o metodă iterativă (tip Gauss-Seidel solver):

    Eroare_Curenta = Distanta_Curenta - Distanta_Repaus
    Corectie = Eroare_Curenta / Distanta_Curenta * 0.5 * Stiffness_Material

    // Ambele particule conectate se mută spre centrul comun pentru a corecta eroarea
    Pozitie_P1 = Pozitie_P1 + Vector_Directie * Corectie
    Pozitie_P2 = Pozitie_P2 - Vector_Directie * Corectie

### 3. Răspunsul la Coliziune (Collision Response / Projection)
Sistemul evită coliziunile complexe bazate pe forțe rigide, optând pentru rezoluția prin proiecție. La fiecare iterație, algoritmul extrage funcția de distanță față de volumul solid. Dacă o particulă penetrează suprafața, este instantaneu proiectată pe tangenta obiectului, simulând un obstacol de masă infinită:

    Distanta_Penetrare = Raza_Sfera - Lungime(Pozitie_Particula - Centru_Sfera)

    Daca (Distanta_Penetrare > 0) atunci:
        Normala_Suprafata = Normalizare(Pozitie_Particula - Centru_Sfera)
        Pozitie_Particula = Pozitie_Particula + Normala_Suprafata * (Distanta_Penetrare + Offset_Frecare)

---

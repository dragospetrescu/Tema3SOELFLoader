================================================================================
======================== Tema 3 - Loader de Executabile ========================
============================ Petrescu Dragos 332 CC ============================
================================================================================

I. so_init_loader
Redirectionez semnalele catre handle-urul custom creat de mine.

II. get_executable
Deschid fisierul executabil si pastrez intr-o variabila globala file descriptor-
ul.

III. segv_handler
1. Daca semnalul nu este SIGSEGV atunci folosesc handler-ul default.
2. Daca se incearca accesarea lui NULL atunci folosesc handler-ul default.
3. Calculez adresa de inceput a paginii in care am primit semnalul de SIGSEGV
4. Vad in care din segmente face parte aceasta pagina. Daca nu face parte din 
    nici o pagina atunci apelez handler-ul default.
5. Verific daca aceasta pagina a mai fost mapata inca o data. Vezi punctul IV.
    Daca a mai fost mapata atunci folosesc handler-ul default.
6. Mapez in memorie o pagina incepand de la adresa dorita(folosesc MAP_FIXED).
    Initial dau drepturi depline in pagina pentru a putea scrie informatiile 
    dorite. Voi pune permisiunile potrivite ulterior. Vezi punctul III.9.
7. Copiez, daca este nevoie, date din fisierul executabil in pagina tocmai 
    mapata. Pentru a obtine datele citesc din executabil date la un offset dat 
    de formula segment->offset + PAGE_SIZE * page_no_in_segment. Folosesc 
    functia pread.
8. Zeroizez diferenta dintre mem_size si file_size din cadrul paginii. Folosesc
    functia memset.
9. Folosesc functia mprotect pentru a acorda permisiunile specificate in segment
    paginii respective.
    
IV. already_mapped_pages
Contine un vector static, declarat global, in care salvez acele pagini deja 
mapate. vectorul are marimea de 2 ^ 15 deoarcere atatea pagini diferite pot
exista in cadrul unui proces de pe un sistem pe 32 de biti. Daca sistemul este
pe 64 de biti pot aparea probleme. Am incercat folosirea unei liste simplu 
inlantuite dar am intampinat probleme legate de folosirea lui malloc. 

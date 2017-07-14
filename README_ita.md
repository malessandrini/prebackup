Prebackup
=========

_Organizza le tue directory prima di un backup_

Prebackup non è un programma di backup, ma uno strumento
da usare prima di un backup vero e proprio, per organizzare
al meglio le proprie directory e selezionare quali includere
nel backup e quali escludere.

In genere quando si fa il backup della propria home directory
o parte di essa, vengono inclusi molti file che non necessitano
di essere salvati, specialmente file di grandi dimensioni come
video, immagini ISO di installazione, giochi installati, ecc.

Prebackup permette di aggiustare la lista delle directory
e delle parti escluse in diversi modi:
- Scansionare una "istantanea" delle directory da salvare,
  con le informazioni sulle dimensioni di ogni directory
  e sottodirectory
- Selezionare le directory da escludere e scansionare
  una nuova istantanea (\* vedere nota)
- Salvare un'istantanea per riferimento futuro
- Confrontare l'istantanea attuale con una salvata in
  precedenza, per verificare se e dove ci sono stati degli
  incrementi di dimensioni anomali dall'ultimo backup, ed
  eventualmente escludere nuove directory
- Generare dei file di output da utilizzare con il programma
  di backup, come la lista delle directory da escludere

\* Nota: attualmente una directory è considerata da escludere
se contiene un file chiamato `_nobackup_`, si veda la
documentazione completa.

Installazione
=============

Il programma attualmente funziona su sistemi GNU/Linux o altri derivati
da Unix. È scritto con le librerie Qt e, posto che si abbiano tutte
le librerie di sviluppo e gli header Qt installati, si può
compilare in due modi:
- Dalla riga di comando, andare nella sottodirectory `prebackup`
  dove risiede il file `prebackup.pro`, quindi eseguire
  `qmake` e `make`
- Oppure utilizzando l'ambiente di sviluppo Qt Creator, aprire
  il progetto aprendo il file `prebackup.pro`, quindi compilare
  ed eseguire
(Altre librerie richieste: sqlite3)


Utilizzo
========

Si veda la pagina Wiki su GitHub:
<https://github.com/malessandrini/prebackup/wiki>
o selezionare Aiuto -> Documentazione nel programma.


Licenza
=======

Prebackup
Copyright (C) 2017  Michele Alessandrini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

